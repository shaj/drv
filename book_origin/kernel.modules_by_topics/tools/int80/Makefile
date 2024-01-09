CC = gcc -Wall

PROGS = mpsys mplib int80.32 int80.64 syscall.64

all:  $(PROGS)

int80.32:  int80.s
	nasm -f elf int80.s -o int80.32.o
	ld -m elf_i386 int80.32.o -o int80.32

int80.64:  int80.s
	nasm -f elf64 int80.s -o int80.64.o
	ld int80.64.o -o int80.64

syscall.64: syscall.s
	nasm -f elf64 syscall.s -o syscall.o
	ld syscall.o -o syscall.64

clean:
	@rm -f *.o $(PROGS)

disclean: clean
