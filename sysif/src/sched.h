#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

// Macros ----------------------------------------------------------------------
#define NB_SAVED_REGISTERS 13
#define SIZE_STACK_PROCESS 10000 // En Octet = 10Ko

#define RUNNING 2
#define READY 1
#define TERMINATED 0

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
    int exit_code;
    func_t * entry;
};

// Variables globales ----------------------------------------------------------
struct pcb_s * current_process;

// Gestion des processus -------------------------------------------------------
void sched_init();
struct pcb_s * create_process(func_t* entry);
void elect();
void start_current_process();

// Appel système : yieldto -----------------------------------------------------
void sys_yieldto(struct pcb_s* dest);
void do_sys_yieldto(struct pcb_s * context);

// Appel système : yield -------------------------------------------------------
void sys_yield();
void do_sys_yield(struct pcb_s * context);

// Appel système : exit --------------------------------------------------------
void sys_exit(int status);
void do_sys_exit(struct pcb_s * context);

// Handler des timer système ---------------------------------------------------
void irq_handler();

#endif //SCHED_H
