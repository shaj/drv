#include <linux/module.h>

extern int sys_close(int fd);

static int __init sys_init(void) {
   void* Addr;
   Addr = (void*)sys_close;
   printk(KERN_INFO "sys_close address: %p\n", Addr);
   return -1;
}

module_init(sys_init);
MODULE_LICENSE("GPL v2");

