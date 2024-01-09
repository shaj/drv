#include <linux/module.h>

static const char* levl[] = {
   KERN_DEBUG,   KERN_INFO, KERN_NOTICE,
   KERN_WARNING, KERN_ERR,  KERN_CRIT,  KERN_ALERT
};

static int __init log_init(void) {
    char out[80];
    int i;
    for(i = 0; i < sizeof(levl) / sizeof(levl[0]); i++) {
	sprintf(out, "message level: %s\n", levl[i] ); 
	printk(out);
    }
    return -1;
}

module_init(log_init);
