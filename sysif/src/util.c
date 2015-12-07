#include <stdint.h>
#include "util.h"

void
kernel_panic(char* string, int number)
{
    for(;;)
    {
        /* do nothing */
    }
}

uint64_t
divide(uint64_t x, uint64_t y) {
    int quotient = 0;
    while (x >= y) {
        x  -=  y;
        quotient++;
    }
    return quotient;
}

uint32_t
divide32(uint32_t x, uint32_t y) {
    int quotient = 0;
    while (x >= y) {
        x  -=  y;
        quotient++;
    }
    return quotient;
}

uint32_t
mod32(uint32_t x, uint32_t y) {
    return x - (y * divide32(x,y));
}

char*
itoa(int i, char b[]) {
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

uint32_t
getUpperBoundFromDivide32(uint32_t number, uint32_t diviser) {
    if (mod32(number,diviser) == 0) {
        return divide32(number, diviser);
    } else {
        return divide32(number, diviser) + 1;
    }
}
