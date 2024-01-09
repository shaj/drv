#include <stdint.h>
#include <stdio.h>

static uint16_t d1 = 1234;
static uint16_t d2;

int main(void) {
	uint16_t d3 = 1234;
	printf("%20p%20p%20p\n", &d1, &d2, &d3);
	printf("%20lx%20lx%20lx\n", 
	       (uintptr_t)&d1,
	       (uintptr_t)&d2,
	       (uintptr_t)&d3);
	printf("%20llx%20llx%20llx\n", 
	       (long long unsigned int)&d1,
	       (long long unsigned int)&d2,
	       (long long unsigned int)&d3);
	return 0;
}
