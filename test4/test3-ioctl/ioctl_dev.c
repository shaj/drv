

#include "ioctl.h"
#include "dev.h"
#include <asm-generic/ioctl.h>
#include <linux/version.h>

#define HELLO_MAJOR 200

static int dev_open(struct inode *n, struct file *f)
{
    printk(KERN_INFO "=== open device %u:%u\n", HELLO_MAJOR, iminor(n));
    return 0;
}


static int dev_release(struct inode *n, struct file *f)
{
    return 0;
}


static long dev_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    if((_IOC_TYPE(cmd) != IOC_MAGIC))
        return -EINVAL;
    switch(cmd)
    {
        case IOCTL_GET_STRING:
            {
                if(copy_to_user((void*)arg, hello_str, _IOC_SIZE(cmd)))
                    return -EINVAL;
                break;
            }
        default:
            return -ENOTTY;
    }
    return 0;
}


static const struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .unlocked_ioctl = dev_ioctl,
};


#define HELLO_MOD_NAME "my_ioctl_dev"

static int __init dev_init(void)
{
    int ret = register_chrdev(HELLO_MAJOR, HELLO_MOD_NAME, &hello_fops);
    if(ret < 0)
    {
        printk(KERN_ERR "=== Can not register char device\n");
        goto err;
    }

err:
    return ret;
}


static void __exit dev_exit(void)
{
    unregister_chrdev(HELLO_MAJOR, HELLO_MOD_NAME);
}


