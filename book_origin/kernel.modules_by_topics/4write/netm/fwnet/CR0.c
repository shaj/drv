// 16 bit WP:          |
//                     V
//   3    2    2    1    1    1    0    0  0
//   1    7    3    9    5    1    7    3  0 
//   0000 0000 0000 0001 0000 0000 0000 0000 => 0x00010000
//   1111 1111 1111 1110 1111 1111 1111 1111 => 0xfffeffff


// show system register CR0: printk( "! CR0 = %x\n", r_eax ); 
#define show_cr0()                        \
{  register unsigned r_eax asm ( "eax" ); \
   asm( "pushl %eax" );                   \
   asm( "movl %cr0, %eax" );              \
   DEB2( "CR0 = %x\n", r_eax );           \
   asm( "popl %eax");                     \
}

// page write protect - on
#define rw_enable()              \
asm( "cli \n"                    \
     "pushl %eax \n"             \
     "movl %cr0, %eax \n"        \
     "andl $0xfffeffff, %eax \n" \
     "movl %eax, %cr0 \n"        \
     "popl %eax" );

// page write protect - off
#define rw_disable()             \
asm( "pushl %eax \n"             \
     "movl %cr0, %eax \n"        \
     "orl $0x00010000, %eax \n"  \
     "movl %eax, %cr0 \n"        \
     "popl %eax \n"              \
     "sti " );

/* for 64-bit ... don't verified!

// page write protect - on
asm("pushq %rax");
asm("movq %cr0, %rax");
asm("andq $0xfffffffffffeffff, %rax");
asm("movq %rax, %cr0");
asm("popq %rax");

// page write protect - off
asm("pushq %rax");
asm("movq %cr0, %rax");
asm("xorq $0x0000000000001000, %rax");
asm("movq %rax, %cr0");
asm("popq %rax");

*/
