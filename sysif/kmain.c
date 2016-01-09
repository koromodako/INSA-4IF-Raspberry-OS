#include "config.h"
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
#include "vmem.h"

extern unsigned char _binary_img_lena_ppm_start;
extern unsigned char _binary_img_lena_ppm_end;
extern unsigned char _binary_img_lena_pgm_start;
extern unsigned char _binary_img_lena_pgm_end;
extern unsigned char _binary_img_landscape_ppm_start;
extern unsigned char _binary_img_landscape_ppm_end;
extern unsigned char _binary_img_landscape_pgm_start;
extern unsigned char _binary_img_landscape_pgm_end;

void display_process_top_info() {
    FontTable * font = initFont();
    // Affichage text haut gauche
    FontCursor * cursor = initCursor(10, 10, getResolutionX(), getResolutionY());
    drawLetters(cursor, font, "Hello World !\n");
#ifdef USE_VMEM
    char * resolutionX = (char *) sys_mmap(sizeof (char) * 12);
    char * resolutionY = (char *) sys_mmap(sizeof (char) * 12);
#else
    char * resolutionX = (char *) kAlloc(sizeof (char) * 12);
    char * resolutionY = (char *) kAlloc(sizeof (char) * 12);
#endif
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
#ifdef USE_VMEM
    char * nbClavierString = (char *) sys_mmap(sizeof (char) * 14);
#else
    char * nbClavierString = (char *) kAlloc(sizeof (char) * 14);
#endif

    uint32_t nbClavier = getNbKeyboard();
    itoa(nbClavier, nbClavierString);
    drawLetters(cursorClavierTime, font, "Nb de claviers : ");
    drawLetters(cursorClavierTime, font, nbClavierString);
    drawLetters(cursorClavierTime, font, "\n");
    while (1) {
        sys_yield();
    }
}

void display_process_right_top_text() {
    FontTable * font = initFont();
    FontCursor * cursorRight = initCursor(divide32(getResolutionX(), 2) + 10, 90, getResolutionX() - 10, divide32(getResolutionY(), 2) - 10);

    uint32_t letter = 33;
    while (1) {
         drawLetter(cursorRight, font, (char) letter);
         letter++;
         if (letter > 126) {
             letter = 33;
         }
         //uint32_t sleep = 0;
         //for (sleep = 0; sleep < 100000; sleep++);
    }
}

void display_process_right_bottom_keyboard() {
    FontTable * font = initFont();
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
    while (1) {
        sys_yield();
    }
}

void display_process_left_image() {
    Image img[] = {
            loadImage(PPM, &_binary_img_lena_pgm_start, &_binary_img_lena_pgm_end),
            loadImage(PPM, &_binary_img_lena_ppm_start, &_binary_img_lena_ppm_end),
            loadImage(PPM, &_binary_img_landscape_pgm_start, &_binary_img_landscape_pgm_end),
            loadImage(PPM, &_binary_img_landscape_ppm_start, &_binary_img_landscape_ppm_end)
    };

    for (;;) {
        for (uint8_t i=0; i<4; ++i) {
            draw(10, 90, divide32(getResolutionX(), 2) - 5, getResolutionY() - 5, 0, 0, 0);
            displayImage(img[i], 10, 90, divide32(getResolutionX(), 2) - 10, getResolutionY() - 10);
            //uint32_t sleep = 0;
            //for (sleep = 0; sleep < 1000000; sleep++);
        }
    }
}

void kmain(void) {

    // Initialisation de l'USB
    //usb_init();

    // Initialisation du scheduler
    sched_init(SP_SIMPLE);

    // Initialisation des LEDs ...
    //hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*) & display_process_top_info, PP_HIGH);
    //create_process((func_t*) & display_process_left_image, PP_HIGH);
    create_process((func_t*) & display_process_right_top_text, PP_LOW);
    //create_process((func_t*) & display_process_right_bottom_keyboard, PP_MEDIUM);

    // Initialisation du timer materiel pour les IRQ
    timer_init();
    ENABLE_IRQ();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    while (1) {
        sys_yield();
    }
}
