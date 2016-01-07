#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

// ------------------------ Les fonctions utils donnees par le prof
#define PANIC() do { kernel_panic(__FILE__,__LINE__) ; } while(0)
#define ASSERT(exp) do { if(!(exp)) PANIC(); } while(0)

void kernel_panic(char* string, int number);

// ------------------------ Nos fonctions utils
// Attention, cette macro fonctionne seulement avec GCC >= 4.8.2
#define SWI(syscallid) \
        __asm("mov r0, %0\n\t" \
          "swi #0" : : "r"(syscallid))

// Macros for stacking registers
#define STACK_REGS __asm("stmfd sp!, {r0-r12, lr}")
#define UNSTACK_REGS __asm("ldmfd sp!, {r0-r12, pc}^")

// Macros for switching proc modes
#define SWITCH_TO_USER_MODE __asm("cps 0b10000")
#define SWITCH_TO_FIQ_MODE __asm("cps 0b10001")
#define SWITCH_TO_IRQ_MODE __asm("cps 0b10010")
#define SWITCH_TO_SVC_MODE __asm("cps 0b10011")
#define SWITCH_TO_ABORT_MODE __asm("cps 0b10111")
#define SWITCH_TO_SYSTEM_MODE __asm("cps 0b11111")

// Enum for syscall ids
enum SYS_CALL_ID {
	SCI_REBOOT = 0x01,
	SCI_NOP = 0x02,
	SCI_SETTIME = 0x03,
	SCI_GETTIME = 0x04,
	SCI_YIELDTO = 0x05,
	SCI_YIELD = 0x06,
	SCI_EXIT = 0x07,
	SCI_MMAP = 0x08,
	SCI_MUNMAP = 0x09
};

// Enum for proc states
typedef enum {
	PS_RUNNING = 0x01,
	PS_READY = 0x02,
	PS_TERMINATED = 0X03
} PROCESS_STATE;

// Nombre de niveaux de priorite
#define PRIORITY_COUNT 6
// Enum proc priority
typedef enum {
	PP_KERNEL = 0x00,
	PP_ULTRA_HIGH = 0x01,
	PP_HIGH = 0x02,
	PP_MEDIUM = 0x03,
	PP_LOW = 0x04,
	PP_ULTRA_LOW = 0x05
} PROCESS_PRIORITY;

// Enum proc sheduling policy
typedef enum {
	SP_SIMPLE = 0x01,
	SP_PRIORITY = 0x02
} SCHEDULING_POLICY;

// Convertir entier en string
char* itoa(uint64_t i, char b[]);

// Convertir string en entier
uint64_t atoi(const char *c);

// Retourne l'adresse dans str du premier fnd trouvé, sinon stop à '\0'
char* getNextChar(char* str, char fnd);

// Return true if the c if a digit
uint8_t isdigit(char c);

#endif
