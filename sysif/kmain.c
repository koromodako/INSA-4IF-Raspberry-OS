#include <stdint.h>
#include "util.h"
#include "sched.h"
#include "hw.h"
#include "asm_tools.h"
#include "fb.h"
#include "graphics.h"
#include "font.h"
#include "kheap.h"
#include "usb.h"
#include "math.h"
#include "keyboard.h"

static FontTable * font;

void display_process_info() {

    // Affichage text haut gauche
    FontCursor * cursor = initCursor(10, 10, getResolutionX(), getResolutionY());
    drawLetters(cursor, font, "Hello World !\n");
    char * resolutionX = (char *) kAlloc(sizeof (char) * 12);
    char * resolutionY = (char *) kAlloc(sizeof (char) * 12);
    itoa(getResolutionX() + 1, resolutionX);
    itoa(getResolutionY() + 1, resolutionY);
    drawLetters(cursor, font, "Resolution : ");
    drawLetters(cursor, font, resolutionX);
    drawLetters(cursor, font, "x");
    drawLetters(cursor, font, resolutionY);
    drawLetters(cursor, font, "\n");

    // Affichage séparateurs
    drawLine(10, 70, getResolutionX() - 10, 70);
    drawLine(divide32(getResolutionX(), 2), 80, divide32(getResolutionX(), 2), getResolutionY() - 10);

    // Affichage infos clavier
    FontCursor * cursorNbClavier = initCursor(divide32(getResolutionX(), 2) + 10, 10, getResolutionX() - 10, 60);
    char * nbClavierString = (char *) kAlloc(sizeof (char) * 14);
    uint32_t nbClavier = getNbKeyboard();
    itoa(nbClavier, nbClavierString);
    drawLetters(cursorNbClavier, font, "Nb de claviers : ");
    drawLetters(cursorNbClavier, font, nbClavierString);
    drawLetters(cursorNbClavier, font, "\n");

}

void display_process_text_left() {
    FontCursor * cursorLeft = initCursor(10, 90, divide32(getResolutionX(), 2) - 10, getResolutionY() - 10);
    for (;;) {
        KeyboardUpdate();
        char c = KeyboardGetChar();
        if (c != 0) {
            drawLetter(cursorLeft, font, c);
        }
    }
}

void display_process_text_right() {
    FontCursor * cursorRight = initCursor(divide32(getResolutionX(), 2) + 10, 90, getResolutionX() - 10, getResolutionY() - 10);
    uint32_t letter = 33;
    while (1) {
        drawLetter(cursorRight, font, (char) letter);
        letter++;
        if (letter > 126) {
            letter = 33;
        }

        uint32_t sleep = 0;
        for (sleep = 0; sleep < 100000; sleep++);
    }
}

void kmain(void) {

    // Initialisation de l'USB
    usb_init();

    // Initialisation du scheduler
    sched_init(SP_SIMPLE);

    // Initialisation des LEDs ...
    hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*) & display_process_info, PP_HIGH);
    create_process((func_t*) & display_process_text_left, PP_ULTRA_LOW);
    create_process((func_t*) & display_process_text_right, PP_ULTRA_LOW);

    // Initialisation du timer matériel pour les IRQ
    timer_init();
    ENABLE_IRQ();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    font = initFont();

    while (1) {
        sys_yield();
    }
}
