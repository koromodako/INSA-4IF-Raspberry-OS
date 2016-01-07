#include "math.h"
#include "util.h"

int min(int a, int b)
{
    return (a < b ? a : b);
}

int max(int a, int b)
{
    return (a < b ? b : a);
}

uint64_t divide(uint64_t x, uint64_t y) {
    if (y == 0)
    {
        PANIC();
    }
    int quotient = 0;
    while (x >= y) {
        x  -=  y;
        quotient++;
    }
    return quotient;
}

uint32_t divide32(uint32_t x, uint32_t y) {
    if (y == 0)
    {
        PANIC();
    }
    int quotient = 0;
    while (x >= y) {
        x  -=  y;
        quotient++;
    }
    return quotient;
}

uint32_t divide32ceil(uint32_t x, uint32_t y) {
    // if 0/y return 0
    if(x == 0) { return 0; }
    // create quotient and devide x as much as possible to increase it
    int quotient = 0;
    while (x >= y) {
        x  -=  y;
        quotient++;
    }
    // ceil if x still above 0
    if(x > 0) { quotient++; } 
    // return quotient
    return quotient;
}

uint64_t mod(uint64_t x, uint64_t y) {
    return x - (y * divide(x,y));
}

uint32_t mod32(uint32_t x, uint32_t y) {
    return x - (y * divide32(x,y));
}

uint32_t getUpperBoundFromDivide32(uint32_t number, uint32_t diviser) {
    if (mod32(number,diviser) == 0) {
        return divide32(number, diviser);
    } else {
        return divide32(number, diviser) + 1;
    }
}
