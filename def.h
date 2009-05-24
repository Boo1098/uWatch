
#ifndef __DEF_H
#define __DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <rtcc.h>
#include <timer.h>
#include <uart.h>
#include <pwrmgnt.h>
#include <ports.h>
#include <string.h>
#include "uWatch-op.h"

#define LUNAR_LANDER 0


#define RevString   "1.6.0.1"

//define all the I/O pins
#define Row1        _RB10
#define Row2        _RB11
#define Row3        _RB12
#define Row4        _RB13
#define Row5        _RA10
#define Row6        _RA7
#define Row7        _RB14
#define Col1        _RB15
#define Col2        _RA0
#define Col3        _RC9
#define Col4        _RA1



#define LCD_RS      _RA2
#define LCD_EN      _RA3

#define LCD_POWER   _RA8
#define LCD_PWM     _RB2
#define LED_POWER   _RA9
#define LED_PWM     _RB3

#define SDA         _RB9
#define SCL         _RB8
#define PER_POWER   _RC8
#define TXD         _RB6
#define RXD         _RB5
#define IRLED       _RB7

#define KeyDelay        200     //200ms key delay
#define MaxLCDdigits    16
#define XBufSize        128

#define OR0  1          //OR function bit masks
#define OR1  2
#define OR2  4
#define OR3  8
#define OR4  16
#define OR5  32
#define OR6  64
#define OR7  128
#define OR8  256
#define OR9  512
#define OR10 1024
#define OR11 2048
#define OR12 4096
#define OR13 8192
#define OR14 16384
#define OR15 32768

#define AND0  0xFFFE        //AND function bit masks
#define AND1  0xFFFD
#define AND2  0xFFFB
#define AND3  0xFFF7
#define AND4  0xFFEF
#define AND5  0xFFDF
#define AND6  0xFFBF
#define AND7  0xFF7F
#define AND8  0xFEFF
#define AND9  0xFDFF
#define AND10 0xFBFF
#define AND11 0xF7FF
#define AND12 0xEFFF
#define AND13 0xDFFF
#define AND14 0xBFFF
#define AND15 0x7FFF

// define LCD bit port commands
// write to the LATCH register instead of the PORT register
//   to avoid issues with Read-Modify-Write
#define SetLCD_POWER    LATA=LATA|OR8
#define ClearLCD_POWER  LATA=LATA&AND8
#define SetLED_POWER    LATA=LATA|OR9
#define ClearLED_POWER  LATA=LATA&AND9
#define SetLCD_D4       LATC=LATC|OR4
#define SetLCD_D5       LATC=LATC|OR5
#define SetLCD_D6       LATC=LATC|OR6
#define SetLCD_D7       LATC=LATC|OR7
#define ClearLCD_D4     LATC=LATC&AND4
#define ClearLCD_D5     LATC=LATC&AND5
#define ClearLCD_D6     LATC=LATC&AND6
#define ClearLCD_D7     LATC=LATC&AND7
#define SetLCD_RS       LATA=LATA|OR2
#define ClearLCD_RS     LATA=LATA&AND2
#define SetLCD_EN       LATA=LATA|OR3
#define ClearLCD_EN     LATA=LATA&AND3

#define PeripheralIOON()    LATC=LATC|OR8
#define PeripheralIOOFF()   LATC=LATC&AND8

#define DisableSleepTimer()     IEC0bits.T1IE = 0       //disable Timer1 interrupt
#define EnableSleepTimer()      IEC0bits.T1IE = 1       //enable Timer1 interrupt

#define StopSleepTimer()  T1CONbits.TON = 0

#define StartSleepTimer() T1CONbits.TON = 1

#define SDA1            LATB=LATB|OR9
#define SDA0            LATB=LATB&AND9
#define SCL1            LATB=LATB|OR8
#define SCL0            LATB=LATB&AND8

//define keypad rows and columns
// "set" outputs a 0 on the row pin
// "clear sets the row pin to an input, to prevent to keys at once causing a short
#define SetRow1         _TRISB10=0
#define SetRow2         _TRISB11=0
#define SetRow3         _TRISB12=0
#define SetRow4         _TRISB13=0
#define SetRow5         _TRISA10=0
#define SetRow6         _TRISA7=0
#define SetRow7         _TRISB14=0
#define ClearRow1       _TRISB10=1
#define ClearRow2       _TRISB11=1
#define ClearRow3       _TRISB12=1
#define ClearRow4       _TRISB13=1
#define ClearRow5       _TRISA10=1
#define ClearRow6       _TRISA7=1
#define ClearRow7       _TRISB14=1


#define MODE_NONE 0
#define MODE_EXIT -1
#define MODE_KEYMODE -2
#define MODE_KEYCLEAR -3
#define MODE_KEY_NEXT -4
#define MODE_KEY_PREVIOUS -5

#define Key1        17
#define Key2        18
#define Key3        19
#define Key4        10
#define Key5        11
#define Key6        12
#define Key7        3
#define Key8        4
#define Key9        5
#define Key0        16
#define KeyPlus     22
#define KeyMinus    15
#define KeyDiv      1
#define KeyMult     8
#define KeyPoint    9
#define KeyMenu     21
#define KeyEnter    25
#define KeyClear    7
#define KeySign     2
#define KeyEXP      20
#define KeyRCL      24
#define KeyRP       14
#define KeyXY       6
#define KeyLP       13
#define KeyMode     23

void UpdateLCDline1 ( const char* s );
void UpdateLCDline2 ( const char* s );
int OneLineNumberEntry();

#define ENTER(key) ( ((key) == KeyEnter) || ((key)==KeyLP) )

unsigned int GetDebouncedKey();
void backlightControl();

extern char out[];

extern double Regs[], iRegs[];
extern char DisplayXreg[];

#define Xreg (Regs[0])
#define Yreg (Regs[1])
#define Zreg (Regs[2])
#define Treg (Regs[3])
#define iXreg (iRegs[0])
#define iYreg (iRegs[1])
#define iZreg (iRegs[2])
#define iTreg (iRegs[3])

void Clock1MHz ( void );
void Clock4MHz ( void );
void Clock250KHz ( void );
void Clock125KHz ( void );
void Clock32KHz ( void );

//************************************
// resets the sleep timer
#define ResetSleepTimer()   TMR1=0


extern rtccTime Time;
extern rtccDate Date;

unsigned int BCDtoDEC ( unsigned int num );
unsigned int DECtoBCD ( unsigned int num );
unsigned int KeyScan2();

void dayHasChanged();
extern BOOL DST;


BOOL inDST ( int* gap );

void UpdateXregDisplay ( void );
void ResetFlags ( void );

extern int  EnableXregOverwrite;

#define NEXT( key ) ( (key) == KeyMenu || (key) == KeyPlus || (key) == KeySign )
#define PREVIOUS( key ) ( (key) == KeyRP || (key) == KeyMinus || (key) == KeyEXP )
#define IFEXIT( key ) \
{  int _k = (key); \
   if ( _k == KeyMode || _k == KeyClear ) return MODE_KEYMODE; \
}

int OneLineNumberEntry();

extern BOOL TwelveHour;

extern const char *monthName[12];

void I2CmemoryWRITE ( unsigned int address, unsigned char data );

/*// timezone information
typedef struct
{
    const char*               region;
    unsigned char       dstStart;    // when to jump forward
    unsigned char       dstEnd;      // when to fall back
    unsigned char       hour;        // hour of the day to change (BCD)
    char                amount;      // amount to change
} TimeZone;
*/

//extern const TimeZone TimeZones[7];

extern int dstRegion;

#define DIM(_x) (sizeof(_x)/sizeof((_x)[0]))


extern double Longitude;
extern double Latitude;

unsigned char I2CmemoryREAD ( unsigned int address );

void DelayMs ( int num );

extern int OperatorsXY[];
extern int OperatorsYZ[];
extern int OperatorsZT[];


#define OperatorXY OperatorsXY[0]
#define OperatorYZ OperatorsYZ[0]
#define OperatorZT OperatorsZT[0]

int opPrec ( int op );

extern int ValueEntered;

extern char DisplayYreg[];
extern int ComplexIncluded;
extern int ExponentIncluded;
extern int DecimalIncluded;
extern int CalcDisplayBase;

extern int WatchMode;


int ReturnNumber ( int key );

#define WATCH_MODE_TIME         0
#define WATCH_MODE_CALC         1
#define WATCH_MODE_APPS         2
#define WATCH_MODE_TIME_MENU    3
#define WATCH_MODE_CALC_MENU    4

#define PAREN_LEVELS 4

extern double Sreg[];
extern double iSreg[];

int GetNumBCD();

extern unsigned int MemPointer;

extern BOOL ProgPlay;
extern BOOL ProgRec;
extern BOOL squash;

extern int CurrentMenu;
extern BOOL NextMode;

extern char displayBuffer[];

int doCalendar();

extern int displayFormat;

#define FORMAT_BASE2 2
#define FORMAT_DATE 1

int seconds ( rtccTime *t );

void setBase ( int base );
extern unsigned long rand32();
extern int displayMode;
extern int displayAccuracy;
extern int displayEngN;

char *printNumber( int *number, int max );

#endif
