#include "timer.h"

void configClocks()
{
    BCSCTL1 = CALBC1_12MHZ;          // Set range
    DCOCTL = CALDCO_16MHZ;           // Set DCO step + modulation
    BCSCTL3 |= LFXT1S_2;            // LFXT1 = VLO
    IFG1 &= ~OFIFG;                 // Clear OSCFault flag
    BCSCTL2 |= SELM_0 + DIVM_2 + DIVS_2; 

}
void configTimerA2()
{
    CCTL0 = CCIE;
    CCR0  = 50000;

    TACTL = TASSEL_2 + MC_2; 
}
