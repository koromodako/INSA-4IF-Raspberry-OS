#ifndef SIMPLE_SCHED_H
#define SIMPLE_SCHED_H

#include "sched.h"

/**
 *	Cette procedure initialise le scheduler simple
 */
pcb_s * simple_sched_init(pcb_s * kmain_pcb);
/**
 *	Cette procedure elit un nouveau processus et supprime les processus termines
 */
pcb_s * simple_sched_elect(void);
/**
 *	Cette procedure enregistre un nouveau processus aupres du scheduler
 */
void simple_sched_add(pcb_s * newProcess);

#endif //SIMPLE_SCHED_H 