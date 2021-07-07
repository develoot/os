# COS

`COS` is a hobby operating system being under development for better understanding of computer
architecture and operating systems. (and for fun!)

## How to build and run

If you're using debian-based distro, you can just run `install-deps` script to install all
dependencies.

If you meet all the dependencies, type `make run` to run the operating system under QEMU.

This project is under development, so I don't recommend you to run this on real hardware.
(It will run flawlessly, though.)

## Notes

### Why no multiboot support?

I want to design and write everything including bootloader from scratch.

### POSIX support?

This project is for studying and fun. I don't feel it's necessary.
