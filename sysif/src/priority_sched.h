#ifndef PRIORITY_SCHED_H
#define PRIORITY_SCHED_H

#include "sched.h"

/**
 *	Cette procédure initialise le scheduler simple
 */
pcb_s * priority_sched_init(void);
/**
 *	Cette procédure élit un nouveau processus et supprime les processus terminés
 */
pcb_s * priority_sched_elect(void);
/**
 *	Cette procédure enregistre un nouveau processus auprès du scheduler
 */
void priority_sched_add(pcb_s * newProcess);

#endif //PRIORITY_SCHED_H 