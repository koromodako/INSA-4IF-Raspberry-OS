#include "src/syscall.h"
#include "src/sched.h"
#include "util.h"

struct pcb_s pcb1, pcb2;
struct pcb_s *p1, *p2;

void user_process_1()
{
    int v1=5;
    while(1)
    {
        v1++;
        sys_yieldto(p2);
    }
}

void user_process_2()
{
    int v2=-12;
    while(1)
    {
        v2-=2;
        sys_yieldto(p1);
    }
}

void dummy()
{
    return;
}

void __attribute__((naked)) dummyNaked()
{
    return;
}

int div(int dividend, int divisor)
{

    int result = 0;
    int remainder = dividend;

    while (remainder >= divisor) {
        result++;
        remainder -= divisor;
    }
    return result;
}

int compute_volume(int rad)
{
    int rad3 = rad * rad * rad;
    return div(4*355*rad3, 3*113);
}

int kmain( void )
{
    sched_init();

    p1 = &pcb1;
    p2 = &pcb2;

    // initialize p1 and p2
    //p1->lr = (uint32_t) &user_process_1;
    p1->lr = (uint32_t) 0x42;
    p2->lr = (uint32_t) &user_process_2;

    __asm("cps 0x10"); // switch CPU to USER mode

    // **********************************************************************
    sys_yieldto(p1);

    // this is now unreachable
    PANIC();

    // Changements de mode -----------------------------------------------------

    // On change le mode en USER
    __asm("cps #16");

    // On change le mode en SVC
    //__asm("cps #19");

    // Appels systemes ---------------------------------------------------------
    //struct pcb_s* dest = (struct pcb_s*)malloc(sizeof(struct pcb_s));
    //sys_yieldto();
    //free(dest);

    sys_settime(0x42);

    sys_gettime();

    sys_nop();
    sys_reboot();

    //__asm("bl dummy");

    int radius = 5;

    // On met radius dans r2
    //__asm("mov r2, %0" : : "r"(radius));

    // On met r3 dans radius
    //__asm("mov %0, r3" : "=r"(radius));

    int volume;
    dummy();
    dummyNaked();
    volume = compute_volume(radius);
    return volume;
}
