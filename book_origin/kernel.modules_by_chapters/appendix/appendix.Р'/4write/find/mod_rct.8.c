#include <linux/module.h>
#include <linux/fs.h>

uintptr_t hex2adr(const char *b) {
    uintptr_t res = 0;
    const char *p = b, *d;
    const char *dig = "0123456789abcdef"; 
    while((d = strchr(dig, *p)) != NULL) {
        res = (res << 4) + (d - dig);
        p++; 
    }
    return res; 
}

void* find_sym(const char *sym, int* perr) {  // find address kernel symbol    
    loff_t pos = 0;
    void *result = NULL;
    char *file = "/proc/kallsyms";
    #define BUF_LEN 255
    char buff[BUF_LEN + 1], fsymb[80];
    struct file *f = filp_open(file, O_RDONLY, 0);
    size_t k;
    if(IS_ERR(f)) {
        *perr = -ENOENT;
        printk("+ failed to open: %s\n", file);
        return NULL;
    }
    printk("+ openning file: %s\n", file);
    if(!(f->f_mode & FMODE_READ)) {
        *perr = -EBADF;
        goto close;
    }
    if(!f->f_op) {
        if(!f->f_op->read)
            printk("+ read op. => %px\n", f->f_op->read);
    }
    else
        printk("+ op. table => %px\n", (void*)0);

    sprintf(fsymb, " %s\n", sym);

    while(1) {
            char *p = buff, *find = NULL;
        int n;
        for(n = 0; n < BUF_LEN; n++) {
            k = kernel_read(f, p++, 1, &pos);
            if(k < 0) {
                printk("+ failed to read\n");
                *perr = -EIO;
                goto close;                
            }
            if(0 == k) break;
            *p = '\0';
            if('\n' == *(p - 1)) break; // считана строка
        }
        if(0 == k) break;                 // EOF
        if(NULL == (find = strstr(buff, fsymb))) continue;
        printk("+ %s", buff);
        break;
    }
    if(0 == k) {
        printk("+ not found: EOF\n");
    }
    else {
        uintptr_t ip = hex2adr(buff);
        printk("+ %lx\n", ip);
        result = (void*)ip;
        printk("+ %px\n", result); 
        
		strreplace(buff, ' ', '\0');
		*perr = kstrtoul(buff, 16, &ip); 
        result = (void*)ip;
		printk("+ %px\n", result); 
    }
close:
    printk("+ close file: %s\n", file);
    filp_close(f, NULL);
    return result;
}

static int __init ksys_call_tbl_init(void) {
    int errno = 0;
	const char* symbol = "sys_call_table";
    void** sct;
    if(!IS_ENABLED(CONFIG_KALLSYMS)) {
        printk("+ no config: CONFIG_KALLSYMS\n");
        return -ENOENT;
    }
    sct = (void**)find_sym(symbol, &errno);
    printk("+ sys_call_table address = %px\n", sct);
    printk("+ error=%d\n", errno);
    if(errno != 0)
		return errno;
    return -EPERM;
}

module_init(ksys_call_tbl_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <olej.tsil@gmail.com>");
MODULE_VERSION("3.7");
