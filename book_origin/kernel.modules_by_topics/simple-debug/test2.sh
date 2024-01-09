sudo insmod umd1.ko
sudo insmod umt1.ko
dmesg | grep 'test_' | tail -n3
sudo rmmod umd1 
