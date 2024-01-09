#include <linux/module.h>
static int __init init(void) {

#ifdef __i386__
   printk("разрядность системы 32\n");
#else
   printk("разрядность системы 64\n");
#endif
   printk("размер страницы RAM = %lu [%luKb]\n", PAGE_SIZE, PAGE_SIZE / 1024);
   return -1;
}

MODULE_LICENSE("GPL");
module_init(init);
