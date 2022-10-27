#!/bin/bash

for var in $@
do
  case $var in
    0)
      echo "0"
      echo 0 > /sys/class/gpio/gpio21/value
      echo 0 > /sys/class/gpio/gpio20/value
      echo 0 > /sys/class/gpio/gpio16/value
      echo 0 > /sys/class/gpio/gpio12/value
      ;;
    1)
      echo "1"
      echo 0 > /sys/class/gpio/gpio21/value
      echo 0 > /sys/class/gpio/gpio20/value
      echo 0 > /sys/class/gpio/gpio16/value
      echo 1 > /sys/class/gpio/gpio12/value
      ;;
    2)
      echo "2"
      echo 0 > /sys/class/gpio/gpio21/value
      echo 0 > /sys/class/gpio/gpio20/value
      echo 1 > /sys/class/gpio/gpio16/value
      echo 0 > /sys/class/gpio/gpio12/value
      ;;
    3)
      echo "3"
      echo 0 > /sys/class/gpio/gpio21/value
      echo 0 > /sys/class/gpio/gpio20/value
      echo 1 > /sys/class/gpio/gpio16/value
      echo 1 > /sys/class/gpio/gpio12/value
      ;;
    4)
      echo "4"
      echo 0 > /sys/class/gpio/gpio21/value
      echo 1 > /sys/class/gpio/gpio20/value
      echo 0 > /sys/class/gpio/gpio16/value
      echo 0 > /sys/class/gpio/gpio12/value
      ;;
    5)
      echo "5"
      echo 0 > /sys/class/gpio/gpio21/value
      echo 1 > /sys/class/gpio/gpio20/value
      echo 0 > /sys/class/gpio/gpio16/value
      echo 1 > /sys/class/gpio/gpio12/value
      ;;
    6)
      echo "6"
      echo 0 > /sys/class/gpio/gpio21/value
      echo 1 > /sys/class/gpio/gpio20/value
      echo 1 > /sys/class/gpio/gpio16/value
      echo 0 > /sys/class/gpio/gpio12/value
      ;;
    7)
      echo "7"
      echo 0 > /sys/class/gpio/gpio21/value
      echo 1 > /sys/class/gpio/gpio20/value
      echo 1 > /sys/class/gpio/gpio16/value
      echo 1 > /sys/class/gpio/gpio12/value
      ;;
    8)
      echo "8"
      echo 1 > /sys/class/gpio/gpio21/value
      echo 0 > /sys/class/gpio/gpio20/value
      echo 0 > /sys/class/gpio/gpio16/value
      echo 0 > /sys/class/gpio/gpio12/value
      ;;
    9)
      echo "9"
      echo 1 > /sys/class/gpio/gpio21/value
      echo 0 > /sys/class/gpio/gpio20/value
      echo 0 > /sys/class/gpio/gpio16/value
      echo 1 > /sys/class/gpio/gpio12/value
      ;;
    *)
      echo "Error"
  esac

  sleep 1

done
