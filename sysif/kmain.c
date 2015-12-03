#include "util.h"
#include "sched.h"
#include "hw.h"
#include "asm_tools.h"
#include "fb.h"

void user_process_1() {
    while(1) {
        //drawRed();
    }
}

void user_process_2() {
    while(1) {
        //drawBlue();
    }
}

void user_process_3() {
    while(1) {
        //draw();
        drawSomething();
    }
}

void kmain(void) {
    // Initialisation du scheduler
    sched_init(SP_PRIORITY_QUEUE);

    hw_init();
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*)&user_process_1, PP_HIGH);
    create_process((func_t*)&user_process_2, PP_HIGH);
    create_process((func_t*)&user_process_3, PP_HIGH);
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
