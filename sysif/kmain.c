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
#include "vmem.h"


void user_process_1(void) {
    uint8_t * addr = (uint8_t *) sys_mmap(8);
    (*addr) = 0xF;

    while (1) {
        addr = addr + 1;
    }
}

void kmain(void) {

    // Initialisation du scheduler
    sched_init(SP_SIMPLE);

    // Creation des processus
    create_process((func_t*) & user_process_1, PP_HIGH);

    // Initialisation du timer matériel pour les IRQ
    timer_init();
    ENABLE_IRQ();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    while (1) {
        sys_yield();
    }
}
