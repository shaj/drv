#include <linux/module.h>

extern int __x64_sys_open(int fd);

static int __init sys_init(void) {
   void* Addr;
   Addr = (void*)__x64_sys_open;
   printk(KERN_INFO "sys_open address: %px\n", Addr);
   return -1;
}

module_init(sys_init);
MODULE_LICENSE("GPL v2");
