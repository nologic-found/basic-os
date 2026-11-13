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
#compile shell.c
bcc -ansi -c -o shell_c.o shell.c
#assemble userlib.asm
as86 userlib.asm -o userlib.o
#link userlib.o
ld86 -o shell -d shell_c.o userlib.o
#compile the loadFile
gcc -o loadFile loadFile.c
#loads kernel on to diskc.img
./loadFile kernel
#loads shell on to diskc.img
./loadFile shell
# loads message.txt onto diskc.img
./loadFile message.txt


# load tstpr1
bcc -ansi -c -o tstpr1.o tstpr1.c
as86 -o userlib.o userlib.asm
ld86 -d -o tstpr1 tstpr1.o userlib.o
./loadFile tstpr1

# load tstpr2
bcc -ansi -c -o tstpr2.o tstpr2.c
as86 -o userlib.o userlib.asm
ld86 -d -o tstpr2 tstpr2.o userlib.o
./loadFile tstpr2

# run the simulator
java -jar simulator.jar
