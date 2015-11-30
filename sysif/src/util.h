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
	SCI_EXIT = 0x07
};

// Enum for proc states
enum PROC_STATE {
	PROC_RUNNING = 0x01,
	PROC_READY = 0x02,
	PROC_TERMINATED = 0X03
};

#endif
