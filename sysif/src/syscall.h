#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "sched.h"

#define QEMU

// Taille d'un élement dans la pile
#define SIZE_OF_STACK_SEG sizeof(uint32_t*)

void sys_reboot();
void do_sys_reboot();

void sys_nop();
void do_sys_nop();

void sys_settime(uint64_t date_ms);
void do_sys_settime(struct pcb_s * context);

uint64_t sys_gettime();
void do_sys_gettime(struct pcb_s * context);

void swi_handler();

#endif //SYSCALL_H
