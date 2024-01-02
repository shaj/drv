
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey SHAJ <kolobrod2002@yandex.ru>");
MODULE_VERSION("4.0");

static char *hello_str = "Hello world!\n";

static ssize_t dev_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    int len = strlen(hello_str);
    printk(KERN_INFO "=== read : %ld\n", (long)count);
    if(count < len) return -EINVAL;
    if(*ppos != 0)
    {
        printk(KERN_INFO "=== read return : 0\n");
        return 0;
    }
    if(copy_to_user(buf, hello_str, len)) return -EINVAL;
    *ppos = len;
    printk(KERN_INFO "=== read return : %d\n", len);
    return len;
}

static int __init dev_init(void);
module_init(dev_init);

static void __exit dev_exit(void);
module_exit(dev_exit);

