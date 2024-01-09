#include <linux/module.h> 
#include <linux/sched.h> 
#include <linux/kthread.h>
#include <linux/delay.h> 

static int param = 5;
module_param(param, uint, 0);

static int thread_fun(void * data) { 
   struct completion *finished = (struct completion*)data; 
   printk("child process [%d] is running\n", current->pid);
   msleep(param * 1000);                  /* Пауза param сек. */
   printk("child process [%d] is completed\n", current->pid);
   complete(finished);                     /* Отмечаем факт выполнения условия. */
   return 0; 
} 

int test_thread(void) { 
   struct task_struct *t1;
   DECLARE_COMPLETION(finished); 
   printk("main process [%d] is running\n", current->pid); 
   t1 = kthread_create(thread_fun, &finished, "my_thread");
   printk("create new kernel thread [%d]\n", task_pid_nr(t1));
   wake_up_process(t1);   
   wait_for_completion(&finished);         /* Ожидаем выполнения условия */
   printk("main process [%d] is completed\n", current->pid); 
   return -1; 
} 

module_init(test_thread);
MODULE_LICENSE("GPL");
