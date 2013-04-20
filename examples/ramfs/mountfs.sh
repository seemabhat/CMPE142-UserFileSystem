#!bin/sh
sudo insmod ./cmpe142_fs.ko
rv=$?
if [ $rv -ne 0 ]; then exit $rv ;fi
sudo mount -t cmpe142 none $HOME/mntcmpe142
rv=$?
if [ $rv -ne 0 ]; then exit $rv ;fi
