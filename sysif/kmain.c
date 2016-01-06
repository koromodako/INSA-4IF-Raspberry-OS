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
#include "drivers/usb/csud/include/device/hid/keyboard.h"

static FontTable * font;

extern unsigned char _binary_blob_bin_start;
extern unsigned char _binary_blob_bin_end;

void display_process_info() {
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

    drawLine(10, 70, getResolutionX() - 10, 70);
    drawLine(divide32(getResolutionX(), 2), 80, divide32(getResolutionX(), 2), getResolutionY() - 10);
}

void display_process_info_keyboard() {
    uint32_t x = divide32(getResolutionX(), 2) + 10;
    FontCursor * cursorNbClavier = initCursor(x, 10, getResolutionX() - 10, 60);

    char * nbClavierString = (char *) kAlloc(sizeof (char) * 14);
    char * nbTouchesString = (char *) kAlloc(sizeof (char) * 14);

    uint32_t nbClavier = getNbKeyboard();
    uint32_t nbTouches = getKeyDownCount();

    uint32_t firstTime = 1;

    while (1) {
        uint32_t sleep = 0;

        if (firstTime == 1 || nbClavier != getNbKeyboard() || nbTouches != getKeyDownCount()) {
            firstTime = 0;
            draw(x, 10, getResolutionX() - 10, 60, 0, 0, 0);
            cursorNbClavier->cursor_x = x;
            cursorNbClavier->cursor_y = 10;
            nbClavier = getNbKeyboard();
            itoa(nbClavier, nbClavierString);
            drawLetters(cursorNbClavier, font, "Nb de claviers : ");
            drawLetters(cursorNbClavier, font, nbClavierString);
            drawLetters(cursorNbClavier, font, "\n");
            itoa(nbTouches, nbTouchesString);
            drawLetters(cursorNbClavier, font, "Nb de touches enfoncees : ");
            drawLetters(cursorNbClavier, font, nbTouchesString);
        }

        for (sleep = 0; sleep < 100000; sleep++);
    };

}

void display_process_text_left() {
    FontCursor * cursorLeft = initCursor(10, 90, divide32(getResolutionX(), 2) - 10, getResolutionY() - 10);
    for (;;) {
        char c = keyboardCall();
        //if (c != 'a') {
            drawLetter(cursorLeft, font, c);
            led_switch();
        //}
    }
}

void display_process_text_right() {
    FontCursor * cursorRight = initCursor(divide32(getResolutionX(), 2) + 10, 90, getResolutionX() - 10, getResolutionY() - 10);
    drawLetters(cursorRight, font, "lecture de l'image ... \n");

    unsigned char *pblob = &_binary_blob_bin_start;
    while(((char) *pblob) != '\n')
    {
       pblob++;
    }

    // On lit la largeur
    drawLetters(cursorRight, font, "width ");

    uint32_t size_char_x = 0;
    while(((char) *pblob) != ' ')
    {
       size_char_x++;
       pblob++;
    }

    drawLetters(cursorRight, font, " : ");

    uint32_t width = 0;
    uint32_t mult = size_char_x - 1;
    uint32_t power_mult = 1;
    while(mult > 0)
    {
       pblob--;
       //width += ((uint32_t) ((char) *pblob)) * (size_char_x - mult); 
       width += (((uint32_t) ((char) *pblob)) - 48) * (power_mult); 
       power_mult *= 10;
       mult--;
    }

    char * width_string = (char *) kAlloc(sizeof (char) * 14);
    itoa(width, width_string);
    drawLetters(cursorRight, font, width_string);


    // on avance jusqu'au début de la hauteur
    while(((char) *pblob) != ' ')
    {
       pblob++;
    }

    // On lit la hauteur
    drawLetters(cursorRight, font, "\nheight");
    uint32_t size_char_y = 0;
    while(((char) *pblob) != '\n')
    {
       size_char_y++;
       pblob++;
    }    

   drawLetters(cursorRight, font, " : ");

    uint32_t height = 0;
    mult = size_char_y - 1;
    power_mult = 1;
    while(mult > 0)
    {
       pblob--;
       height += ((uint32_t) ((char) *pblob) - 48) * (power_mult); 
       power_mult *= 10;
       mult--;
    }

    char * height_string = (char *) kAlloc(sizeof (char) * 14);
    itoa(height, height_string);
    drawLetters(cursorRight, font, height_string);

    while(((char) *pblob) != '\n')
    {
       pblob++;
    }

    while(((char) *pblob) != '\n')
    {
       pblob++;
    }

    drawLetters(cursorRight, font, "\nfin de lecture du header");


    uint32_t i;
    uint32_t j;

    for (j = 0; j < height; ++j)
    {
      for (i = 0; i < width; ++i)
       {
        uint32_t red = (uint32_t) *pblob;  /* red */
        pblob++;
        uint32_t green  = (uint32_t) *pblob;  /* green */
        pblob++;
        uint32_t blue = (uint32_t) *pblob;  /* blue */
        pblob++;
        put_pixel_RGB24(i, j, red, green, blue);
        }
    }

    // uint32_t letter = 33;
    // while (1) {
    //     drawLetter(cursorRight, font, (char) letter);
    //     letter++;
    //     if (letter > 126) {
    //         letter = 33;
    //     }

    uint32_t sleep = 0;
    for (sleep = 0; sleep < 100000; sleep++);

    return;
    
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
    create_process((func_t*) & display_process_info_keyboard, PP_HIGH);
   // create_process((func_t*) & display_process_text_left, PP_MEDIUM);
    create_process((func_t*) & display_process_text_right, PP_MEDIUM);

    // Initialisation du timer matériel pour les IRQ
    timer_init();
    ENABLE_IRQ();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    font = initFont();
    KeyboardLedsOn();

    while (1) {
        sys_yield();
    }
}
