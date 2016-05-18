#! /bin/bash

sudo losetup -o 2097152 /dev/loop0 hdd.raw
sudo mount /dev/loop0 hdd_dir
