#include <linux/module.h>
MODULE_LICENSE("GPL");
static int __init init(void);
module_init(init);

extern char* test_01(void); 
extern char* test_02(void); 
extern char* test_03(void); 

typedef char* (*utest)(void);
utest tests[] = { test_01, test_02, test_03 };
