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
#include "keyboard.h"
#include "math.h"
#include "img.h"
#include "syscall.h"
#include "drivers/usb/csud/include/device/hid/keyboard.h"

static FontTable * font;

void display_process_1() {
    FontCursor * cursorInfo = initCursor(15, 100 - font->max_height - 1, getResolutionX() - 10, 100);
    drawLetters(cursorInfo, font, "Processus 1 : Tres prioritaire");
    FontCursor * cursorRun = initCursor(15, 100, getResolutionX() - 10, 100 + font->max_height + 1);
    uint32_t sleep;
    while (1) {
         drawLetter(cursorRun, font, '_');
         sleep = 0;
         for (sleep = 0; sleep < 10000000; sleep++);
    }
}

void display_process_2() {
    FontCursor * cursorInfo = initCursor(15, 150 - font->max_height - 1, getResolutionX() - 10, 150);
    drawLetters(cursorInfo, font, "Processus 2 : Prioritaire");
    FontCursor * cursorRun = initCursor(15, 150, getResolutionX() - 10, 150 + font->max_height + 1);
    uint32_t sleep;
    while (1) {
         drawLetter(cursorRun, font, '_');
         sleep = 0;
         for (sleep = 0; sleep < 10000000; sleep++);
    }
}

void display_process_3() {
    FontCursor * cursorInfo = initCursor(15, 200 - font->max_height - 1, getResolutionX() - 10, 200);
    drawLetters(cursorInfo, font, "Processus 3 : Normal");
    FontCursor * cursorRun = initCursor(15, 200, getResolutionX() - 10, 200 + font->max_height + 1);
    uint32_t sleep;
    while (1) {
         drawLetter(cursorRun, font, '_');
         sleep = 0;
         for (sleep = 0; sleep < 10000000; sleep++);
    }
}

void display_process_4() {
    FontCursor * cursorInfo = initCursor(15, 250 - font->max_height - 1, getResolutionX() - 10, 250);
    drawLetters(cursorInfo, font, "Processus 4 : Peu prioritaire");
    FontCursor * cursorRun = initCursor(15, 250, getResolutionX() - 10, 250 + font->max_height + 1);
    uint32_t sleep;
    while (1) {
         drawLetter(cursorRun, font, '_');
         sleep = 0;
         for (sleep = 0; sleep < 10000000; sleep++);
    }
}

void display_process_5() {
    FontCursor * cursorInfo = initCursor(15, 300 - font->max_height - 1, getResolutionX() - 10, 300);
    drawLetters(cursorInfo, font, "Processus 5 : Tres peu prioritaire");
    FontCursor * cursorRun = initCursor(15, 300, getResolutionX() - 10, 300 + font->max_height + 1);
    uint32_t sleep;
    while (1) {
         drawLetter(cursorRun, font, '_');
         sleep = 0;
         for (sleep = 0; sleep < 10000000; sleep++);
    }
}

void kmain(void) {

    SCHEDULING_POLICY policy = SP_SIMPLE;

    // Initialisation du scheduler
    sched_init(policy);

    // Initialisation des LEDs ...
    hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*) & display_process_1, PP_ULTRA_HIGH);
    create_process((func_t*) & display_process_2, PP_HIGH);
    create_process((func_t*) & display_process_3, PP_MEDIUM);
    create_process((func_t*) & display_process_4, PP_LOW);
    create_process((func_t*) & display_process_5, PP_ULTRA_LOW);

    // Initialisation du timer matériel pour les IRQ
    timer_init();
    ENABLE_IRQ();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    font = initFont();

    // Affichage text haut gauche
    uint32_t resolutionXValue = getResolutionX();
    uint32_t resolutionYValue = getResolutionY();
    FontCursor * cursor = initCursor(10, 10, getResolutionX(), getResolutionY());
    drawLetters(cursor, font, "Test Ordonnancement : ");
    if (policy == SP_SIMPLE) {
        drawLetters(cursor, font, "Round-robin");
    } else {
        drawLetters(cursor, font, "Files de priorites");
    }
    drawLetters(cursor, font, "\n");
    char * resolutionX = (char *) kAlloc(sizeof (char) * 12);
    char * resolutionY = (char *) kAlloc(sizeof (char) * 12);
    itoa(resolutionXValue + 1, resolutionX);
    itoa(resolutionYValue + 1, resolutionY);
    drawLetters(cursor, font, "Resolution : ");
    drawLetters(cursor, font, resolutionX);
    drawLetters(cursor, font, "x");
    drawLetters(cursor, font, resolutionY);
    drawLetters(cursor, font, "\n");

    // Affichage séparateurs
    drawLine(10, 70, resolutionXValue - 10, 70); // Horizontal : Entre les infos du haut et le reste
    drawLine(10, 80, 10, resolutionYValue - 10); // Vertical : Ligne de départ
    drawLine(resolutionXValue - 10, 80, resolutionXValue - 10, resolutionYValue - 10); // Vertical : Ligne de fin

    while (1) {
        sys_yield();
    }
}
