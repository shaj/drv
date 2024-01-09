#!/bin/bash

sudo uname -a
inxi -SCxxx

cat /lib/modules/`uname -r`/build/.config | grep "CONFIG_X86_64=\|CONFIG_64BIT=\|CONFIG_X86_32=\|CONFIG_64BIT="
cat /boot/config-`uname -r` | grep "CONFIG_X86_64=\|CONFIG_64BIT=\|CONFIG_X86_32=\|CONFIG_64BIT="

cat /lib/modules/`uname -r`/build/.config | grep CONFIG_ARM=
cat /boot/config-`uname -r` | grep CONFIG_ARM=

cat /lib/modules/`uname -r`/build/.config | grep CONFIG_KALLSYMS
#sudo cat /proc/kallsyms | grep ' T ' | grep kallsyms_on_each_symbol
sudo grep ' T ' /proc/kallsyms | grep kallsyms_on_each_symbol
#sudo cat /proc/kallsyms | grep ' T ' | grep kallsyms_lookup
sudo grep ' T ' /proc/kallsyms | grep kallsyms_lookup 

sudo grep 'sys_call_table' /proc/kallsyms 

cat /lib/modules/`uname -r`/build/Module.symvers | grep 'printk' | head -n3
cat /lib/modules/`uname -r`/build/Module.symvers | grep kallsyms_lookup | head -n3
cat /lib/modules/`uname -r`/build/Module.symvers | grep kallsyms_on_each_symbol | head -n3
