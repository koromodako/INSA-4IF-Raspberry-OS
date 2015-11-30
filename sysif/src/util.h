#ifndef UTIL_H
#define UTIL_H

// ------------------------ Les fonctions utils données par le prof
#define PANIC() do { kernel_panic(__FILE__,__LINE__) ; } while(0)
#define ASSERT(exp) do { if(!(exp)) PANIC(); } while(0)

void kernel_panic(char* string, int number);

// ------------------------ Nos fonctions utils
// Attention, cette macro fonctionne seulement avec GCC >= 4.8.2
#define SWI(syscallid) \
	__asm("mov r0, %0\n\tswi #0" : : "r"(syscallid))

#endif
