#ifndef MATH_H
#define MATH_H

#include <stdint.h>

// Fonction min ----------------------------------------------------------------
int min(int a, int b);

// Fonction max ----------------------------------------------------------------
int max(int a, int b);

// Division sur un int 64 bits  ------------------------------------------------
uint64_t divide(uint64_t x, uint64_t y);

// Division sur un int 32 bits  ------------------------------------------------
uint32_t divide32(uint32_t x, uint32_t y);

// Division sur un int 32 bits et retourne l'arrondi superieur ------------------------------------------------
uint32_t divide32ceil(uint32_t x, uint32_t y);

// Modulo sur un int 64 bits  --------------------------------------------------
uint64_t mod(uint64_t x, uint64_t y);

// Modulo sur un int 32 bits  --------------------------------------------------
uint32_t mod32(uint32_t x, uint32_t y);

// Retourne l'arrondie superieur d'une division --------------------------------
uint32_t getUpperBoundFromDivide32(uint32_t number, uint32_t diviser);

#endif //MATH_H
