#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "sched.h"

// Macros ----------------------------------------------------------------------
#define QEMU // Mode d'execution
#define SIZE_OF_STACK_SEG sizeof(uint32_t*) // Taille d'un élement dans la pile

// Appel système : reboot ------------------------------------------------------
void sys_reboot();
void do_sys_reboot();

// Appel système : nop ---------------------------------------------------------
void sys_nop();
void do_sys_nop();

// Appel système : settime -----------------------------------------------------
void sys_settime(uint64_t date_ms);
void do_sys_settime(struct pcb_s * context);

// Appel système : gettime -----------------------------------------------------
uint64_t sys_gettime();
void do_sys_gettime(struct pcb_s * context);

// Handler des appels systèmes -------------------------------------------------
void swi_handler();

#endif //SYSCALL_H
