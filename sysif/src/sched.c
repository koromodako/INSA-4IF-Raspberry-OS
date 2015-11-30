#include "sched.h"
#include "syscall.h"
#include "kheap.h"
#include "hw.h"
#include "util.h"
#include "asm_tools.h"

// Variable globale pour le processus kmain (point d'entrée de l'OS)
struct pcb_s kmain_process;

struct pcb_s * priority_queues[PRIORITY_NB];

SCHEDULING_POLICY sched_policy;

void sched_init(SCHEDULING_POLICY schedPolicy)
{
    // Initialisation de la mémoire
    kheap_init();

    // Initialisation du premier processus
    kmain_process.state = PS_RUNNING;
    current_process = &kmain_process;
    current_process->pcb_next = &kmain_process;

    // Initialisation de la politique de scheduling
    sched_policy = schedPolicy;

    queue_sched_init();
    priority_queue_sched_init();
}

void queue_sched_init() 
{
    // Rien à faire
}

void priority_queue_sched_init() 
{
    int p;
    for (p = 0; p < PRIORITY_NB; ++p)
    {
        priority_queues[p] = (struct pcb_s *) kAlloc(sizeof(struct pcb_s));
        priority_queues[p]->pcb_next = priority_queues[p];
    }
}

struct pcb_s * create_process(func_t* entry, PROC_PRIORITY priority)
{
    if(priority < 0x00 || priority > PRIORITY_NB-1)
    {   
        PANIC();
    }

    struct pcb_s * pcb = (struct pcb_s *) kAlloc(sizeof(struct pcb_s));
    pcb->lr_user = (func_t *) &start_current_process;
    pcb->lr_svc = (func_t *) &start_current_process;

    // Initialisation des champs paramétrables
    pcb->entry = entry;
    pcb->priority = priority;

    // Pile de 2500 uint32_t
    pcb->sp_start = (uint32_t *) kAlloc(SIZE_STACK_PROCESS);
    pcb->sp = pcb->sp_start + SIZE_STACK_PROCESS + 1;

    // Initialisation du champ SPSR
    pcb->cpsr = 0b10000; // Valeur du SPSR en mode USER

    // Ajout de la pcb aux structures de scheduling
    switch(sched_policy) {
        case SP_QUEUE: 
            queue_sched_add(pcb);
            break;
        case SP_PRIORITY_QUEUE:
            priority_queue_sched_add(pcb);
            break;
    }
    pcb->state = PS_READY;

    return pcb;
}

void queue_sched_add(struct pcb_s * newProcess) 
{   // On insère le processus après le processus courant dans la liste
    struct pcb_s * nextProc = current_process->pcb_next;
    newProcess->pcb_next = nextProc;
    current_process->pcb_next = newProcess;
}

void priority_queue_sched_add(struct pcb_s * newProcess) 
{   // On insère le processus au debut de la liste de priorite
    struct pcb_s * nextProc = priority_queues[newProcess->priority]->pcb_next;
    newProcess->pcb_next = nextProc;
    priority_queues[newProcess->priority]->pcb_next = newProcess;
}

void elect()
{
    // On change l'état du processus courant sauf s'il termine
    if(current_process->state != PS_TERMINATED)
    {
        current_process->state = PS_READY;
    }

    // On supprime tous les processus terminés devant le processus courant
    while(current_process->pcb_next && current_process->pcb_next->state == PS_TERMINATED) {

        // Sauvegarde du pcb à détruire
        struct pcb_s * pcbToDestroy = current_process->pcb_next;

        // Suppression du pcb des structures de scheduling
        // /!\ Cette ligne est commune aux politiques d'ordonnacement
        current_process->pcb_next = pcbToDestroy->pcb_next;
        // ---------------------------------------------------------
        
        // Destruction en deux temps car on ne peut pas prévoir comment est gérée la mémoire
        // Destruction de la pile
        kFree((void *)pcbToDestroy->sp_start, SIZE_STACK_PROCESS);
        // Destruction du pcb + la pile allouée
        kFree((void *)pcbToDestroy, sizeof(struct pcb_s));

    }

    // S'il n'y a plus de processus on stop le noyau
    if (current_process->pcb_next == current_process)
    {
        terminate_kernel();
    }

    // Passage au processus suivant avec changement de son état
    switch(sched_policy) {
        case SP_QUEUE: 
            current_process = queue_sched_elect();
            break;
        case SP_PRIORITY_QUEUE:
            current_process = priority_queue_sched_elect();
            break;
    }
    // Switch to ruuning state
    current_process->state = PS_RUNNING;
}

struct pcb_s * queue_sched_elect() 
{
    return  current_process->pcb_next;
}

struct pcb_s * priority_queue_sched_elect()
{
    // Pour chaque niveau de priorité...
    int p;
    for (p = 0; p < PRIORITY_NB; ++p)
    {   // Si la priorité du processus est la même que la liste actuelle
        if(current_process->priority == p)
        {   // Si le processus est seul dans la file et non terminé 
            if (current_process->pcb_next->pcb_next == current_process)
            {
                if(current_process->state != PS_TERMINATED)
                {
                    return current_process;    
                }
            }
            else // Le processus n'est pas tout seul dans la liste
            {   
                if(current_process->pcb_next != priority_queues[p])
                {
                    return current_process->pcb_next;
                }
                else
                {   // On saute la sentinelle
                    return current_process->pcb_next->pcb_next;
                }
            }   
        }
        else
        {
            return priority_queues[p]->pcb_next;
        }
    }
    PANIC(); // Ce point ne doit pas être atteint
    return NULL; 
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
    SWI(SCI_YIELDTO);
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
    SWI(SCI_YIELD);
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
    SWI(SCI_EXIT);
}

void do_sys_exit(struct pcb_s * context)
{
    current_process->state = PS_TERMINATED;
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
    STACK_REGS;

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

    UNSTACK_REGS;
}
