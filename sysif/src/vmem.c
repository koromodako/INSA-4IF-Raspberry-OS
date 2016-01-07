#include "vmem.h"
#include "kheap.h"
#include "asm_tools.h"
#include "hw.h"
#include "math.h"

#ifdef USE_VMEM

// Globales -------------------------------------------
/*  
    Adresse de la table des pages du kernel
*/
uint32_t kernel_page_table_base;
/*  
    Pointeur sur la premiere case de la table d'occupation des frames
*/
uint8_t * free_frames_table;
/*  
    Protection d'initialisation
*/
int vmem_init_locker = 0;

// Fonctions -------------------------------------------

int get_frame_index(uint32_t phy_addr) 
{   return divide32(phy_addr, FRAME_SIZE);
}
uint32_t get_phy_addr_from_frame_index(int frame_index)
{   return frame_index * FRAME_SIZE;
}
uint32_t get_phy_addr_from_offsets(int lvl_1_oft, int lvl_2_oft)
{   return FRAME_SIZE * (lvl_2_oft + lvl_1_oft * SECON_LVL_TT_COUN);
}
int get_frame_count_from_phy_addrs(uint32_t phy_addr_start, uint32_t phy_addr_end)
{   return get_frame_index(phy_addr_end) + 1 - get_frame_index(phy_addr_start);
}
int get_frame_count_from_size_octet(int size_octet)
{   return divide32ceil(size_octet,FRAME_SIZE);
}
int get_lvl_1_offset(int frame_index)
{   return divide32(frame_index, SECON_LVL_TT_COUN);
}
int get_lvl_2_offset(int frame_index)
{   return mod32(frame_index, SECON_LVL_TT_COUN);
}

/* 
    <!> Cette methode suppose que la section a mapper n'entre en conflit avec aucune section deja mappee <!>
*/
void init_miror_section(uint32_t * table_1_base, 
                        uint32_t lvl_1_flags,
                        uint32_t lvl_2_flags,
                        uint32_t phy_addr_start,
                        uint32_t phy_addr_end)
{
    // on commence par calculer le numero de la frame qui contient l'adresse physique de depart
    int start_frame_ind = get_frame_index(phy_addr_start);
    // on calcule le nombre de frames a allouer
    int frame_count = get_frame_count_from_phy_addrs(phy_addr_start, phy_addr_end);
    // Definition de l'offset de niveau 1
    int lvl_1_oft = get_lvl_1_offset(start_frame_ind);
    int lvl_2_oft = get_lvl_2_offset(start_frame_ind);
    // Remplissage des entrees de niveau 1
    int frame_counter = 0;
    while(frame_counter < frame_count)
    {   // Allocation d'une entree de niveau 2
        uint32_t * table_2_base = (uint32_t*)kAlloc_aligned( SECON_LVL_TT_SIZE, SECON_LVL_TT_INDEX_SIZE ); 
        // Enregistrement de cette entree
        *(table_1_base + lvl_1_oft) = (uint32_t)table_2_base | lvl_1_flags;
        // Remplissage des entree de niveau 2
        while( frame_counter < frame_count && lvl_2_oft < SECON_LVL_TT_COUN)
        {   // Inscription de l'adresse de frame dans la table de niveau 2
            uint32_t phy_addr = get_phy_addr_from_offsets(lvl_1_oft,lvl_2_oft);
            *(table_2_base + lvl_2_oft) = phy_addr | lvl_2_flags;
            // increment de l'offset pour decaler l'iterateur
            lvl_2_oft++;
            // increment du compteur de frames allouees
            frame_counter++;
        }
        // reset de l'offset de niveau 2
        lvl_2_oft = 0;
        // increment de l'offset pour decaler l'iterateur
        lvl_1_oft++; 
    }
    
}

uint32_t init_kern_translation_table(void) 
{
    // Initialisation des variables de flags
    uint32_t device_flags = DEVICE_FLAGS;
    uint32_t table_1_page_flags = TABLE_1_PAGE_FLAGS;
    uint32_t table_2_page_flags = TABLE_2_PAGE_FLAGS;

    // Allocation de la section contenant les adresses des pages des tables de second niveau
    uint32_t * translation_base = (uint32_t*)kAlloc_aligned( FIRST_LVL_TT_SIZE, FIRST_LVL_TT_INDEX_SIZE );

    // Pour les pages du kernel -------------------------------------------
    init_miror_section( translation_base,
                        table_1_page_flags, 
                        table_2_page_flags,
                        0x0,
                        (uint32_t)(kernel_heap_limit));

    // Pour les pages des devices -------------------------------------------
    init_miror_section( translation_base,
                        table_1_page_flags, 
                        device_flags,
                        DEVICE_START,
                        DEVICE_END);

    // On retourne l'adresse de la page de niveau 1
    return (uint32_t)translation_base;
}

uint32_t init_ps_translation_table(void)
{
    // Initialisation des variables de flags
    uint32_t device_flags = DEVICE_FLAGS;
    uint32_t table_1_page_flags = TABLE_1_PAGE_FLAGS;
    uint32_t table_2_page_flags = TABLE_2_PAGE_FLAGS;

    // Allocation de la section contenant les adresses des pages des tables de second niveau
    uint32_t * translation_base = (uint32_t*)kAlloc_aligned( FIRST_LVL_TT_SIZE, FIRST_LVL_TT_INDEX_SIZE );

    // Pour le code commun -------------------------------------------
    init_miror_section(translation_base,
                      table_1_page_flags, 
                      table_2_page_flags,
                      0x0,
                      /*(uint32_t)(kernel_heap_start)*/(uint32_t)(kernel_heap_limit));

    // Pour les pages des devices -------------------------------------------
    init_miror_section(translation_base,
                      table_1_page_flags, 
                      device_flags,
                      DEVICE_START,
                      DEVICE_END);

    // On retourne l'adresse de la page de niveau 1
    return (uint32_t)translation_base;
}

void init_occupation_table(void)
{
    free_frames_table = (uint8_t*)kAlloc(FREE_FRAMES_TABLE_SIZE);

    int fm_index;
    int device_lower_bound = get_frame_index(DEVICE_START);
    int kernel_upper_bound = get_frame_index((uint32_t)(kernel_heap_limit));
    for (fm_index = 0; fm_index < FREE_FRAMES_TABLE_SIZE; ++fm_index)
    {
        // Si fm_index en zone occupee 
        // code / donnees / structures noyaux    
        if( (fm_index >= 0 && 
             fm_index <= kernel_upper_bound) ||
            (fm_index >= device_lower_bound && 
             fm_index < FREE_FRAMES_TABLE_SIZE) )
        {
            LOCK_FRAME(free_frames_table[fm_index]);
        } 
        // Sinon zone libre
        else
        {
            FREE_FRAME(free_frames_table[fm_index]);
        }
    }
}

// --------------------------------------------------------------------------------------------------
// --------------------------------------- MEMORY MANAGEMENT ----------------------------------------
// --------------------------------------------------------------------------------------------------

uint32_t vmem_init(void) 
{
    if(vmem_init_locker)
    {   log_err("Virtual memory initialization called twice !");
        log_cr();
        return NULL;
    }
    // Initialisation de la table d'occupation des frames
    init_occupation_table();
    // Initialisation de la memoire physique
    kernel_page_table_base = init_kern_translation_table();
    // Configuration de la MMU
    configure_mmu_C(kernel_page_table_base);
    // Activation des interruptions et data aborts
    ENABLE_AB();
    // Demarrage de la MMU
    start_mmu_C();
    // Activation de la protection d'initialisation
    vmem_init_locker = 1;
    // On retourne pour la seule et unique fois l'adresse a l'appelant
    return kernel_page_table_base;
}

uint8_t * vmem_alloc_in_userland(pcb_s * process, uint32_t size)
{
    // Trouver une entree libre dans la table du processus --------------------------------
    // Iterateur sur la premiere entree de niveau 1
    uint32_t * table_1_it = process->page_table;
    int lvl_1_oft_ref = 0;
    int lvl_2_oft_ref = 0;
    int ram_overflow_flag = 0;
    int found_flag = 0;
    // Pour chaque entree de niveau 1
    for (lvl_1_oft_ref = 0; !found_flag && lvl_1_oft_ref < FIRST_LVL_TT_COUN; ++lvl_1_oft_ref, ++table_1_it)
    {   
        if( ! ((*table_1_it) & 0x03) ) // si l'entree de niveau 1 est libre
        {
            found_flag = 1;     // on leve le drapeau de decouverte
            lvl_2_oft_ref = 0;  // reset de l'offset de reference de niveau 2
            break;              // sortie de bouble de recherche
        }
        else if( get_phy_addr_from_offsets(lvl_1_oft_ref, lvl_2_oft_ref) >= DEVICE_START) // sinon si on a depasse la limite des device
        {
            ram_overflow_flag = 1; // on leve le drapeau pour le depassement de ram
            break;
        }
        else // sinon alors l'entree pointe sur un niveau 2 partiellement ou completement plein 
        {   
            uint32_t * table_2_it = (uint32_t*)((*table_1_it) & 0xFFFFFC00); // on recupere l'adresse de la premiere entree de niveau 2
            // pour chaque entree de niveau 2
            for (lvl_2_oft_ref = 0; lvl_2_oft_ref < SECON_LVL_TT_COUN; ++lvl_2_oft_ref, ++table_2_it)
            {
                if( ! ((*table_2_it) & 0x03) ) // une entree de niveau 2 est libre
                {
                    found_flag = 1; // on leve le drapeau de decouverte
                    break;          // sortie de la boucle de recherche
                }
            }
        }
    }
    // si le drapeau de depassement de ram est leve
    if (ram_overflow_flag)
    {   
        return NULL; // on retourne NULL
    }

    // Allouer frames -----------------------------------------
    // On calcule le nombre de frames a allouer
    int nb_frame_to_alloc = get_frame_count_from_size_octet(size);
    uint32_t lvl_1_flags = TABLE_1_PAGE_FLAGS;
    uint32_t lvl_2_flags = TABLE_2_PAGE_FLAGS;

    // Pour chaque frame a allouer
    int fta; // frame to allocate index
    int lvl_2_oft, lvl_1_oft;

    uint32_t firstPhysicalAddress = 0;

    for (fta=0; fta < nb_frame_to_alloc; ++fta)
    {
        // calculs des offsets locaux
        lvl_2_oft = mod32(lvl_2_oft_ref + fta, SECON_LVL_TT_COUN);
        lvl_1_oft = lvl_1_oft_ref + divide32(lvl_2_oft_ref + fta, SECON_LVL_TT_COUN);
        // index de la premiere frame libre
        int ff_ind = find_next_free_frame();
        if(ff_ind == -1)
        {
            return NULL;
        }
        // adresse physique de la premiere frame libre
        uint32_t f_phy_addr = get_phy_addr_from_frame_index(ff_ind);

        // memorisation de la premiere adresse physique
        if (firstPhysicalAddress == 0) {
            firstPhysicalAddress = f_phy_addr;
        }

        // recuperation des entrees et allocation a la volee si necessaire
        uint32_t * entry_1 = process->page_table + lvl_1_oft;
        uint32_t * entry_2;
        // allocation de l'entree si necessaire
        if( !(*entry_1 & 0x03) )
        {   // creation d'une nouvelle entree de niveau 2
            entry_2 = (uint32_t*)kAlloc_aligned( SECON_LVL_TT_SIZE, SECON_LVL_TT_INDEX_SIZE ); 
            (*entry_1) = (uint32_t)entry_2 | lvl_1_flags;
        }
        else
        {   entry_2 = (uint32_t*)((*entry_1) & 0xFFFFFC00);
        }
        // on affecte l'entree de niveau 2 decalee de l'index
        *(entry_2 + lvl_2_oft) = f_phy_addr | lvl_2_flags;
        // on verouille la frame allouee
        LOCK_FRAME(free_frames_table[ff_ind]);
    }

    // Adresse virtuelle
    uint32_t modifiedVirtualAddress = 0x0;

    // First-level table index dans l'adresse virtuelle
    modifiedVirtualAddress |= (lvl_1_oft_ref << 20);

    // Second-level table index
    modifiedVirtualAddress |= (lvl_2_oft_ref << 12);

    // Page index
    uint32_t pageIndex = firstPhysicalAddress & 0xC;
    modifiedVirtualAddress |= pageIndex;

    return (uint8_t*)modifiedVirtualAddress;
}

int find_next_free_frame(void) 
{
    int f;
    for(f=0; f < FREE_FRAMES_TABLE_SIZE; ++f)
    {   if(IS_FREE_FRAME(free_frames_table[f]))
        {   return f;   
        } 
    }
    return -1;
}

void vmem_free(uint8_t* vAddress, pcb_s * process, uint32_t size)
{
    // Valeur des indexes
    uint32_t va = (uint32_t) vAddress;
    uint32_t first_level_index = (va >> 20);
    uint32_t second_level_index = ((va << 12) >> 24);

    // Nombre de frames a liberer
    int nb_frame_to_dealloc = divide32(size, FRAME_SIZE)+1;

    // Liberation des frames ----------------------------------
    int fta; // frame to allocate index
    int ind_2, ind_1;
    for (fta=0; fta < nb_frame_to_dealloc; ++fta)
    {
        ind_2 = (second_level_index + fta) % SECON_LVL_TT_COUN;
        ind_1 = first_level_index + divide32( second_level_index + fta, SECON_LVL_TT_COUN);
        // On va chercher l'entree de niveau 2
        uint32_t * del_entry = (uint32_t*)(
            *(process->page_table + ind_1) & 0xFFFFFC00);
        // On affecte l'entree de niveau 2 decalee de l'index
        uint32_t del_addr = *(del_entry+ind_2) & 0xFFFFF000;
        int del_frame_ind = del_addr/FRAME_SIZE; 
        // On specifie que la frame est liberee
        FREE_FRAME(free_frames_table[del_frame_ind]);
        // On ecrase l'entree de niveau 2
        *(del_entry+ind_2) &= 0x0;
    } 
}

// -----------------------------------------------------------------------------------------------
// --------------------------------------- APPELS SYSTEME ----------------------------------------
// -----------------------------------------------------------------------------------------------

void* sys_mmap(uint32_t size)
{
    // Deplacement du registre contenant size
    __asm("mov r1, r0");
    // SWI
    SWI(SCI_MMAP);

    uint32_t vAddr;
    __asm("mov %0, r0" : "=r"(vAddr));

    return (void*)(vAddr);
}

void do_sys_mmap(pcb_s * context)
{
    // Allocation grâceầ la methode privee d'allocation memoire
    uint32_t * virtualAddress = (uint32_t*) vmem_alloc_in_userland(current_process, context->registres[1]);

    // Retour dans r0 de l'adresse virtuelle du debut du bloc alloue
    context->registres[0] = (uint32_t)(virtualAddress);
}

void sys_munmap(void * addr, uint32_t size)
{
    // Deplacement des registres contenant addr et size
    __asm("mov r2, r1\n\t"
          "mov r1, r0");
    // SWI
    SWI(SCI_MUNMAP);

    return;
}

void do_sys_munmap(pcb_s * context)
{
    // Reconstruction de addr
    uint8_t * addr = (uint8_t*)(context->registres[1]);
    // Reconstruction de size
    uint32_t size = context->registres[2];
    // Appel a vmem free
    vmem_free(addr, context, size);
}

// -----------------------------------------------------------------------------------------------
// --------------------------------------- FONCTIONS FOURNIES ------------------------------------
// -----------------------------------------------------------------------------------------------

uint32_t vmem_translate(uint32_t va, uint32_t table_base)
{
    uint32_t pa; /* The result */
    /* 1st and 2nd table addresses */
    uint32_t second_level_table;
    /* Indexes */
    uint32_t first_level_index;
    uint32_t second_level_index;
    uint32_t page_index;
    /* Descriptors */
    uint32_t first_level_descriptor;
    uint32_t* first_level_descriptor_address;
    uint32_t second_level_descriptor;
    uint32_t* second_level_descriptor_address;

    table_base = table_base & 0xFFFFC000;

    /* Indexes*/
    first_level_index = (va >> 20);
    second_level_index = ((va << 12) >> 24);
    page_index = (va & 0x00000FFF);
    /* First level descriptor */
    first_level_descriptor_address = (uint32_t*) (table_base | (first_level_index << 2));
    first_level_descriptor = *(first_level_descriptor_address);
    /* Translation fault*/
    if (! (first_level_descriptor & 0x3)) {
        return (uint32_t) FORBIDDEN_ADDRESS;
    }
    /* Second level descriptor */
    second_level_table = first_level_descriptor & 0xFFFFFC00;
    second_level_descriptor_address = (uint32_t*) (second_level_table | (second_level_index << 2));
    second_level_descriptor = *((uint32_t*) second_level_descriptor_address);
    /* Translation fault*/
    if (! (second_level_descriptor & 0x3)) {
        return (uint32_t) FORBIDDEN_ADDRESS;
    }
    /* Physical address */
    pa = (second_level_descriptor & 0xFFFFF000) | page_index;
    return pa;
}

uint32_t vmem_translate_ps(uint32_t va, pcb_s* process)
{
    uint32_t pa; /* The result */
    /* 1st and 2nd table addresses */
    uint32_t table_base;
    uint32_t second_level_table;
    /* Indexes */
    uint32_t first_level_index;
    uint32_t second_level_index;
    uint32_t page_index;
    /* Descriptors */
    uint32_t first_level_descriptor;
    uint32_t* first_level_descriptor_address;
    uint32_t second_level_descriptor;
    uint32_t* second_level_descriptor_address;
    if (process == 0x0)
    {
        __asm("mrc p15, 0, %[tb], c2, c0, 0" : [tb] "=r"(table_base));
    }
    else
    {
        table_base = (uint32_t) process->page_table;
    }
    table_base = table_base & 0xFFFFC000;
    /* Indexes*/
    first_level_index = (va >> 20);
    second_level_index = ((va << 12) >> 24);
    page_index = (va & 0x00000FFF);
    /* First level descriptor */
    first_level_descriptor_address = (uint32_t*) (table_base | (first_level_index << 2));
    first_level_descriptor = *(first_level_descriptor_address);
    /* Translation fault*/
    if (! (first_level_descriptor & 0x3)) {
        return (uint32_t) FORBIDDEN_ADDRESS;
    }
    /* Second level descriptor */
    second_level_table = first_level_descriptor & 0xFFFFFC00;
    second_level_descriptor_address = (uint32_t*) (second_level_table | (second_level_index << 2));
    second_level_descriptor = *((uint32_t*) second_level_descriptor_address);
    /* Translation fault*/
    if (! (second_level_descriptor & 0x3)) {
        return (uint32_t) FORBIDDEN_ADDRESS;
    }
    /* Physical address */
    pa = (second_level_descriptor & 0xFFFFF000) | page_index;
    return pa;
}

void start_mmu_C(void) 
{
    uint32_t control;
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

void configure_mmu_C(uint32_t translation_base) 
{
    uint32_t pt_addr = translation_base;
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

#endif //USE_VMEM

// -----------------------------------------------------------------------------------------------
// ------------------------------------------ DATA HANDLER ---------------------------------------
// -----------------------------------------------------------------------------------------------

void __attribute__((naked)) data_handler(void)
{
    uint32_t addr;
    int error;
    // on recupere l'adresse qui a genere l'erreur
    __asm("mrc p15, 0, %0, c6, c0, 0" : "=r"(addr));
    log_nfo("Data error at addr=");
    log_int((int)addr);
    log_cr();
    // on recupere le code de l'erreur
    __asm("mrc p15, 0, %0, c5, c0, 0" : "=r"(error));
    // on masque sur les 4 bits de poids faible
    error = error & 0b1111; 
    // on identifie l'erreur
    switch(error)
    {
        case TRANSLATION_FAULT: 
            log_err("Translation fault !"); 
            log_cr();
            break;
        case ACCESS_FAULT:      
            log_err("Access fault !"); 
            log_cr();
            break;        
        case PRIVILEGES_FAULT:  
            log_err("Privileges fault !"); 
            log_cr();
            break;
        default:
            log_err("Unhandled error ! Something wrong happened !");
            log_cr();
    }
    // On termine l'execution du noyau
    terminate_kernel();
}