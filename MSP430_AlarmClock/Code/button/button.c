#include "button.h"

unsigned char keyMask[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned int key_code[8] = {0,0,0,0,0,0,0,0};

void configButtons()
{
    P2SEL = 0;
    P2SEL2 = 0;
    P2DIR &= ~0xfe;
    P2OUT |= 0xfe;
    P2REN |= 0xfe;
}
void scan_key_button()
{
    int i;
    for(i=0;i<8;i++)
    {
        if((BUTTON & keyMask[i]) == 0)
            key_code[i] = key_code[i] + 1;
        else
            key_code[i] = 0;

    }
}
