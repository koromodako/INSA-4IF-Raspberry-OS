#ifndef PRIORITY_SCHED_H
#define PRIORITY_SCHED_H

#include "sched.h"

/**
 *	Cette procedure initialise le scheduler simple
 */
pcb_s * priority_sched_init(pcb_s * kmain_pcb);
/**
 *	Cette procedure elit un nouveau processus et supprime les processus termines
 */
pcb_s * priority_sched_elect(void);
/**
 *	Cette procedure enregistre un nouveau processus aupres du scheduler
 */
void priority_sched_add(pcb_s * newProcess);

#endif //PRIORITY_SCHED_H 