#include <linux/module.h> 
#include <linux/delay.h> 
#include <linux/kthread.h>
#include <linux/mutex.h> 

#include "../prefix.c"

#define num 4                    // num - число рабочих потоков

static ulong rep = 10000;        // rep - число повторений (объём работы)
module_param(rep, ulong, 0);

static bool sync = 1;            // sync - синхронизация: 0 - да, 1 - нет
module_param(sync, bool, 0);

DEFINE_MUTEX(mtx); 
static ulong locked = 0;         // накапливаемая сумма

static struct completion finished[num];

static int thread_func(void* data) { 
   uint i, n = *(uint*)data;     // порядковый номер потока (локальный!)
   printk("! %s is started\n", st(n));
   for(i = 0; i < rep; i++) {
      if(!sync) mutex_lock(&mtx);
      locked++;   
      if(!sync) mutex_unlock(&mtx);
   }
   printk("! %s is finished\n", st(n));
   complete(finished + n);
   return 0;
}

static int test_mlock(void) {
   unsigned long j1 = jiffies;   // время точки старта
   struct task_struct *t[num];
   uint i;
   printk("! потоков = %d | повторов = %ld | синхронизация: %s\n",
          num, rep, sync ? "нет" : "да" );
   for(i = 0; i < num; i++)
      init_completion(finished + i);
   for(i = 0; i < num; i++)
      #define IDENT "mlock_thread_%d"
      t[i] = kthread_run(thread_func, (void*)&i, IDENT, i);
   for(i = 0; i < num; i++)
      wait_for_completion(finished + i);
   j1 = jiffies - j1;
   printk("! время работы: %ld.%02ld сек. (%ld), сумма=%ld\n",
           j1 / HZ, (j1 * 100) / HZ % 100, j1, locked);
   return -1; 
} 

module_init(test_mlock);
MODULE_LICENSE("GPL");
