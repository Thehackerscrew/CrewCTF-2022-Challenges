#!/bin/sh

cd src
if [ $# -gt 0 ]
then
    make
    cp chall.ko ../rootfs/
    #cp exp ../rootfs
# else
#     gcc exp.c -o exp --static
#     cp exp ../rootfs
fi
cd ..

if [ $# -gt 0 ]
then
    cd src
    make clean
    cd ..
fi

cd rootfs
find . | cpio -o -H newc > ../rootfs.cpio
cd ..
