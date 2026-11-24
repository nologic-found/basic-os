#!/bin/bash

# make an empty disk 512000 bytes
dd if=/dev/zero of=diskc.img bs=512 count=1000

# compile bootloader
nasm bootload.asm

# copy bootloader to start of disk (sector 0)
dd if=bootload of=diskc.img bs=512 conv=notrunc seek=0

# compile the C and assembly parts of the kernel
bcc -ansi -c -o kernel_c.o kernel.c
as86 -o kernel_asm.o kernel.asm
ld86 -d -o kernel kernel_c.o kernel_asm.o

# assemble userlib
as86 -o userlib.o userlib.asm

# compile shell
bcc -ansi -c -o shell.o shell.c
ld86 -d -o shell shell.o userlib.o

# compile tstpr1 and tstpr2
bcc -ansi -c -o tstpr1.o tstpr1.c
ld86 -d -o tstpr1 tstpr1.o userlib.o

bcc -ansi -c -o tstpr2.o tstpr2.c
ld86 -d -o tstpr2 tstpr2.o userlib.o

# compile loadFile utility
gcc -o loadFile loadFile.c

# load files onto disk image (kernel MUST be loaded first at sector 3)
./loadFile kernel
./loadFile shell
./loadFile tstpr1
./loadFile tstpr2
./loadFile message.txt

# for Brad's laptop
# java -Dsun.java2d.opengl=true -jar simulator.jar
