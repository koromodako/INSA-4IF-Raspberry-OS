#include <stdint.h>
#define NB_SAVED_REGISTERS 13

struct pcb_s {
    uint32_t registres[NB_SAVED_REGISTERS];
    uint32_t lr;
};

void sched_init();

void sys_yieldto(struct pcb_s* dest);
void do_sys_yieldto();
