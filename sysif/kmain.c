#include "src/syscall.h"
#include "src/sched.h"
#include "util.h"
#include "kheap.h"
#include "sched.h"

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

void kmain( void )
{
    sched_init();

    p1 = create_process((func_t*) &user_process_1);
    p2 = create_process((func_t*) &user_process_2);

    __asm("cps 0x10"); // switch CPU to USER mode
    // **********************************************************************

    sys_yieldto(p1);

    // this is now unreachable
    PANIC();
}
