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

static FontTable * font;

void display_process_info()
{
    FontCursor * cursor = initCursor(10, 10, getResolutionX(), getResolutionY());
    drawLetters(cursor, font, "Hello World !\n");
    drawLetters(cursor, font, "Nombre de claviers : ");
    char * resolutionX = (char *)kAlloc(sizeof (char) * 12);
    char * resolutionY = (char *)kAlloc(sizeof (char) * 12);
    itoa(getResolutionX() + 1, resolutionX);
    itoa(getResolutionY() + 1, resolutionY);
    drawLetters(cursor, font, "Resolution : ");
    drawLetters(cursor, font, resolutionX);
    drawLetters(cursor, font, "x");
    drawLetters(cursor, font, resolutionY);
    drawLetters(cursor, font, "\n");

    drawLine(10,70,getResolutionX()-10, 70);
    drawLine(divide32(getResolutionX(), 2), 80, divide32(getResolutionX(), 2), getResolutionY()-10);
}

void display_process_info_keyboard()
{
    uint32_t x = divide32(getResolutionX(), 2)+10;
    FontCursor * cursorNbClavier = initCursor(x, 10, getResolutionX()-10, 30);
    FontCursor * cursorNbTouches = initCursor(x, 30, getResolutionX()-10, 50);

    char * nbClavierString = (char *)kAlloc(sizeof (char) * 14);
    char * nbTouchesString = (char *)kAlloc(sizeof (char) * 14);

    uint32_t nbClavier = getNbKeyboard();
    uint32_t nbTouches = getKeyDownCount();

    drawLetters(cursorNbClavier, font, "Nombre de claviers : ");
    uint32_t cursorNbClavierX = cursorNbClavier->cursor_x;
    uint32_t cursorNbClavierY = cursorNbClavier->cursor_y;
    itoa(nbClavier, nbClavierString);
    drawLetters(cursorNbClavier, font, nbClavierString);

    drawLetters(cursorNbTouches, font, "Nombre de touches enfoncees : ");
    uint32_t cursorNbTouchesX = cursorNbTouches->cursor_x;
    uint32_t cursorNbTouchesY = cursorNbTouches->cursor_y;
    itoa(nbTouches, nbTouchesString);
    drawLetters(cursorNbTouches, font, nbTouchesString);

    do {
        uint32_t sleep = 0;
        for (sleep = 0; sleep < 100000; sleep++);
        sys_yield();

        if (nbClavier != getNbKeyboard()) {
            draw(cursorNbClavierX, cursorNbClavierY, getResolutionX()-10, 30, 0, 0, 0);
            cursorNbClavier->cursor_x = cursorNbClavierX;
            cursorNbClavier->cursor_y = cursorNbClavierY;
            nbClavier = getNbKeyboard();
            itoa(nbClavier, nbClavierString);
            drawLetters(cursorNbClavier, font, nbClavierString);
        }
        if (nbTouches != getKeyDownCount()) {
            draw(cursorNbTouchesX, cursorNbTouchesY, getResolutionX()-10, 50, 0, 0, 0);
            cursorNbTouches->cursor_x = cursorNbTouchesX;
            cursorNbTouches->cursor_y = cursorNbTouchesY;
            nbTouches = getKeyDownCount();
            itoa(nbTouches, nbTouchesString);
            drawLetters(cursorNbTouches, font, nbTouchesString);
        }
    } while(1);

}

void display_process_text_left()
{
    FontCursor * cursorLeft = initCursor(10, 90, divide32(getResolutionX(), 2)-10, getResolutionY()-10);
    for(;;)
    {
        KeyboardsUpdate();
        char c = KeyboardGetChar();
        if(c != 0)
        {
            drawLetter(cursorLeft, font, c);
            led_switch();
        }
        sys_yield();
    }
}

void display_process_text_right()
{
    FontCursor * cursorRight = initCursor(divide32(getResolutionX(), 2)+10, 90, getResolutionX()-10, getResolutionY()-10);
    uint32_t letter = 33;
    while (1) {
        drawLetter(cursorRight, font, (char)letter);
        letter++;
        if (letter > 126) {
             letter = 33;
        }

        uint32_t sleep = 0;
        for (sleep = 0; sleep < 100000; sleep++);
        sys_yield();
    }
}

void kmain(void) {

    // Initialisation de l'USB
    usb_init();
    // Initialisation du scheduler
    sched_init(SP_QUEUE);

    // Initialisation des LEDs ...
    hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*)&display_process_info, PP_HIGH);
    create_process((func_t*)&display_process_info_keyboard, PP_HIGH);
    create_process((func_t*)&display_process_text_left, PP_MEDIUM);
    create_process((func_t*)&display_process_text_right, PP_MEDIUM);

    // Initialisation du timer matériel pour les IRQ
    //timer_init();
    //ENABLE_IRQ();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    font = initFont();
    KeyboardLedsOn();

    while (1) {
        sys_yield();
    }
}
