#ifndef VMEM_H
#define VMEM_H

#include <stdint.h>
#include "sched.h"

// Macros & Constants ----------------------------------------------------------------------
#define FRAME_SIZE 4096  // En Octet = 4Ko

#define FIRST_LVL_TT_INDEX_SIZE 14 // Talle de l'index pour une page de niveau 1
#define FIRST_LVL_TT_COUN 4096 // (soit 2^12, 12 bits pour une entrée du niveau 1) Nombre d'entrées dans la table de niveau 1
#define FIRST_LVL_TT_SIZE 16384 // (Nombre d'entrées x 4) En Octet

#define SECON_LVL_TT_INDEX_SIZE 10 // Talle de l'index pour une page de niveau 2
#define SECON_LVL_TT_COUN 256 // Nombre d'entrées niveau 2
#define SECON_LVL_TT_SIZE 1024 // (Nombre d'entrées x 4) En Octet

// Memory limits
#define DEVICE_START 0x20000000
#define DEVICE_END 0x20FFFFFF

#define FREE_FRAMES_TABLE_SIZE (DEVICE_END+1)/FRAME_SIZE // 135168 en décimal
// Macros pour gérer la table d'occupation des frames
#define FREE_FRAME(frame) frame=0x00
#define LOCK_FRAME(frame) frame=0x01
#define IS_FREE_FRAME(frame) frame==0x00

// Flags des pages
#define TABLE_1_PAGE_FLAGS	0b000000000001
#define TABLE_2_PAGE_FLAGS	0b010001110010
#define DEVICE_FLAGS		0b010000110111

// Codes erreurs TLB
#define TRANSLATION_FAULT 	0b0111
#define ACCESS_FAULT		0b0110
#define PRIVILEDGES_FAULT	0b1111

// Globales -------------------------------------------

unsigned int kernel_page_table_base;

// Fonctions ------------------------------------------

// Memory initialization -----------
/**
 *	Initialise la mémoire physique
 */
uint32_t init_kern_translation_table(void);
/**
 *	Initialise la mémoire physique
 */
uint32_t init_ps_translation_table(void);
/**
 *	Initialise la table d'occupation des pages
 */
void init_occupation_table(void);
/**
 *	Démarre la MMU
 */
void start_mmu_C(void);
/**
 *	Configure la MMU
 */
void configure_mmu_C(uint32_t translation_base);
/**
 *	Initialise la mémoire virtuelle
 */
void vmem_init(void);

// Data handler

void data_handler(void);

// Memory debug --------------------
/**
 *	Réalise la traduction de l'adresse logique en adresse physique
 */
uint32_t vmem_translate(uint32_t va, uint32_t table_base);
uint32_t vmem_translate_ps(uint32_t va, pcb_s* process);


// ----------------- Memory management ---------------
/**
 *
 */
uint8_t * vmem_alloc_in_userland(pcb_s * process, uint32_t size);
/**
 *
 */
void vmem_free(uint8_t* vAddress, pcb_s * process, uint32_t size);
/**
 *	Cette fonction retourne l'index de la prochaine frame libre
 */
int find_next_free_frame(void) ;

// Appel système : reboot ------------------------------------------------------
/**
 *	Appel système pour allouer size octets dans l'espace d'adressage du processus
 */
void* sys_mmap(uint32_t size);
/**
 *	Appel noyau pour allouer size octets dans l'espace d'adressage du processus
 */
void do_sys_mmap(pcb_s * context);
// Appel système : reboot ------------------------------------------------------
/**
 *	Appel système pour liberer size octet à partir de addr
 */
void sys_munmap(void * addr, uint32_t size);
/**
 *	Appel noyau pour liberer size octet à partir de addr
 */
void do_sys_munmap(pcb_s * context);


#endif //VMEM_H

