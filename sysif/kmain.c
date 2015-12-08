#include <stdint.h>
#include "util.h"
#include "sched.h"
#include "hw.h"
#include "asm_tools.h"
#include "fb.h"
#include "graphics.h"
#include "font.h"
#include "kheap.h"

void display_process() {
    initFont();

    initCursor(10, 10, getResolutionX(), getResolutionY());
    drawLetters("Hello World !\n");
    drawLetters("Pitch :");
    char * pitch = (char *)kAlloc(sizeof (char) * 12);
    itoa(getPitch(), pitch);
    drawLetters(pitch);
    drawLetters("\n");
    char * resolutionX = (char *)kAlloc(sizeof (char) * 12);
    char * resolutionY = (char *)kAlloc(sizeof (char) * 12);
    itoa(getResolutionX() + 1, resolutionX);
    itoa(getResolutionY() + 1, resolutionY);
    drawLetters("Resolution : ");
    drawLetters(resolutionX);
    drawLetters("x");
    drawLetters(resolutionY);
    drawLetters("\n");

    initCursor(10, 100, getResolutionX(), getResolutionY());
    //uint32_t letter = 33;
    drawLine(10,70,getResolutionX()-10, 100);

    // while (1) {
    //     drawLetter((char)letter);
    //     letter++;
    //     if (letter > 126) {
    //         letter = 33;
    //     }

    //     uint32_t sleep = 0;
    //     for (sleep = 0; sleep < 100000; sleep++);
    // }
}

void kmain(void) {
    // Initialisation du scheduler
    sched_init(SP_PRIORITY_QUEUE);

    // Initialisation des LEDs ...
    hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*)&display_process, PP_HIGH);

    // Initialisation du timer matériel pour les IRQ
    timer_init();
    ENABLE_IRQ();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    while(1) {
        sys_yield();
    }
}
