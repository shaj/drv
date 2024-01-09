#include <linux/module.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>
#include <linux/syscalls.h>

//static char* file = "/proc/kallsyms";
// module_param(file, charp, 0); 
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


//static int __init stread_init(void) {
//static int __init ksys_call_tbl_init(void) {
//    void** sct = (void**)find_sym(symbol);    
void* find_sym(const char *sym, int* perr) {  // find address kernel symbol sym    
    loff_t pos = 0;
    void *result = NULL;
    char *file = "/proc/kallsyms";
    #define BUF_LEN 255
    static char buff[BUF_LEN + 1];
    struct file *f = filp_open(file, O_RDONLY, 0);
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

    while(1) {
        size_t k;
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
        if((debug != 0) && (strlen(buff) > 0)) {
            if('\n' == buff[strlen(buff) - 1]) printk("+ %s", buff);
            else printk("+ %s|\n", buff);
        }
        if(0 == k) break;                 // EOF
//        if(NULL == (find = strstr(buff, " sys_call_table\n"))) continue;
        if(NULL == (find = strstr(buff, sym))) continue;
//        put_table(buff);
        break;
    }
    {
        uintptr_t s = hex2adr(buff);
        //uintptr_t* 
        result = (uintptr_t*)s;
//        return psct
    }
close:
    printk("+ close file: %s\n", file);
    filp_close(f, NULL);
    return result;
}

static int __init ksys_call_tbl_init(void) {
    int errno = 0; //-EPERM;
    const char *symbol = " sys_call_table\n";
    char symb[30]; //strlen(symbol) + 2];
    void** sct;
    if(!IS_ENABLED(CONFIG_KALLSYMS)) {
        printk("+ no config: CONFIG_KALLSYMS\n");
        return -ENOENT;
    }
    sprintf(symb, " %s\n", symbol);
    sct = (void**)find_sym(symb, &errno);
    printk("+ sys_call_table address = %px\n", sct);
    printk("error=%d\n", errno);
    if(errno < 0)
        return errno;
//    put_table(sct);
    return -EPERM;
}

//module_init(stread_init);

/*
static int __init ksys_call_tbl_init(void) {
   const char* symbol = "sys_call_table";
   void** sct = (void**)find_sym(symbol);
   printk("+ %s address = %px\n", symbol, sct); 
   if(sct) {
      char table[200] = "";
      int i;
      for(i = 0; i < 10; i++) 
         sprintf(table + strlen(table), "%px ", sct[i]);
      printk("+ %s : %s ...\n", symbol, table);
   }
   else
      printk("+ %s : %s\n", symbol, "not found");
   return -EPERM;
}
*/
module_init(ksys_call_tbl_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <olej.tsil@gmail.com>");
MODULE_VERSION("3.5");
