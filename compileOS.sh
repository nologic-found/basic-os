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
#put kernel on disk at sector3
dd if=kernel of=diskc.img bs=512 conv=notrunc seek=3
#put message into sector 30
dd if=message.txt of=diskc.img bs=512 count=1 seek=30 conv=notrunc
#compile the loadFile
gcc -o loadFile loadFile.c

#compile tstprl.c
bcc -ansi -c -o tstprl.o tstprl.c
as86 -o userlib.o userlib.asm
ld86 -d -o tstprl tstprl.o userlib.o
./loadFile tstprl
