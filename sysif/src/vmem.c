//#include "vmem.h"
//#include "kheap.h"
//
//
//unsigned int init_kern_translation_table(void) {
//    // Allocation des pages
//    uint32_t * first_table = (uint32_t*)kAlloc_aligned(FIRST_LVL_TT_SIZE,
//            FIRST_LVL_TT_INDEX_SIZE);
//    // Itérateur sur la zone de mémoire allouée pour la premiere page
//    uint32_t * first_table_it = first_table;
//
//    // On remplit l'espace memoire de la page 1 avec les entrées des pages 2
//    int lvl_2_page;
//    for (lvl_2_page = 0; lvl_2_page < FIRST_LVL_TT_COUN; ++lvl_2_page) {
//        (*first_table_it) = (uint32_t)kAlloc_aligned(SECON_LVL_TT_SIZE,
//                SECON_LVL_TT_INDEX_SIZE);
//        first_table_it++;
//    }
//
//    // Initialisation des pages
//    uint32_t device_flags = 0b010000110111;
//    uint32_t * first_table_it = first_table; // reset de l'iterateur
//    for (lvl_2_page = 0; lvl_2_page < FIRST_LVL_TT_COUN; ++lvl_2_page) {
//            (*first_table_it) |= device_flags;
//            first_table_it++;
//        }
//
//
//    // On retourne l'adresse de la page de niveau 1
//    return (unsigned int) (first_table);
//}
//
//void start_mmu_C() {
//    register unsigned int control;
//    __asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0));
//    //Disable cache
//    __asm("mcr p15, 0, r0, c7, c7, 0");
//    //Invalidate cache (data and instructions) */
//    __asm("mcr p15, 0, r0, c8, c7, 0");
//    //Invalidate TLB entries
//    /* Enable ARMv6 MMU features (disable sub-page AP) */
//    control = (1 << 23) | (1 << 15) | (1 << 4) | 1;
//    /* Invalidate the translation lookaside buffer (TLB) */
//    __asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
//    /* Write control register */
//    __asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));
//}
//
//void configure_mmu_C() {
//    register unsigned int pt_addr = MMUTABLEBASE;
//    /* Translation table 0 */
//    __asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
//    /* Translation table 1 */
//    __asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
//    /* Use translation table 0 for everything */
//    __asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
//    /* Set Domain 0 ACL to "Manager", not enforcing memory permissions
//     * Every mapped section/page is in domain 0
//     */
//    __asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
//}
//
//void vmem_init() {
//
//}
