#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>
#include "util.h"

// Macros ----------------------------------------------------------------------
#define NB_SAVED_REGISTERS 13
#define SIZE_STACK_PROCESS 10240 // En Octet = 10Ko

#define FREE_PS(cell) \
    vmem_free((uint8_t*)(cell->pcb->sp_start), (cell->pcb), SIZE_STACK_PROCESS); \
    kFree((void*)(cell->pcb), sizeof(pcb_s)); \
    kFree((void*)(cell), sizeof(SimpleCell))


// Types et structures ---------------------------------------------------------
typedef int (func_t) (void);

struct pcb_s {
    // Internal memory and stack management
    uint32_t registres[NB_SAVED_REGISTERS];
    func_t * lr_user;
    func_t * lr_svc;
    uint32_t * sp_start;
    uint32_t * sp;
    uint32_t cpsr;
    uint32_t * page_table;
    // Process entry point and exit value
    func_t * entry;
    int exit_code;
    // Process meta-data
    PROCESS_STATE state;
    PROCESS_PRIORITY priority;
};
typedef struct pcb_s pcb_s;

// Variables globales ----------------------------------------------------------
// Pointeur sur le processus en cours d'execution
pcb_s * current_process;

// Gestion des processus -------------------------------------------------------
/**
 *  Initialise les schedulers
 */
void sched_init(SCHEDULING_POLICY schedPolicy);
/**
 *  Cree un nouveau processus en allouant de la memoire pour ce dernier
 */
pcb_s * create_process(func_t* entry, PROCESS_PRIORITY priority);
/**
 *  Realise l'election d'un nouveau processus
 */
void elect(void);
/**
 *  Demarre le processus courant
 */
void start_current_process(void);
// Appel systeme : yieldto -----------------------------------------------------
/**
 *  Appel systeme pour passer a un autre process si on connait l'adresse de son PCB
 */
void sys_yieldto(pcb_s* dest);
/**
 * Appel noyau pour passer a un autre process si on connait l'adresse de son PCB 
 */
void do_sys_yieldto(pcb_s * context);

// Appel systeme : yield -------------------------------------------------------
/**
 *  Appel systeme pour passer a un autre process (rendre la main)
 */
void sys_yield(void);
/**
 *  Appel noyau pour passer a un autre process (rendre la main)
 */
void do_sys_yield(pcb_s * context);

// Appel systeme : exit --------------------------------------------------------
/**
 *  Appel systeme pour terminer un processus
 */
void sys_exit(int status);
/**
 *  Appel noyau pour terminer un processus
 */
void do_sys_exit(pcb_s * context);

// Handler des timer systeme ---------------------------------------------------
/**
 *
 */
void irq_handler(void);

#endif //SCHED_H
