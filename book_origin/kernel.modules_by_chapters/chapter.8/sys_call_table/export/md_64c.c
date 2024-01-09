#include <linux/module.h>

extern int __x64_sys_close(int fd);

static int __init sys_init(void) {
   void* Addr;
   Addr = (void*)__x64_sys_close;
   printk("sys_close address: %px\n", Addr);
   return -1;
}

module_init(sys_init);
MODULE_LICENSE("GPL v2");

