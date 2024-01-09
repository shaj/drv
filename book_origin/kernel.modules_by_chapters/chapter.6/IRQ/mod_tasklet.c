#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/version.h>      /* LINUX_VERSION_CODE */

MODULE_AUTHOR("Oleg Tsiliuric <olej@front.ru>");
MODULE_LICENSE("GPL v2");

static cycles_t cycles1, cycles2;
static u32 j1, j2;
static int context;

char tasklet_data[] = "tasklet function was called";

void tasklet_function(unsigned long data) { /* Bottom Half Function */
   context = in_atomic();
   j2 = jiffies;
   cycles2 = get_cycles();
   printk("%010lld [%05d] : %s in contxt %d\n", 
          (long long unsigned)cycles2, j2, (char*)data, context);
   return;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,9,0)
DECLARE_TASKLET(my_tasklet, tasklet_function, 
                (unsigned long)&tasklet_data);
#else
DECLARE_TASKLET_OLD(my_tasklet, tasklet_function);
#endif                          

int init_module(void) {
	context = in_atomic();
	j1 = jiffies;
	cycles1 = get_cycles();
	my_tasklet.data = (unsigned long)&tasklet_data; //(char*)data;
	tasklet_schedule(&my_tasklet);           /* Schedule the Bottom Half */
	printk("%010lld [%05d] : tasklet was scheduled in contxt %d\n",
          (long long unsigned)cycles1, j1, context);
	return 0;
}

void cleanup_module(void) {
	tasklet_kill(&my_tasklet);                /* Stop the tasklet before we exit */
	return;
}
