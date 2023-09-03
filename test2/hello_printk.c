
/* Message level */

#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Andrey Shaj <shaj@test.io>");

static const char* levl[] = {
    KERN_DEBUG, KERN_INFO, KERN_NOTICE,
    KERN_WARNING, KERN_ERR, KERN_CRIT, KERN_ALERT};

static uint16_t d1 = 1234;
static uint16_t d2;

static int __init hello_init(void)
{
    uint16_t d3 = 1234;
    char out[80];
    int i;
    
    for(i=0; i<(sizeof(levl) / sizeof(levl[0])); i++)
    {
        sprintf(out, "Message level: <%s> %p\n", levl[i], levl[i]);
        printk(out);
    }
    printk("Hello world!\n");
    
    printk("%20p%20p%20p\n", &d1, &d2, &d3);
    printk("%20lx%20lx%20lx\n",
           (uintptr_t)&d1,
           (uintptr_t)&d2,
           (uintptr_t)&d3);

    printk("%20llx%20llx%20llx\n",
           (long long unsigned int)&d1,
           (long long unsigned int)&d2,
           (long long unsigned int)&d3);

    return 0;
}

static void __exit hello_exit(void)
{
    printk("Goodbye world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

