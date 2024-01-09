#include "libdiag.h"

int main(int argc, char *argv[]) {
   printf("%016lX\n", rdtsc());
   printf("%016lX\n", rdtsc());   
   printf("%016lX\n", rdtsc()); 
   printf("%ld\n", proc_hz());  
   return EXIT_SUCCESS;
}

