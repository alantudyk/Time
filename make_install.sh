#!/bin/sh

sudo apt update &&
sudo apt install -y libgtk-3-dev &&
cc `pkg-config --cflags gtk+-3.0` -o Time Time.c `pkg-config --libs gtk+-3.0` &&
sudo mv Time /bin/Time
