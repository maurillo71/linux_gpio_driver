/*
 * gpio_lib.h
 *
 *  Created on: Dec 30, 2014
 *      Author: mihai
 */

#ifndef GPIO_LIB_H_
#define GPIO_LIB_H_

typedef enum
{
	FALSE = 0, TRUE

} BOOL;

typedef enum
{
	READ_OPERATION=0,WRITE_OPERATION
}ACCEPTED_OPERATIONS;

typedef struct _gpio_ioctl_parameters
{
  unsigned address;
  unsigned write_value;
  unsigned read_value;
}gpio_ioctl;



#define _IOCTL_MAGIC 'K'
#define _IOCTL_WRITE _IOW(_IOCTL_MAGIC,1,gpio_ioctl*)
#define _IOCTL_READ  _IOR(_IOCTL_MAGIC,2,gpio_ioctl*)



#define HIGH 0x1
#define LOW 0x0


#define BCM2835_PERI_BASE               0x20000000
#define BCM2835_GPIO_BASE               (BCM2835_PERI_BASE + 0x200000)


#define BCM2835_GPFSEL0                      0x0000 ///< GPIO Function Select 0
#define BCM2835_GPFSEL1                      0x0004 ///< GPIO Function Select 1
#define BCM2835_GPFSEL2                      0x0008 ///< GPIO Function Select 2
#define BCM2835_GPFSEL3                      0x000c ///< GPIO Function Select 3
#define BCM2835_GPFSEL4                      0x0010 ///< GPIO Function Select 4
#define BCM2835_GPFSEL5                      0x0014 ///< GPIO Function Select 5
#define BCM2835_GPSET0                       0x001c ///< GPIO Pin Output Set 0
#define BCM2835_GPSET1                       0x0020 ///< GPIO Pin Output Set 1
#define BCM2835_GPCLR0                       0x0028 ///< GPIO Pin Output Clear 0
#define BCM2835_GPCLR1                       0x002c ///< GPIO Pin Output Clear 1
#define BCM2835_GPLEV0                       0x0034 ///< GPIO Pin Level 0
#define BCM2835_GPLEV1                       0x0038 ///< GPIO Pin Level 1
#define BCM2835_GPEDS0                       0x0040 ///< GPIO Pin Event Detect Status 0
#define BCM2835_GPEDS1                       0x0044 ///< GPIO Pin Event Detect Status 1
#define BCM2835_GPREN0                       0x004c ///< GPIO Pin Rising Edge Detect Enable 0
#define BCM2835_GPREN1                       0x0050 ///< GPIO Pin Rising Edge Detect Enable 1
#define BCM2835_GPFEN0                       0x0058 ///< GPIO Pin Falling Edge Detect Enable 0
#define BCM2835_GPFEN1                       0x005c ///< GPIO Pin Falling Edge Detect Enable 1
#define BCM2835_GPHEN0                       0x0064 ///< GPIO Pin High Detect Enable 0
#define BCM2835_GPHEN1                       0x0068 ///< GPIO Pin High Detect Enable 1
#define BCM2835_GPLEN0                       0x0070 ///< GPIO Pin Low Detect Enable 0
#define BCM2835_GPLEN1                       0x0074 ///< GPIO Pin Low Detect Enable 1
#define BCM2835_GPAREN0                      0x007c ///< GPIO Pin Async. Rising Edge Detect 0
#define BCM2835_GPAREN1                      0x0080 ///< GPIO Pin Async. Rising Edge Detect 1
#define BCM2835_GPAFEN0                      0x0088 ///< GPIO Pin Async. Falling Edge Detect 0
#define BCM2835_GPAFEN1                      0x008c ///< GPIO Pin Async. Falling Edge Detect 1
#define BCM2835_GPPUD                        0x0094 ///< GPIO Pin Pull-up/down Enable
#define BCM2835_GPPUDCLK0                    0x0098 ///< GPIO Pin Pull-up/down Enable Clock 0
#define BCM2835_GPPUDCLK1                    0x009c ///< GPIO Pin Pull-up/down Enable Clock 1

int8_t device_status;

volatile uint32_t *bcm2835_gpio=NULL;

int32_t control_device(ACCEPTED_OPERATIONS,uint32_t,uint32_t);


#ifdef __cplusplus
extern "C" {
#endif

  extern BOOL init_device();

  extern BOOL close_device();

  extern BOOL set_pin(uint8_t pin,uint8_t value);

  extern uint8_t get_pin(uint8_t pin);

  extern BOOL write_data(uint8_t pin,uint32_t value);

  extern uint32_t read_data(uint8_t pin);

#ifdef __cplusplus
}
#endif
#endif /* GPIO_LIB_H_ */
