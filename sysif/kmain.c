#include <stdint.h>
#include "util.h"
#include "sched.h"
#include "hw.h"
#include "asm_tools.h"
#include "fb.h"
#include "font.h"

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
    drawLetters("Hello World !\nVoici la première phrase de notre Raspberry OS par Blehhhhxanome !\n~SWAG~\n\n");
    initCursor(10, 100, 790, 590);

    uint32_t letter = 33;
    while (1) {
        drawLetter((char)letter);
        letter++;
        if (letter > 126) {
            letter = 33;
        }

        uint32_t sleep = 0;
        for (sleep = 0; sleep < 10000; sleep++);
    }

}
