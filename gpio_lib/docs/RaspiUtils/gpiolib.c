///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Raspi GPIO interface lib functions and main test.
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

#define WANT_TEST_MAIN 1
//#define TEST_WITH_PRINTS 1


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Includes
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "gpiolib.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RPi Device functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int handle_raspi_device = 0;


BOOL RPiGpio_OpenDevice()
{
#if TEST_WITH_PRINTS
	return TRUE;
#endif

	if( handle_raspi_device != 0 )
	{
		return FALSE;
	}

	handle_raspi_device = open( "/dev/raspi0", O_RDONLY );
	if( handle_raspi_device <= 0 )
	{
		static char errstr[ 2048 ];
		int strerrret = 0;

		strerrret = strerror_r( handle_raspi_device, errstr, 2048 );
		if( strerrret == 0 )
		{
			fprintf( stderr, "Error ( %d ): %s\n", handle_raspi_device, errstr );
		}
		else
		{
			fprintf( stderr, "Unknown error of %d\n", handle_raspi_device );
		}

		handle_raspi_device = 0;
		return FALSE;
	}

	printf( "Device is now open\n" );

	return TRUE;
}


BOOL RPiGpio_CloseDevice()
{
#if TEST_WITH_PRINTS
	return TRUE;
#endif

	if( handle_raspi_device == 0 )
	{
		return FALSE;
	}
	
	close( handle_raspi_device );
	handle_raspi_device = 0;

	printf( "Device closed.\n" );

	return TRUE;
}


unsigned RPiGpio_ControlDevice( unsigned op, unsigned address, unsigned mask, unsigned val )
{
	raspiutil_ioctl_params io;

#if DEBUG
	if( handle_raspi_device == 0 )
	{
		return 0;
	}
#endif

	io.operation = op;
	io.kern_virt_addr = address;
	io.mask = mask;
	io.write_val = val;
	io.return_val = 0;

	switch( op )
	{
	// All the writes
	case RASPI_OP_KVA_WRITE:
		ioctl( handle_raspi_device, RASPI_IOCTL_WRITE, &io );
		break;
		
	// All the reads
	case RASPI_OP_KVA_READ:
		ioctl( handle_raspi_device, RASPI_IOCTL_READ, &io );
		break;

	default:
		break;
	}

	return io.return_val;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RPi GPIO Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned rpigpio_input_mask[ NUM_GPIO_BANKS ];


void RPiGpio_InitMasks()
{
	int n;
	for( n=0; n<NUM_GPIO_BANKS; n++ )
	{
		rpigpio_input_mask[n] = 0;
	}
}


BOOL RPiGpio_SelectPinAsInput( int p )
{
#if DEBUG
	if( p < 0 || p >= NUM_GPIO_PINS )
	{
		return FALSE;
	}
#endif

	GP_MASKED_WRITE( GPFSEL_REG(p), GPFSEL_MASK( p ), GPFSEL_VALSHIFT( p, GPFSEL_INPUT ) );

	rpigpio_input_mask[ p / 32 ] |= (1 << (p % 32));
	
	return TRUE;
}


BOOL RPiGpio_SelectPinAsOutput( int p )
{
#if DEBUG
	if( p < 0 || p >= NUM_GPIO_PINS )
	{
		return FALSE;
	}
#endif

	GP_MASKED_WRITE( GPFSEL_REG(p), GPFSEL_MASK( p ), GPFSEL_VALSHIFT( p, GPFSEL_OUTPUT ) );

	rpigpio_input_mask[ p / 32 ] &= ~(1 << (p % 32));
	
	return TRUE;
}


BOOL RPiGpio_SetBit( int p, int v )
{
#if DEBUG
	if( p < 0 || p >= NUM_GPIO_PINS || v < 0 || v > 1 )
	{
		return FALSE;
	}
#endif

	if( v )
	{
		GP_WRITE( GPSET_REG(p), GPSET_VALSHIFT( p, 1 ) );
	}
	else
	{
		GP_WRITE( GPCLR_REG(p), GPCLR_VALSHIFT( p, 1 ) );
	}
	
	return TRUE;
}


int RPiGpio_GetBit( int p )
{
#if DEBUG
	if( p < 0 || p >= NUM_GPIO_PINS )
	{
		return 0;
	}
#endif

	if( GP_MASKED_READ( GPLEV_REG(p), GPLEV_MASK( p ) ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


unsigned RPiGpio_GetBank( int b )
{
	unsigned val;
	
#if DEBUG
	if( b < 0 || b >= NUM_GPIO_BANKS )
	{
		return 0;
	}
#endif

	val = GP_READ( GPLEV_REG_BANK(b) ) & rpigpio_input_mask[b];

	return val;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Read the free running timer
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL RpiTimer_Read( unsigned *p_hi, unsigned *p_lo )
{
	if( p_hi == NULL || p_lo == NULL )
	{
		return FALSE;
	}
	
	// Because this is a free running asynchronous timer we should read the hi word then read the lo word then
	// read the hi word again and only if the 2nd hi word is the same as 1st should we trust it
	// else keep reading the lo then high again until we get a match.
	// Otherwise we could have rolled over and not know if the lo val was for the 1st or 2nd hi and become 1<<32 out.
	// I'm told this free running timer runs at 1MHz so it will roll over every 4000 secs approx (~ 1hr) so this
	// should loop a second time only very occasionally.

	volatile unsigned hi1 = GP_READ( TIMER_CHI );
	volatile unsigned hi2;
	volatile unsigned lo;
	
	for(;;)
	{
		lo  = GP_READ( TIMER_CLO );
		hi2 = GP_READ( TIMER_CHI );
		
		if( hi1 == hi2 )
		{
			break;
		}
		
		hi1 = hi2;
	}
	
	*p_hi = hi1;
	*p_lo = lo;
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Testing
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WANT_TEST_MAIN

void sleep_ms( int ms )
{
	usleep( ms * 1000 );
}


#define WANT_TEST_GPIO 1
#define WANT_TEST_TIMER 1
#define WANT_TEST_IRLED 1


int main( int argc, char *argv[] )
{
	int n, req_pulses;
	unsigned hi_start, lo_start;
	unsigned hi_last, lo_last;
	unsigned hi_finish, lo_finish;
	unsigned step;
	float secs;
	
	RPiGpio_InitMasks();

	if( RPiGpio_OpenDevice() )
	{
#ifdef WANT_TEST_GPIO
		printf( "Checking the GPIO pins...\n" );
		
		RPiGpio_SelectPinAsOutput( 17 );
		RPiGpio_SelectPinAsOutput( 18 );
		RPiGpio_SelectPinAsOutput( 27 );
		RPiGpio_SelectPinAsOutput( 22 );
		RPiGpio_SelectPinAsOutput( 23 );
		RPiGpio_SelectPinAsOutput( 24 );

		for( n=0; n<63; n++ )
		{
			RPiGpio_SetBit( 17, (n & (1<<0)) ? 1 : 0 );
			RPiGpio_SetBit( 18, (n & (1<<1)) ? 1 : 0 );
			RPiGpio_SetBit( 27, (n & (1<<2)) ? 1 : 0 );
			RPiGpio_SetBit( 22, (n & (1<<3)) ? 1 : 0 );
			RPiGpio_SetBit( 23, (n & (1<<4)) ? 1 : 0 );
			RPiGpio_SetBit( 24, (n & (1<<5)) ? 1 : 0 );
			sleep_ms( 250 );
		}

		RPiGpio_SetBit( 17, 0 );
		RPiGpio_SetBit( 18, 0 );
		RPiGpio_SetBit( 27, 0 );
		RPiGpio_SetBit( 22, 0 );
		RPiGpio_SetBit( 23, 0 );
		RPiGpio_SetBit( 24, 0 );
#endif // WANT_TEST_GPIO

#ifdef WANT_TEST_TIMER
		printf( "Checking the hardware timer against usleep( 5000000 ) for 5 secs time test - hold on...\n" );
		RpiTimer_Read( &hi_start, &lo_start );
		sleep_ms( 5000 );
		RpiTimer_Read( &hi_finish, &lo_finish );
		printf( "Raw times were (0x%08x, 0x%08x) to (0x%08x, 0x%08x)\n",
			hi_start, lo_start, hi_finish, lo_finish );
			
		if( hi_start == hi_finish )
		{
			printf( "Real time was %f ms difference\n",
				(float)(lo_finish - lo_start) / 1000.0f );
		}
		else if( (hi_start+1) == hi_finish )
		{
			printf( "Real time was %f ms difference\n",
				(float)(0xFFFFFFFFU - lo_start + lo_finish) / 1000.0f );
		}
		else
		{
			printf( "Real time was approx %f secs difference\n",
				(float)(hi_finish - hi_start) * 4295.967f );
		}
#endif // WANT_TEST_TIMER


#ifdef WANT_TEST_IRLED
		RPiGpio_SelectPinAsOutput( 17 );

		// 38KHz is 26.315789 uS. Since our counter is uS one we need to step in lumps of 26 and
		// occasionally add a little bit more for the remainder
		// Of course this can only be measured to the accuracy of the Rpi on board timer.
		RpiTimer_Read( &hi_start, &lo_start );
		hi_last = hi_start;
		lo_last = lo_start;
		n = 0;
		req_pulses = 38000;		// 1 secs worth
		
		while( n < req_pulses )
		{
			// LED on
			RPiGpio_SetBit( 17, 1 );
			
			// wait a fixed 5uS on pulse

			// Set new target 5us into the future
			if( lo_last <= (0xFFFFFFFFU - 5) )
			{
				lo_last += 5;
			}
			else
			{
				hi_last++;
				lo_last = 5 - (0xFFFFFFFFU - lo_last);
			}

			// Busy wait for target time
			do
			{
				RpiTimer_Read( &hi_finish, &lo_finish );
			} while( (hi_finish < hi_last) || ((hi_finish == hi_last) && (lo_finish < lo_last)) );

			// LED off
			RPiGpio_SetBit( 17, 0 );

			// Set new target (26.315789 - 5)us into the future
			// So thats 21us plus
			//  plus one every four pulses => (21 + 5 + 0.25) = 26.25
			//	plus one every 16 pulses => 26.25 + 0.0625 = 26.3125
			//  plus one every 305 pulses => 26.3125 + 0.003289 = 26.315789
			step = 21;
			if( n % 4 == 0 )
			{
				step++;
			}
			if( n % 16 == 0 )
			{
				step++;
			}
			if( n % 305 == 0 )
			{
				step++;
			}
			
			if( lo_last <= (0xFFFFFFFFU - step) )
			{
				lo_last += step;
			}
			else
			{
				hi_last++;
				lo_last = step - (0xFFFFFFFFU - lo_last);
			}

			// Busy wait for target time
			do
			{
				RpiTimer_Read( &hi_finish, &lo_finish );
			} while( (hi_finish < hi_last) || ((hi_finish == hi_last) && (lo_finish < lo_last)) );
			
			n++;
		}

		if( hi_start == hi_finish )
		{
			secs = (float)(lo_finish - lo_start) / 1000000.0f;
		}
		else if( (hi_start+1) == hi_finish )
		{
			secs = (float)(0xFFFFFFFFU - lo_start + lo_finish) / 1000000.0f;
		}
		else
		{
			secs = 1.0f;
		}
		printf( "Clocked %d pulses in %f secs -> %f Hz.\n", req_pulses, secs, (float)req_pulses / secs );
		
		RPiGpio_SetBit( 17, 0 );
#endif

		RPiGpio_CloseDevice();
	}
	else
	{
		printf( "Failed to open the device !\n" );
	}
	
	return 0;
}

#endif // WANT_TEST_MAIN

