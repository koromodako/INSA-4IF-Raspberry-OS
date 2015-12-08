#ifndef VMEM_H
#define VMEM_H

#include <stdint.h>
#include "sched.h"

// Macros ----------------------------------------------------------------------
#define PAGE_SIZE 4096  // En Octet = 4Ko

#define FIRST_LVL_TT_INDEX_SIZE 14 // Talle de l'index pour une page de niveau 1
#define FIRST_LVL_TT_COUN 4096 // (soit 2^12, 12 bits pour une entrée du niveau 1) Nombre d'entrées dans la table de niveau 1
#define FIRST_LVL_TT_SIZE 16384 // (Nombre d'entrées x 4) En Octet

#define SECON_LVL_TT_INDEX_SIZE 10 // Talle de l'index pour une page de niveau 2
#define SECON_LVL_TT_COUN 256 // Nombre d'entrées niveau 2
#define SECON_LVL_TT_SIZE 1024 // (Nombre d'entrées x 4) En Octet

// Memory limits
#define DEVICE_START 0x20000000
#define DEVICE_END 0x20FFFFFF

// Flags des pages
#define TABLE_1_PAGE_FLAGS	0b000000000001
#define TABLE_2_PAGE_FLAGS	0b010001110011
#define DEVICE_FLAGS		0b010000110111

// Fonctions ------------------------------------------

// Memory initialization -----------
/**
 *	Initialise la mémoire physique
 */
unsigned int init_kern_translation_table(void);
/**
 *	Démarre la MMU
 */
void start_mmu_C(void);
/**
 *	Configure la MMU
 */
void configure_mmu_C(unsigned int translation_base);
/**
 *	Initialise la mémoire virtuelle
 */
void vmem_init(void);

// Memory debug --------------------
/**
 *	Réalise la traduction de l'adresse logique en adresse physique
 */
uint32_t vmem_translate(uint32_t va, uint32_t table_base);
uint32_t vmem_translate_ps(uint32_t va, struct pcb_s* process);


// ----------------- Memory management ---------------

// Appel système : reboot ------------------------------------------------------
/**
 *	Appel système pour allouer size octets dans l'espace d'adressage du processus
 */
void* sys_mmap(unsigned int size);
/**
 *	Appel noyau pour allouer size octets dans l'espace d'adressage du processus
 */
void do_sys_mmap();
// Appel système : reboot ------------------------------------------------------
/**
 *	Appel système pour liberer size octet à partir de addr
 */
 void sys_munmap(void * addr, unsigned int size);
/**
 *	Appel noyau pour liberer size octet à partir de addr
 */
void do_sys_munmap();


#endif //VMEM_H

