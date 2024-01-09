section     .text
global      _start

_start:

    mov     rdx,len                             ;message length
    mov     rsi,msg                             ;message to write
    mov     rdi,1                               ;file descriptor (stdout)
    mov     rax,1                               ;system call number (sys_write)
    syscall

    mov     rax,60                              ;system call number (sys_exit)
    syscall

section     .data

msg     db  'Hello, world!',0xa
len     equ $ - msg
