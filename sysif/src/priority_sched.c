#include "priority_sched.h"
#include "kheap.h"
#include "hw.h"
#include "vmem.h"

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
// Conteneur de files de priorite
PriorityQueues pqueues;
// Pointeur sur la cellule du processus courant
SimpleCell * current_cell;
// Compteur de tours
int loop_counter;

// Procedures privees ------------------------------------------
void priority_sched_clean(void)
{
	// On libere tant que le process suivant le process courant est termine
	while(current_cell->next != current_cell &&
		  current_cell->next->pcb->state == PS_TERMINATED)
	{
		// Memorisation de la cellule du processus a detruire
		SimpleCell * cell = current_cell->next;
		// Modification du lien
		current_cell->next = current_cell->next->next;
		// On decremente la taille de la file
		pqueues.size[cell->pcb->priority]--;
		// Liberation de la memoire allouee pour le processus (pile, pcb, cellule de liste)
		FREE_PS(cell);	
	}
	// Test de terminaison du kernel
	int p;
	for( p=0; p < PRIORITY_COUNT; ++p )
	{	// Si on trouve une file non vide
		if(pqueues.size[p] != 0)
		{	// On interrompt la procedure de terminaison
			return;
		}
	}
	// Si on a pas ete interrompu on demande la terminaison du noyau
	terminate_kernel();
}

void next_priority()
{
	// On calcule la prochaine priorite
	PROCESS_PRIORITY nextPriority = current_cell->pcb->priority+1;
	if(nextPriority >= PRIORITY_COUNT)
	{
	    nextPriority = 0;
	}
	
	// Modification dynamique de la priorite de kmain pour que la sentinelle est toujours la même priorite que la file qu'elle represente
	// Rappel : 
	//   Le pcb de kmain est pointe par toutes les sentinelles selon l'implementation actuelle
	pqueues.cell[0].pcb->priority = nextPriority;
	
	// On passe a la priorite suivante
	current_cell = &(pqueues.cell[nextPriority]);
	
	// RAZ du compteur de tours
	loop_counter = 0;
}

void next_process()
{
	current_cell = current_cell->next;
}

// Procedures publiques ----------------------------------------

/**
 *	Cette procedure initialise le scheduler simple
 */
pcb_s * priority_sched_init(pcb_s * kmain_pcb)
{
	int p;
	for( p=0; p < PRIORITY_COUNT; ++p )
	{
		// On lie la sentinelle a elle même
		pqueues.cell[p].next = &(pqueues.cell[p]);
		// On initialise le pcb de la sentinelle sur le processu point d'entree du noyau
		pqueues.cell[p].pcb = kmain_pcb;
		// On initialise la taille de la file a 0 même si toutes les sentinelles pointent vers le pcb de kmain
		pqueues.size[p] = 0;
	}
	// On initialise la cellule courante sur le pcb du point d'entree
	current_cell = &(pqueues.cell[PP_KERNEL]);
	// On initialise le compteur de tours
	loop_counter = 0;
	// On retourne le pcb de la cellule courante donc celle du point d'entree
	return current_cell->pcb;
}
/**
 *	Cette procedure elit un nouveau processus et supprime les processus termines
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
		// On incremente le compteur de tours
		loop_counter++;
		// Si le compteur de tour depasse la valeur (nb_de_priorite - priorite_courante)
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
	// On retourne le nouveau pcb elu
	return current_cell->pcb;
}
/**
 *	Cette procedure enregistre un nouveau processus aupres du scheduler
 */
void priority_sched_add(pcb_s * newProcess)
{
	// On alloue une nouvelle cellule
	SimpleCell * cell = (SimpleCell*)kAlloc(sizeof(SimpleCell));
	// On affecte le nouveau process a cette cellule
	cell->pcb = newProcess;
	// On ajoute la nouvelle cellule au debut de la file de priorite correspondante
	cell->next = pqueues.cell[newProcess->priority].next;
	pqueues.cell[newProcess->priority].next = cell;
	// On incremente la taille de la file en question
	pqueues.size[newProcess->priority]++;
}
