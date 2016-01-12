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
static uint32_t process1Begin;
static uint32_t process2Begin;
static uint32_t process3Begin;
static uint32_t process4Begin;
static uint32_t process5Begin;

#ifdef QEMU
#define PROCESS_1_BEGIN process1Begin
#define PROCESS_2_BEGIN process2Begin
#define PROCESS_3_BEGIN process3Begin
#define PROCESS_4_BEGIN process4Begin
#define PROCESS_5_BEGIN process5Begin
#else
#define PROCESS_1_BEGIN 150
#define PROCESS_2_BEGIN PROCESS_1_BEGIN + 2 * font->max_height + 20
#define PROCESS_3_BEGIN PROCESS_2_BEGIN + 2 * font->max_height + 20
#define PROCESS_4_BEGIN PROCESS_3_BEGIN + 2 * font->max_height + 20
#define PROCESS_5_BEGIN PROCESS_4_BEGIN + 2 * font->max_height + 20
#endif

void run(FontCursor * cursor) {
    uint32_t sleep;
    while (1) {
         drawLetter(cursor, font, '_');
         sleep = 0;
#ifdef QEMU
         for (sleep = 0; sleep < 10000000; sleep++);
#else
         for (sleep = 0; sleep < 1000000; sleep++);
#endif
    }
}

void display_process_1() {
    FontCursor * cursorInfo = initCursor(15, PROCESS_1_BEGIN, resolutionXValue - 10, PROCESS_1_BEGIN + font->max_height);
    drawLetters(cursorInfo, font, "Processus 1 : Tres prioritaire");
    uint32_t beginRun = cursorInfo->cursor_y + font->max_height + 1;
    FontCursor * cursorRun = initCursor(15, beginRun, resolutionXValue - 10, beginRun + font->max_height);
    run(cursorRun);
}

void display_process_2() {
    FontCursor * cursorInfo = initCursor(15, PROCESS_2_BEGIN, resolutionXValue - 10, PROCESS_2_BEGIN + font->max_height);
    drawLetters(cursorInfo, font, "Processus 2 : Prioritaire");
    uint32_t beginRun = cursorInfo->cursor_y + font->max_height + 1;
    FontCursor * cursorRun = initCursor(15, beginRun, resolutionXValue - 10, beginRun + font->max_height);
    run(cursorRun);
}

void display_process_3() {
    FontCursor * cursorInfo = initCursor(15, PROCESS_3_BEGIN, resolutionXValue - 10, PROCESS_3_BEGIN + font->max_height);
    drawLetters(cursorInfo, font, "Processus 3 : Normal");
    uint32_t beginRun = cursorInfo->cursor_y + font->max_height + 1;
    FontCursor * cursorRun = initCursor(15, beginRun, resolutionXValue - 10, beginRun + font->max_height);
    run(cursorRun);
}

void display_process_4() {
    FontCursor * cursorInfo = initCursor(15, PROCESS_4_BEGIN, resolutionXValue - 10, PROCESS_4_BEGIN + font->max_height);
    drawLetters(cursorInfo, font, "Processus 4 : Peu prioritaire");
    uint32_t beginRun = cursorInfo->cursor_y + font->max_height + 1;
    FontCursor * cursorRun = initCursor(15, beginRun, resolutionXValue - 10, beginRun + font->max_height);
    run(cursorRun);
}

void display_process_5() {
    FontCursor * cursorInfo = initCursor(15, PROCESS_5_BEGIN, resolutionXValue - 10, PROCESS_5_BEGIN + font->max_height);
    drawLetters(cursorInfo, font, "Processus 5 : Tres peu prioritaire");
    uint32_t beginRun = cursorInfo->cursor_y + font->max_height + 5;
    FontCursor * cursorRun = initCursor(15, beginRun, resolutionXValue - 10, beginRun + font->max_height);
    run(cursorRun);
}

void kmain(void) {

    SCHEDULING_POLICY policy = SP_SIMPLE;

    // Initialisation du scheduler
    sched_init(policy);

    // Initialisation des LEDs ...
    hw_init();

    // Initialisation de l'affichage
    FramebufferInitialize();

    // Creation des processus
    create_process((func_t*) & display_process_1, PP_ULTRA_HIGH);
    create_process((func_t*) & display_process_2, PP_HIGH);
    create_process((func_t*) & display_process_3, PP_MEDIUM);
    create_process((func_t*) & display_process_4, PP_LOW);
    create_process((func_t*) & display_process_5, PP_ULTRA_LOW);

    // Initialisation du timer matériel pour les IRQ
    timer_init();
    ENABLE_IRQ();

    // switch CPU to USER mode
    SWITCH_TO_USER_MODE;

    font = initFont();

    // Affichage text haut gauche
    resolutionXValue = getResolutionX();
    resolutionYValue = getResolutionY();
    FontCursor * cursor = initCursor(10, 10, resolutionXValue, resolutionYValue);
    drawLetters(cursor, font, "Test : ");
    if (policy == SP_SIMPLE) {
        drawLetters(cursor, font, "Round-robin");
    } else {
        drawLetters(cursor, font, "Files de priorites");
    }
    drawLetters(cursor, font, "\n");

    // Affichage séparateurs
    drawLine(10, cursor->cursor_y + 5, resolutionXValue - 10, cursor->cursor_y + 5); // Horizontal : Entre les infos du haut et le reste
    drawLine(10, cursor->cursor_y + 15, 10, resolutionYValue - 10); // Vertical : Ligne de départ
    drawLine(resolutionXValue - 10, cursor->cursor_y + 15, resolutionXValue - 10, resolutionYValue - 10); // Vertical : Ligne de fin

#ifdef QEMU
    process1Begin = cursor->cursor_y + 25;
    process2Begin = process1Begin + 2 * font->max_height + 20;
    process3Begin = process2Begin + 2 * font->max_height + 20;
    process4Begin = process3Begin + 2 * font->max_height + 20;
    process5Begin = process4Begin + 2 * font->max_height + 20;
#endif

    while (1) {
        sys_yield();
    }
}
