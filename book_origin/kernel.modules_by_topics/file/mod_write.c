#include <linux/module.h>
#include <linux/fs.h>

static char* log = NULL;
module_param(log, charp, 0);

#define BUF_LEN 255
#define DEFLOG "./module.log"
#define TEXT "...............\n"

static int __init kread_init(void) {
    struct file *f;
    ssize_t n = 0;
    loff_t offset = 0;
    char buff[BUF_LEN + 1] = DEFLOG;
    if(log != NULL) strcpy(buff, log);
    printk("+++ openning file: %s\n", buff);    
    f = filp_open(buff,
                 O_CREAT | O_RDWR | O_TRUNC,
                 S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
    if(IS_ERR(f)) {
        printk("+++ file open failed: %s\n", buff);
        return -ENOENT;
    }
    printk("+++ file open %s\n", buff);
    strcpy(buff, TEXT);
    // extern ssize_t kernel_write(struct file *, const void *, size_t, loff_t *);   
    if((n = kernel_write(f, buff, strlen(buff), &offset)) != strlen(buff)) {
        printk("! failed to write: %d\n", (int)n);
        return -EIO;
    }
    printk("+++ write %d bytes\n", (int)n);
    printk("+++ close file\n");
    filp_close(f, NULL);
    return -EPERM;
}

module_init( kread_init );
MODULE_AUTHOR("Oleg Tsiliuric");
MODULE_VERSION("2.1");
MODULE_LICENSE("GPL");
