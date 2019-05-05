#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <msp430g2553.h>

#define BUTTON P2IN

void configButtons(void);
void scan_key_button(void);


extern unsigned int key_code[];
#endif
