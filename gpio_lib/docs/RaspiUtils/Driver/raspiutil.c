///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Device driver for the Raspberry Pi utilities.
//
//  Copyright (c) 2013 Viewing Ltd.
//  All Rights Reserved.
//
//  This software is released under a Dual BSD (3 Clause) / GPL license
//
//  BSD:
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//      * Neither the name of the Viewing Ltd nor the
//        names of its contributors may be used to endorse or promote products
//        derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GPL:
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/device.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Defines
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DRIVER_VERSION		"v0.0.1"
#define DRIVER_AUTHOR		"Viewing Ltd"
#define DRIVER_DESC			"Raspi Utils"

#define CLASS_NAME			"raspiclass"
#define DEVICE_NAME			"raspi"

// ARM TRM section 3-83, 3-84
// Need a read barrier after the last read from peripheral, and a write barrier before first write to peripheral.
// Not so sure which of these to use but its leaning towards the sync.
#define ARM_DATA_SYNC_BARRIER __asm__ __volatile__ ("mcr p15, 0, r0, c7, c10, 4" : : : "memory")
#define ARM_DATA_MEM_BARRIER  __asm__ __volatile__ ("mcr p15, 0, r0, c7, c10, 5" : : : "memory")


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IOCTLs
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RASPI_OP_NULL		0
#define RASPI_OP_KVA_WRITE	1		// Address is ~masked and value & mask is written in, 0 is returned.
#define RASPI_OP_KVA_READ	2		// Address is read & mask, value returned.
#define RASPI_OP_MAX		3

typedef struct raspiutil_ioctl_params_def
{
	unsigned operation;
	unsigned kern_virt_addr;
	unsigned mask;
	unsigned write_val;
	unsigned return_val;
} raspiutil_ioctl_params;

// Pick a random magic number
#define RASPI_IOCTL_MAGIC	'R'
#define RASPI_IOCTL_WRITE	_IOW( RASPI_IOCTL_MAGIC, 1, raspiutil_ioctl_params * )
#define RASPI_IOCTL_READ	_IOR( RASPI_IOCTL_MAGIC, 2, raspiutil_ioctl_params * )


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Standard function declarations
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int	raspi_open( struct inode *, struct file * );
static long	raspi_ioctl( struct file *, unsigned int ioctl_num, unsigned long ioctl_param );
static int	raspi_release( struct inode *, struct file * );


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Device context & other standard driver structures
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct raspi_dev_def
{
	struct cdev			cdev;
	struct device		*p_device;
	struct mutex		io_mutex;
	int					is_open;
} raspi_dev;


static struct file_operations raspi_fops =
{
	.owner =			THIS_MODULE,
	.open =				raspi_open,
	.unlocked_ioctl =	raspi_ioctl,
	.release =			raspi_release,
};


static dev_t raspi_dev_number;
static struct class *p_raspi_class = NULL;
static raspi_dev *p_raspi_dev = NULL;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Debug
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define WANT_DEBUG 1
#define WANT_OVERRIDE_LEVEL 1

#ifdef WANT_DEBUG
#ifdef WANT_OVERRIDE_LEVEL
#define OUR_KERN_DEBUG   KERN_EMERG
#define OUR_KERN_INFO    KERN_EMERG
#define OUR_KERN_WARNING KERN_EMERG
#define OUR_KERN_ERROR   KERN_EMERG
#else
#define OUR_KERN_DEBUG   KERN_DEBUG
#define OUR_KERN_INFO    KERN_INFO
#define OUR_KERN_WARNING KERN_WARNING
#define OUR_KERN_ERROR   KERN_ERROR
#endif
#define raspi_dbg(  format, arg... ) do { printk( OUR_KERN_DEBUG   format, ## arg ); } while(0)
#define raspi_info( format, arg... ) do { printk( OUR_KERN_INFO    format, ## arg ); } while(0)
#define raspi_warn( format, arg... ) do { printk( OUR_KERN_WARNING format, ## arg ); } while(0)
#define raspi_err(  format, arg... ) do { printk( OUR_KERN_ERROR   format, ## arg ); } while(0)
#else
#define raspi_dbg(  format, arg... )
#define raspi_info( format, arg... )
#define raspi_warn( format, arg... )
#define raspi_err(  format, arg... )
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  init & cleanup
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int __init raspi_init( void )
{
	int retval;
	
	raspi_dbg( "RASPI: Init" );

	p_raspi_dev = kmalloc( sizeof(raspi_dev), GFP_KERNEL );
	if( p_raspi_dev == NULL )
	{
		raspi_err( "RASPI: ERROR, Out of memory" );
		goto failed_malloc;
	}

	memset( p_raspi_dev, 0, sizeof(raspi_dev) );
	
	if( alloc_chrdev_region( &raspi_dev_number, 0, 1, DEVICE_NAME ) < 0 )
	{
		raspi_dbg( "RASPI: Init cant register driver." );
		goto failed_register;
	}
	
	p_raspi_class = class_create( THIS_MODULE, CLASS_NAME );
	if( IS_ERR( p_raspi_class ) )
	{
		raspi_dbg( "RASPI: Init cant create class." );
		goto failed_class_create;
	}
	
	// cdev init is void return
	cdev_init( &(p_raspi_dev->cdev), &raspi_fops );
	p_raspi_dev->cdev.owner = THIS_MODULE;
	
	retval = cdev_add( &(p_raspi_dev->cdev), raspi_dev_number, 1 );
	if( retval != 0 )
	{
		raspi_err( "RASPI: ERROR, Failed to add device %s", DEVICE_NAME );
		goto failed_cdev_add;
	}
	
	p_raspi_dev->p_device = device_create( p_raspi_class, NULL, MKDEV( MAJOR( raspi_dev_number ), 0 ), NULL, "raspi%d", 0 );
	if( IS_ERR( p_raspi_dev->p_device ) )
	{
		raspi_err( "RASPI: ERROR, Failed to create device %s", DEVICE_NAME );
		goto failed_devcreate;
	}
	
	mutex_init( &(p_raspi_dev->io_mutex) );
	
	raspi_dbg( "RASPI: Registered device as maj=%d, min=%d", MAJOR( raspi_dev_number ), MINOR( raspi_dev_number ) );

	printk( KERN_INFO "RASPI Utility driver registered. Built on %s %s", __DATE__, __TIME__ );
	return 0;
	
	device_destroy( p_raspi_class, MKDEV( MAJOR( raspi_dev_number ), 0 ) );
failed_devcreate:

	cdev_del( &(p_raspi_dev->cdev) );
failed_cdev_add:

	class_destroy( p_raspi_class );
	p_raspi_class = NULL;
failed_class_create:

	unregister_chrdev_region( raspi_dev_number, 1 );
failed_register:	
	
	kfree( p_raspi_dev );
	p_raspi_dev = NULL;
failed_malloc:
	
	return -1;
}


void __exit raspi_exit( void )
{
	raspi_dbg( "RASPI: Exit" );

	if( p_raspi_dev != NULL )
	{
		device_destroy( p_raspi_class, MKDEV( MAJOR( raspi_dev_number ), 0 ) );
	}

	if( p_raspi_dev != NULL )
	{
		cdev_del( &(p_raspi_dev->cdev) );
	}
	
	if( p_raspi_class != NULL )
	{
		class_destroy( p_raspi_class );
		p_raspi_class = NULL;
	}

	unregister_chrdev_region( raspi_dev_number, 1 );

	if( p_raspi_dev != NULL )
	{
		kfree( p_raspi_dev );
		p_raspi_dev = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Open
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int raspi_open( struct inode *p_inode, struct file *p_file )
{
	raspi_dbg( "RASPI: Open" );

	if( !mutex_trylock( &p_raspi_dev->io_mutex ) )
	{
		raspi_err( "RASPI: ERROR (open), mutex not free" );
		return -EBUSY;
	}

	if( p_raspi_dev->is_open )
	{
		raspi_err( "RASPI: ERROR (open), device already open" );
		mutex_unlock( &p_raspi_dev->io_mutex );
		return -EBUSY;
	}

	p_raspi_dev->is_open = 1;

	mutex_unlock( &p_raspi_dev->io_mutex );
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Ioctl
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static long raspi_ioctl( struct file *p_file, unsigned int ioctl_num, unsigned long ioctl_param )
{
	raspiutil_ioctl_params __user *p_user_params;
	raspiutil_ioctl_params params;
	volatile unsigned *p_mem = NULL;
	unsigned tmp;
	int want_copy_to_user = 0;
	
	raspi_dbg( "RASPI: Ioctl" );

	if( p_raspi_dev == NULL )
	{
		raspi_err( "RASPI: ERROR (ioctl), cannot find device" );
		return -ENODEV;
	}

	if( !mutex_trylock( &p_raspi_dev->io_mutex ) )
	{
		raspi_err( "RASPI: ERROR (ioctl), mutex not free" );
		return -EBUSY;
	}

	if( p_raspi_dev->is_open == 0 )
	{
		raspi_err( "RASPI: ERROR (ioctl), Dev not open" );
		mutex_unlock( &p_raspi_dev->io_mutex );
		return -EBUSY;
	}
	
	p_user_params = (raspiutil_ioctl_params __user *)ioctl_param;
	if( copy_from_user( &params, p_user_params, sizeof(raspiutil_ioctl_params) ) != 0 )
	{
		raspi_dbg( "RASPI: IOCTL Could not get user params." );
		mutex_unlock( &p_raspi_dev->io_mutex );
		return -EINVAL;
	}
	
	params.return_val = 0;

	switch( ioctl_num )
	{
		case RASPI_IOCTL_WRITE:
			switch( params.operation )
			{
				case RASPI_OP_KVA_WRITE:
					raspi_dbg( "RASPI: IOCTL Write -> KVA WRITE 0x%08X 0x%08X 0x%08X",
									params.kern_virt_addr, params.mask, params.write_val );

					// Spec says that we dont need these on each write but we wont know with a call like this
					// whether there will be more or not.
					ARM_DATA_SYNC_BARRIER;
					p_mem = (unsigned *)params.kern_virt_addr;
					if( params.mask == 0xFFFFFFFF )
					{
						// Some regs are write only so we cant do read modify write actions
						*p_mem = params.write_val;
					}
					else
					{
						tmp = *p_mem;
						tmp &= ~params.mask;
						tmp |= (params.write_val & params.mask);
						*p_mem = tmp;
					}
					ARM_DATA_SYNC_BARRIER;
					break;
					
				default:
					raspi_dbg( "RASPI: IOCTL Write -> Unknown operation" );
					break;
			}
			break;

		case RASPI_IOCTL_READ:
			switch( params.operation )
			{
				case RASPI_OP_KVA_READ:
					raspi_dbg( "RASPI: IOCTL Read -> KVA READ 0x%08X 0x%08X 0x%08X",
									params.kern_virt_addr, params.mask, params.write_val );

					// Spec says that we dont need these on each write but we wont know with a call like this
					// whether there will be more or not.
					ARM_DATA_SYNC_BARRIER;
					p_mem = (unsigned *)params.kern_virt_addr;
					tmp = *p_mem;
					tmp &= params.mask;
					ARM_DATA_SYNC_BARRIER;

					params.return_val = tmp;

					want_copy_to_user = 1;
					break;
					
				default:
					raspi_dbg( "RASPI: IOCTL Read -> Unknown operation" );
					break;
			}
			break;

		default:
			mutex_unlock( &p_raspi_dev->io_mutex );
			return -ENOTTY;
	}

	if( want_copy_to_user )
	{
		if( copy_to_user( p_user_params, &params, sizeof(raspiutil_ioctl_params) ) != 0 )
		{
			raspi_dbg( "RASPI: IOCTL Could not write params to user space." );
			mutex_unlock( &p_raspi_dev->io_mutex );
			return -EINVAL;
		}
	}
	
	mutex_unlock( &p_raspi_dev->io_mutex );
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Release
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int raspi_release( struct inode *p_inode, struct file *p_file )
{
	mutex_lock( &p_raspi_dev->io_mutex );

	p_raspi_dev->is_open = 0;

	raspi_dbg( "RASPI: now released" );

	mutex_unlock( &p_raspi_dev->io_mutex );
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Module stuff
//
///////////////////////////////////////////////////////////////////////////////////////////////////
module_init( raspi_init );
module_exit( raspi_exit );

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE( "Dual BSD/GPL" );

