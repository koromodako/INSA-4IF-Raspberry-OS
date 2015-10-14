#include "sched.h"
#include "syscall.h"
#include "kheap.h"
#include "hw.h"
#include "util.h"

struct pcb_s kmain_process;

void sched_init()
{
    // Initialisation de la mémoire
    kheap_init();

    // Initialisation du premier processus
    kmain_process.state = RUNNING;
    current_process = &kmain_process;
    current_process->pcb_next = &kmain_process;
}

struct pcb_s * create_process(func_t* entry)
{
    struct pcb_s * pcb = (struct pcb_s *) kAlloc(sizeof(struct pcb_s));
    pcb->lr_user = entry;

    // Pile de 2500 uint32_t
    pcb->sp_start = (uint32_t *) kAlloc(SIZE_STACK_PROCESS);
    pcb->sp = pcb->sp_start + SIZE_STACK_PROCESS + 1;

    // Initialisation du champ SPSR
    pcb->cpsr = 0x600001d0; // Valeur du SPSR en mode USER

    // Ajout de la pcb à la liste chaînée
    struct pcb_s * nextProc = current_process->pcb_next;
    pcb->pcb_next = nextProc;
    current_process->pcb_next = pcb;

    pcb->state = READY;

    return pcb;
}

void elect()
{
    // On change l'état du processus courant sauf s'il termine
    if(current_process->state != TERMINATED)
    {
        current_process->state = READY;
    }

    // On supprime tous les processus terminés devant le processus courant
    while(current_process->pcb_next && current_process->pcb_next->state == TERMINATED) {

        // Sauvegarde du pcb à détruire
        struct pcb_s * pcbToDestroy = current_process->pcb_next;

        // Suppression du pcb de la liste
        current_process->pcb_next = pcbToDestroy->pcb_next;

        // Destruction de la pile
        kFree((void *)pcbToDestroy->sp_start, SIZE_STACK_PROCESS);

        // Destruction du pcb
        kFree((void *)pcbToDestroy, sizeof(struct pcb_s));

    }

    // S'il n'y a plus de processus on stop le noyau
    if (current_process->pcb_next == current_process)
    {
        terminate_kernel();
    }

    // Passage au processus suivant avec changement de son l'état
    current_process = current_process->pcb_next;
    current_process->state = RUNNING;
}

// Appel système : yieldto -----------------------------------------------------
void sys_yieldto(struct pcb_s* dest)
{
    // On place dest dans le registre R1
    __asm("mov r1, r0");

    // Positionne le numéro de l'appel système dans r0 : numéro = 5
    __asm("mov r0, #5": : : "r1", "r0");

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

    current_process = dest;
}

// Appel système : yield -----------------------------------------------------
void sys_yield()
{
    // Positionne le numéro de l'appel système dans r0 : numéro = 6
    __asm("mov r0, #6" : : : "r0");

    // Interruption
    __asm("swi #0");
}

void do_sys_yield(struct pcb_s * context)
{
    // Sauvegarde
    for (int i = 0; i < NB_SAVED_REGISTERS; ++i)
    {
        current_process->registres[i] = context->registres[i];
    }

    elect();

    // Restauration
    for (int i = 0; i < NB_SAVED_REGISTERS; ++i)
    {
        context->registres[i] = current_process->registres[i];
    }
}

// Appel système : exit --------------------------------------------------------
void sys_exit(int status)
{
    // On place status dans le registre R1
    __asm("mov r1, r0");

    // Positionne le numéro de l'appel système dans r0 : numéro = 7
    __asm("mov r0, #7": : : "r1", "r0");
}

void do_sys_exit(struct pcb_s * context)
{
    current_process->state = TERMINATED;
    current_process->exit_code = context->registres[1];

    elect();

    // Restauration
    for (int i = 0; i < NB_SAVED_REGISTERS; ++i)
    {
        context->registres[i] = current_process->registres[i];
    }
}
