#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "sched.h"

// Macros ----------------------------------------------------------------------
#define QEMU // Mode d'execution
#define SIZE_OF_STACK_SEG sizeof(uint32_t*) // Taille d'un élement dans la pile

// Appel système : reboot ------------------------------------------------------
/**
 *	Appel système pour rebooter l'OS
 */
void sys_reboot();
/**
 *	Appel noyau pour rebooter l'OS
 */
void do_sys_reboot();

// Appel système : nop ---------------------------------------------------------
/**
 *	Appel système pour ne rien faire
 */
void sys_nop();
/**
 *	Appel noyau pour ne rien faire
 */
void do_sys_nop();

// Appel système : settime -----------------------------------------------------
/**
 *	Appel système pour paramétrer le temps
 */
void sys_settime(uint64_t date_ms);
/**
 *	Appel noyau pour paramétrer le temps
 */
void do_sys_settime(pcb_s * context);

// Appel système : gettime -----------------------------------------------------
/**
 *	Appel système pour recupérer le temps
 */
uint64_t sys_gettime();
/**
 *	Appel noyau pour recupérer le temps	
 */
void do_sys_gettime(pcb_s * context);

// Handler des appels systèmes -------------------------------------------------
/**
 *	SWI_HANDLER
 */
void swi_handler();

#endif //SYSCALL_H
