**

**GPIO Linux Device Driver and Library for Raspberry Pi**
-----------------------------------------------------

**
-----------------------------------------------------

The GPIO Linux device driver was made with the purpose of learning how to write drivers and also to provide an access to the GPIO pins at high speed.

Also, with the driver, a shared library was implemented to offer easy access to functions that write on or reads from the GPIO pins. The library and the device driver were tested on a Raspberry PI model B+ with Raspbian OS and kernel 3.12.35+.

**

INSTALLING DRIVER AND LIBRARY
-----------------------------

**
If you want to install the driver and or the library I made for using in your projects or for testing it, it's enough to download the sources.  In each folder, **gpio** or **gpio_lib/Release** you can find **Makefiles** for the driver, respectively for the shared library. 

**Installing the driver**

To install the GPIO driver is enough to download the sources and open a terminal in the **gpio** folder. From the terminal run make and the driver will be compiled.  Also, the Makefile provide some options like:

 1. all= compile the driver
 2. insmod= insert the driver in your kernel, load the driver
 3. run= run dmesg command
 4. rmmod= remove the driver from your kernel
 5. clean= remove old objects

**NOTE:** For compiling the driver on your device (PC or Raspberry PI) you'll need to have installed the Linux headers and sources for your kernel.

**Installing the library**

To install the shared library gpio_lib you have to download the sources and to open a terminal in **gpio_lib/Release** folder. Here you'll have to type in terminal **make clean && make** to remove the old objects and to compile the library. 

The next needed step is to copy the shared object to /usr/lib/, step that can be achieved by typing in terminal:
**sudo cp libgpio_library.so /usr/lib/**

**Note:** To use the library in you programs, you have to copy the header **gpio_lib.h** to the folder where you'll have the sources of your program and to include it in your sources. For compiling the programs that use this library, you have to add in command line the flag "-lgpio_library".


----------

23ars:

Hope that what I made will be useful for someone, not only for me. Consider that the GPIO driver is the first real Linux device driver that I created, until now I wrote some that were just for learning and didn't interact with the hardware, didn't  have a real scope.

Feel free to create issues ( [HERE](https://github.com/23ars/linux_gpio_driver/issues) is the address where you can create issues) for improving the quality of the code, products (driver an library) and also to improve the functionality.


