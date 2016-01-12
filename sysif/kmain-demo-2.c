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
static uint32_t resolutionXValue;
static uint32_t resolutionYValue;
static uint32_t processBegin;

void display_process_keyboard() {
    FontCursor * cursorLeft = initCursor(10, processBegin, resolutionXValue - 10, resolutionYValue - 10);
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
    create_process((func_t*) & display_process_keyboard, PP_MEDIUM);

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    font = initFont();

    // Affichage text haut gauche
    resolutionXValue = getResolutionX();
    resolutionYValue = getResolutionY();
    FontCursor * cursor = initCursor(10, 10, getResolutionX(), getResolutionY());
    drawLetters(cursor, font, "Test : Clavier\n");

    // Affichage séparateurs
    drawLine(10, cursor->cursor_y + 5, resolutionXValue - 10, cursor->cursor_y + 5); // Horizontal : Entre les infos du haut et le reste

    processBegin = cursor->cursor_y + 25;

    sys_yield();
}
