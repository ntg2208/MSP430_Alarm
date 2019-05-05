

#ifndef lcd_1602_
#define lcd_1602_
#include "lcd_1602.c"

 

#ifndef MCLK_F
#define MCLK_F 1 // frequency of Master Clock in MHz
#endif
// This definition supports to lcd delay functions. You should change it to the
// right MCLK frequency that you configure through "Config_Clocks" function.


 /****************************************************************************
* FUNCTIONS 'S PROTOTYPES
******************************************************************************/
// For further description, see LCD.c
void lcd_delay_us (unsigned long t);
void lcd_delay_ms (unsigned long t);
void lcd_cmd(unsigned char rs, unsigned char data);

void conf_lcd(unsigned char backlight_controlled);
//void lcd_backlight(unsigned char on);
void lcd_clear(void);
void lcd_gotoxy(unsigned char col, unsigned char row);
void lcd_putc(char c);
void lcd_puts(const char* s);
void lcd_2line (const char* s1 , const char* s2);
void lcd_number (unsigned long val, char dec, unsigned char neg);
//******************************************************************************
//-----------------Updated by Thanh Trieu---------------------------------------
void lcd_shift2l (const char* s1,const char* s2);//shift all to left
void lcd_shift2r (const char* s1,const char* s2);//shift all to right
void lcd_2str(int i,char *s);//move cursor to home

void DisplayLcdScreenOld();
unsigned long lcd_power_of(int A, int x);
void lcd_print_num(long num);
void LcdPrintStringS(unsigned char x, unsigned char y, unsigned char *string);
void LcdClearS();
void LcdPrintNumS(unsigned char x, unsigned char y, long num);
void LcdPrintCharS(unsigned char x, unsigned char y,unsigned char c);
void lcd_print_charS(unsigned char c);
void LcdPrintString(unsigned char x, unsigned char y, unsigned char * string);
void LcdPrintNum(unsigned char x, unsigned char y, long num);
#endif 

