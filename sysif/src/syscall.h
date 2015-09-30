#include <stdint.h>

#define QEMU


void sys_reboot();
void do_sys_reboot();

void sys_nop();
void do_sys_nop();

void sys_settime(uint64_t date_ms);
void do_sys_settime();

void swi_handler();
