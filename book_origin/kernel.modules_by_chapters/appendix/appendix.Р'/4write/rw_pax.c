#include <asm/paravirt.h>
#include <asm-generic/bug.h>
#include <linux/version.h>
#include <linux/preempt.h>

static inline unsigned long native_pax_open_kernel(void) {
   unsigned long cr0;
   preempt_disable();
   barrier();
   cr0 = read_cr0() ^ X86_CR0_WP;
   BUG_ON(unlikely( cr0 & X86_CR0_WP));
   write_cr0(cr0);
   return cr0 ^ X86_CR0_WP;
}

static inline unsigned long native_pax_close_kernel(void) {
   unsigned long cr0;
   cr0 = read_cr0() ^ X86_CR0_WP;
   BUG_ON(unlikely(!(cr0 & X86_CR0_WP)));
   write_cr0(cr0);
   barrier();
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0)
   preempt_enable_no_resched();
#else
   preempt_enable();
// preempt_count_dec();
#endif 
   return cr0 ^ X86_CR0_WP;
}

