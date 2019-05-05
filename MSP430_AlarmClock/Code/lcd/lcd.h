#ifndef LCD16_H_
#define LCD16_H_


#include <msp430g2553.h>
#define  EN BIT6
#define  RS BIT7
#define LcdDIR P1DIR
#define LcdOUT P1OUT

void lcd_write_data(unsigned char l);
void configLCD(void);
void waitlcd(unsigned int x);
void gotoXy(unsigned char  x,unsigned char y);
void DisplayLcdScreenOld(void);
void lcd_print_char (char c);
unsigned long lcd_power_of(int A, int x);
void lcd_print_num(long num);
void lcd_print_str (unsigned char *string);
void LcdPrintNum(unsigned char x, unsigned char y, long num);
void LcdPrintString(unsigned char x, unsigned char y, unsigned char * string);
void lcd_print_charS(unsigned char c);
void LcdPrintCharS(unsigned char x, unsigned char y,unsigned char c);
void LcdPrintNumS(unsigned char x, unsigned char y, long num);
void LcdPrintStringS(unsigned char x, unsigned char y, unsigned char *string);
void LcdClearS(void);

#endif
