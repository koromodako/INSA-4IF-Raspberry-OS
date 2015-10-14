#include "sched.h"
#include "syscall.h"
#include "kheap.h"

struct pcb_s kmain_process;

void sched_init()
{
    kheap_init();
    current_process = &kmain_process;
}

struct pcb_s * create_process(func_t* entry)
{
    struct pcb_s * pcb = (struct pcb_s *) kAlloc(sizeof(struct pcb_s));
    pcb->lr_user = entry;

    // Pile de 2500 uint32_t
    pcb->sp = (uint32_t *) kAlloc(SIZE_STACK_PROCESS);
    pcb->sp += SIZE_STACK_PROCESS;

    // Initialisation du champ SPSR
    pcb->cpsr_user = 0x600001d0; //Valeur du SPSR en mode USER

    return pcb;
}

// Appel système : yieldto -----------------------------------------------------
void sys_yieldto(struct pcb_s* dest)
{
    // On place dest dans le registre R1
    __asm("mov r1, %0" : : "r"(dest) : "r1", "r0");

    // Positionne le numéro de l'appel système dans r0 : numéro = 5
    __asm("mov r0, #5": : : "r1", "r0");

    // On dépile de deux blocs pour dépiler l'argument dest passé dans la pile
    __asm("add sp, #8");

    // Interruption
    __asm("swi #0");
}

void do_sys_yieldto(struct pcb_s * context)
{
    // Dest est dans R1
    struct pcb_s* dest = (struct pcb_s*) context->registres[1];

    // Sauvegarde
    for (int i = 0; i < NB_SAVED_REGISTERS; ++i)
    {
        current_process->registres[i] = context->registres[i];
        context->registres[i] = dest->registres[i];
    }

    context->lr_user = dest->lr_user;

    current_process = dest;
}
