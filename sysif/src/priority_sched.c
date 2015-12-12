#include "priority_sched.h"
#include "kheap.h"
#include "hw.h"

// Types -------------------------------------------------------
struct SimpleCell {
	struct SimpleCell * next;
	pcb_s * pcb;
};
typedef struct SimpleCell SimpleCell;

typedef struct {
	SimpleCell cell[PRIORITY_COUNT];
	int size[PRIORITY_COUNT];
} PriorityQueues;

// Globales ----------------------------------------------------
// Conteneur de files de priorité
PriorityQueues pqueues;
// Pointeur sur la cellule du processus courant
SimpleCell * current_cell;
// Pcb du process de base
pcb_s kmain_process;
// Compteur de tours
int loop_counter;

// Procédures privées ------------------------------------------
void priority_sched_clean(void)
{
	// On libère tant que le process suivant le process courant est terminé
	while(current_cell->next != current_cell &&
		  current_cell->next->pcb->state == PS_TERMINATED)
	{
		// Mémorisation de la cellule du processus à détruire
		SimpleCell * cell = current_cell->next;
		// Modification du lien
		current_cell->next = current_cell->next->next;
		// On décrémente la taille de la file
		pqueues.size[cell->pcb->priority]--;
		// Libération de la mémoire allouée au processus
		FREE_PCB(cell->pcb);
		// Libération de la cellule
		kFree((void*)cell, sizeof(SimpleCell));
	}
	// Test de terminaison du kernel
	int p;
	for( p=0; p < PRIORITY_COUNT; ++p )
	{	// Si on trouve une file non vide
		if(pqueues.size[p] != 0)
		{	// On interrompt la procédure de terminaison
			return;
		}
	}
	// Si on a pas été interrompu on demande la terminaison du noyau
	terminate_kernel();
}

void next_priority()
{
	// On calcule la prochaine priorité
	PROCESS_PRIORITY nextPriority = (current_cell->pcb->priority+1 % PRIORITY_COUNT);
	
	// Modification dynamique de la priorité de kmain pour que la sentinelle est toujours la même priorité que la file qu'elle représente
	// Rappel : 
	//   Le pcb de kmain est pointé par toutes les sentinelles selon l'implémentation actuelle
	kmain_process.priority = nextPriority;
	
	// On passe à la priorité suivante
	current_cell = &(pqueues.cell[nextPriority]);
	
	// RAZ du compteur de tours
	loop_counter = 0;
}

void next_process()
{
	current_cell = current_cell->next;
}

// Procédures publiques ----------------------------------------

/**
 *	Cette procédure initialise le scheduler simple
 */
pcb_s * priority_sched_init(void)
{
	int p;
	for( p=0; p < PRIORITY_COUNT; ++p )
	{
		// On lie la sentinelle à elle même
		pqueues.cell[p].next = &(pqueues.cell[p]);
		// On initialise le pcb de la sentinelle sur le processu point d'entrée du noyau
		pqueues.cell[p].pcb = &kmain_process;
		// On initialise la taille de la file à 0 même si toutes les sentinelles pointent vers le pcb de kmain
		pqueues.size[p] = 0;
	}
	// On initialise la cellule courante sur le pcb du point d'entrée
	current_cell = &(pqueues.cell[PP_KERNEL]);
	// On initialise le compteur de tours
	loop_counter = 0;
	// On retourne le pcb de la cellule courante donc celle du point d'entrée
	return current_cell->pcb;
}
/**
 *	Cette procédure élit un nouveau processus et supprime les processus terminés
 */
pcb_s * priority_sched_elect(void)
{
	// On nettoye les liste de processus
	priority_sched_clean();
	// Cas fin de liste : 
	// cellule suivante = sentinelle ET cellule courante != sentinelle
	if( current_cell->next == 
		&(pqueues.cell[current_cell->pcb->priority]) 
		&&
		pqueues.size[current_cell->pcb->priority] != 0 )
	{
		// On incrémente le compteur de tours
		loop_counter++;
		// Si le compteur de tour dépasse la valeur (nb_de_priorité - priorité_courante)
		if( loop_counter >= 
			(PRIORITY_COUNT - current_cell->pcb->priority) )
		{
			next_priority();
		}
		// Sinon on passe au processus suivant dans la liste
		else
		{
			next_process();
		}
	}
	// Cas liste vide : 
	// cellule suivante = sentinelle ET cellule courante = sentinelle
	else if( current_cell->next == current_cell 
		     &&
		     pqueues.size[current_cell->pcb->priority] == 0 )
	{
		next_priority();
	}
	// Autres cas : 
	// cellule suivante != sentinelle
	else
	{	
		next_process();
	}
	// On retourne le nouveau pcb élu
	return current_cell->pcb;
}
/**
 *	Cette procédure enregistre un nouveau processus auprès du scheduler
 */
void priority_sched_add(pcb_s * newProcess)
{
	// On alloue une nouvelle cellule
	SimpleCell * cell = (SimpleCell*)kAlloc(sizeof(SimpleCell));
	// On affecte le nouveau process à cette cellule
	cell->pcb = newProcess;
	// On ajoute la nouvelle cellule au début de la file de priorité correspondante
	cell->next = pqueues.cell[newProcess->priority].next;
	pqueues.cell[newProcess->priority].next = cell;
	// On incrémente la taille de la file en question
	pqueues.size[newProcess->priority]++;
}