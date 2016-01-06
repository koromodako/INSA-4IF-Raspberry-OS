#include "simple_sched.h"
#include "hw.h"
#include "kheap.h"
#include "vmem.h"

// Types ------------------------------------
struct SimpleCell {
	struct SimpleCell * next;
	pcb_s * pcb;
};
typedef struct SimpleCell SimpleCell;

// Globales ------------------------------------
// Sentinelle de la liste simplement chaînee des processus
SimpleCell ps_list;
// Pointeur sur la cellule du processus courant
SimpleCell * current_cell;

// Procedure privees -------------------------------------------
void simple_sched_clean(void)
{
	// Tant que les processus devant le processus de la cellule courante sont termines, on supprime la cellule directement devant
	while(current_cell->next != current_cell &&
		  current_cell->next->pcb->state == PS_TERMINATED)
	{
		// Memorisation de la cellule du processus a detruire
		SimpleCell * cell = current_cell->next;
		// Modification du lien
		current_cell->next = current_cell->next->next;
		// Liberation de la memoire allouee pour le processus (pile, pcb, cellule de liste)
		FREE_PS(cell);	
	}
	// Test de terminaison du kernel
	if(current_cell->next == current_cell)
	{	// Ordre de terminaison du kernel
		terminate_kernel();
	}
}
//--------------------------------------------------------------
pcb_s * simple_sched_init(pcb_s * kmain_pcb)
{
	// Chaînage de la sentinelle sur elle même
	ps_list.next = &ps_list;
	// Affectation du process point d'entree
	ps_list.pcb = kmain_pcb;
	// Definition de la cellule courante
	current_cell = &ps_list;
	// Retourne le processus point d'entree
	return ps_list.pcb;
}
//--------------------------------------------------------------
pcb_s * simple_sched_elect(void)
{
	// On enleve tous les processus termines devant le processus courant
	simple_sched_clean();
	// On se deplace d'une cellule vers l'avant
	current_cell = current_cell->next;
	// On retourne le processus stocke dans cette cellule
	return current_cell->pcb;
}
//--------------------------------------------------------------
void simple_sched_add(pcb_s * newProcess)
{
	// Construction d'une nouvelle cellule
	SimpleCell * cell = (SimpleCell *)kAlloc(sizeof(SimpleCell));
	// Affectation du processus a la cellule
	cell->pcb = newProcess;
	// Ajout de la nouvelle cellule apres la sentinelle de la liste
	cell->next = ps_list.next;
	ps_list.next = cell;
}