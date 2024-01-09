#include <linux/module.h>

extern int printk(const char *fmt, ...);

static int __init sys_init(void) {
   void* Addr;
   Addr = (void*)printk;
   printk("printk address: %px\n", Addr);
   return -1;
}

module_init(sys_init);
MODULE_LICENSE("GPL v2");

