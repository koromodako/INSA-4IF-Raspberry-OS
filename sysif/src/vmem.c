#include "vmem.h"
#include "kheap.h"
#include "asm_tools.h"

//
//
//
unsigned int init_kern_translation_table(void) {
    // Initialisation des variables de flags
    uint32_t device_flags = DEVICE_FLAGS;
    uint32_t table_1_page_flags = TABLE_1_PAGE_FLAGS;
    uint32_t table_2_page_flags = TABLE_2_PAGE_FLAGS;

    // Allocation de la section contenant les adresses des pages des tables de second niveau
    uint32_t * translation_base = (uint32_t*)kAlloc_aligned( FIRST_LVL_TT_SIZE, FIRST_LVL_TT_INDEX_SIZE );
    // Itérateur sur la zone de mémoire allouée pour la premiere page
    uint32_t * first_table_it = translation_base;


    // Pour les pages du kernel -------------------------------------------
    // Calcul du nombre de page 1 necessaires pour mapper de 0x000000000 à kernel_heap_limit
    // On ajoute 1 à kernel_heap_limit car on map depuis l'adresse 0
    int kern_page_count = ((int)(kernel_heap_limit)+1) / (/*8* RAM en mots de 8 bits*/PAGE_SIZE*SECON_LVL_TT_COUN);

    // On remplit l'espace memoire de la page 1 avec les entrées des pages 2
    int lvl_1_page; // Itération sur la table 1 pour allocation table 2
    for ( lvl_1_page = 0; 
          lvl_1_page < kern_page_count; 
          ++lvl_1_page) 
    {
        // Allocation de la section contenant les adresses des pages des sections de RAM
        uint32_t * second_table_it = (uint32_t*)kAlloc_aligned( SECON_LVL_TT_SIZE, SECON_LVL_TT_INDEX_SIZE ); 
        // Inscription dans la table de premier niveau de l'adresse de la page de second niveau tout juste allouée 
        (*first_table_it) = (uint32_t)second_table_it | table_1_page_flags;
        // On itère sur les pages de second niveau pour renseigner les adresses physiques
        int lvl_2_page;
        for ( lvl_2_page = 0; 
              lvl_2_page < SECON_LVL_TT_COUN; 
              ++lvl_2_page)
        {
            // Inscription de l'adresse physique dans l'entrée de la table de niveau 2
            (*second_table_it) = (lvl_1_page * SECON_LVL_TT_COUN * PAGE_SIZE + 
                                 lvl_2_page * PAGE_SIZE) | table_2_page_flags;
            // Passage à l'entrée suivante dans la table de niveau 2
            second_table_it++;  
        }
        // Passage à l'entrée suivante dans la table de premier niveau
        first_table_it++;
    }

    // Incrément de l'itérateur sur la table 1 pour aller pointer l'équivalent de l'adresse 0x20000000 mappée
    first_table_it = translation_base + (DEVICE_START / (/*8* RAM en mots de 8 bits*/PAGE_SIZE*SECON_LVL_TT_COUN));

    // Pour les pages des devices -------------------------------------------
    // Calcul du nombre de page 1 necessaires pour mapper de 0x20000000 à 0x20FFFFFF
    // On ajoute 1 à la différence d'adresses car on map depuis l'adresse 0x20000000 incluse
    int device_page_count = ((DEVICE_END - DEVICE_START) + 1) /(/*8* RAM en mots de 8 bits*/PAGE_SIZE*SECON_LVL_TT_COUN);

    // On remplit l'espace memoire de la page 1 avec les entrées des pages 2
    // Itération sur la table 1 pour allocation table 2
    for ( lvl_1_page = 0; 
          lvl_1_page < device_page_count; 
          ++lvl_1_page) 
    {
        // Allocation de la section contenant les adresses des pages des sections de RAM
        uint32_t * second_table_it = (uint32_t*)kAlloc_aligned( SECON_LVL_TT_SIZE, SECON_LVL_TT_INDEX_SIZE ); 
        // Inscription dans la table de premier niveau de l'adresse de la page de second niveau tout juste allouée 
        (*first_table_it) = (uint32_t)second_table_it | table_1_page_flags;
        // On itère sur les pages de second niveau pour renseigner les adresses physiques
        int lvl_2_page;
        for ( lvl_2_page = 0; 
              lvl_2_page < SECON_LVL_TT_COUN; 
              ++lvl_2_page)
        {
            // Inscription de l'adresse physique dans l'entrée de la table de niveau 2
            (*second_table_it) = (DEVICE_START + (lvl_1_page * SECON_LVL_TT_COUN * PAGE_SIZE + 
                                 lvl_2_page * PAGE_SIZE)) | device_flags; // <<<<<<< Note : here the flag is for devices
            // Passage à l'entrée suivante dans la table de niveau 2
            second_table_it++;  
        }
        // Passage à l'entrée suivante dans la table de premier niveau
        first_table_it++;
    }

    // On retourne l'adresse de la page de niveau 1
    return (unsigned int) (translation_base);
}
//
//
//
void start_mmu_C(void) {
    register unsigned int control;
    __asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0));
    //Disable cache
    __asm("mcr p15, 0, r0, c7, c7, 0");
    //Invalidate cache (data and instructions) */
    __asm("mcr p15, 0, r0, c8, c7, 0");
    //Invalidate TLB entries
    /* Enable ARMv6 MMU features (disable sub-page AP) */
    control = (1 << 23) | (1 << 15) | (1 << 4) | 1;
    /* Invalidate the translation lookaside buffer (TLB) */
    __asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
    /* Write control register */
    __asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));
}
//
//
//
void configure_mmu_C(unsigned int translation_base) {
    register unsigned int pt_addr = translation_base;
    /* Translation table 0 */
    __asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
    /* Translation table 1 */
    __asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
    /* Use translation table 0 for everything */
    __asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
    /* Set Domain 0 ACL to "Manager", not enforcing memory permissions
     * Every mapped section/page is in domain 0
     */
    __asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}
//
//
//
void vmem_init(void) {
    // Initialisation de la mémoire physique
    unsigned int translation_base = init_kern_translation_table();
    // Configuration de la MMU
    configure_mmu_C(translation_base);
    // Activation des interruptions et data aborts
    ENABLE_AB();
    // Démarrage de la MMU
    start_mmu_C();
}
//
//
//
// uint32_t vmem_translate(uint32_t va, struct pcb_s* process)
// {
//     uint32_t pa; /* The result */
//     /* 1st and 2nd table addresses */
//     uint32_t table_base;
//     uint32_t second_level_table;
//     /* Indexes */
//     uint32_t first_level_index;
//     uint32_t second_level_index;
//     uint32_t page_index;
//     /* Descriptors */
//     uint32_t first_level_descriptor;
//     uint32_t* first_level_descriptor_address;
//     uint32_t second_level_descriptor;
//     uint32_t* second_level_descriptor_address;
//     if (process == NULL)
//     {
//         __asm("mrc p15, 0, %[tb], c2, c0, 0" : [tb] "=r"(table_base));
//     }
//     else
//     {
//         table_base = (uint32_t) process->page_table;
//     }
//     table_base = table_base & 0xFFFFC000;
//     /* Indexes*/
//     first_level_index = (va >> 20);
//     second_level_index = ((va << 12) >> 24);
//     page_index = (va & 0x00000FFF);
//     /* First level descriptor */
//     first_level_descriptor_address = (uint32_t*) (table_base | (first_level_index << 2));
//     first_level_descriptor = *(first_level_descriptor_address);
//     /* Translation fault*/
//     if (! (first_level_descriptor & 0x3)) {
//         return (uint32_t) FORBIDDEN_ADDRESS;
//     }
//     /* Second level descriptor */
//     second_level_table = first_level_descriptor & 0xFFFFFC00;
//     second_level_descriptor_address = (uint32_t*) (second_level_table | (second_level_index << 2));
//     second_level_descriptor = *((uint32_t*) second_level_descriptor_address);
//     /* Translation fault*/
//     if (! (second_level_descriptor & 0x3)) {
//         return (uint32_t) FORBIDDEN_ADDRESS;
//     }
//     /* Physical address */
//     pa = (second_level_descriptor & 0xFFFFF000) | page_index;
//     return pa;
// }