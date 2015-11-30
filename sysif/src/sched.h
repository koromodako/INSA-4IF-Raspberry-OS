#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>
#include "util.h"

// Macros ----------------------------------------------------------------------
#define NB_SAVED_REGISTERS 13
#define SIZE_STACK_PROCESS 10000 // En Octet = 10Ko

// Types et structures ---------------------------------------------------------
typedef int (func_t) (void);

struct pcb_s {
    uint32_t registres[NB_SAVED_REGISTERS];
    func_t * lr_user;
    func_t * lr_svc;
    uint32_t * sp_start;
    uint32_t * sp;
    uint32_t cpsr;
    struct pcb_s * pcb_next;
    int state;
    int priority;
    int exit_code;
    func_t * entry;
};

// Variables globales ----------------------------------------------------------
struct pcb_s * current_process;

// Gestion des processus -------------------------------------------------------
/**
 *  Initialise les schedulers
 */
void sched_init(SCHEDULING_POLICY schedPolicy);
void queue_sched_init();
void priority_queue_sched_init();
/**
 *  Crée un nouveau processus en allouant de la mémoire pour ce dernier
 */
struct pcb_s * create_process(func_t* entry, PROC_PRIORITY priority);
void queue_sched_add(struct pcb_s * newProcess);
void priority_queue_sched_add(struct pcb_s * newProcess);
/**
 *  Réalise l'election d'un nouveau processus
 */
void elect();
struct pcb_s * queue_sched_elect();
struct pcb_s * priority_queue_sched_elect();
/**
 *  Démarre le processus courant
 */
void start_current_process();

// Appel système : yieldto -----------------------------------------------------
/**
 *  Appel système pour passer a un autre process si on connait l'adresse de son PCB
 */
void sys_yieldto(struct pcb_s* dest);
/**
 * Appel noyau pour passer a un autre process si on connait l'adresse de son PCB 
 */
void do_sys_yieldto(struct pcb_s * context);

// Appel système : yield -------------------------------------------------------
/**
 *  Appel système pour passer a un autre process (rendre la main)
 */
void sys_yield();
/**
 *  Appel noyau pour passer a un autre process (rendre la main)
 */
void do_sys_yield(struct pcb_s * context);

// Appel système : exit --------------------------------------------------------
/**
 *  Appel système pour terminer un processus
 */
void sys_exit(int status);
/**
 *  Appel noyau pour terminer un processus
 */
void do_sys_exit(struct pcb_s * context);

// Handler des timer système ---------------------------------------------------
/**
 *
 */
void irq_handler();

#endif //SCHED_H
