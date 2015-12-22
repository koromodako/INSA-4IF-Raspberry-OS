#ifndef SIMPLE_SCHED_H
#define SIMPLE_SCHED_H

#include "sched.h"

/**
 *	Cette procédure initialise le scheduler simple
 */
pcb_s * simple_sched_init(pcb_s * kmain_pcb);
/**
 *	Cette procédure élit un nouveau processus et supprime les processus terminés
 */
pcb_s * simple_sched_elect(void);
/**
 *	Cette procédure enregistre un nouveau processus auprès du scheduler
 */
void simple_sched_add(pcb_s * newProcess);

#endif //SIMPLE_SCHED_H 