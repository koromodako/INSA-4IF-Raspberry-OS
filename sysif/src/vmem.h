#ifndef VMEM_H
#define VMEM_H


// Macros ----------------------------------------------------------------------
#define PAGE_SIZE 4000  // En Octet = 4Ko

#define FIRST_LVL_TT_COUN 4096 // (soit 2^12, 12 bits pour l'adresse niveau 1) Nombre d'entrées niveau 1
#define FIRST_LVL_TT_SIZE 16384 // (Nombre d'entrées x 4) En Octet

#define SECON_LVL_TT_COUN 256 // Nombre d'entrées niveau 2
#define SECON_LVL_TT_SIZE 1024 // (Nombre d'entrées x 4) En Octet


#endif //VMEM_H
