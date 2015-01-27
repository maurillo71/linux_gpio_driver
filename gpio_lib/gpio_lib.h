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
    FALSE = 0, 
    TRUE
  } BOOL;

typedef enum
  {
    READ_OPERATION = 0, 
    WRITE_OPERATION
  } ACCEPTED_OPERATIONS;

typedef enum
  {
    INPUT_MODE=0x00,
    OUTPUT_MODE=0x01
  }PIN_MODE;

typedef enum
  {
    GPIO2=3,
    GPIO3=5,
    GPIO4=7,
    GPIO14=8,
    GPIO15=10,
    GPIO17=11,
    GPIO18=12,
    GPIO27=13,
    GPIO22=15,
    GPIO23=16,
    GPIO24=18,
    GPIO10=19,
    GPIO9=21,
    GPIO25=22,
    GPIO11=23,
    GPIO8=24,
    GPIO7=26,
    GPIO5=29,
    GPIO6=31,
    GPIO12=32,
    GPIO13=33,
    GPIO19=35,
    GPIO16=36,
    GPIO26=37,
    GPIO20=38,
    GPIO21=40,
    ID_SD=27,
    ID_SC=28
  }DEVICE_PINS;


typedef struct _gpio_ioctl_parameters
{
  unsigned address;
  unsigned write_value;
  unsigned read_value;
} gpio_ioctl;



#define _IOCTL_MAGIC 'K'
#define _IOCTL_WRITE _IOW(_IOCTL_MAGIC,1,gpio_ioctl*)
#define _IOCTL_READ  _IOR(_IOCTL_MAGIC,2,gpio_ioctl*)

#define HIGH 0x1
#define LOW 0x0

#define KERNEL_ADDRESS               0xF2000000
#define GPIO_BASE               (KERNEL_ADDRESS + 0x200000)

#define GPFSEL0                      0x0000 ///< GPIO Function Select 0
#define GPFSEL1                      0x0004 ///< GPIO Function Select 1
#define GPFSEL2                      0x0008 ///< GPIO Function Select 2
#define GPFSEL3                      0x000c ///< GPIO Function Select 3
#define GPFSEL4                      0x0010 ///< GPIO Function Select 4
#define GPFSEL5                      0x0014 ///< GPIO Function Select 5
#define GPSET0                       0x001c ///< GPIO Pin Output Set 0
#define GPSET1                       0x0020 ///< GPIO Pin Output Set 1
#define GPCLR0                       0x0028 ///< GPIO Pin Output Clear 0
#define GPCLR1                       0x002c ///< GPIO Pin Output Clear 1
#define GPLEV0                       0x0034 ///< GPIO Pin Level 0
#define GPLEV1                       0x0038 ///< GPIO Pin Level 1
#define GPEDS0                       0x0040 ///< GPIO Pin Event Detect Status 0
#define GPEDS1                       0x0044 ///< GPIO Pin Event Detect Status 1
#define GPREN0                       0x004c ///< GPIO Pin Rising Edge Detect Enable 0
#define GPREN1                       0x0050 ///< GPIO Pin Rising Edge Detect Enable 1
#define GPFEN0                       0x0058 ///< GPIO Pin Falling Edge Detect Enable 0
#define GPFEN1                       0x005c ///< GPIO Pin Falling Edge Detect Enable 1
#define GPHEN0                       0x0064 ///< GPIO Pin High Detect Enable 0
#define GPHEN1                       0x0068 ///< GPIO Pin High Detect Enable 1
#define GPLEN0                       0x0070 ///< GPIO Pin Low Detect Enable 0
#define GPLEN1                       0x0074 ///< GPIO Pin Low Detect Enable 1
#define GPAREN0                      0x007c ///< GPIO Pin Async. Rising Edge Detect 0
#define GPAREN1                      0x0080 ///< GPIO Pin Async. Rising Edge Detect 1
#define GPAFEN0                      0x0088 ///< GPIO Pin Async. Falling Edge Detect 0
#define GPAFEN1                      0x008c ///< GPIO Pin Async. Falling Edge Detect 1
#define GPPUD                        0x0094 ///< GPIO Pin Pull-up/down Enable
#define GPPUDCLK0                    0x0098 ///< GPIO Pin Pull-up/down Enable Clock 0
#define GPPUDCLK1                    0x009c ///< GPIO Pin Pull-up/down Enable Clock 1

int8_t device_status;

BOOL debugging=FALSE;

int32_t control_device(ACCEPTED_OPERATIONS, uint32_t, uint32_t);

#define GPSET_REG(P)				(GPSET0+((GPSET1-GPSET0)*((P)/32)))
#define GPCLR_REG(P)				(GPCLR0+((GPCLR1-GPCLR0)*((P)/32)))
#define GPSEL_REG(P)	                        (GPFSEL0 + ((GPFSEL1 - GPFSEL0) * ((P) / 10)))
#define SHIFTED_VALUE(P,V) 	((V)?(1<<((P)%32)):0)
#define SHIFTED_SELECT_VALUE(P,V) ((V)<<(((P)%10)*3))

#define READ_BIT(A)		control_device(READ_OPERATION,(GPIO_BASE | (A)),0)
#define	WRITE_BIT(A,V)	control_device(WRITE_OPERATION,(GPIO_BASE| (A)),V)

#define GPLEV_REG(P)				(GPLEV0+((GPLEV1-GPLEV0)*((P)/32)))

#ifdef __cplusplus
extern "C"
{
#endif
  extern void enable_debugging(BOOL state);
  extern BOOL init_device();
  
  extern BOOL close_device();
  
  extern BOOL set_pin_mode(DEVICE_PINS pin,PIN_MODE mode);
  
  extern BOOL send_bit(DEVICE_PINS pin, uint8_t value);
  
  extern uint8_t receive_bit(DEVICE_PINS pin);

  extern BOOL write_data(DEVICE_PINS pin, uint32_t value);//will accept function as parameter, function that will map data to binary value
  
  extern uint32_t read_data(DEVICE_PINS pin);//will accept function as parameter, function that will map binary to data


  //READ & WRITE ON ADDRESS OF THE PERIFERIC
  extern uint32_t periferic_read(volatile uint32_t* address,BOOL barrier);

  extern void periferic_write(volatile uint32_t *address, uint32_t value,BOOL barrier);

  extern void periferic_set_bits(volatile uint32_t *address, uint32_t value,uint32_t mask);




  
#ifdef __cplusplus
}
#endif
#endif /* GPIO_LIB_H_ */
