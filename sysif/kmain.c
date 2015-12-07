#include <stdint.h>
#include "util.h"
#include "sched.h"
#include "hw.h"
#include "asm_tools.h"
#include "fb.h"
#include "font.h"
#include "kheap.h"

char* itoa(int i, char b[]){
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

void kmain(void) {
    // Initialisation du scheduler
    sched_init(SP_PRIORITY_QUEUE);

    hw_init();
    FramebufferInitialize();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;
    // **********************************************************************
    initFont();
    initCursor(10, 10, 790, 590);
    drawLetters("Hello World !\n");
    drawLetters("Pitch :");
    char * pitch = (char *)kAlloc(sizeof (char) * 12);
    itoa(getPitch(), pitch);
    drawLetters(pitch);
    drawLetters("\n");
    char * resolutionX = (char *)kAlloc(sizeof (char) * 12);
    char * resolutionY = (char *)kAlloc(sizeof (char) * 12);
    itoa(getResolutionX(), resolutionX);
    itoa(getResolutionY(), resolutionY);
    drawLetters("Resolution :");
    drawLetters(resolutionX);
    drawLetters(" x ");
    drawLetters(resolutionY);
    drawLetters("\n");
    initCursor(10, 100, 790, 590);

    /*
    uint32_t letter = 33;
    while (1) {
        drawLetter((char)letter);
        letter++;
        if (letter > 126) {
            letter = 33;
        }

        uint32_t sleep = 0;
        for (sleep = 0; sleep < 10000; sleep++);
    }*/

}
