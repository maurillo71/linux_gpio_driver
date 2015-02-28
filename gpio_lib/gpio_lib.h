/** 
\mainpage  Library that can be used in C/C++ for controlling Raspberry Pi B+ pins via Linux Device Driver

// <p>The idea of this project was to offer a fast method to controll the pins from C code via driver, to offer a fast and easy to use shared library.</p>
// <p>Author: Mihai Seba <<b>23ars</b>></p>
// <p>Version: 1.0</p>
// <p>License: GPL</p>
// @see https://github.com/23ars/linux_gpio_driver/wiki
*/
#ifndef GPIO_LIB_H_
#define GPIO_LIB_H_

/// \defgroup datatypes Library defined data types
/// These data types allow you to control/interpret the parameters and return values of functions
/// @{
/// Defines the BOOL type. By convention, FALSE means 0 and TRUE means 1.
typedef enum
  {
    FALSE = 0, 
    TRUE
  } BOOL;

/// Defines the operations accepted by the defined ioctl functions. These types are use internally by the function that control the GPIO driver
typedef enum
  {
    READ_OPERATION = 0, ///< <b>Read operation</b>
    WRITE_OPERATION///< <b>Write operations</b>
  } ACCEPTED_OPERATIONS;

/// Defines the BCM2835 supported modes.
typedef enum
  {
    INPUT_MODE=0x000,///< <b>GPIO input mode</b>
    OUTPUT_MODE=0x001,///< <b>GPIO output mode</b>
    ALT_FUNCTION_0=0x100,///< <b>alternate function 0</b>
    ALT_FUNCTION_1=0x101,///< <b>alternate function 1</b>
    ALT_FUNCTION_2=0x110,///< <b>alternate function 2</b>
    ALT_FUNCTION_3=0x111,///< <b>alternate function 3</b>
    ALT_FUNCTION_4=0x011,///< <b>alternate function 4</b>
    ALT_FUNCTION_5=0x010///< <b>alternate function 5</b>
  }PIN_MODE;

/// Define the logical HIGH value, 1
#define HIGH 0x1///< HIGH Value

/// Define the logical LOW value, 0
#define LOW 0x0///< LOW Value

/// Defines the DEVICE_PINS type, type that map the available pins on Raspberry Pi B+
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



/// @} //end of datatypes

typedef struct _gpio_ioctl_parameters
{
  unsigned address;
  unsigned write_value;
  unsigned read_value;
} gpio_ioctl;



#define _IOCTL_MAGIC 'K'
#define _IOCTL_WRITE _IOW(_IOCTL_MAGIC,1,gpio_ioctl*)
#define _IOCTL_READ  _IOR(_IOCTL_MAGIC,2,gpio_ioctl*)



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

#define TIMER_LO 0x00003004
#define TIMER_HI 0x00003008

int8_t device_status;

BOOL debugging=FALSE;

int32_t control_device(ACCEPTED_OPERATIONS, uint32_t, uint32_t);

uint32_t timer(void);

#define GPSET_REG(P)				(GPSET0+((GPSET1-GPSET0)*((P)/32)))

#define GPLEV_REG(P)				(GPLEV0+((GPLEV1-GPLEV0)*((P)/32)))

#define GPCLR_REG(P)				(GPCLR0+((GPCLR1-GPCLR0)*((P)/32)))

#define GPSEL_REG(P)	                        (GPFSEL0 + ((GPFSEL1 - GPFSEL0) * ((P) / 10)))

#define GPAFEN_REG(P)                           (GPAFEN0+((GPAFEN1-GPAFEN0)*((P)/32)))

#define GPAREN_REG(P)                           (GPAREN0+((GPAREN1-GPAREN0)*((P)/32)))

#define GPEDS_REG(P)                            (GPEDS0+((GPEDS1-GPEDS0)*((P)/32)))

#define GPREN_REG(P)                            (GPREN0+((GPREN1-GPREN0)*((P)/32)))

#define GPFEN_REG(P)                            (GPFEN0+((GPFEN1-GPFEN0)*((P)/32)))

#define GPHEN_REG(P)                            (GPHEN0+((GPHEN1-GPHEN0)*((P)/32)))

#define GPLEN_REG(P)                            (GPLEN0+((GPLEN1-GPLEN0)*((P)/32)))

#define GPPUDCLK_REG(P)                         (GPPUDCLK0+((GPPUDCLK1-GPPUDCLK0)*((P)/32)))

#define SHIFTED_VALUE(P,V) 	((V)?(1<<((P)%32)):0)
#define SHIFTED_SELECT_VALUE(P,V) ((V)<<(((P)%10)*3))

#define READ_BIT(A)		control_device(READ_OPERATION,(GPIO_BASE | (A)),0)
#define	WRITE_BIT(A,V)	        control_device(WRITE_OPERATION,(GPIO_BASE| (A)),V)



#ifdef __cplusplus
extern "C"
{
#endif
  /// \defgroup init Device Control functions
  /// These functions are used to control the GPIO device driver and the debugging mode of the library.
  /// @{
 
  /// Enable debugging mode of the library. In this mode values are not written on the peripherals. By default debugging mode is disabled.
  /// \param[in] state The state of the debugging mode, enable/disabled. Value is of type BOOL, TRUE or FALSE, meaning enabled, respectively disabled.
  extern void enable_debugging(BOOL state);

  /// Device initialization function. This function must be called before any operations on GPIO pins.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL init_device();

  ///Device closing function. This function must be called after all operations on GPIO pins are done.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL close_device();

  /// @} //end of init

  
  /// \defgroup gpiocontrol GPIO Control
  /// Functions from this group are used to control the pins, by writing different values to the BCM2835 registers.
  /// @{
 
  /// This function is used to set the pin mode, output,input or alternate function.
  /// param[in] pin Represents the pin choosed for a specific operation.
  /// param[in] mode Represents the mode choosed for pin.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL set_pin_mode(DEVICE_PINS pin,PIN_MODE mode);

  /// Function used to send a bit on the choosed pin.
  /// \param[in] pin Represent the mapped pin.
  /// \param[in] value Represent the value that will be set, 0 or 1.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL send_bit(DEVICE_PINS pin, uint8_t value);

  /// Function used to read a bit from the choosed pin.
  /// param[in] pin Represents the pin choosed for a specific operation.
  /// \return read value
  extern uint8_t receive_bit(DEVICE_PINS pin);

  /// Delay function. Used to delay a specified number of milliseconds.
  /// param[in] millis Number of millis to sleep.
  extern void delay(uint16_t millis);

  /// Udelay function. Used to delay a specified number of microseconds.
  /// param[in] micros Number of microseconds to sleep.
  extern void udelay(uint32_t micros);

  /// Function used to set-up rising edge detection.
  /// \param[in] pin Represent the mapped pin.
  /// \param[in] value Represent the value that will be set, 0 or 1.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL rising_edge_detect(DEVICE_PINS pin, uint8_t value);

  /// Function used to set-up falling edge detection.
  /// \param[in] pin Represent the mapped pin.
  /// \param[in] value Represent the value that will be set, 0 or 1.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL falling_edge_detect(DEVICE_PINS pin,uint8_t value);

  /// Function used to set-up high detection.
  /// \param[in] pin Represent the mapped pin.
  /// \param[in] value Represent the value that will be set, 0 or 1.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL high_detect(DEVICE_PINS pin,uint8_t value);

  /// Function used to set-up low detection.
  /// \param[in] pin Represent the mapped pin.
  /// \param[in] value Represent the value that will be set, 0 or 1.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL low_detect(DEVICE_PINS pin,uint8_t value);

  /// Function used to set-up asynchronous falling edge detection.
  /// \param[in] pin Represent the mapped pin.
  /// \param[in] value Represent the value that will be set, 0 or 1.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL async_falling_edge_detect(DEVICE_PINS pin,uint8_t value);

  /// Function used to set-up asynchronous rising edge detection.
  /// \param[in] pin Represent the mapped pin.
  /// \param[in] value Represent the value that will be set, 0 or 1.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL async_rising_edge_detect(DEVICE_PINS pin,uint8_t value);

  /// Function used to set-up event detection
  /// \param[in] pin Represent the mapped pin.
  /// \return HIGH or LOW
  extern uint8_t event_detect_status(DEVICE_PINS pin);

  ///Function used to enable pull up/down
  /// \param[in] pin Represent the mapped pin.
  /// \param[in] value Represent the value that will be set, 0 or 1.
  /// \return TRUE if successful, FALSE otherwise. 
  extern BOOL pull_ud_clk(DEVICE_PINS pin,uint8_t value);
  
  /// @} //end of gpiocontrol
#ifdef __cplusplus
}
#endif
#endif /* GPIO_LIB_H_ */
