#!/bin/bash

# make an empty disk 512000 bytes
dd if=/dev/zero of=diskc.img bs=512 count=1000

# compile bootloader
nasm bootload.asm

# copy bootloader to start of disk. Sector 0
dd if=bootload of=diskc.img bs=512 conv=notrunc seek=0

# Compile the c part of the kernel
bcc -ansi -c -o kernel_c.o kernel.c

# Assembling the assembly part of the kernel
as86 -o kernel_asm.o kernel_asm.asm

# Linking them tohether into one kernel
ld86 -d -o kernel kernel_c.o kernel_asm.o

# Put kernel in sector 3 of the disk
dd if=kernel of=diskc.img bs=512 conv=notrunc seek=3

