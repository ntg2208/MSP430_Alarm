#include "main.h"

#define LED     P2OUT
#define BUTTON  P2IN
#define ON      1
#define OFF     2

unsigned char ledOutput[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char ledStatus[8] = {0,0,0,0,0,0,0,0};
unsigned char flag_TimerA = 0;
unsigned char count = 0;
unsigned char datawrite[8],dataread[8];

void init_system(void);
void configLEDs(void);

void OpenOutput(int i);
void CloseOutput(int i);
void ReverseOuput(int i);

void rtc_settime(int sec, int min, int hour, int day, int date, int month, int year);
void read_ds1307(void);
unsigned char read_data(unsigned char addr);

unsigned char isButton(unsigned char i);

#define INIT_SYSTEM         0

#define SET_HOUR_ALARM      1
#define SET_MINUTE_ALARM    2
#define SET_ENABLE_ALARM    3

#define SET_HOUR_TIMER      4
#define SET_MINUTE_TIMER    5
#define SET_SECOND_TIMER    6
#define START_TIMER         7

#define START_STOPWATCH     8

#define SET_HOUR            9
#define SET_MINUTE          10
#define SET_DAY             11
#define SET_DATE            12
#define SET_MONTH           13
#define SET_YEAR            14

unsigned char isButtonMode();
unsigned char isButtonSet();
unsigned char isButtonUp();
unsigned char isButtonDown();
unsigned char isButtonTurnOffAlarm();
unsigned char isButtonResetSecond();
unsigned char isButtonStart();

char timeBlink = 0;
char statusSetupTime = INIT_SYSTEM;

void DisplayTime(void);
void ResetSecond(void);
char timeOut = 0;

void DisplayAlarm();
void SetHourAlarm();
void SetMinuteAlarm();
void SetEnableAlarm();
void Alarm();
char enableAlarm = 1, hourAlarm = 0, minAlarm = 0, flagAlarm = 0;

void DisplayTimer();
void SetHourTimer();
void SetMinuteTimer();
void SetSecondTimer();
void Timer();
void TimeOutTimer();
char hourTimer = 0, minTimer = 0, secTimer = 0, enableTimer = 0;

void DisplayStopWatch();
void StopWatch();
void ResetStopWatch();
char hourSW = 0, minSW = 0, secSW = 0, msecSW = 0, enableSW = 0;

void SetupTime();
void SetHour();
void SetMinute();
void SetDay();
void SetDate();
void SetMonth();
void SetYear();
char sec = 0,min = 0,hour = 0;
char day = 1,date = 25,month = 11,year = 18;

//=================================================================================================
void main()
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    init_system();
    rtc_settime(0,0,0,1,25,11,18);
    while(1)
    {
        while(!flag_TimerA);
        flag_TimerA = 0;
        SetupTime();
        Alarm();
        scan_key_button();
        DisplayLcdScreenOld();
    }
}
//===================================================================================================
void init_system()
{
    configLEDs();
    configClocks();
    configTimerA2();
    configInterrupts();
    configButtons();
    configLCD();
    LcdClearS();
    I2C_Init(0x68);
}
void configLEDs()
{
    P2SEL = 0;
    P2SEL2 = 0;
    P2DIR = 0x01;
    P2OUT = 0;
}
void OpenOutput(int i)
{
    if (i>=0 && i <=7)
    {
        LED |= ledOutput[i];
    }
}
void CloseOutput(int i)
{
    if (i>=0 && i <=7)
    {
        LED &= ~ledOutput[i];
    }
}
void ReverseOuput(int i)
{
    if (ledStatus[i]  == ON)
    {
        CloseOutput(i);
        ledStatus[i] = OFF;
    }
    else
    {
        OpenOutput(i);
        ledStatus[i] = ON;
    }
}
unsigned char isButton(unsigned char i)
{
    if (key_code[i] == 1)
        return 1;
    else
        return 0;
}
//====================================================================================================
void rtc_settime(int sec, int min, int hour, int day, int date, int month, int year)
{
    datawrite[0] = Dec2BCD(sec);
    datawrite[1] = Dec2BCD(min);
    datawrite[2] = Dec2BCD(hour);
    datawrite[3] = Dec2BCD(day);
    datawrite[4] = Dec2BCD(date);
    datawrite[5] = Dec2BCD(month);
    datawrite[6] = Dec2BCD(year);
    I2C_Write(0x00,7, datawrite);
}

void read_ds1307()
{
    I2C_Read(0x00, 8, dataread);
    sec = BCD2Dec(dataread[0]);
    min = BCD2Dec(dataread[1]);
    hour = BCD2Dec(dataread[2]);
    day = BCD2Dec(dataread[3]);
    date = BCD2Dec(dataread[4]);
    month = BCD2Dec(dataread[5]);
    year = BCD2Dec(dataread[6]);
}
//=====================================================================================================
void SetupTime()
{
    static unsigned char cnt = 0;
    cnt = (cnt + 1)%20;
    if (cnt == 0)
        timeOut = timeOut + 1;
    switch (statusSetupTime)
    {
    case INIT_SYSTEM:
        DisplayTime();
        ResetSecond();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = SET_HOUR_ALARM;
        }
        break;
    case SET_HOUR_ALARM:
        DisplayAlarm();
        SetHourAlarm();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = SET_HOUR_TIMER;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_MINUTE_ALARM;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_MINUTE_ALARM:
        DisplayAlarm();
        SetMinuteAlarm();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = SET_HOUR_TIMER;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_ENABLE_ALARM;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_ENABLE_ALARM:
        DisplayAlarm();
        SetEnableAlarm();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = SET_HOUR_TIMER;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_HOUR_ALARM;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_HOUR_TIMER:
        DisplayTimer();
        SetHourTimer();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = START_STOPWATCH;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_MINUTE_TIMER;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_MINUTE_TIMER:
        DisplayTimer();
        SetMinuteTimer();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = START_STOPWATCH;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_SECOND_TIMER;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_SECOND_TIMER:
        DisplayTimer();
        SetSecondTimer();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = START_STOPWATCH;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = START_TIMER;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case START_TIMER:
        DisplayTimer();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = START_STOPWATCH;
        }
        if (isButtonSet() && enableTimer == 0)
        {
            timeOut = 0;
            statusSetupTime = SET_HOUR_TIMER;
        }
        if (isButtonStart())
        {
            timeOut = 0;
            enableTimer = enableTimer + 1;
            if (enableTimer > 1)
                enableTimer = 0;
        }
        if (enableTimer == 1)
        {
            timeOut = 0;
            Timer();
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case START_STOPWATCH:
        DisplayStopWatch();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = SET_HOUR;
        }
        if (isButtonSet() && enableSW == 0)
        {
            timeOut = 0;
            ResetStopWatch();
        }
        if (isButtonStart())
        {
            timeOut = 0;
            if (enableSW == 0)
            {
                enableSW = 1;
            }
            else
                enableSW = 0;
        }
        if (enableSW == 1)
        {
            timeOut = 0;
            StopWatch();
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_HOUR:
        DisplayTime();
        SetHour();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_MINUTE;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_MINUTE:
        DisplayTime();
        SetMinute();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_DAY;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_DAY:
        DisplayTime();
        SetDay();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_DATE;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_DATE:
        DisplayTime();
        SetDate();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_MONTH;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_MONTH:
        DisplayTime();
        SetMonth();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_YEAR;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    case SET_YEAR:
        DisplayTime();
        SetYear();
        if (isButtonMode())
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        if (isButtonSet())
        {
            timeOut = 0;
            statusSetupTime = SET_HOUR;
        }
        if (timeOut > 5)
        {
            timeOut = 0;
            statusSetupTime = INIT_SYSTEM;
        }
        break;
    default:
        statusSetupTime = INIT_SYSTEM;
        break;
    }
}
//===============================================================================
unsigned char isButtonMode()
{
    if (key_code[5] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonSet()
{
    if (key_code[4] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonResetSecond()
{
    if (key_code[3] >= 1 && key_code[2] >= 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonTurnOffAlarm()
{
    if (key_code[3] >= 1 && key_code[2] >= 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonUp()
{
    if ((key_code[3] == 1) || (key_code[3] >= 10 && key_code[3]%2 == 0))
        return 1;
    else
        return 0;
}

unsigned char isButtonDown()
{
   if ((key_code[2] == 1) || (key_code[2] >= 10 && key_code[2]%2 == 0))
       return 1;
   else
       return 0;
}

unsigned char enableBlink()
{
    if (key_code[3] == 0 && key_code[2] == 0)
        return 1;
    else
        return 0;
}

unsigned char isButtonStart()
{
    if ((key_code[1] == 1) || (key_code[1] >= 10 && key_code[1]%2 == 0))
        return 1;
    else
        return 0;
}
//==============================================================================
void DisplayTime()
{
    LcdClearS();
    read_ds1307();
    switch(day)
    {
    case 1:
        LcdPrintStringS(0,0,"SUN");
        break;
    case 2:
        LcdPrintStringS(0,0,"MON");
        break;
    case 3:
        LcdPrintStringS(0,0,"TUE");
        break;
    case 4:
        LcdPrintStringS(0,0,"WED");
        break;
    case 5:
        LcdPrintStringS(0,0,"THU");
        break;
    case 6:
        LcdPrintStringS(0,0,"FRI");
        break;
    case 7:
        LcdPrintStringS(0,0,"SAT");
        break;
    default:
        LcdPrintStringS(0,0,"ERR");
        break;
    }
    LcdPrintNumS(0,4,hour/10);
    LcdPrintNumS(0,5,hour%10);
    LcdPrintStringS(0,6,":");
    LcdPrintNumS(0,7,min/10);
    LcdPrintNumS(0,8,min%10);
    LcdPrintStringS(0,9,":");
    LcdPrintNumS(0,10,sec/10);
    LcdPrintNumS(0,11,sec%10);

    switch(enableAlarm)
    {
    case 0:
        LcdPrintStringS(0,13,"OFF");
        break;
    case 1:
        LcdPrintStringS(0,13,"ON ");
        break;
    }

    switch(month)
    {
    case 1:
        LcdPrintStringS(1,2,"JAN");
        break;
    case 2:
        LcdPrintStringS(1,2,"FEB");
        break;
    case 3:
        LcdPrintStringS(1,2,"MAR");
        break;
    case 4:
        LcdPrintStringS(1,2,"APR");
        break;
    case 5:
        LcdPrintStringS(1,2,"MAY");
        break;
    case 6:
        LcdPrintStringS(1,2,"JUN");
        break;
    case 7:
        LcdPrintStringS(1,2,"JUL");
        break;
    case 8:
        LcdPrintStringS(1,2,"AUG");
        break;
    case 9:
        LcdPrintStringS(1,2,"SEP");
        break;
    case 10:
        LcdPrintStringS(1,2,"OCT");
        break;
    case 11:
        LcdPrintStringS(1,2,"NOV");
        break;
    case 12:
        LcdPrintStringS(1,2,"DEC");
        break;
    default:
        LcdPrintStringS(1,2,"ERR");
        // LcdPrintNumS(1, 2, month);
        break;
    }

    LcdPrintStringS(1,5," ");
    LcdPrintNumS(1,6,date/10);
    LcdPrintNumS(1,7,date%10);
    LcdPrintStringS(1,8," ");
    LcdPrintNumS(1,9,20);
    LcdPrintNumS(1,11,year/10);
    LcdPrintNumS(1,12,year%10);
}

void SetHour()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(0,4,"  ");
    if(isButtonUp())
    {
        hour = hour + 1;
        if(hour > 23)
            hour = 0;
        rtc_settime(sec, min, hour, day, date, month, year);
        timeOut = 0;
    }
   if(isButtonDown())
   {
       if(hour == 0)
           hour = 23;
       else hour = hour - 1;
       rtc_settime(sec, min, hour, day, date, month, year);
       timeOut = 0;
     }
}

void SetMinute()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(0,7,"  ");
    if(isButtonUp())
    {
        min = min + 1;
        if(min > 59)
            min = 0;
        rtc_settime(sec, min, hour, day, date, month, year);
        timeOut = 0;
    }
       if(isButtonDown())
       {
           if(min == 0)
                min = 59;
           else min = min - 1;
           rtc_settime(sec, min, hour, day, date, month, year);
           timeOut = 0;
       }
}

void SetDay()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(0,0,"   ");
    if(isButtonUp())
    {
        day = day + 1;
        if(day > 7)
            day = 1;
        rtc_settime(sec, min, hour, day, date, month, year);
        timeOut = 0;
    }
   if(isButtonDown())
   {
         day = day - 1;
         if(day < 1)
            day = 7;
         rtc_settime(sec, min, hour, day, date, month, year);
         timeOut = 0;
       }
}

void SetDate()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,6,"  ");
    if(isButtonUp())
    {
        date = date + 1;
        if(date > 31)
            date = 1;
        rtc_settime(sec, min, hour, day, date, month, year);
        timeOut = 0;
    }
       if(isButtonDown())
       {
           date = date - 1;
           if(date < 1)
               date = 31;
           rtc_settime(sec, min, hour, day, date, month, year);
           timeOut = 0;
       }
}

void SetMonth()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,2,"   ");
    if(isButtonUp())
    {
        month = month + 1;
        if(month > 12)
            month = 1;
        rtc_settime(sec, min, hour, day, date, month, year);
        timeOut = 0;
    }
       if(isButtonDown())
       {
           month = month - 1;
           if(month < 1)
               month = 12;
           rtc_settime(sec, min, hour, day, date, month, year);
           timeOut = 0;
       }
}

void SetYear()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,9,"    ");
    if(isButtonUp())
    {
        year = year + 1;
        if(year > 99)
            year = 0;
        rtc_settime(sec, min, hour, day, date, month, year);
        timeOut = 0;
    }
       if(isButtonDown())
       {
           if(year == 0)
               year = 99;
           else year = year - 1;
           rtc_settime(sec, min, hour, day, date, month, year);
           timeOut = 0;
       }
}

void ResetSecond(void)
{
    if (isButtonResetSecond())
    {
        sec = 0;
        rtc_settime(sec, min, hour, day, date, month, year);
    }
}
//===============================================================================================
void DisplayAlarm()
{
    LcdClearS();
    LcdPrintStringS(0,0,"    SET ALARM    ");
    LcdPrintStringS(1,0,"  ");

    LcdPrintNumS(1,2,hourAlarm/10);
    LcdPrintNumS(1,3,hourAlarm%10);

    LcdPrintStringS(1,4,":");

    LcdPrintNumS(1,5,minAlarm/10);
    LcdPrintNumS(1,6,minAlarm%10);

    LcdPrintStringS(1,7,"   ");

    switch(enableAlarm)
    {
    case 0:
        LcdPrintStringS(1,10,"OFF");
        break;
    case 1:
        LcdPrintStringS(1,10,"ON ");
        break;
    }
    LcdPrintStringS(1,13,"    ");
}

void SetHourAlarm()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,2,"  ");
    if(isButtonUp())
    {
        hourAlarm = hourAlarm + 1;
        if(hourAlarm > 23)
            hourAlarm = 0;
        timeOut = 0;
    }
    if(isButtonDown())
    {
        if(hourAlarm == 0)
           hourAlarm = 23;
        else hourAlarm = hourAlarm - 1;
        timeOut = 0;
    }
}

void SetMinuteAlarm()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,5,"  ");
    if(isButtonUp())
    {
        minAlarm = minAlarm + 1;
        if(minAlarm > 59)
            minAlarm = 0;
        timeOut = 0;
    }
    if(isButtonDown())
    {
        if(minAlarm == 0)
            minAlarm = 59;
        else minAlarm = minAlarm - 1;
        timeOut = 0;
    }
}

void SetEnableAlarm()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,10,"   ");
    if(isButtonUp())
    {
        enableAlarm = enableAlarm + 1;
        if(enableAlarm > 1)
            enableAlarm = 0;
        timeOut = 0;
    }
    if(isButtonDown())
    {
        if(enableAlarm == 0)
           enableAlarm = 1;
        else enableAlarm = enableAlarm - 1;
        timeOut = 0;
    }
}

void Alarm()
{
    if (hour == hourAlarm && min == minAlarm && sec == 0 && enableAlarm == 1)
        flagAlarm = 1;
    if (flagAlarm == 1)
    {
        LcdPrintStringS(0,0,"     ALARM      ");
        LcdPrintStringS(1,0,"                ");
        OpenOutput(0);
        if (isButtonSet())
        {
            CloseOutput(0);
            flagAlarm = 0;
        }
    }
}
//================================================================================================
void DisplayTimer()
{
    LcdClearS();
    LcdPrintStringS(0,5,"Timer:");
    LcdPrintStringS(1,4,"  :  :  ");
    LcdPrintNumS(1, 4, hourTimer/10);
    LcdPrintNumS(1, 5, hourTimer%10);
    LcdPrintNumS(1, 7, minTimer/10);
    LcdPrintNumS(1, 8, minTimer%10);
    LcdPrintNumS(1, 10, secTimer/10);
    LcdPrintNumS(1, 11, secTimer%10);
}

void SetHourTimer()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,4,"  ");
    if(isButtonUp())
    {
        hourTimer = hourTimer + 1;
        if(hourTimer > 23)
            hourTimer = 0;
        timeOut = 0;
    }
    if(isButtonDown())
    {

        if(hourTimer == 0)
          hourTimer = 23;
        else hourTimer = hourTimer - 1;
        timeOut = 0;
      }
}

void SetMinuteTimer()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,7,"  ");
    if(isButtonUp())
    {
        minTimer = minTimer + 1;
        if(minTimer > 59)
            minTimer = 0;
        timeOut = 0;
    }
    if(isButtonDown())
    {
        if(minTimer == 0)
            minTimer = 59;
        else minTimer = minTimer - 1;
        timeOut = 0;
    }
}

void SetSecondTimer()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,10,"  ");
    if(isButtonUp())
    {
        secTimer = secTimer + 1;
        if(secTimer > 59)
            secTimer = 0;
        timeOut = 0;
    }
    if(isButtonDown())
    {
        if(secTimer == 0)
            secTimer = 59;
        else secTimer = secTimer - 1;
        timeOut = 0;
    }
}

void Timer()
{
    static unsigned char cntTimer = 0;
    cntTimer = (cntTimer + 1)%10;
    if (secTimer == 0 && minTimer == 0 && hourTimer == 0)
        TimeOutTimer();
    else
    {
        if (cntTimer == 0)
        {
            if (secTimer == 0)
                secTimer = 59;
            else secTimer = secTimer - 1;
            if (secTimer == 59)
            {
                if (minTimer == 0)
                    minTimer = 59;
                else minTimer = minTimer - 1;
            }
            if (minTimer == 59 && secTimer == 59)
            {
                if (hourTimer == 0)
                    hourTimer = 23;
                else
                    hourTimer = hourTimer - 1;
            }
        }
    }
}

void TimeOutTimer()
{
    timeBlink = (timeBlink + 1)%10;
    if(timeBlink < 5 && enableBlink())
        LcdPrintStringS(1,4,"  :  :  ");
}
//=====================================================================================================
void DisplayStopWatch()
{
    LcdClearS();
    LcdPrintStringS(0, 3, "Stop Watch");
    LcdPrintStringS(1, 2, "  :  :  :  ");
    LcdPrintNumS(1, 2, hourSW/10);
    LcdPrintNumS(1, 3, hourSW%10);
    LcdPrintNumS(1, 5, minSW/10);
    LcdPrintNumS(1, 6, minSW%10);
    LcdPrintNumS(1, 8, secSW/10);
    LcdPrintNumS(1, 9, secSW%10);
    LcdPrintNumS(1, 11, msecSW);
    LcdPrintNumS(1, 12, 0);
}

void StopWatch()
{
    static unsigned char cntSW = 0;
    cntSW = (cntSW + 1) % 5;
    msecSW = cntSW * 2;
    if (msecSW >9)
        msecSW = 0;
    if (cntSW == 0)
        secSW = (secSW + 1);
    if (secSW > 59)
    {
        minSW = minSW + 1;
        secSW = 0;
    }
    if (minSW > 59)
    {
        hourSW = hourSW + 1;
        minSW = 0;
    }
    if (hourSW > 23)
        hourSW = 0;
}

void ResetStopWatch()
{
    msecSW = 0;
    secSW = 0;
    minSW = 0;
    hourSW = 0;
}
//====================================================================================================
