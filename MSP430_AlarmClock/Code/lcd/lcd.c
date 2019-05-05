#include "lcd.h"
#define DON          0x0F  /* Display on      */
#define DOFF         0x0B  /* Display off     */
#define CURSOR_ON    0x0F  /* Cursor on       */
#define CURSOR_OFF   0x0D  /* Cursor off      */
#define BLINK_ON     0x0F  /* Cursor Blink    */
#define BLINK_OFF    0x0E  /* Cursor No Blink */


#define SHIFT_CUR_LEFT     0x04  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT    0x05  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT    0x06  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT   0x07  /* Display shifts to the right */


#define FOUR_BIT    0x2C  /* 4-bit Interface               */
#define EIGHT_BIT   0x3C  /* 8-bit Interface               */
#define LINE_5X7    0x30  /* 5x7 characters, single line   */
#define LINE_5X10   0x34  /* 5x10 characters               */
#define LINES_5X7   0x38  /* 5x7 characters, multiple line */

union reg
{    unsigned char _byte;            // byte declaration
     struct bit
       {
       unsigned char b0:1; //bit 0
       unsigned char b1:1; //bit 1
       unsigned char b2:1; //bit 2
       unsigned char b3:1; //bit 3
       unsigned char b4:1; //bit 4
       unsigned char b5:1; //bit 5
       unsigned char b6:1; //bit 6
       unsigned char b7:1; //bit 7
       } _bit;
};


union reg* P2_dir = (union reg*)0x2a;
union reg* P2_out = (union reg*)0x29;
union reg* P2_in  = (union reg*)0x28; 

union reg* P1_dir = (union reg*)0x22;
union reg* P1_out = (union reg*)0x21;
union reg* P1_in  = (union reg*)0x20;


#define LCD_RS         P1_out -> _bit.b0
#define LCD_RS_DIR     P1_dir -> _bit.b0
#define LCD_EN         P1_out -> _bit.b1
#define LCD_EN_DIR     P1_dir -> _bit.b1

#define LCD_DATA_4     P1_out -> _bit.b2
#define LCD_DATA_4_DIR P1_dir -> _bit.b2
#define LCD_DATA_5     P1_out -> _bit.b3
#define LCD_DATA_5_DIR P1_dir -> _bit.b3
#define LCD_DATA_6     P1_out -> _bit.b4
#define LCD_DATA_6_DIR P1_dir -> _bit.b4
#define LCD_DATA_7     P1_out -> _bit.b5
#define LCD_DATA_7_DIR P1_dir -> _bit.b5
#define LCD_DATA_7     P1_out -> _bit.b5
#define LCD_DATA_7_DIR P1_dir -> _bit.b5

unsigned char LcdScreen[2][16] = {
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
unsigned char current_row, current_col;

void lcdcmd(unsigned char rs, unsigned char data)
{

    LCD_RS = 0;
    if(rs) LCD_RS = 1;
    waitlcd(2);
    LCD_EN = 0;

    // send the high nibble
	if (data&BIT4) LCD_DATA_4 = 1;
		else LCD_DATA_4 = 0;
	if (data&BIT5) LCD_DATA_5 = 1;
		else LCD_DATA_5 = 0;
	if (data&BIT6) LCD_DATA_6 = 1;
		else LCD_DATA_6 = 0;
	if (data&BIT7) LCD_DATA_7 = 1;
		else LCD_DATA_7 = 0;
    
    waitlcd(2);
    LCD_EN = 1;
    waitlcd(2);
    LCD_EN = 0;

    // send the low nibble
    if (data&BIT0) LCD_DATA_4 = 1;
    	else LCD_DATA_4 = 0;
    if (data&BIT1) LCD_DATA_5 = 1;
    	else LCD_DATA_5 = 0;
    if (data&BIT2) LCD_DATA_6 = 1;
    	else LCD_DATA_6 = 0;
    if (data&BIT3) LCD_DATA_7 = 1;
    	else LCD_DATA_7 = 0;

    waitlcd(2);
    LCD_EN = 1;
    waitlcd(2);
    LCD_EN = 0;

}


void configLCD(void)
{
   // Set all signal pins as output
	LCD_RS_DIR = 1;
	LCD_EN_DIR = 1;
	LCD_DATA_4_DIR = 1;
	LCD_DATA_5_DIR = 1;
	LCD_DATA_6_DIR = 1;
	LCD_DATA_7_DIR = 1;
	//if (backlight_controlled)
		//LCD_BL_DIR = 1;
	
    LCD_RS = 0;
    LCD_EN = 0;
    waitlcd(200);                // delay for power on

    // reset LCD
    lcdcmd(0,0x30);
    waitlcd(50);
    lcdcmd(0,0x30);
    waitlcd(50);
    lcdcmd(0,0x32);
    waitlcd(200);                // delay for LCD reset

    waitlcd(2);     // wait for LCD
    lcdcmd(0,FOUR_BIT & LINES_5X7);            // Set LCD type
    waitlcd(2);     // wait for LCD

    lcdcmd(0,DOFF&CURSOR_OFF&BLINK_OFF);        // display off
    waitlcd(2);     // wait for LCD
    lcdcmd(0,DON&CURSOR_OFF&BLINK_OFF);        // display on
    waitlcd(2);     // wait for LCD

    lcdcmd(0,0x01);              // clear display and move cursor to home
    waitlcd(2);     // wait for LCD
    lcdcmd(0,SHIFT_CUR_LEFT);                // cursor shift mode
    waitlcd(2);     // wait for LCD
    lcdcmd(0,0x01);              // clear display and move cursor to home
    waitlcd(2);     // wait for LCD
}

void waitlcd(volatile unsigned int x)
{
    volatile unsigned int i;
    for (x ;x>1;x--)
    {
        for (i=0;i<=100;i++);
    }
}
void gotoXy(unsigned char  x,unsigned char y)
{
    if(y<40)
    {
        if(x) y |= 0x40;
        y |=0x80;
        lcdcmd(0,y);
    }
}
void DisplayLcdScreenOld()
{
    unsigned char i;
    gotoXy(0,0);
    for (i = 0; i<16; i++)
        lcd_print_char(LcdScreen[0][i]);
    gotoXy(1,0);
    for (i = 0; i<16; i++)
        lcd_print_char(LcdScreen[1][i]);
}

void lcd_print_char (char c)
{
    switch(c){
        case '\f':
            lcdcmd(0, 0x01);
            waitlcd(2);     // wait for LCD
            break;
        case '\n':
            gotoXy(0, 0x01);
            break;
        default:
            lcdcmd(1, c);
            waitlcd(2);     // wait for LCD
            break;
    }
}
unsigned long lcd_power_of(int A, int x)
{
    char i;
    unsigned long temp = 1;
    for(i = 0; i < x; i++)
        temp *= A;
    return temp;
}

void lcd_print_num(long num)
{
    char num_flag = 0;
    char i;

    if(num == 0)
    {
        lcd_print_char('0');
        return;
    }
    if(num < 0)
    {
        lcd_print_char('-');
        num *= -1;
    }

    for(i = 10; i > 0; i--)
    {
        if((num / lcd_power_of(10, i-1)) != 0)
        {
            num_flag = 1;
            lcd_print_char(num/lcd_power_of(10, i-1) + '0');
        }
        else
        {
            if(num_flag != 0)
                lcd_print_char('0');
        }
        num %= lcd_power_of(10, i-1);
    }
}
void lcd_print_str (unsigned char *string)
{
    while (*string)
    {
        lcd_print_char (*string++);
    }
}
void LcdPrintNum(unsigned char x, unsigned char y, long num)
{
    gotoXy(x, y);
    lcd_print_num(num);
}
void LcdPrintString(unsigned char x, unsigned char y, unsigned char * string)
{
    gotoXy(x, y);
    lcd_print_str(string);
}

void lcd_print_charS(unsigned char c)
{
    LcdScreen[current_row][ current_col] = c;
    current_col ++;
}

void LcdPrintCharS(unsigned char x, unsigned char y,unsigned char c)
{
    current_row = x%2;
    current_col = y%16;
    LcdScreen[current_row][ current_col] = c;
}

void LcdPrintNumS(unsigned char x, unsigned char y, long num)
{
    char num_flag = 0;
    char i;
    current_row = x%2;
    current_col = y%16;

    if(num == 0) {
        lcd_print_charS('0');
        return;
    }
    if(num < 0) {
        lcd_print_charS('-');
        num *= -1;
    }
    //else
    //  lcd_print_charS(' ');

    for(i = 10; i > 0; i--)
    {
        if((num / lcd_power_of(10, i-1)) != 0)
        {
            num_flag = 1;
            lcd_print_charS(num/lcd_power_of(10, i-1) + '0');
        }
        else
        {
            if(num_flag != 0)
                lcd_print_charS('0');
        }
        num %= lcd_power_of(10, i-1);
    }
}
void LcdClearS()
{
    char i;
    for (i = 0; i<16; i++)
    {
        LcdScreen[0][i] = ' ';
        LcdScreen[1][i] = ' ';
    }
}
void LcdPrintStringS(unsigned char x, unsigned char y, unsigned char *string)
{
    current_row = x%2;
    current_col = y%16;
    while (*string)  {
        lcd_print_charS (*string++);}
}
