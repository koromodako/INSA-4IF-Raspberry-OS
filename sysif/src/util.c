#include <stdint.h>
#include "util.h"
#include "math.h"

void
kernel_panic(char* string, int number)
{
    for(;;)
    {
        /* do nothing */
    }
}

uint8_t isdigit(char c)
{
    return c == '0' ||
           c == '1' ||
           c == '2' ||
           c == '3' ||
           c == '4' ||
           c == '5' ||
           c == '6' ||
           c == '7' ||
           c == '8' ||
           c == '9' ;
}

char* itoa(uint64_t i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    uint64_t shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = divide(shifter, 10);
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[mod(i,10)];
        i = divide(i, 10);
    }while(i);
    return b;
}

uint64_t atoi(const char *c)
{
    long long int value = 0;
    uint8_t sign = 1;
    if( *c == '+' || *c == '-' )
    {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (isdigit(*c))
    {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}

char* getNextChar(char* str, char fnd)
{
    char* it = str;
    while(*it != '\0' && *it != fnd)
    {
       it++;
    }
    return it;
}
