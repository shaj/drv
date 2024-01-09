#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <uapi/linux/fs.h>

static char* file = NULL;
module_param(file, charp, 0);

#define BUF_LEN 120
#define DEFNAME "/etc/apt/sources.list";
static char buff[BUF_LEN + 1] = DEFNAME;

static int __init kread_init( void ) {
    struct file *f;
    int errno = -EPERM;
    size_t len;
    mm_segment_t old_fs;
    loff_t file_offset = 0;

    if(file != NULL) strcpy(buff, file);
    printk("+++ openning file: %s\n", buff);
    f = filp_open(buff, O_RDONLY, 0);
    if(IS_ERR(f)) {
        printk("+++ file open failed: %s\n", buff);
        return -ENOENT;
    }
         
    len = vfs_llseek(f, 0L, SEEK_END); // длина файла
    if(len <= 0) {
        printk("+++ failed to lseek %s\n", buff);
        return -EINVAL;
    }
    printk("+++ file size = %d bytes\n", (int)len);
    vfs_llseek(f, 0L, SEEK_SET);       // установка в начало

    old_fs = get_fs();                 // буфер в kernel space
    set_fs(KERNEL_DS);
    while(1) {
        #define SIZE 120
        char data[SIZE + 1];
        size_t n;    
        // extern ssize_t vfs_read(struct file *, char __user *, size_t, loff_t *);
        n = vfs_read(f, data, SIZE, &file_offset);
        if(n < 0) {
            printk("+++ failed to read: %ld\n", n);
            errno = -EIO;
            break;
        }
        else if(n == 0) {
            printk("+++ read EOF\n");
            break;
        }
        else {
            data[n] = '\0';
            printk("+++ read %d bytes:\n%s\n", (int)n, data); 
            errno = -EPERM;
        }
    }
    set_fs(old_fs);

    printk( KERN_ALERT "+++ close file\n" );
    filp_close(f, NULL);
    return (int)errno;
}

module_init(kread_init);
MODULE_LICENSE("GPL");
