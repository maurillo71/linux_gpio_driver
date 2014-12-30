#ifndef GPIO_H_
#define GPIO_H_

/*
====================================
DRIVER's INFO
====================================
*/
#define DRIVER_AUTHOR "23ars <ardeleanasm@gmail.com>"
#define DRIVER_DESC "Just a simple driver for Raspberry B+ GPIOs"
#define DEVICE_NAME "gpio"
#define DEVICE_CLASS_NAME "gpio_class"
#define DEVICE_PROCESS "gpio%d"


/*
====================================
DRIVER's DEBUGGING MACROS
====================================
*/
#define DEBUGGING 1
#ifdef DEBUGGING
#define DEBUGGING_DEBUG KERN_DEBUG
#define DEBUGGING_INFO KERN_INFO
#define DEBUGGING_WARNING KERN_WARNING
#define DEBUGGING_ERROR KERN_ERR

#define driver_dbg(format,arg...)	do { printk( DEBUGGING_DEBUG   format, ## arg ); } while(0)
#define driver_info(format,arg...)	do { printk( DEBUGGING_INFO   format, ## arg ); } while(0)
#define driver_warn(format,arg...)	do { printk( DEBUGGING_WARNING   format, ## arg ); } while(0)
#define driver_err(format,arg...)	do { printk( DEBUGGING_ERROR   format, ## arg ); } while(0)
#else

#define driver_dbg(format,arg...)	
#define driver_info(format,arg...)	
#define driver_warn(format,arg...)	
#define driver_err(format,arg...)	

#endif

/*
====================================
DRIVER's STATUS
====================================
*/
typedef enum
  {
    SUCCESS=0,FAILURE
  }STATUS;
typedef enum
  {
    FALSE=0,TRUE
  }BOOLEAN;

/*
====================================
DRIVER's REGISTER VARIABLES
====================================
*/
typedef struct gpio_dev
{
  struct cdev cdev;
  struct device *p_device;
  struct mutex io_mutex;
  BOOLEAN is_open;
  
}gpio_device;

static gpio_device *p_gpio_dev=NULL;
static dev_t gpio_dev_no;
static struct class *p_gpio_class=NULL;

/*
====================================
DRIVER's IOCTL
====================================
*/
typedef struct _gpio_ioctl_parameters
{
  unsigned address;
  unsigned write_value;
  unsigned read_value;
}gpio_ioctl;


#define _IOCTL_MAGIC 'K'
#define _IOCTL_WRITE _IOW(_IOCTL_MAGIC,1,gpio_ioctl*) 
#define _IOCTL_READ  _IOR(_IOCTL_MAGIC,2,gpio_ioctl*)


/*
====================================
DRIVER's FILE OPERATIONS 
====================================
*/
static int device_open(struct inode *,struct file *);
static int device_release(struct inode *,struct file *);
static long device_ioctl(struct file *file, unsigned int ioctl_num,unsigned long ioctl_param);
struct file_operations fops=
  {
    .open=device_open,
    .release=device_release,
    .unlocked_ioctl=device_ioctl
  };

/*
====================================
DRIVER's FUNCTIONS
====================================
*/
static int __init gpio_init(void);
static void __exit gpio_exit(void);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("device")
#endif
