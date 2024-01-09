#include <asm/pgtable_types.h>
#include <asm/tlbflush.h>

static inline void mem_setrw( void **table ) {
   unsigned int l;
   pte_t *pte = lookup_address( (long unsigned int)table, &l );
   pte->pte |= _PAGE_RW;
   __flush_tlb_one( (unsigned long)table );
}

static inline void mem_setro( void **table ) {
   unsigned int l;
   pte_t *pte = lookup_address( (long unsigned int)table, &l );
   pte->pte &= ~_PAGE_RW;
   __flush_tlb_one( (unsigned long)table );
}

// <arch/x86/include/asm/pgtable-2level_types.h>
// typedef unsigned long   pteval_t;
// typedef union {
//         pteval_t pte;
//         pteval_t pte_low;
// } pte_t;

// <include/linux/const.h>
// #define _AT(T,X)        ((T)(X))

// <arch/x86/include/asm/pgtable_types.h>
// #define _PAGE_BIT_RW            1       /* writeable */
// #define _PAGE_RW        (_AT(pteval_t, 1) << _PAGE_BIT_RW)