# COS

`COS` is a hobbyist operating system under development for better understanding of computer
architectures and operating systems. (and for fun!)

# How to build and run

In debian-based distributions, you can use `install-deps` script to install dependencies.

Otherwise, please install all the dependencies listed in the `install-deps` script by yourself.

If you meet all the dependencies, you can type `make run` to run the COS using QEMU.

This project is not completed, so I recommend you not to run this on real hardwares. (It will work
without any error, though.)

# Implemented features

 - A bootloader for IA-32e UEFI environment.
 - Support memory segmentation and level-4 paging.
 - Support exceptions of IA-32e architecture.
 - Support interrupts of Intel 8259A interrupt controller.
 - Implemented a keyboard device driver.
 - Implemented a basic graphic library.
 - Implemented a basic shell.

# To-do

 - Implement a RTC driver.
 - Support APIC.
 - Implement a CFS scheduler.
 - Support at least one file system.
 - Implement a sophisticated graphic library.
 - Implement a sophisticated shell.

