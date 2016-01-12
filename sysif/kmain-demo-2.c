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

void kmain(void) {

    // Initialisation du scheduler
    sched_init(SP_SIMPLE);

    // Initialisation des LEDs ...
    hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    // Affichage text haut gauche
    uint32_t resolutionXValue = getResolutionX();
    uint32_t resolutionYValue = getResolutionY();

    // Affichage séparateurs
    for(uint32_t y = 0; y * 5 < resolutionYValue; y++) {
        if (!mod32(y, 2)) {
            drawLine(10, y * 5, resolutionXValue - 10, y * 30);
        } else {
            uint32_t sleep;
            for(sleep = 0; sleep < 100000000; sleep++);
        }
    }
}
