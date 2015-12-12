#include "util.h"
#include "syscall.h"
#include "sched.h"

pcb_s *p1, *p2;

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

void kmain( void )
{    
    sched_init(SP_SIMPLE);
    
    p1=create_process((func_t*)&user_process_1, PP_MEDIUM);
    p2=create_process((func_t*)&user_process_2, PP_MEDIUM);
    
    __asm("cps 0x10"); // switch CPU to USER mode
    // **********************************************************************
    
    sys_yieldto(p1);

    // this is now unreachable
    PANIC();
}
