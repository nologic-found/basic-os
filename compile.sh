#!/bin/sh
#Group: Kait Tucker, Cecilia Rosado, Bradley Chaurand

#make an empty disk
dd if=/dev/zero of=diskc.img bs=512 count=1000
#compile the bootloader
nasm bootload.asm
#put the booatloader at sector 0
dd if=bootload of=diskc.img bs=512 count=1 conv=notrunc
#compile the c part of the kernel
bcc -ansi -c -o kernel_c.o kernel.c
#compile the assembly part of the kernel
as86 kernel.asm -o  kernel_asm.o
#link them together into one kernel
ld86 -o kernel -d kernel_c.o kernel_asm.o

#compile the loadFile
gcc -o loadFile loadFile.c

#runs loadFile with kernel as the arg
./loadFile kernel

# load message.txt
./loadFile message.txt



bcc -ansi -c -o tstpr1.o tstpr1.c
as86 -o userlib.o userlib.asm
ld86 -d tstpr1 tstpr1.o userlib.o
./loadFile tstpr1

# run the simulator
java -jar simulator.jar
