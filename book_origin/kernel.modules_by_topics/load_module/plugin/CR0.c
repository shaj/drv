// 16 бит WP:          |
//                     V
//   3    2    2    1    1    1    0    0  0
//   1    7    3    9    5    1    7    3  0 
//   0000 0000 0000 0001 0000 0000 0000 0000 => 0x00010000
//   1111 1111 1111 1110 1111 1111 1111 1111 => 0xfffeffff

#ifdef __i386__ // код для 32 бит

// показать управляющий регистр CR0
#define show_cr0()                         \
{  register unsigned r_eax asm ( "eax" ); \
   asm( "pushl %eax" );                   \
   asm( "movl %cr0, %eax" );              \
   printk( "! CR0 = %x\n", r_eax );       \
   asm( "popl %eax");                     \
}

//код выключения защиты записи:
#define rw_enable()               \
asm( "pushl %eax \n"             \
     "movl %cr0, %eax \n"        \
     "andl $0xfffeffff, %eax \n"    \
     "movl %eax, %cr0 \n"        \
     "popl %eax" );

//код включения защиты записи:
#define rw_disable()              \
asm( "pushl %eax \n"             \
     "movl %cr0, %eax \n"        \
     "orl $0x00010000, %eax \n"  \
     "movl %eax, %cr0 \n"        \
     "popl %eax" );

#else           // код для 64-бит

// показать управляющий регистр CR0
#define show_cr0()                         \
{  register unsigned r_rax asm ( "rax" ); \
   asm( "pushq %rax" );                   \
   asm( "movq %cr0, %rax" );              \
   printk( "! CR0 = %x\n", r_rax );       \
   asm( "popq %rax");                     \
}

//код выключения защиты записи:
#define rw_enable()                \
asm( "cli \n"                     \
     "pushq %rax \n"              \
     "movq %cr0, %rax \n"         \
     "andq $0xfffffffffffeffff, %rax \n" \
     "movq %rax, %cr0 \n"         \
     "popq %rax ");

//код включения защиты записи:
#define rw_disable()                         \
asm( "pushq %rax \n"                        \
     "movq %cr0, %rax \n"                \
     "xorq $0x0000000000001000, %rax \n" \
     "movq %rax, %cr0 \n"                \
     "popq %rax \n"                      \
     "sti ");

#endif
