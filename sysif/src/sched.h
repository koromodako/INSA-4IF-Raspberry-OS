#include <stdint.h>

struct pcb_s {
    uint32_t registres[13];
    uint32_t lr;
};

void sys_yieldto(struct pcb_s* dest);
void do_sys_yieldto();
