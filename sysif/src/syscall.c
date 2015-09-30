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

// SWI Handler -----------------------------------------------------------------

void swi_handler()
{
	int syscallNumber = -1;

	// Récupération dans le registre r0 de num"ro de l'appel système
	__asm("mov %0, r0" : "=r"(syscallNumber): :"r0");

	switch (syscallNumber)
	{
		case 1:
			do_sys_reboot();
			break;

        case 2:
            do_sys_nop();
            break;

		default:
			PANIC();
            break;
	}
}
