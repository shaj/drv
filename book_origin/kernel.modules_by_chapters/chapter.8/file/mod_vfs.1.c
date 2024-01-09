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
    size_t n;
//    long l;
    size_t len;
    loff_t file_offset = 0;
    mm_segment_t old_fs;
    old_fs = get_fs();
    set_fs(KERNEL_DS);
//    mm_segment_t fs = get_fs();
//    set_fs( get_ds() );

    if(file != NULL) strcpy(buff, file);
    printk("*** openning file: %s\n", buff);
    f = filp_open(buff, O_RDONLY, 0);

    if(IS_ERR(f)) {
        printk("*** file open failed: %s\n", buff);
        len = -ENOENT;
        goto fail_oupen;
    }
         
//    len = vfs_llseek(f, 0L, 2); // 2 means SEEK_END 
    len = vfs_llseek(f, 0L, SEEK_END);
    if(len <= 0) {
        printk("*** failed to lseek %s\n", buff);
        len = -EINVAL;
        goto failure;
    }
    printk("*** file size = %d bytes\n", (int)len);

    n = vfs_llseek(f, 0L, SEEK_SET);
    printk("n=%ld", n);

//    mm_segment_t old_fs = get_fs();
// extern ssize_t vfs_read(struct file *, char __user *, size_t, loff_t *);


    n = vfs_read(f, buff, BUF_LEN, &file_offset);
    printk("read bytes = %ld\n", n);
//    if((n = vfs_read(f, buff, len, &file_offset)) != len) {
    if(n != BUF_LEN) {
        printk("*** failed to read\n");
        len = -EIO;
        goto failure;
    }
    buff[n] = '\0';
    printk("%s\n", buff);
    len = -EPERM;
failure:
    set_fs(old_fs);
    printk( KERN_ALERT "**** close file\n" );
    filp_close(f, NULL);
fail_oupen:
//    set_fs( fs );
//mm_segment_t old_fs = get_fs();
    return (int)len;
}

module_init(kread_init);
MODULE_LICENSE("GPL");
