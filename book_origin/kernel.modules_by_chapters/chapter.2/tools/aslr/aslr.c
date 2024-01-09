#include <linux/module.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Oleg Tsiliuric <olej.tsil@gmail.com>>");

static int __init my_init(void) {
	uint16_t d = 1234;
	void show(void* p) {
		printk("%20p%20px%20pK%20lx\n", 
		       p, p, p, (uintptr_t)p);
	}
	if (IS_ENABLED(CONFIG_64BIT)) {
		printk("64-bit");
	}
	else {
		printk("32-bit");
	}
	printk("%20s%20s%20s%20s\n", "%p", "%px", "%pK", "int");
	show(&d);
#if defined(CONFIG_64BIT)
	show(printk);
#else
	show(_printk);
#endif
	show((void*)PAGE_OFFSET);
	show((void*)current);
	show((void*)0xffff800000000000);
	return -2;
}

module_init(my_init);
