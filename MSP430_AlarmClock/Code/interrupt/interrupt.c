#include "interrupt.h"

void configInterrupts()
{
    _BIS_SR(GIE);
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    flag_TimerA = 1;
    CCR0 += 50000;
}
