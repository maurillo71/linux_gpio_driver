#include <linux/module.h> /* needed by all modules */
#include <linux/kernel.h> /* needed by KERN_INFO */
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/string.h>
#include "gpio.h"
/*
====================================
FILE OPERATIONS
====================================
*/

static int device_open(struct inode *inode,struct file *file)
{
  driver_info("%s:Open...\n",DEVICE_NAME);
  if(mutex_trylock(&p_gpio_dev->io_mutex)==FALSE)
    {
      driver_err("\t%s:Mutex not free!\n",DEVICE_NAME);
      return -EBUSY;
    }
  if(p_gpio_dev->is_open==TRUE)
    {
      driver_err("\t%s:Driver already open!\n",DEVICE_NAME);
      mutex_unlock(&p_gpio_dev->io_mutex);
      return -EBUSY;
    }
  p_gpio_dev->is_open=TRUE;
  mutex_unlock(&p_gpio_dev->io_mutex);
  return SUCCESS;
}

static int device_release(struct inode *inode,struct file *file)
{
  driver_info("%s:Release...\n",DEVICE_NAME);

  mutex_lock(&p_gpio_dev->io_mutex);
  p_gpio_dev->is_open=FALSE;
  mutex_unlock(&p_gpio_dev->io_mutex);
  return SUCCESS;
}

static long device_ioctl(struct file *file,unsigned int ioctl_num,unsigned long ioctl_param)
{
  
  driver_info("%s:IOCTL...\n",DEVICE_NAME);
  gpio_ioctl __user *p_user_ioctl;
  gpio_ioctl ioctl_buffer;
  volatile unsigned *p_memory=NULL;
  BOOLEAN var_to_user=FALSE;

  if(p_gpio_dev==NULL)
    {
      driver_err("\t%s: Device not found!\n",DEVICE_NAME);
      return -ENODEV;
    }
  
  if(mutex_trylock(&p_gpio_dev->io_mutex)==FALSE)
    {
      driver_err("\t%s:Mutex not free!\n",DEVICE_NAME);
      return -EBUSY;
    }
  
  if(p_gpio_dev->is_open==FALSE)
    {
      driver_err("%s:Device not open!\n", DEVICE_NAME);
      mutex_unlock(&p_gpio_dev->io_mutex);
      return -EBUSY;
    }

  p_user_ioctl=(gpio_ioctl __user*) ioctl_param;
  
  if(copy_from_user(&ioctl_buffer,p_user_ioctl,sizeof(gpio_ioctl))!=SUCCESS)
    {
      driver_err("\t%s:Could not read values from user!\n",DEVICE_NAME);
      mutex_unlock(&p_gpio_dev->io_mutex);
      return -EINVAL;
    }
  ioctl_buffer.read_value=0;
  switch(ioctl_num)
    {
    case _IOCTL_WRITE:
      {
	//delay in ARM NOP will be added
	driver_info("\t%s:IOCTL Write:0x%08X 0x%08X!\n",DEVICE_NAME,ioctl_buffer.address,ioctl_buffer.write_value);
	p_memory=(unsigned *)ioctl_buffer.address;
	*p_memory=ioctl_buffer.write_value;
	//delay in ARM NOP will be added
      }
      break;
    case _IOCTL_READ:
      {
	driver_info("\t%s:IOCTL Read:0x%08X 0x%08X!\n",DEVICE_NAME,ioctl_buffer.address,ioctl_buffer.write_value);
	//delay in ARM NOP will be added
	p_memory=(unsigned *)ioctl_buffer.address;
	ioctl_buffer.read_value=*p_memory;
	var_to_user=TRUE;
      }
      break;
    default:
      {
	mutex_unlock(&p_gpio_dev->io_mutex);
	return -ENOTTY;
      }
    }
  if(var_to_user==TRUE)
    {
      if(copy_to_user(p_user_ioctl,&ioctl_buffer,sizeof(gpio_ioctl))!=SUCCESS)
	{
	  driver_err("\t%s:Cout not write values to user!\n",DEVICE_NAME);
	  mutex_unlock(&p_gpio_dev->io_mutex);
	  return -EINVAL;
	}
    }
  mutex_unlock(&p_gpio_dev->io_mutex);
  return SUCCESS;
}

/*
====================================
INIT
====================================
*/

static int __init gpio_init(void)
{
  driver_info("%s:Init...\n",DEVICE_NAME);
  p_gpio_dev=kmalloc(sizeof(gpio_device),GFP_KERNEL);
  if(!p_gpio_dev)
    {
      driver_err("\t%s:Out of memory!\n",DEVICE_NAME);
      goto failed_alloc;
    }
  memset(p_gpio_dev,0,sizeof(gpio_device));
  
  if(alloc_chrdev_region(&gpio_dev_no,0,1,DEVICE_NAME)<0)
    {
      driver_err("\t%s:Not registered!\n",DEVICE_NAME);
      goto failed_register;
    }	 
  
  p_gpio_class=class_create(THIS_MODULE,DEVICE_CLASS_NAME);
  if(IS_ERR(p_gpio_class))
    {
      driver_err("\t%s:Class not created!\n",DEVICE_NAME);
      goto failed_class;
    }
  
  cdev_init(&(p_gpio_dev->cdev),&fops);
  p_gpio_dev->cdev.owner=THIS_MODULE;
  if(cdev_add(&(p_gpio_dev->cdev),gpio_dev_no,1)!=SUCCESS)
    {
      driver_err("\t%s:Failed to add device!\n",DEVICE_NAME);
      goto failed_add;
    }
  p_gpio_dev->p_device=device_create(p_gpio_class,NULL,MKDEV(MAJOR(gpio_dev_no),0),NULL,DEVICE_PROCESS,0);
  if(IS_ERR(p_gpio_dev->p_device))
    {
      driver_err("\t%s:Failed to create device!\n",DEVICE_NAME);
      goto failed_create;
    }
  mutex_init(&(p_gpio_dev->io_mutex));
  driver_info("\t%s:Registered device with (%d,%d)\n",DEVICE_NAME,MAJOR(gpio_dev_no),MINOR(gpio_dev_no));
  driver_info("Driver %s loaded.Build on %s %s!\n",DEVICE_NAME,__DATE__,__TIME__);
  goto success;
 success:
  return SUCCESS;
  
  device_destroy(p_gpio_class,MKDEV(MAJOR(gpio_dev_no),0));
  
 failed_create:
  cdev_del(&(p_gpio_dev->cdev));
  
 failed_add:
  class_destroy(p_gpio_class);
  p_gpio_class=NULL;
  
 failed_class:
  unregister_chrdev_region(gpio_dev_no,1);
  
 failed_register:
  kfree(p_gpio_dev);
  p_gpio_dev=NULL;
  
 failed_alloc:
  return FAILURE;
}

/*
====================================
EXIT
====================================
*/
static void __exit gpio_exit(void)
{
  driver_info("%s:Exit...\n",DEVICE_NAME);
  if(p_gpio_class!=NULL)
    {
      class_destroy(p_gpio_class);
      p_gpio_class=NULL;
    }
  unregister_chrdev_region(gpio_dev_no,1);
  if(p_gpio_dev!=NULL)
    {
      device_destroy(p_gpio_class,MKDEV(MAJOR(gpio_dev_no),0));
      cdev_del(&(p_gpio_dev->cdev));
      kfree(p_gpio_dev);
      p_gpio_dev=NULL;
    }
  driver_info("Driver %s unloaded.Build on %s %s!\n",DEVICE_NAME,__DATE__,__TIME__);
  
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("device")
