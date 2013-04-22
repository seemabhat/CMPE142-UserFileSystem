#!bin/sh
sudo umount $HOME/mntcmpe142
rv=$?
if [ $rv -ne 0 ]; then exit $rv ;fi
sudo rmmod cmpe142_fs
rv=$?
if [ $rv -ne 0 ]; then exit $rv ;fi
