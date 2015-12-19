#include "sched.h"
#include "syscall.h"
#include "kheap.h"
#include "hw.h"
#include "util.h"
#include "asm_tools.h"
#include "simple_sched.h"
#include "priority_sched.h"
#include "vmem.h"

// Globales -----------------------------------------------------
// Varaiable de mémorisation de la politique d'ordonnancement choisie
SCHEDULING_POLICY sched_policy = 0x00;

void sched_init(SCHEDULING_POLICY schedPolicy)
{
    // Initialisation de la mémoire
    kheap_init();
#ifdef USE_VMEM
    vmem_init();
#endif

    // Initialisation de la politique d'ordonnancement
    sched_policy = schedPolicy;

    // Initialisation du scheduler associé
    switch(sched_policy) {
        case SP_SIMPLE: 
            current_process = simple_sched_init();
            break;
        case SP_PRIORITY:
            current_process = priority_sched_init();
            break;
    }
}

pcb_s * create_process(func_t* entry, PROCESS_PRIORITY priority)
{
    if(priority < 0x0 || priority > PRIORITY_COUNT-1)
    {   // Valeur de priorité illégale
        terminate_kernel();
    }

    
    // On ne peut pas mettre de sys_mmap ici car cela utiliserait le proc courant (kmain)
    // Donc la table du processus courant
    // Creer le PCB et la pile dans l'espace propre du processus <=> difficile 
    pcb_s * pcb = (pcb_s *) kAlloc(sizeof(pcb_s)); 
    
    // Initialisation de la table de page du processus
    pcb->page_table = (uint32_t*)init_ps_translation_table();

    pcb->lr_user = (func_t *) &start_current_process;
    pcb->lr_svc = (func_t *) &start_current_process;

    // Initialisation des champs paramétrables
    pcb->entry = entry;
    pcb->priority = priority;

    // Pile de 2500 uint32_t
    // On ne peut pas mettre de sys_mmap ici car cela utiliserait le proc courant (kmain)
    // Donc la table du processus courant 
    pcb->sp_start = (uint32_t *) kAlloc(SIZE_STACK_PROCESS);
    pcb->sp = pcb->sp_start + SIZE_STACK_PROCESS + 1;

    // Initialisation du champ SPSR
    pcb->cpsr = 0b10000; // Valeur du SPSR en mode USER

    // Ajout de la pcb aux structures de scheduling
    switch(sched_policy) {
        case SP_SIMPLE: 
            simple_sched_add(pcb);
            break;
        case SP_PRIORITY:
            priority_sched_add(pcb);
            break;
    }
    pcb->state = PS_READY;

    return pcb;
}

void elect(void)
{
    // On change l'état du processus courant sauf s'il termine
    if(current_process->state != PS_TERMINATED)
    {
        current_process->state = PS_READY;
    }

    // On supprime tous les processus terminés devant le processus courant
    // Verification de terminaison de kernel puis election si nécessaire
    switch(sched_policy) {
        case SP_SIMPLE:
            current_process = simple_sched_elect();
            break;
        case SP_PRIORITY:
            current_process = priority_sched_elect();
            break;
    }
    // Switch to ruuning state
    current_process->state = PS_RUNNING;
}

void start_current_process(void)
{
    current_process->entry();
    sys_exit(0);
}

// Appel système : yieldto -----------------------------------------------------
void sys_yieldto(pcb_s* dest)
{
    // On place dest dans le registre R1
    __asm("mov r1, r0");

    // Positionne le numéro de l'appel système dans r0 : numéro = 5
    SWI(SCI_YIELDTO);
}

void do_sys_yieldto(pcb_s * context)
{
    // Dest est dans R1
    pcb_s* dest = (pcb_s*) context->registres[1];

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

void do_sys_yield(pcb_s * context)
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

    // Invalidate TLB
    __asm("mcr p15, 0, r0, c8, c6, 0");
    // Configure MMU with new page table
    configure_mmu_C((unsigned int) current_process->page_table);
}

// Appel système : exit --------------------------------------------------------
void sys_exit(int status)
{
    // On place status dans le registre R1
    __asm("mov r1, r0");

    // Positionne le numéro de l'appel système dans r0 : numéro = 7
    SWI(SCI_EXIT);
}

void do_sys_exit(pcb_s * context)
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

void __attribute__((naked)) irq_handler(void)
{
    // Sauvegarde des registres et de LR
    STACK_REGS;

    // Récupération du pointeur de pile après la sauvegarde
    pcb_s * context;
    __asm("mov %0, sp" : "=r"(context));

    // On veut sauvegarder SPSR
    __asm("mrs %0, spsr" : "=r"(current_process->cpsr));

    // Sauvegarde du LR_USER et SP_USER
    SWITCH_TO_SYSTEM_MODE;
    __asm("mov %0, lr" : "=r"(current_process->lr_user));
    __asm("mov %0, sp" : "=r"(current_process->sp));
    SWITCH_TO_IRQ_MODE;

    // Remise au bon endroit du LR (/!\ c'est le LR_IRQ)
    context->lr_user -= 4;

    // Sauvegarde de LR_IRQ dans LR_SVC
    current_process->lr_svc = context->lr_user;

    do_sys_yield(context);

    // Restauration LR_IRQ
    context->lr_user = current_process->lr_svc;

    // Restauration de SP_USER et LR_USER
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
