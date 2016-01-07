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
/* 
    Varaiable de memorisation de la politique d'ordonnancement choisie
*/
SCHEDULING_POLICY sched_policy = 0x00;
/*
    PCB du process kmain
*/
pcb_s kmain_pcb;
/*
    Protection d'initialisation
*/
int sched_init_locker = 0;

// Fonctions -----------------------------------------------------

void sched_init(SCHEDULING_POLICY schedPolicy)
{
    if(sched_init_locker)
    {   log_err("Sheduler initialization called twice !");
        log_cr();
    }

    // Initialisation de la memoire
    kheap_init();
    log_nfo("Kernel heap initialized.");
    log_cr();
#ifdef USE_VMEM
    // Initialisation du pcb de kmain avec la table des pages
    kmain_pcb.page_table = (uint32_t*)vmem_init();
    log_nfo("Virtual memory initialized.");
    log_cr();
#endif

    // Initialisation de la politique d'ordonnancement
    sched_policy = schedPolicy;

    // Initialisation du scheduler associe
    switch(sched_policy) {
        case SP_SIMPLE: 
            log_nfo("Scheduling policy : simple.");
            log_cr();
            current_process = simple_sched_init(&kmain_pcb);
            break;
        case SP_PRIORITY:
            log_nfo("Scheduling policy : priority.");
            log_cr();
            current_process = priority_sched_init(&kmain_pcb);
            break;
    }
    // Activation de la protection d'initialisation
    sched_init_locker = 1;
}

pcb_s * create_process(func_t* entry, PROCESS_PRIORITY priority)
{
    if(priority < 0x0 || priority > PRIORITY_COUNT-1)
    {   // Valeur de priorite illegale
        terminate_kernel();
    }

    // le PCB est stocke dans l'espace memoire reserve au noyau donc on garde kAlloc ici
    pcb_s * pcb = (pcb_s *) kAlloc(sizeof(pcb_s)); 

    pcb->lr_user = (func_t *) &start_current_process;
    pcb->lr_svc = (func_t *) &start_current_process;

    // initialisation des champs parametrables
    pcb->entry = entry;
    pcb->priority = priority;
    
#ifdef USE_VMEM
    // initialisation de la table de page du processus
    pcb->page_table = (uint32_t*)init_ps_translation_table();
    // On alloue dans la zone mémoire kernel avec kAlloc
    pcb->sp_start = (uint32_t *) vmem_alloc_in_userland(pcb, SIZE_STACK_PROCESS);
#else
    // Null translation base pour la table des pages
    pcb->page_table = (uint32_t*)NULL;
    // On alloue dans la zone mémoire kernel avec kAlloc
    pcb->sp_start = (uint32_t*) kAlloc(SIZE_STACK_PROCESS);
#endif
    // on replace le pointeur de pile au depart de cette derniere
    pcb->sp = (uint32_t*)((uint32_t)(pcb->sp_start) + SIZE_STACK_PROCESS + 1);

    // initialisation du champ SPSR
    pcb->cpsr = 0b10000; // Valeur du SPSR en mode USER

    log_nfo("New process created, entry=");
    log_int((int)entry);
    log_cr();

    // ajout de la pcb aux structures de scheduling
    switch(sched_policy) {
        case SP_SIMPLE: 
            simple_sched_add(pcb);
            break;
        case SP_PRIORITY:
            priority_sched_add(pcb);
            break;
    }
    pcb->state = PS_READY;
    log_nfo("Process scheduled for execution.");
    log_cr();

    return pcb;
}

void elect(void)
{
    // On change l'etat du processus courant sauf s'il termine
    if(current_process->state != PS_TERMINATED)
    {
        current_process->state = PS_READY;
    }

    // On supprime tous les processus termines devant le processus courant
    // Verification de terminaison de kernel puis election si necessaire
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

// Appel systeme : yieldto -----------------------------------------------------
void sys_yieldto(pcb_s* dest)
{
    // On place dest dans le registre R1
    __asm("mov r1, r0");

    // Positionne le numero de l'appel systeme dans r0 : numero = 5
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

// Appel systeme : yield -----------------------------------------------------
void sys_yield()
{
    // Positionne le numero de l'appel systeme dans r0 : numero = 6
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
}

// Appel systeme : exit --------------------------------------------------------
void sys_exit(int status)
{
    // On place status dans le registre R1
    __asm("mov r1, r0");

    // Positionne le numero de l'appel systeme dans r0 : numero = 7
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

    // Recuperation du pointeur de pile apres la sauvegarde
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

    // Remise a zero du compteur
    set_next_tick_default();
    ENABLE_TIMER_IRQ();

    UNSTACK_REGS;
}
