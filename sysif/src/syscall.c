#include "syscall.h"
#include "util.h"

// Appel système : reboot ------------------------------------------------------
void sys_reboot() 
{
	// Positionne le numéro de l'appel système dans r0 : numéro = 1
	__asm("mov r0, #1");

	// Appel SWI Handler (interruption Système)
	__asm("swi #0");
}

void do_sys_reboot()
{
	// Reboot pour l'émulateur
	#if defined QEMU
		__asm("bl 0x8000");
	// Reboot pour le Raspberry Pi
	#else
		const int PM_RSTC = 0x2010001c;
		const int PM_WDOG = 0x20100024;
		const int PM_PASSWORD = 0x5a000000;
		const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
		PUT32(PM_WDOG, PM_PASSWORD | 1);
		PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
		while(1);
	#endif
}

// Appel système : nop ---------------------------------------------------------

void sys_nop()
{
    // Positionne le numéro de l'appel système dans r0 : numéro = 2
    __asm("mov r0, #2");

    // Interruption 
    __asm("swi #0");
}

void do_sys_nop()
{
    return;
}

// Appel système : settime -----------------------------------------------------
void sys_settime(uint64_t date_ms)
{
    // Positionne le numéro de l'appel système dans r0 : numéro = 3
    __asm("mov r0, #3");

    // Argument date_ms dans r1
    __asm("mov r1, %0" : : "r"(date_ms) : "r1");

    // Interruption 
    __asm("swi #0");
}

void do_sys_settime()
{
    int a = 0x0A;
    int b = 0x0B;
    int c = 0x0C;

    int d = a + b * 2;

    for (;c < d; ++c) 
    {
    }
}

// SWI Handler -----------------------------------------------------------------

void __attribute__((naked)) swi_handler()
{
    __asm("stmfd sp!, {r0-r12, lr}");
	int syscallNumber = -1;

	// Récupération dans le registre r0 du numero de l'appel système
	__asm("mov %0, r0" : "=r"(syscallNumber): :"r0");

	switch (syscallNumber)
	{
		case 1:
			do_sys_reboot();
			break;

        case 2:
            do_sys_nop();
            break;

        case 3:
            do_sys_settime();
            break;

		default:
			PANIC();
            break;
	}

    __asm("ldmfd sp!, {r0-r12, pc}^");
}
