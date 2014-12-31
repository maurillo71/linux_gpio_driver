/*
 * gpio_lib.c
 *
 *  Created on: Dec 30, 2014
 *      Author: mihai
 */

#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "gpio_lib.h"

extern BOOL init_device()
{
	if (device_status != 0)
	{
		return FALSE;
	}
	device_status = open("/dev/gpio0", O_RDONLY);
	if (device_status < 0)
	{
		static char error_string[2048];
		if (strerror_r(device_status, error_string, 2048) == 0)
		{
			fprintf(stderr, "Error %d:%s\n", device_status, error_string);
		}
		else
		{
			fprintf(stderr, "Unknown error %d\n", device_status);
		}
		device_status = 0;
		return FALSE;
	}
	bcm2835_gpio = (uint32_t*) GPIO_BASE;
	return TRUE;
}

extern BOOL close_device()
{
	if (device_status == 0)
	{
		return FALSE;
	}
	close(device_status);
	device_status = 0;
	bcm2835_gpio = NULL;
	return TRUE;
}

int32_t control_device(ACCEPTED_OPERATIONS operation, uint32_t address,
        uint32_t value)
{
	if (device_status == 0) return -1;
	gpio_ioctl io;

	io.address = address;
	io.write_value = value;
	io.read_value = 0;
	switch (operation)
	{
		case WRITE_OPERATION:
			ioctl(device_status, _IOCTL_WRITE, &io);
			break;
		case READ_OPERATION:
			ioctl(device_status, _IOCTL_READ, &io);
			break;
		default:
			break;
	}
	return io.read_value;

}

extern BOOL set_pin_input(uint8_t pin)
{
	if (pin < 0 || pin > 32) return FALSE;
	return (WRITE_BIT(GPSEL_REG(pin),SHIFTED_VALUE(pin,LOW)) == -1) ?
	        FALSE : TRUE;
}

extern BOOL set_pin_output(uint8_t pin)
{
	if (pin < 0 || pin > 32) return FALSE;
	return (WRITE_BIT(GPSEL_REG(pin),SHIFTED_VALUE(pin,HIGH)) == -1) ?
	        FALSE : TRUE;
}

extern BOOL send_bit(uint8_t pin, uint8_t value)
{
	if (pin < 0 || pin > 32) return FALSE;

	if (value)
		return (WRITE_BIT(GPSET_REG(pin),SHIFTED_VALUE(pin,value)) == -1) ?
		        FALSE : TRUE;
	else
		return (WRITE_BIT(GPCLR_REG(pin),SHIFTED_VALUE(pin,value)) == -1) ?
		        FALSE : TRUE;

}

extern uint8_t receive_bit(uint8_t pin)
{
	if (pin < 0 || pin > 32) return -1;


	uint32_t value = READ_BIT(GPLEV_REG(pin));
	if (value == -1)
	{
		return -1;
	}
	return SHIFTED_VALUE(pin,value);

}

extern BOOL write_data(uint8_t pin, uint32_t value)
{
	if (pin < 0 || pin > 32) return FALSE;
	return (WRITE_BIT(GPSET_REG(pin),value) == -1) ? FALSE : TRUE;
}

extern uint32_t read_data(uint8_t pin)
{
	if (pin < 0 || pin > 32) return -1;
	return READ_BIT(GPLEV_REG(pin));

}
