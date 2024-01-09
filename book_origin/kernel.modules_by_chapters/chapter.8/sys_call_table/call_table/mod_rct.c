#include <linux/module.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>
#include <linux/syscalls.h>

static char* file = "/proc/kallsyms";
module_param(file, charp, 0); 
static int debug = 0; 
module_param(debug, int, 0); 

uintptr_t hex2adr(char *b) {
    uintptr_t res = 0;
    char *p = b, *d;
    const char *dig = "0123456789abcdef"; 
    while((d = strchr(dig, *p)) != NULL) {
        res = (res << 4) + (d - dig);
        p++; 
    }
    return res; 
}

void put_table(char *b) {
    uintptr_t s = hex2adr(b);    
    uintptr_t* psct = (uintptr_t*)s;
    int i;
    char table[200] = "sys_call_table : "; 
    printk("+ %s", b);
    printk("+ %lx\n", s);
    printk("+ sys_call_table address = %px\n", psct);
    for(i = 0; i < 10; i++) {
        uintptr_t sa = *(psct + i);
        sprintf(table + strlen(table), "%px ", (void*)sa);
    }
    printk("+ %s ...\n", table);
}

#define BUF_LEN 255
static  char buff[BUF_LEN + 1];

static int __init stread_init(void) {
    struct file *f;
    loff_t pos = 0;
    int err = -EPERM;

    if(!IS_ENABLED(CONFIG_KALLSYMS)) {
        printk("+ no config: CONFIG_KALLSYMS\n");
        return -ENOENT;
    }
    f = filp_open(file, O_RDONLY, 0);
    if(IS_ERR(f)) {
        printk("+ failed to open: %s\n", file);
        err = -ENOENT;
        goto close;
    }
    printk("+ openning file: %s\n", file);
    if(!(f->f_mode & FMODE_READ)) {
        err = -EBADF;
        goto close;
    }
    if(!f->f_op) {
        if(!f->f_op->read)
            printk("+ read op. => %px\n", f->f_op->read);
    }
    else
        printk("+ op. table => %px\n", (void*)0);

    while(1) {
        size_t k;
        char *p = buff, *find = NULL;
        int n;
        for(n = 0; n < BUF_LEN; n++) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0))
//   extern int kernel_read(struct file *, loff_t, char *, unsigned long);
            k = kernel_read(f, pos, p++, 1);
            pos += k;
#else
//   extern ssize_t kernel_read(struct file *, void *, size_t, loff_t *);
            k = kernel_read(f, p++, 1, &pos);
#endif
            if(k < 0) {
                printk("+ failed to read\n");
                err = -EIO;
                goto close;                
            }
            if(0 == k) break;
            *p = '\0';
            if('\n' == *(p - 1)) break; // считана строка
        }
        if((debug != 0) && (strlen(buff) > 0)) {
            if('\n' == buff[strlen(buff) - 1]) printk("+ %s", buff);
            else printk("+ %s|\n", buff);
        }
        if(0 == k) break;                 // EOF
        if(NULL == (find = strstr(buff, " sys_call_table\n"))) continue;
        put_table(buff);
        break;
    }

close:
    printk("+ close file: %s\n", file);
    filp_close(f, NULL);
    return err;
}

module_init(stread_init);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <olej.tsil@gmail.com>");
MODULE_VERSION("3.3");
