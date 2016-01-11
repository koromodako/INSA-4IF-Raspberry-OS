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

extern unsigned char _binary_img_1_ppm_start;
extern unsigned char _binary_img_1_ppm_end;
extern unsigned char _binary_img_1_pgm_start;
extern unsigned char _binary_img_1_pgm_end;

extern unsigned char _binary_img_2_ppm_start;
extern unsigned char _binary_img_2_ppm_end;
extern unsigned char _binary_img_2_pgm_start;
extern unsigned char _binary_img_2_pgm_end;

extern unsigned char _binary_img_3_ppm_start;
extern unsigned char _binary_img_3_ppm_end;
extern unsigned char _binary_img_3_pgm_start;
extern unsigned char _binary_img_3_pgm_end;

extern unsigned char _binary_img_4_ppm_start;
extern unsigned char _binary_img_4_ppm_end;
extern unsigned char _binary_img_4_pgm_start;
extern unsigned char _binary_img_4_pgm_end;

void display_process_left_image() {
    Image img[] = {
            loadImage(PPM, &_binary_img_1_pgm_start, &_binary_img_1_pgm_end),
            loadImage(PPM, &_binary_img_2_pgm_start, &_binary_img_2_pgm_end),
            loadImage(PPM, &_binary_img_3_pgm_start, &_binary_img_3_pgm_end),
            loadImage(PPM, &_binary_img_4_pgm_start, &_binary_img_4_pgm_end),

            loadImage(PPM, &_binary_img_1_ppm_start, &_binary_img_1_ppm_end),
            loadImage(PPM, &_binary_img_2_ppm_start, &_binary_img_2_ppm_end),
            loadImage(PPM, &_binary_img_3_ppm_start, &_binary_img_3_ppm_end),
            loadImage(PPM, &_binary_img_4_ppm_start, &_binary_img_4_ppm_end)
    };

    for (;;) {
        for (uint8_t i=0; i<8; ++i) {
            draw(0, 0, getResolutionX(), getResolutionY(), 0, 0, 0);
            displayImage(img[i], 5, 5, getResolutionX() - 5, getResolutionY() - 5);
            uint64_t sleep = 0;
            for (sleep = 0; sleep < 300000000; sleep++);
        }
    }
}

void kmain(void) {

    // Initialisation du scheduler
    sched_init(SP_SIMPLE);

    // Initialisation des LEDs ...
    hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*) & display_process_left_image, PP_HIGH);

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    sys_yield();
}
