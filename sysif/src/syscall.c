#include "syscall.h"
#include "util.h"
#include "hw.h"
#include "sched.h"
#include "asm_tools.h"
#include "vmem.h"
#include <stdint.h>

// Appel systeme : reboot ------------------------------------------------------
void sys_reboot() 
{
    // Positionne le numero de l'appel systeme dans r0 : numero = 1
    SWI(SCI_REBOOT);
}

void do_sys_reboot()
{
    // Reboot pour l'emulateur
    #if defined QEMU
        __asm("bl 0x8000");
    // Reboot pour le Raspberry Pi
    #else
        Set32(PM_WDOG, PM_PASSWORD | 1);
        Set32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
        while(1);
    #endif
}

// Appel systeme : nop ---------------------------------------------------------
void sys_nop()
{
    // Positionne le numero de l'appel systeme dans r0 : numero = 2
    SWI(SCI_NOP);
}

void do_sys_nop()
{
    return;
}

// Appel systeme : settime -----------------------------------------------------
void sys_settime(uint64_t date_ms)
{
    // On decoupe date_ms pour le mettre dans le registre 1 et 2
    uint32_t mostSignificantBits = (uint32_t)(date_ms >> 32);
    uint32_t leastSignificantBits = (uint32_t)(date_ms);
    __asm("mov r2, %0" : : "r"(mostSignificantBits) : "r2", "r1", "r0");
    __asm("mov r1, %0" : : "r"(leastSignificantBits) : "r2", "r1", "r0");

    // Positionne le numero de l'appel systeme dans r0 : numero = 3
    SWI(SCI_SETTIME);
}

void do_sys_settime(pcb_s * context)
{
    // date_ms est dans R1 et R2
    // (deux segments car uint64_t en utilise deux)
    uint32_t leastSignificantBits = context->registres[1];
    uint32_t mostSignificantBits = context->registres[2];
    uint64_t date_ms = (uint64_t) mostSignificantBits << 32 | leastSignificantBits;

    // On applique le parametre
    set_date_ms(date_ms);

    return;
}

// Appel systeme : gettime -----------------------------------------------------
uint64_t sys_gettime()
{
    // Positionne le numero de l'appel systeme dans r0 : numero = 4
    __asm("mov r1, r0"); // On sauvegarde r0 dans r1 pour liberer l'espace pour mettre le numero de l'appel dans r0
    SWI(SCI_GETTIME);

    // Il faut reconstruire date_ms avec R0 (bits de poids fort)
    // et R1 (bits de poids faible)
    uint32_t leastSignificantBits;
    uint32_t mostSignificantBits;
    __asm("mov %0, r0" : "=r"(mostSignificantBits) : : "r0", "r1");
    __asm("mov %0, r1" : "=r"(leastSignificantBits) : : "r0", "r1");

    return (uint64_t) mostSignificantBits << 32 | leastSignificantBits;
}

void do_sys_gettime(pcb_s * context)
{
    // On recupere date_ms
    uint64_t date_ms = get_date_ms();
    
    // stack_pointer est sur le numero d'appel systeme donc R0
    // On place alors dans le futur R0 les bits de poids fort
    context->registres[0] = (uint32_t)(date_ms >> 32);
    // Puis dans le futur R1 les bits de poids faible
    context->registres[1] = (uint32_t)(date_ms);

    return;
}

// SWI Handler -----------------------------------------------------------------

void __attribute__((naked)) swi_handler()
{
    // Stop des interruptions IRQ
    DISABLE_IRQ();

    // Sauvegarde des registres et de LR
    STACK_REGS;

    // Recuperation du pointeur de pile apres la sauvegarde
    pcb_s * context;
    __asm("mov %0, sp" : "=r"(context));

    // On veut sauvegarder SPSR
    __asm("mrs %0, spsr" : "=r"(current_process->cpsr));

    // Sauvegarde du LR_USER et SP_USER
    SWITCH_TO_SYSTEM_MODE;
    __asm("mov %0, lr" : "=r"(current_process->lr_user));
    __asm("mov %0, sp" : "=r"(current_process->sp));
    SWITCH_TO_SVC_MODE;

    // Sauvegarde de LR_SWI dans LR_SVC
    current_process->lr_svc = context->lr_user;

    // Numero d'appel systeme
    int syscallId = context->registres[0];

    switch (syscallId)
    {
        case SCI_REBOOT: do_sys_reboot(); break;
        case SCI_NOP: do_sys_nop(); break;
        case SCI_SETTIME: do_sys_settime(context); break;
        case SCI_GETTIME: do_sys_gettime(context); break;
        case SCI_YIELDTO: do_sys_yieldto(context); break;
        case SCI_YIELD: do_sys_yield(context); break;
        case SCI_EXIT: do_sys_exit(context); break;
        case SCI_MMAP: do_sys_mmap(context); break;
        case SCI_MUNMAP: do_sys_munmap(context); break;
        default: PANIC(); break;
    }

    // Restauration LR_SWI
    context->lr_user = current_process->lr_svc;

    // Restauration de SP_USER (pas LR_USER car c'est toujours le même)
    SWITCH_TO_SYSTEM_MODE;
    __asm("mov lr, %0" : : "r"(current_process->lr_user));
    __asm("mov sp, %0" : : "r"(current_process->sp));
    SWITCH_TO_SVC_MODE;

    // On restaure SPSR
    __asm("msr spsr, %0" : : "r"(current_process->cpsr));

#ifdef USE_VMEM
    if(syscallId == SCI_YIELDTO || syscallId == SCI_YIELD) 
    {   // Invalidate TLB
        __asm("mcr p15, 0, r0, c8, c6, 0");
        // Configure MMU with new page table
        configure_mmu_C((unsigned int) current_process->page_table);
    }
#endif

    // Remise en fonction de l'IRQ
    ENABLE_IRQ();

    UNSTACK_REGS;
}
