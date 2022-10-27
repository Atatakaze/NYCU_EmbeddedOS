#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer Ding &
./reader 192.168.0.58 8080 /dev/mydev
