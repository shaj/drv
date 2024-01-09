#include <linux/version.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include "../common.c"
#include "../find.c"

static char* file = "./slave.ko";
module_param(file, charp, 0);

static char this_mod_file[40],                  // имя файла master-модуля
                  slave_name[80];                    // имя файла slave-модуля

static int __init mod_init(void) {
   long res = 0, len;
   struct file *f;
   void *buff;
   ssize_t n;
   asmlinkage long (*sys_init_module)       // системный вызов sys_init_module()
              (void __user *umod, unsigned long len, const char __user *uargs);
   set_mod_name(this_mod_file, __FILE__);
#ifdef __i386__
   #define sys_init "sys_init_module"
#else
   #define sys_init "__x64_sys_init_module"
#endif
   if((sys_init_module = (void*)find_sym(sys_init)) == NULL) {
      printk("> %s не найден\n", sys_init);
      res = -EINVAL;
      goto end;
   }
   printk("> адрес %s = %px\n", sys_init, sys_init_module);
   strcpy(slave_name, file);
   f = filp_open(slave_name, O_RDONLY, 0);
   if(IS_ERR(f)) {
      printk("> ошибка открытия файла %s\n", slave_name);
      res = -ENOENT;
      goto end;
   }
   len = vfs_llseek(f, 0L, 2);                       // 2 - means SEEK_END 
   if(len <= 0) {
      printk("> ошибка lseek\n");
      res = -EINVAL;
      goto close;
   }
   printk("> длина файла модуля = %d байт\n", (int)len);
   if(NULL == (buff = vmalloc(len))) {
      printk("> ошибка выбеления памяти буфера\n");
      res = -ENOMEM;
      goto close;
   }
   printk("> адрес буфера чтения = %px\n", buff);
   vfs_llseek(f, 0L, 0);                                // 0 - means SEEK_SET 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0))
   // extern int kernel_read(struct file *, loff_t, char *, unsigned long);
   n = kernel_read(f, 0, buff, len);
#else
   // extern ssize_t kernel_read(struct file *, void *, size_t, loff_t *);
   n = kernel_read(f, buff, len, NULL);
#endif
   printk("> считано из файла %s %ld байт\n", slave_name, n);
   if(n != len) {
      printk("> ошибка чтения\n");
      res = -EIO;
      goto free;
   }
   {  mm_segment_t old_fs = get_fs();
      set_fs(KERNEL_DS);
      res = sys_init_module(buff, len, ""); 
      set_fs(old_fs);
      if(res < 0) {
         printk("> ошибка загрузки %s\n", sys_init);          
         goto insmod;
      }
   }
   printk("> модуль %s загружен: file=%s\n", this_mod_file, file);
insmod:
free:
   vfree(buff);
close:
   filp_close(f, NULL);
end: 
   return res;
}

static void __exit mod_exit(void) {
   asmlinkage long (*sys_delete_module)  // системный вызов sys_delete_module()
              (const char __user *name, unsigned int flags);
   // flags: O_TRUNC, O_NONBLOCK
   char *slave_mod = strrchr(slave_name, '/') != NULL ?
                     strrchr(slave_name, '/') + 1 :
                     slave_name;
   *strrchr(slave_mod, '.') = '\0';
   printk("> выгружается модуль %s\n", slave_mod);
#ifdef __i386__
   #define sys_delet "sys_delete_module"
#else
   #define sys_delet "__x64_sys_delete_module"
#endif
   if((sys_delete_module = find_sym(sys_delet)) == NULL) {
      printk("> %s не найден\n", sys_delet);
      return;
   }
   printk("> адрес sys_delete_module = %px\n", sys_delete_module);
   {  long res = 0;
      mm_segment_t old_fs = get_fs();
      set_fs(KERNEL_DS);
      res = sys_delete_module(slave_mod, 0); 
      set_fs(old_fs);
      if(res < 0) {
         printk("> ошибка выгрузки модуля %s\n", slave_mod);
         return;
      }
   }
   printk("> модуль %s выгружен\n", this_mod_file);
}
