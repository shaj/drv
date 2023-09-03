

#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Andrey Shaj <shaj@test.io>");

static int __init hello_init(void)
{
        printk("Hello world!\n");
        return 0;
}

static void __exit hello_exigt(void)
{
        printk("Goodbye world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

