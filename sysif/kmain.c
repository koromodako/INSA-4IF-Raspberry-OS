#include "util.h"
#include "sched.h"
#include "hw.h"
#include "asm_tools.h"
#include "usb.h"
#include "keyboard.h"

void keyboard_handler_ps() {
    for(;;)
    {       
        KeyboardsUpdate();
        char c = KeyboardGetChar(0);
        if(c != 0)
        {
            // Print c
        }
    }
}

void kmain(void) {
    // Initialisation de l'USB
    usb_init();
    // Initialisation du scheduler
    sched_init(SP_PRIORITY_QUEUE);
    // Creation des processus
    create_process((func_t*)&keyboard_handler_ps, PP_HIGH);
    // Initialisation du timer matériel pour les IRQ
    timer_init();
    ENABLE_IRQ();
    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;
    // **********************************************************************
    while (1) {
        sys_yield();
    }
}
