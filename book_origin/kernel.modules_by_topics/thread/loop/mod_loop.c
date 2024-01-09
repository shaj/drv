#include <linux/module.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h> 
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
static int  __init proc_init(void);
static void __exit proc_exit(void);
module_init(proc_init);
module_exit(proc_exit);

static uint period = 5;          // целочисленный парамер - период
module_param(period, uint, 0);

#include "fops_rw.c"             // операции чтения-записи 

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

static struct task_struct* ts;
static struct completion final;   // флаг завершения

static int loop(void *data) {
   printk("!! thread %u is running\n", task_pid_nr(ts));
   while(!kthread_should_stop()) {
      printk("!! [%lu] Hello from module!\n", jiffies);
      ssleep(period);
   }
   printk("!! [%lu] thread %u finished\n",  jiffies, task_pid_nr(ts));
   complete(&final);
   return 0; 
}

struct proc_dir_entry *own_proc_dir;
struct proc_dir_entry *own_proc_node;

#define NAME_DIR  "mod_loop"
#define NAME_NODE "period"

static int __init proc_init( void ) {
   own_proc_dir = proc_mkdir(NAME_DIR, NULL);
   if(NULL == own_proc_dir) {
      printk("! can't create directory /proc/%s\n", NAME_NODE);
      return -ENOENT;
   }
   printk("! /proc/%s directory created\n", NAME_DIR);   
   own_proc_node = proc_create(NAME_NODE, S_IFREG | S_IRUGO | S_IWUGO,
                               own_proc_dir, &node_fops);
   if(NULL == own_proc_node) {
      printk("! can't create node /proc/%s/%s\n", NAME_DIR, NAME_NODE);
      remove_proc_entry(NAME_DIR, NULL);
      return -ENOENT;      
   }
   printk("! /proc/%s/%s created\n", NAME_DIR, NAME_NODE);
   ts = kthread_run(loop, (void*)&period, "loop_thread");
   return 0;
}

static void __exit proc_exit(void) {
   init_completion(&final);
   kthread_stop(ts);
   wait_for_completion(&final);
   printk("! [%lu] child thread finished\n", jiffies);
   remove_proc_entry(NAME_NODE, own_proc_dir);
   remove_proc_entry(NAME_DIR, NULL);
   printk("! /proc/%s/%s removed\n", NAME_DIR, NAME_NODE);
} 
