#!/bin/sh

#sudo rmmod -f lab4_driver
#sudo insmod lab4_driver.ko

sudo ./writer bobo &
sudo ./reader 192.168.0.20 1234 /dev/lab4_driver
