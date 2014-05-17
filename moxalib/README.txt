README of Sample Code for Linux

1. build

The available architectures to build for variable ARCH as follows.

uc74xx da66x ia24x uc711x uc84xx x86

To build all sample applications, type

make ARCH=<arch>

where <arch> is one of values of variable ARCH.

Note: some of sample applications were written for Linux only and some of them
were written for specific Linux product models only. For example, LCM sample
applications can only run under DA-66X-LX and UC-7410/7420-LX series.

2. clean

To clean executables and object files of all sample applications, type

make clean
