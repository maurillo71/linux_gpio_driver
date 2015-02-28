#!/bin/bash
echo "Copying header file to ~/usr/include"
sudo cp gpio_lib.h ~/usr/include
cd Release
echo "Compiling library"
make
echo "Copying library to ~/usr/lib"
sudo cp libgpio_library.so ~/usr/lib
echo "Done"

