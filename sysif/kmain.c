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

extern unsigned char _binary_image_ppm_start;
extern unsigned char _binary_image_ppm_end;

void display_process_top_info() {

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
    drawLine(10, 70, getResolutionX() - 10, 70); // Horizontal : Entre les infos du haut et le reste
    drawLine(divide32(getResolutionX(), 2), 10, divide32(getResolutionX(), 2), 60); // Vertical : Entre les infos du haut (milieu)
    drawLine(divide32(getResolutionX(), 2), 80, divide32(getResolutionX(), 2), getResolutionY() - 10); // Vertical : Milieu du bas
    drawLine(divide32(getResolutionX(), 2) + 10, divide32(getResolutionY(), 2), getResolutionX() - 10, divide32(getResolutionY(), 2)); // Horizontal : Milieu de la droite

    // Affichage infos clavier
    FontCursor * cursorClavierTime = initCursor(divide32(getResolutionX(), 2) + 10, 10, getResolutionX() - 10, 60);
    char * nbClavierString = (char *) kAlloc(sizeof (char) * 14);
    uint32_t nbClavier = getNbKeyboard();
    itoa(nbClavier, nbClavierString);
    drawLetters(cursorClavierTime, font, "Nb de claviers : ");
    drawLetters(cursorClavierTime, font, nbClavierString);
    drawLetters(cursorClavierTime, font, "\n");
}

void display_process_right_top_text() {
    FontCursor * cursorRight = initCursor(divide32(getResolutionX(), 2) + 10, 90, getResolutionX() - 10, divide32(getResolutionY(), 2) - 10);

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

void display_process_right_bottom_keyboard() {
    FontCursor * cursorLeft = initCursor(divide32(getResolutionX(), 2) + 10, divide32(getResolutionY(), 2) + 10, getResolutionX() - 10, getResolutionY() - 10);
    drawLetters(cursorLeft, font, "Raspberry-OS:~$ ");
    if (getNbKeyboard() > 0) {
        for (;;) {
            KeyboardUpdate();
            char c = KeyboardGetChar();
            if (c != 0) {
                drawLetter(cursorLeft, font, c);
            }
        }
    }
}

void display_process_left_image() {
    FontCursor * cursorImage = initCursor(10, 90, divide32(getResolutionX(), 2) - 10, getResolutionY() - 10);
    drawLetters(cursorImage, font, "-- Images --");

    Image img = loadImage(PPM, &_binary_image_ppm_start, &_binary_image_ppm_end);

    drawLetters(cursorImage, font, "\nWidth : ");
    char * width_string = (char *) kAlloc(sizeof (char) * 14);
    itoa(img.width, width_string);
    drawLetters(cursorImage, font, width_string);

    drawLetters(cursorImage, font, "\nHeight : ");
    char * height_string = (char *) kAlloc(sizeof (char) * 14);
    itoa(img.height, height_string);
    drawLetters(cursorImage, font, height_string);

    drawLetters(cursorImage, font, "\nColor : ");
    char * color_string = (char *) kAlloc(sizeof (char) * 14);
    itoa(img.colorLevel, color_string);
    drawLetters(cursorImage, font, color_string);

    drawLetters(cursorImage, font, "\n\n");

    displayImage(img, cursorImage->cursor_x, cursorImage->cursor_y);
}

void kmain(void) {

    // Initialisation de l'USB
    usb_init();

    // Initialisation du scheduler
    sched_init(SP_PRIORITY);

    // Initialisation des LEDs ...
    hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*) & display_process_top_info, PP_HIGH);
    create_process((func_t*) & display_process_left_image, PP_HIGH);
    create_process((func_t*) & display_process_right_top_text, PP_MEDIUM);
    create_process((func_t*) & display_process_right_bottom_keyboard, PP_MEDIUM);

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
