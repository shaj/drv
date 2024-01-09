// 16 bit WP:          |
//                     V
//   3    2    2    1    1    1    0    0  0
//   1    7    3    9    5    1    7    3  0 
//   0000 0000 0000 0001 0000 0000 0000 0000 => 0x00010000
//   1111 1111 1111 1110 1111 1111 1111 1111 => 0xfffeffff

// <arch/x86/include/uapi/asm/processor-flags.h>
// #define X86_CR0_WP_BIT          16 /* Write Protect */
// #define X86_CR0_WP              _BITUL(X86_CR0_WP_BIT)

#ifdef __i386__
static inline void rw_enable(void) {
   asm( "cli \n"
        "pushl %eax \n"
        "movl %cr0, %eax \n"
        "andl $0xfffeffff, %eax \n"
        "movl %eax, %cr0 \n"
        "popl %eax ");
}

static inline void rw_disable(void) {
   asm( "pushl %eax \n"
        "movl %cr0, %eax \n"
        "orl $0x00010000, %eax \n"
        "movl %eax, %cr0 \n"
        "popl %eax \n"
        "sti ");
}
#else
static inline void rw_enable(void) {
   asm( "cli \n"
        "pushq %rax \n"
        "movq %cr0, %rax \n"
        "andq $0xfffffffffffeffff, %rax \n"
        "movq %rax, %cr0 \n"
        "popq %rax ");
}

static inline void rw_disable(void) {
   asm( "pushq %rax \n"
        "movq %cr0, %rax \n"
        "xorq $0x0000000000001000, %rax \n"
        "movq %rax, %cr0 \n"
        "popq %rax \n"
        "sti ");
}
#endif
