#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>
#define NB_SAVED_REGISTERS 13
#define SIZE_STACK_PROCESS 10000 // En Octet = 10Ko

typedef int (func_t) (void);

struct pcb_s {
    uint32_t registres[NB_SAVED_REGISTERS];
    func_t * lr_user;
    func_t * lr_svc;
    uint32_t * sp_start;
    uint32_t * sp;
    uint32_t cpsr;
    struct pcb_s * pcb_next;
};

struct pcb_s * current_process;

void sched_init();

void sys_yieldto(struct pcb_s* dest);
void do_sys_yieldto(struct pcb_s * context);
struct pcb_s * create_process(func_t* entry);
void elect();
void sys_yield();
void do_sys_yield(struct pcb_s * context);

#endif //SCHED_H
