#include "syscall.h"
#include "util.h"
#include "hw.h"
#include "sched.h"
#include <stdint.h>

// Appel système : reboot ------------------------------------------------------
void sys_reboot() 
{
    // Positionne le numéro de l'appel système dans r0 : numéro = 1
    SWI(1);
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
    SWI(2);
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
    SWI(3);
}

void do_sys_settime(struct pcb_s * context)
{
    // date_ms est dans R1 et R2
    // (deux segments car uint64_t en utilise deux)
    uint32_t leastSignificantBits = context->registres[1];
    uint32_t mostSignificantBits = context->registres[2];
    uint64_t date_ms = (uint64_t) mostSignificantBits << 32 | leastSignificantBits;

    // On applique le paramètre
    set_date_ms(date_ms);

    return;
}

// Appel système : gettime -----------------------------------------------------
uint64_t sys_gettime()
{
    // Positionne le numéro de l'appel système dans r0 : numéro = 4
    __asm("mov r1, r0"); // On sauvegarde r0 dans r1 pour libérer l'espace pour mettre le numero de l'appel dans r0
    SWI(4);

    // Il faut reconstruire date_ms avec R0 (bits de poids fort)
    // et R1 (bits de poids faible)
    uint32_t leastSignificantBits;
    uint32_t mostSignificantBits;
    __asm("mov %0, r0" : "=r"(mostSignificantBits) : : "r0", "r1");
    __asm("mov %0, r1" : "=r"(leastSignificantBits) : : "r0", "r1");

    return (uint64_t) mostSignificantBits << 32 | leastSignificantBits;
}

void do_sys_gettime(struct pcb_s * context)
{
    // On récupère date_ms
    uint64_t date_ms = get_date_ms();
    
    // stack_pointer est sur le numéro d'appel système donc R0
    // On place alors dans le futur R0 les bits de poids fort
    context->registres[0] = (uint32_t)(date_ms >> 32);
    // Puis dans le futur R1 les bits de poids faible
    context->registres[1] = (uint32_t)(date_ms);

    return;
}

// SWI Handler -----------------------------------------------------------------

void __attribute__((naked)) swi_handler()
{
    // Sauvegarde des registres et de LR
    __asm("stmfd sp!, {r0-r12, lr}");

    // On veut sauvegarder SPSR
    __asm("mrs %0, spsr" : "=r"(current_process->cpsr));

    // Sauvegarde du LR_USER et SP_USER
    SWITCH_TO_SYSTEM_MODE;
    __asm("mov %0, lr" : "=r"(current_process->lr_user));
    __asm("mov %0, sp" : "=r"(current_process->sp));
    SWITCH_TO_SVC_MODE;

    // Récupération du pointeur de pile après la sauvegarde
    struct pcb_s * context;
    __asm("mov %0, sp" : "=r"(context));

    // Numéro d'appel système
    int syscallNumber = context->registres[0];

    switch (syscallNumber)
    {
        case 1:
            do_sys_reboot();
            break;

        case 2:
            do_sys_nop();
            break;

        case 3:
            do_sys_settime(context);
            break;

        case 4:
            do_sys_gettime(context);
            break;

        case 5:
            do_sys_yieldto(context);
            break;

        case 6:
            do_sys_yield(context);
            break;

        case 7:
            do_sys_exit(context);
            break;

        default:
            PANIC();
            break;
    }

    // Restauration de SP_USER (pas LR_USER car c'est toujours le même)
    SWITCH_TO_SYSTEM_MODE;
    __asm("mov lr, %0" : : "r"(current_process->lr_user));
    __asm("mov sp, %0" : : "r"(current_process->sp));
    SWITCH_TO_SVC_MODE;

    // On restaure SPSR
    __asm("msr spsr, %0" : : "r"(current_process->cpsr));

    __asm("ldmfd sp!, {r0-r12, pc}^");
}
