#include "sched.h"
#include "syscall.h"

struct pcb_s kmain_process;
struct pcb_s * current_process;

void sched_init()
{
    current_process = &kmain_process;
}

// Appel système : yieldto -----------------------------------------------------
void sys_yieldto(struct pcb_s* dest)
{
    // On place dest dans le registre R1
    __asm("mov r1, %0" : : "r"(dest) : "r1");

    // Positionne le numéro de l'appel système dans r0 : numéro = 5
    __asm("mov r0, #5": : : "r1", "r0");

    // Interruption
    __asm("swi #0");
}

void do_sys_yieldto(void * stack_pointer)
{
    // stack_pointer est sur le numéro d'appel système
    // On aura le paramètre à la position suivante
    struct pcb_s** pointer_adr_dest = stack_pointer + SIZE_OF_STACK_SEG;
    struct pcb_s* dest = *(pointer_adr_dest);

    // Copies locales
    uint32_t * sp = stack_pointer;
    for (int i = 0; i < NB_SAVED_REGISTERS; ++i)
    {
        current_process->registres[i] = *((uint32_t*)(sp));
        *((uint32_t*)(sp)) = dest->registres[i];
        sp += SIZE_OF_STACK_SEG;
    }
    current_process->lr = *((uint32_t*)(sp));
    *((uint32_t*)(sp)) = dest->lr;

    current_process = dest;

}
