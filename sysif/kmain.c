#include "util.h"
#include "sched.h"
#include "hw.h"
#include "asm_tools.h"

void user_process_1() {
    int v1 = 5;
    while (1) {
        v1++;
    }
}

void user_process_2() {
    int v2 = -12;
    while (1) {
        v2 -= 2;
    }
}

void user_process_3() {
    int v3 = 0;
    while (1) {
        v3 += 5;
    }
}

void kmain(void) {
    sched_init(SP_QUEUE);
    create_process((func_t*)&user_process_1, PP_HIGH);
    create_process((func_t*)&user_process_2, PP_MEDIUM);
    create_process((func_t*)&user_process_3, PP_LOW);
    timer_init();
    ENABLE_IRQ();
    __asm("cps 0x10");
    // switch CPU to USER mode
    // **********************************************************************
    while (1) {
        sys_yield();
    }
}
