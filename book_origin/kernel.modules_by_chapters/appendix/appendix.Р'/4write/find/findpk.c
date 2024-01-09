void* find_sym(const char *sym, int* perr) {  // find address kernel symbol   
//    loff_t pos = 0;
    void *result = NULL;
    char *path = "/proc/kallsyms";
    #define BUF_LEN 255
    char buff[BUF_LEN + 1];
    void *p = buff;
//    void *buff = NULL;
    char fsymb[80];
    struct file *f; // = filp_open(file, O_RDONLY, 0);
    ssize_t k;
/*
    printk("+ openning file: %s\n", file);
    if(!(f->f_mode & FMODE_READ)) {
        *perr = -EBADF;
        goto close;
    } */

    sprintf(fsymb, " %s\n", sym);            // строка для поиска
/*
	if (WARN_ON_ONCE(!(file->f_mode & FMODE_READ)))
		return -EINVAL;
	if (!(file->f_mode & FMODE_CAN_READ))
		return -EINVAL;
*/
    printk("+ ============================\n");

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
//int kernel_read_file(struct file *, void **, loff_t *, loff_t,
//                    enum kernel_read_file_id); 
//int kernel_read_file_from_path(const char *path, void **buf, loff_t *size,
//                    loff_t max_size, enum kernel_read_file_id id)

// file = filp_open(path, O_RDONLY, 0);
// ret = kernel_read_file(file, buf, size, max_size, id);
    f = filp_open("./xxx.tst", O_RDONLY, 0);
    if(IS_ERR(f)) {
        *perr = -ENOENT;
        printk("+ failed to open!\n");
        return NULL;
    }
    loff_t file_size = 0;
//    k = kernel_read_file_from_path(path, (void*)&buff, &file_size, BUF_LEN, READING_POLICY);
//    k = kernel_read_file(f, (void*)&buff, NULL, BUF_LEN, READING_POLICY);
    k = kernel_read_file(f, &p, &file_size, BUF_LEN, READING_POLICY);
#else
//int kernel_read_file_from_path(const char *path, loff_t offset, void **buf,
//                    size_t buf_size, size_t *file_size,
//                    enum kernel_read_file_id id)
    size_t file_size = 0;
    k = kernel_read_file_from_path(path, 0, &buff, INT_MAX, &file_size, READING_POLICY);
// READING_POLICY READING_MODULE READING_UNKNOWN READING_X509_CERTIFICATE READING_FIRMWARE
#endif
    printk("+ file_size=%lld\n", (int64_t)file_size);
    printk("+ k=%lld\n", (int64_t)k);
    if(k<0) *perr=k;
    return NULL;
    
    while(1) {
        char *p = buff, *find = NULL;
        int n;
        for(n = 0; n < BUF_LEN; n++) {
//extern ssize_t kernel_read(struct file *, void *, size_t, loff_t *);
//ssize_t __kernel_read(struct file *file, void *buf, size_t count, loff_t *pos);
//            k = kernel_read(f, p++, 1, &pos);
/*
            int kernel_read_file(struct file *file, loff_t offset,
             void **buf, size_t buf_size,
             size_t *file_size,
             enum kernel_read_file_id id);
*/
//            k = kernel_read_file(f, 0, p++, 1, NULL,  );

/*
* kernel_read_file() - чтение содержимого файла в буфер ядра
 *
 * @file файл для чтения
 * @offset откуда начать чтение (см. ниже).
 * Указатель @buf на буфер "void *" для чтения (если
 * *@buf имеет значение NULL, будет выделен буфер, и
 * @buf_size будет проигнорирован)
 * @buf_size размер буфера памяти, если он уже выделен. Если @buf нет
 * выделено, это максимальный размер для выделения.
 * @file_size, если не NULL, полный размер @file будет
 * написано здесь.
 * @id ID ядра_read_file_id, идентифицирующий тип
 * содержимое файла считывается (для проверки LSM)
 *
 * @offset должен быть равен 0, если и @buf, и @file_size не равны NULL
 * (т. е. вызывающий должен ожидать чтения частичного содержимого файла
 * через уже выделенный @buf, не более чем @buf_size кусками, и
 * сможет определить, когда весь файл был прочитан пользователем
 * проверка @file_size). Это не рекомендуемый способ чтения
 * файл, так как возможно, что содержимое может
 * изменение между вызовами kernel_read_file().
 *
 * Возвращает количество прочитанных байтов (ни одно прочитанное не будет больше
 *, чем INT_MAX), или отрицательный при ошибке.

 */
            
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
        if(debug) printk("+ %s", (char*)buff);
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
    printk("+ close file: %s\n", path);
    filp_close(f, NULL);
    return result;
}
