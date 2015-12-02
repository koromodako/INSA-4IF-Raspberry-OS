#ifndef VMEM_H
#define VMEM_H

// Macros ----------------------------------------------------------------------
#define PAGE_SIZE 4000  // En Octet = 4Ko

#define FIRST_LVL_TT_INDEX_SIZE 12 // Talle de l'index pour une page de niveau 1
#define FIRST_LVL_TT_COUN 4096 // (soit 2^12, 12 bits pour une entrée du niveau 1) Nombre d'entrées dans la table de niveau 1
#define FIRST_LVL_TT_SIZE 16384 // (Nombre d'entrées x 4) En Octet

#define SECON_LVL_TT_INDEX_SIZE 8 // Talle de l'index pour une page de niveau 2
#define SECON_LVL_TT_COUN 256 // Nombre d'entrées niveau 2
#define SECON_LVL_TT_SIZE 1024 // (Nombre d'entrées x 4) En Octet


// Fonctions ------------------------------------------

// Memory initialization -----------
unsigned int init_kern_translation_table(void);

void start_mmu_C();
void configure_mmu_C();
void vmem_init();

// Memory management ---------------
void alloc_page(void);
void free_page(void);


#endif //VMEM_H
