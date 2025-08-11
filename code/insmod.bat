adb shell su -c rm -rf /data/local/tmp/510.ko
adb push 510.ko /data/local/tmp
adb shell su -c chmod 777 /data/local/tmp/510.ko
adb shell su -c insmod /data/local/tmp/510.ko
pause