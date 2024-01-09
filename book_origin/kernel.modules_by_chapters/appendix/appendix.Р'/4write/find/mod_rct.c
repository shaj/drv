#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>

static int debug = 0;
module_param(debug, int, 0);

#include "findp.c"

static int __init ksys_call_tbl_init(void) {
    int errno = 0;
    const char* symbol = "sys_call_table";
    void** sct;
    if(!IS_ENABLED(CONFIG_KALLSYMS)) {
        printk("+ no config: CONFIG_KALLSYMS\n");
        return -ENOENT;
    }
    sct = (void**)find_sym(symbol, &errno);
    if(errno != 0) {
        printk("+ find error: %d\n", errno);
        return errno;
    }
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

module_init(ksys_call_tbl_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Tsiliuric <o.tsiliuric@yandex.ua>");
MODULE_VERSION("3.9");
