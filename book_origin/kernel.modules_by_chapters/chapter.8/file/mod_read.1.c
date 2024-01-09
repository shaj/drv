#include <linux/module.h>
#include <linux/fs.h>

static char* file = NULL;
module_param(file, charp, 0);

#define BUF_LEN 255
#define DEFNAME "/etc/apt/sources.list";

static char buff[BUF_LEN + 1] = DEFNAME;

static int __init kread_init(void) {
    struct file *f;
    loff_t pos = 0;
    if(file != NULL) strcpy(buff, file);
    printk("+++ openning file: %s\n", buff);
    f = filp_open(buff, O_RDONLY, 0);
    if(IS_ERR(f)) { 
        printk("+++ file open failed: %s\n", buff);
        return -ENOENT;
    }
    while(1) {
        #define SIZE 80
        char data[SIZE + 1];
        size_t n;
        // ssize_t kernel_read(struct file *file, void *buf, size_t count, loff_t *pos);
        n = kernel_read(f, data, SIZE, &pos);
        if(0 == n) {
            printk("+++ read EOF\n");
            break;
        } else if(n >= 0) {
            data[n] = '\0';            
            printk("+++ read %d bytes [%d]:\n%s\n", 
                   (int)n, (int)pos, data);
        } else {
            printk("+++ kernel_read failed\n");
            filp_close(f, NULL);
            return -EIO;
        }
    }
    printk("+++ close file\n");
    filp_close(f, NULL);
    return -EPERM;
}

module_init( kread_init );
MODULE_AUTHOR("Oleg Tsiliuric");
MODULE_VERSION("2.1");
MODULE_LICENSE("GPL");
