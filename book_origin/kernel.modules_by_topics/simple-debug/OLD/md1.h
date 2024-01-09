#include <linux/module.h>

MODULE_LICENSE("GPL");

extern char* test_01(void);
extern char* test_02(void); 

static int __init init(void);
module_init(init);


