#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "sched.h"

// Macros ----------------------------------------------------------------------
#define SIZE_OF_STACK_SEG sizeof(uint32_t*) // Taille d'un element dans la pile

// Appel systeme : reboot ------------------------------------------------------
/**
 *	Appel systeme pour rebooter l'OS
 */
void sys_reboot();
/**
 *	Appel noyau pour rebooter l'OS
 */
void do_sys_reboot();

// Appel systeme : nop ---------------------------------------------------------
/**
 *	Appel systeme pour ne rien faire
 */
void sys_nop();
/**
 *	Appel noyau pour ne rien faire
 */
void do_sys_nop();

// Appel systeme : settime -----------------------------------------------------
/**
 *	Appel systeme pour parametrer le temps
 */
void sys_settime(uint64_t date_ms);
/**
 *	Appel noyau pour parametrer le temps
 */
void do_sys_settime(pcb_s * context);

// Appel systeme : gettime -----------------------------------------------------
/**
 *	Appel systeme pour recuperer le temps
 */
uint64_t sys_gettime();
/**
 *	Appel noyau pour recuperer le temps	
 */
void do_sys_gettime(pcb_s * context);

// Handler des appels systemes -------------------------------------------------
/**
 *	SWI_HANDLER
 */
void swi_handler();

#endif //SYSCALL_H
