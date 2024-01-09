#include <linux/module.h>

static int __init config_runtime_init(void) {
	printk("CONFIG_SMP %sопределено\n", 
	       IS_ENABLED(CONFIG_SMP) ? "" : "не ");

	printk("CONFIG_64BIT %sопределено\n", 
	       IS_ENABLED(CONFIG_64BIT) ? "" : "не ");

	if (IS_ENABLED(CONFIG_NET_ACT_BPF))
		printk("CONFIG_NET_ACT_BPF определено\n");
	if (IS_ENABLED(CONFIG_NET_ACT_BPF_MODULE))
		printk("CONFIG_NET_ACT_BPF_MODULE определено\n");
	if (!IS_ENABLED(CONFIG_NET_ACT_BPF))
		printk("CONFIG_NET_ACT_BPF* не определено\n");

	return -1;
}

module_init(config_runtime_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <olej.tsil@gmail.com>");
