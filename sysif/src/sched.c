#include "sched.h"
#include "syscall.h"

struct pcb_s * current_process;

// Appel système : yieldto -----------------------------------------------------
void sys_yieldto(struct pcb_s* dest)
{
    // Positionne le numéro de l'appel système dans r0 : numéro = 5
    __asm("mov r0, #5": : : "r0");

    // Interruption
    __asm("swi #0");
}

void do_sys_yieldto(void * stack_pointer)
{
    // stack_pointer est sur le numéro d'appel système
    // On aura le paramètre à la position suivante
    struct pcb_s* dest = *((struct pcb_s**)(stack_pointer + SIZE_OF_STACK_SEG));
    dest = dest + 1;

    // Copies locales
    uint32_t registres[13];
    for (int i = 0; i < 13; ++i)
    {
        registres[i] = dest->registres[i];
    }
    uint32_t lr = dest->lr;

    // Changement de contexte
    uint32_t * sp = (uint32_t*) stack_pointer;
    for (int i = 0; i < 13; ++i)
    {
        (*sp) = registres[i];
        sp += SIZE_OF_STACK_SEG;
    }
    (*sp) = lr;


}
