///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Raspi GPIO interface lib header
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
#ifdef OS_LINUX

// Referring to: Broadcom BCM2835 ARM Peripherals (6th Feb 2012)
// Section: 1.2.2
//   Virtual addresses in kernel mode will range between 0xC0000000 and 0xEFFFFFFF.
//   Peripherals (at physical address 0x20000000 on) are mapped into the kernel virtual address space starting
//    at address 0xF2000000. Thus a peripheral advertised here at bus address 0x7Ennnnnn is available in the
//    ARM kernel at virtual address 0xF2nnnnnn.
#define BUSADDR  0x7E000000
#define KERNADDR 0xF2000000

#define NUM_GPIO_PINS 54
#define NUM_GPIO_BANKS ((NUM_GPIO_PINS + 31) / 32)

// Section: 6.1
#define	GPFSEL0		0x00200000 	// GPIO Function Select 0, R/W
#define	GPFSEL1		0x00200004 	// GPIO Function Select 1, R/W
#define	GPFSEL2		0x00200008 	// GPIO Function Select 2, R/W
#define	GPFSEL3		0x0020000C 	// GPIO Function Select 3, R/W
#define	GPFSEL4		0x00200010 	// GPIO Function Select 4, R/W
#define	GPFSEL5		0x00200014 	// GPIO Function Select 5, R/W
#define	GPSET0		0x0020001C 	// GPIO Pin Output Set 0, W
#define	GPSET1		0x00200020 	// GPIO Pin Output Set 1, W
#define	GPCLR0		0x00200028 	// GPIO Pin Output Clear 0, W
#define	GPCLR1		0x0020002C 	// GPIO Pin Output Clear 1, W
#define	GPLEV0		0x00200034 	// GPIO Pin Level 0, R
#define	GPLEV1		0x00200038 	// GPIO Pin Level 1, R
#define	GPEDS0		0x00200040 	// GPIO Pin Event Detect Status 0, R/W
#define	GPEDS1		0x00200044 	// GPIO Pin Event Detect Status 1, R/W
#define	GPREN0		0x0020004C 	// GPIO Pin Rising Edge Detect Enable 0, R/W
#define	GPREN1		0x00200050 	// GPIO Pin Rising Edge Detect Enable 1, R/W
#define	GPFEN0		0x00200058 	// GPIO Pin Falling Edge Detect Enable 0, R/W
#define	GPFEN1		0x0020005C 	// GPIO Pin Falling Edge Detect Enable 1, R/W
#define	GPHEN0		0x00200064 	// GPIO Pin High Detect Enable 0, R/W
#define	GPHEN1		0x00200068 	// GPIO Pin High Detect Enable 1, R/W
#define	GPLEN0		0x00200070 	// GPIO Pin Low Detect Enable 0, R/W
#define	GPLEN1		0x00200074 	// GPIO Pin Low Detect Enable 1, R/W
#define	GPAREN0		0x0020007C 	// GPIO Pin Async. Rising Edge Detect 0, R/W
#define	GPAREN1		0x00200080 	// GPIO Pin Async. Rising Edge Detect 1, R/W
#define	GPAFEN0		0x00200088 	// GPIO Pin Async. Falling Edge Detect 0, R/W
#define	GPAFEN1		0x0020008C 	// GPIO Pin Async. Falling Edge Detect 1, R/W
#define	GPPUD		0x00200094 	// GPIO Pin Pull-up/down Enable, 32 R/W
#define	GPPUDCLK0	0x00200098 	// GPIO Pin Pull-up/down Enable Clock 0, R/W
#define	GPPUDCLK1	0x0020009C 	// GPIO Pin Pull-up/down Enable Clock 1, R/W

// 0x00200018 - Reserved
// 0x00200024 - Reserved
// 0x00200030 - Reserved
// 0x0020003C - Reserved
// 0x00200048 - Reserved
// 0x00200054 - Reserved
// 0x00200060 - Reserved
// 0x0020006C - Reserved
// 0x00200078 - Reserved
// 0x00200084 - Reserved
// 0x00200090 - Reserved
// 0x002000A0 - Reserved
// 0x002000B0 - Test, R/W

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GPFSEL
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Where P is from 0 to 53 inclusive
#define GPFSEL_REG(P)			(GPFSEL0 + ((GPFSEL1 - GPFSEL0) * ((P) / 10)))
#define GPFSEL_MASK(P)			(0x7 << (((P) % 10) * 3))
#define GPFSEL_VALSHIFT(P,V)	((V) << (((P) % 10) * 3))

// Where V is one of:
// Section 6.1 Table 6.2
#define	GPFSEL_INPUT	0
#define	GPFSEL_OUTPUT	1
#define	GPFSEL_ALT0		4
#define	GPFSEL_ALT1		5
#define	GPFSEL_ALT2		6
#define	GPFSEL_ALT3		7
#define	GPFSEL_ALT4		3
#define	GPFSEL_ALT5		2

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GPSET, GPCLR, GPLEV
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Where P is from 0 to 53 inclusive
// Set and Clr and both write only so no mask
#define GPSET_REG(P)			(GPSET0 + ((GPSET1 - GPSET0) * ((P) / 32)))
#define GPSET_VALSHIFT(P,V)		((V) ? (1 << ((P) % 32)) : 0)

#define GPCLR_REG(P)			(GPCLR0 + ((GPCLR1 - GPCLR0) * ((P) / 32)))
#define GPCLR_VALSHIFT(P,V)		((V) ? (1 << ((P) % 32)) : 0)

#define GPLEV_REG(P)			(GPLEV0 + ((GPLEV1 - GPLEV0) * ((P) / 32)))
#define GPLEV_REG_BANK(B)		(GPLEV0 + ((GPLEV1 - GPLEV0) * (B)))
#define GPLEV_MASK(P)			(1 << ((P) % 32))


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  The free running timer
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define	TIMER_CLO	0x00003004
#define	TIMER_CHI	0x00003008


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RPi GPIO IOCTL, Globals and Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IOCTL
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
// End of IOCTL

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#ifdef TEST_WITH_PRINTS
	#define GP_READ(A)				printf( "Reading 0x%08X\n", KERNADDR | (A) )
	#define GP_MASKED_READ(A,M)		printf( "Reading 0x%08X with mask 0x%08X\n", KERNADDR | (A), M )
	#define GP_WRITE(A,V)			printf( "Writing 0x%08X with val 0x%08X\n", KERNADDR | (A), V )
	#define GP_MASKED_WRITE(A,M,V)	printf( "Writing 0x%08X with mask 0x%08X and val 0x%08X\n", KERNADDR | (A), M, V )
#else
	#define GP_READ(A)				RPiGpio_ControlDevice( RASPI_OP_KVA_READ,  (KERNADDR | (A)), 0xFFFFFFFF, 0 )
	#define GP_MASKED_READ(A,M)		RPiGpio_ControlDevice( RASPI_OP_KVA_READ,  (KERNADDR | (A)), M,          0 )
	#define GP_WRITE(A,V)			RPiGpio_ControlDevice( RASPI_OP_KVA_WRITE, (KERNADDR | (A)), 0xFFFFFFFF, V )
	#define GP_MASKED_WRITE(A,M,V)	RPiGpio_ControlDevice( RASPI_OP_KVA_WRITE, (KERNADDR | (A)), M,          V )
#endif

extern unsigned rpigpio_input_mask[ NUM_GPIO_BANKS ];
extern void RPiGpio_InitMasks();

// For talking to the device - Use a global file handle for the time being.
extern int handle_raspi_device;
extern BOOL RPiGpio_OpenDevice();
extern BOOL RPiGpio_CloseDevice();
extern unsigned RPiGpio_ControlDevice( unsigned op, unsigned address, unsigned mask, unsigned val );

// GPIO pin API
extern BOOL RPiGpio_SelectPinAsInput( int p );
extern BOOL RPiGpio_SelectPinAsOutput( int p );
extern BOOL RPiGpio_SetBit( int p, int v );
extern int  RPiGpio_GetBit( int p );

// GPIO bank of IO lines API
extern unsigned RPiGpio_GetBank( int b );

// Read the free running H/W timer
extern BOOL RpiTimer_Read( unsigned *p_hi, unsigned *p_lo );

#endif // OS_LINUX
