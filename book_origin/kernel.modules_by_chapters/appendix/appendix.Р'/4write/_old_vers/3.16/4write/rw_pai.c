#include <asm/paravirt.h>
#include <asm-generic/bug.h>
#include <linux/version.h>
#include <linux/preempt.h>

static inline unsigned long native_pai_open_kernel( void ) {
   unsigned long cr0;
   local_irq_disable();
   barrier();
   cr0 = read_cr0() ^ X86_CR0_WP;
   BUG_ON( unlikely( cr0 & X86_CR0_WP ) );
   write_cr0( cr0 );
   return cr0 ^ X86_CR0_WP;
}

static inline unsigned long native_pai_close_kernel( void ) {
   unsigned long cr0;
   cr0 = read_cr0() ^ X86_CR0_WP;
   BUG_ON( unlikely( !( cr0 & X86_CR0_WP ) ) );
   write_cr0( cr0 );
   barrier();
   local_irq_enable();  
   return cr0 ^ X86_CR0_WP;
}
