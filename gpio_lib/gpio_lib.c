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
#include <time.h>
#include "gpio_lib.h"

extern void enable_debugging(BOOL state)
{
  if(state==debugging)
    return;
  else
    debugging=state;
}

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
    default:fprintf(stderr,"Invalid operation %d",operation);
      break;
    }
  return io.read_value;
  
}

extern BOOL set_pin_mode(DEVICE_PINS pin,PIN_MODE mode)
{
  if(debugging==TRUE)
    {
      printf("Setting mode for pin at address 0x%08X with value:0x%08X...\n",GPSEL_REG(pin),SHIFTED_SELECT_VALUE(pin,mode));
      return TRUE;
    }
  
  return (WRITE_BIT(GPSEL_REG(pin),SHIFTED_SELECT_VALUE(pin,mode)) == -1) ?FALSE : TRUE;
  
}

extern BOOL send_bit(DEVICE_PINS pin, uint8_t value)
{
  if(debugging==TRUE)
    {  
      if(value)
	printf("Sending bit 0x%08X on pin at address:0x%08X...\n",SHIFTED_VALUE(pin,value),GPSET_REG(pin));
      else
	printf("Sending bit 0x%08X on pin at address:0x%08X...\n",SHIFTED_VALUE(pin,value),GPSET_REG(pin));
      return 0;
    }
  
  if (value)
    return (WRITE_BIT(GPSET_REG(pin),SHIFTED_VALUE(pin,value)) == -1) ?FALSE : TRUE;
  else
    return (WRITE_BIT(GPCLR_REG(pin),SHIFTED_VALUE(pin,value)) == -1) ?FALSE : TRUE;
  
}

extern uint8_t receive_bit(DEVICE_PINS pin)
{
  if(debugging==TRUE)
    {
      printf("Receiving bit from pin at address:0x%08X...\n",GPLEV_REG(pin));
      return 0;
    }
  
  uint32_t value = READ_BIT(GPLEV_REG(pin));
  if (value == -1)
    {
      return -1;
    }
  return SHIFTED_VALUE(pin,value);
  
}

extern void delay(uint16_t millis)
{
  struct timespec delay;
  delay.tv_sec=(time_t)(millis/1000.0);
  delay.tv_nsec=(uint32_t)(millis*1000000);
  nanosleep(&delay,NULL);
}


extern uint32_t timer(void)
{
  volatile uint32_t high,low;
  uint32_t st; 
  high=READ_BIT(TIMER_HI);
  low=READ_BIT(TIMER_LO);
  st=READ_BIT(TIMER_HI);
  st<<=32;
  if(st==high)
    {
      st+=low;
    }
  else
    {
      st+=READ_BIT(TIMER_LO);
    }
  return st;
}

extern void udelay(uint32_t micros)
{
  uint32_t offset;
  offset=timer();
  while(timer()<(offset+micros));

}

extern uint8_t event_detect_status(DEVICE_PINS pin)
{
  uint8_t value=READ_BIT(GPEDS_REG(pin));
  return SHIFTED_VALUE(pin,value);
}

extern BOOL async_falling_edge_detect(DEVICE_PINS pin,uint8_t value)
{
  return (WRITE_BIT(GPAFEN_REG(pin),SHIFTED_VALUE(pin,value)) == -1) ?FALSE : TRUE;
}

extern BOOL async_rising_edge_detect(DEVICE_PINS pin,uint8_t value)
{
  return (WRITE_BIT(GPAREN_REG(pin),SHIFTED_VALUE(pin,value))==-1)?FALSE:TRUE;
}



extern BOOL rising_edge_detect(DEVICE_PINS pin, uint8_t value)
{
  return (WRITE_BIT(GPREN_REG(pin),SHIFTED_VALUE(pin,value))==-1)?FALSE:TRUE;
}

extern BOOL falling_edge_detect(DEVICE_PINS pin,uint8_t value)
{
  return (WRITE_BIT(GPFEN_REG(pin),SHIFTED_VALUE(pin,value))==-1)?FALSE:TRUE;
}

extern BOOL high_detect(DEVICE_PINS pin,uint8_t value)
{
  return (WRITE_BIT(GPHEN_REG(pin),SHIFTED_VALUE(pin,value))==-1)?FALSE:TRUE;
}

extern BOOL low_detect(DEVICE_PINS pin,uint8_t value)
{
  return (WRITE_BIT(GPLEN_REG(pin),SHIFTED_VALUE(pin,value))==-1)?FALSE:TRUE;
}




extern BOOL pull_ud_clk(DEVICE_PINS pin,uint8_t value)
{
  return (WRITE_BIT(GPPUDCLK_REG(pin),SHIFTED_VALUE(pin,value))==-1)?FALSE:TRUE;
}
