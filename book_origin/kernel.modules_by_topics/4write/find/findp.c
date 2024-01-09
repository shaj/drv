void* find_sym(const char *sym, int* perr) {  // find address kernel symbol   
    loff_t pos = 0;
    void *result = NULL;
    char *file = "/proc/kallsyms";
    #define BUF_LEN 255
    char buff[BUF_LEN + 1], fsymb[80];
    struct file *f = filp_open(file, O_RDONLY, 0);
    ssize_t k;
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
    sprintf(fsymb, " %s\n", sym);            // строка для поиска
    while(1) {
        char *p = buff, *find = NULL;
        int n;
        for(n = 0; n < BUF_LEN; n++) {
            k = kernel_read(f, p++, 1, &pos);
            if(k < 0) {
                printk("+ failed to read\n");
                *perr = k;
                goto close;                
            }
            if(0 == k) break;
            *p = '\0';
            if('\n' == *(p - 1)) break; // считана строка
        }
        if(0 == k) break;                 // EOF
        if(NULL == (find = strstr(buff, fsymb))) continue;
        if(debug) printk("+ %s", buff);
        break;
    }
    if(0 == k)
        printk("+ symbol not found: EOF\n");
    else {
        uintptr_t ip;
        strreplace(buff, ' ', '\0');
        *perr = kstrtoul(buff, 16, &ip); 
        result = (void*)ip;
        if(debug) printk("+ %px\n", result); 
    }
close:
    printk("+ close file: %s\n", file);
    filp_close(f, NULL);
    return result;
}
