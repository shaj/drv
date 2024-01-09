#include <linux/module.h>
MODULE_LICENSE("GPL");

static int __init do_init(void) {
   void* Addr = (void*)vprintk;
   printk("? интересующий нас адрес: %px\n", Addr);
   return -1;
}

module_init(do_init);
