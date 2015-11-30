#include "sched.h"
#include "syscall.h"
#include "kheap.h"
#include "hw.h"
#include "util.h"
#include "asm_tools.h"

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
    pcb->lr_user = (func_t *) &start_current_process;
    pcb->lr_svc = (func_t *) &start_current_process;

    pcb->entry = entry;

    // Pile de 2500 uint32_t
    pcb->sp_start = (uint32_t *) kAlloc(SIZE_STACK_PROCESS);
    pcb->sp = pcb->sp_start + SIZE_STACK_PROCESS + 1;

    // Initialisation du champ SPSR
    pcb->cpsr = 0b10000; // Valeur du SPSR en mode USER

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

void start_current_process()
{
    current_process->entry();
    sys_exit(0);
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

// IRQ Handler -----------------------------------------------------------------

void __attribute__((naked)) irq_handler()
{
    // Sauvegarde des registres et de LR
    __asm("stmfd sp!, {r0-r12, lr}");

    // On veut sauvegarder SPSR
    __asm("mrs %0, spsr" : "=r"(current_process->cpsr));

    // Sauvegarde du LR_USER et SP_USER
    SWITCH_TO_SYSTEM_MODE;
    __asm("mov %0, lr" : "=r"(current_process->lr_user));
    __asm("mov %0, sp" : "=r"(current_process->sp));
    SWITCH_TO_IRQ_MODE;

    // Récupération du pointeur de pile après la sauvegarde
    struct pcb_s * context;
    __asm("mov %0, sp" : "=r"(context));

    context->lr_user -= 4;

    // Sauvegarde de lr_irq dans lr_svc
    current_process->lr_svc = context->lr_user;

    do_sys_yield(context);

    // Restauration lr_irq
    context->lr_user = current_process->lr_svc;

    // Restauration de SP_USER (pas LR_USER car c'est toujours le même)
    SWITCH_TO_SYSTEM_MODE;
    __asm("mov lr, %0" : : "r"(current_process->lr_user));
    __asm("mov sp, %0" : : "r"(current_process->sp));
    SWITCH_TO_IRQ_MODE;

    // On restaure SPSR
    __asm("msr spsr, %0" : : "r"(current_process->cpsr));

    // Remise à zéro du compteur
    set_next_tick_default();
    ENABLE_TIMER_IRQ();

    __asm("ldmfd sp!, {r0-r12, pc}^");
}
