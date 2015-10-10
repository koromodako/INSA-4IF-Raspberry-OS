#include "syscall.h"
#include "util.h"
#include "hw.h"
#include "sched.h"
#include <stdint.h>

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
    // On découpe date_ms pour le mettre dans le registre 1 et 2
    uint32_t mostSignificantBits = (uint32_t)(date_ms >> 32);
    uint32_t leastSignificantBits = (uint32_t)(date_ms);
    __asm("mov r2, %0" : : "r"(mostSignificantBits) : "r2", "r1", "r0");
    __asm("mov r1, %0" : : "r"(leastSignificantBits) : "r2", "r1", "r0");

    // Positionne le numéro de l'appel système dans r0 : numéro = 3
    __asm("mov r0, #3");

    // Interruption 
    __asm("swi #0");
}

void do_sys_settime(void * stack_pointer)
{
    // stack_pointer est sur le numéro d'appel système
    // date_ms est sur les deux autres segments suivants
    // (deux segments car uint64_t en utilise deux)
    uint32_t leastSignificantBits = *((uint32_t*)(stack_pointer + SIZE_OF_STACK_SEG));
    uint32_t mostSignificantBits = *((uint32_t*)(stack_pointer + 2 * SIZE_OF_STACK_SEG));
    uint64_t date_ms = (uint64_t) mostSignificantBits << 32 | leastSignificantBits;

    // On applique le paramètre
    set_date_ms(date_ms);

    return;
}

// Appel système : gettime -----------------------------------------------------
uint64_t sys_gettime()
{
    // Positionne le numéro de l'appel système dans r0 : numéro = 4
    __asm("mov r0, #4": : : "r0");

    // Interruption 
    __asm("swi #0");

    // Il faut reconstruire date_ms avec R0 (bits de poids fort)
    // et R1 (bits de poids faible)
    uint32_t leastSignificantBits;
    uint32_t mostSignificantBits;
    __asm("mov %0, r0" : "=r"(mostSignificantBits) : : "r0", "r1");
    __asm("mov %0, r1" : "=r"(leastSignificantBits) : : "r0", "r1");

    return (uint64_t) mostSignificantBits << 32 | leastSignificantBits;
}

void do_sys_gettime(void * stack_pointer)
{
    // On récupère date_ms
    uint64_t date_ms = get_date_ms();
    
    // stack_pointer est sur le numéro d'appel système donc R0
    // On place alors dans le futur R0 les bits de poids fort
    *((uint32_t*)(stack_pointer)) = (uint32_t)(date_ms >> 32);
    // Puis dans le futur R1 les bits de poids faible
    *((uint32_t*)(stack_pointer + SIZE_OF_STACK_SEG)) = (uint32_t)(date_ms);

    return;
}

// SWI Handler -----------------------------------------------------------------

void __attribute__((naked)) swi_handler()
{
    // Sauvegarde des registres et de LR
    __asm("stmfd sp!, {r0-r12, lr}");

    // Récupération du pointeur de pile après la sauvegarde
    void * stack_pointer;
    __asm("mov %0, sp" : "=r"(stack_pointer));

    // Numéro d'appel système
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
            do_sys_settime(stack_pointer);
            break;

        case 4:
            do_sys_gettime(stack_pointer);
            break;

        case 5:
            do_sys_yieldto(stack_pointer);
            break;

        default:
            PANIC();
            break;
    }

    __asm("ldmfd sp!, {r0-r12, pc}^");
}
