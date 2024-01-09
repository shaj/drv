#include "mod_proc.h"
#include "fops_rw.c"         // чтение-запись для /proc/mod_node 

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,9,0)
const struct file_operations node_fops = {
   .owner = THIS_MODULE,
   .read  = node_read,
   .write  = node_write
#else
const struct proc_ops node_fops = {
   .proc_read  = node_read,
   .proc_write = node_write
#endif
};

static int __init proc_init(void) {
   int ret;
   struct proc_dir_entry *own_proc_node;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,9,0)
   own_proc_node = proc_create(NAME_NODE, S_IFREG | S_IRUGO | S_IWUGO, NULL, &node_fops);
#else
   own_proc_node = proc_create(NAME_NODE, S_IFREG | S_IRUGO | S_IWUGO, NULL, &node_fops);
#endif
   if(NULL == own_proc_node) {
      ret = -ENOENT;
      ERR("can't create /proc/%s\n", NAME_NODE);
      goto err_node;
   }
   LOG("/proc/%s installed\n", NAME_NODE);
   return 0;
err_node:
   return ret;
}

static void __exit proc_exit(void) {
   remove_proc_entry(NAME_NODE, NULL);
   LOG("/proc/%s removed\n", NAME_NODE);
} 

/*
struct proc_ops {
	unsigned int proc_flags;
	int	(*proc_open)(struct inode *, struct file *);
	ssize_t	(*proc_read)(struct file *, char __user *, size_t, loff_t *);
	ssize_t (*proc_read_iter)(struct kiocb *, struct iov_iter *);
	ssize_t	(*proc_write)(struct file *, const char __user *, size_t, loff_t *);
//	 mandatory unless nonseekable_open() or equivalent is used 
	loff_t	(*proc_lseek)(struct file *, loff_t, int);
	int	(*proc_release)(struct inode *, struct file *);
	__poll_t (*proc_poll)(struct file *, struct poll_table_struct *);
	long	(*proc_ioctl)(struct file *, unsigned int, unsigned long);
#ifdef CONFIG_COMPAT
	long	(*proc_compat_ioctl)(struct file *, unsigned int, unsigned long);
#endif
	int	(*proc_mmap)(struct file *, struct vm_area_struct *);
	unsigned long (*proc_get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
} __randomize_layout;
*/
