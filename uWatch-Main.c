//********************************************************
// uWatch
// Version 1.3
// Last Update: 12th Junes 08
// Written for the Microchip C30 Compiler
// Copyright(c) 2008 David L. Jones
// http://www.calcwatch.com
// EMAIL: david@alternatezone.com
// Target Device: PIC24FJ64GA004 (44pin)
//********************************************************

/*********************************************************
This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************************************/

#include <p24Fxxxx.h>

//define the config fuse bits
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_OFF & COE_OFF & FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS1);
_CONFIG2(IESO_OFF & FCKSM_CSECME & OSCIOFNC_ON & IOL1WAY_ON & I2C1SEL_PRI & POSCMOD_NONE);

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <rtcc.h>
#include <timer.h>
#include <uart.h>
#include <pwrmgnt.h>
#include <ports.h>
#include <string.h>

#define RevString   "Rev 1.3.3"

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

#define KeyDelay        150     //250ms key delay
#define MaxLCDdigits    16

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
#define EnableSleepTimer()      IEC0bits.T1IE = 1       //disable Timer1 interrupt

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

//#define MaxRPNmenuItems 4       //this must match the numbr of lines in the menu below

//global variables for both RPN and Algebraic calculator

#define OPRplus         1       //arbitrary value definitions for algebraic operators
#define OPRminus        2
#define OPRmult         3
#define OPRdiv          4
#define OPRpar1         5       //parentheses
#define OPRpar2         6
#define OPRpar3         7
#define OPRpar4         8
#define OPRpar5         9
#define OPRpar6         10
#define OPRnPr          11
#define OPRnCr          12
#define OPRparra        13
#define OPRxpwry        14
#define OPRrtop         15
#define OPRptor         16

#define PI              3.14159265358979
#define RAD             (180.0/PI)

#define DIM(_x) (sizeof(_x)/sizeof((_x)[0]))

int  CurrentMenu;               // The number of the currently active menu line. 0 to MaxRPNmenuItems
char ExponentIncluded;          //FLAG, TRUE if Exponent has already been entered
char DecimalIncluded;           //FLAG, TRUE if decimal point has already been entered
char MinusIncluded;             //FLAG, TRUE if minus sign already included
char MinusIncludedInExponent;   //FLAG, TRUE if minus sign already included in exponent
char InverseKey;                //FLAG, TRUE if the inverse key has been pressed
char HYPkey;                    //FLAG, TRUE if the HYP key has been pressed
char DegreesMode;               //FLAG, TRUE if degres mode is on, otherwise radians
char DisplayXreg[MaxLCDdigits+1];   //holds the value currently being entered by the user, or the Xreg
char DisplayYreg[MaxLCDdigits+1];   //holds the value currently in Yreg
char ValueEntered;              //FLAG, TRUE if value has been entered by using the ENTER key
//char MenuMode;                  //FLAG, TRUE is the menu if switched on.
int  EnableXregOverwrite;       //FLAG, TRUE if the Xreg will be automatically overwritten on first key press (the ENTER key enables this for example)
double Xreg, Yreg, Zreg, Treg;  //the working registers (Treg not used for Algebraic)
double Yreg1, Zreg1;        //extra working registers for algebraic parentheses
double Yreg2, Zreg2;        //extra working registers for algebraic parentheses
double Yreg3, Zreg3;        //extra working registers for algebraic parentheses
double Yreg4, Zreg4;        //extra working registers for algebraic parentheses
double Yreg5, Zreg5;        //extra working registers for algebraic parentheses
double Yreg6, Zreg6;        //extra working registers for algebraic parentheses

int OperatorXY,  OperatorYZ;        //algebraic operators
int OperatorXY1, OperatorYZ1;
int OperatorXY2, OperatorYZ2;
int OperatorXY3, OperatorYZ3;
int OperatorXY4, OperatorYZ4;
int OperatorXY5, OperatorYZ5;
int OperatorXY6, OperatorYZ6;

char WatchMode;             //0=time mode, 1=calc mode, 2=setup mode

double Sreg[10];                //the storage registers. Contents retained when calc mode is exited.

#define CALC_OP_NULL            0
#define CALC_OP_RECIPROCAL      1
#define CALC_OP_SQUARE          2
#define CALC_OP_SQRT            3
#define CALC_OP_LN              4
#define CALC_OP_EXP             5
#define CALC_OP_NPOW            6
#define CALC_OP_NROOT           7
#define CALC_OP_LN10            8
#define CALC_OP_10X             9
#define CALC_OP_SIN             10
#define CALC_OP_COS             11
#define CALC_OP_TAN             12
#define CALC_OP_ASIN            13
#define CALC_OP_ACOS            14
#define CALC_OP_ATAN            15
#define CALC_OP_MODEDEG         16
#define CALC_OP_MODERAD         17
#define CALC_OP_PI              18
#define CALC_OP_HMS             19
#define CALC_OP_R2P             20
#define CALC_OP_FACTORIAL       21
#define CALC_OP_DMY             22
#define CALC_OP_HOURS           23
#define CALC_OP_P2R             24
#define CALC_OP_SUNSET          25
#define CALC_OP_DAYS            26
#define CALC_OP_RECORD          27
#define CALC_OP_PLAY            28
#define CALC_OP_CONV            29
#define CALC_OP_PARALLEL        30


typedef struct
{
    char*       lines[4];
    unsigned char ops[10];
} CalcMenuInfo;


CalcMenuInfo MainMenus[] = 
{

    { // menu 0
        {" 1/x  x^2   Sqrt ",   
         " Inv  y^x   Ln   ", 
         " 1/x  x^2   Sqrt ", 
         " Inv  y^1/x e^x  "
        },

        { CALC_OP_RECIPROCAL, 
          CALC_OP_SQUARE,
          CALC_OP_SQRT,
          CALC_OP_NPOW,
          CALC_OP_LN,
          CALC_OP_RECIPROCAL, 
          CALC_OP_SQUARE,
          CALC_OP_SQRT,
          CALC_OP_NROOT,
          CALC_OP_EXP 
        }
    },

    { // menu 1
        {" Sin  Cos  Tan  ",   
         " Inv  Pi   Deg  ", 
         " aSin aCos aTan ", 
         " Inv       Rad   "
        },

        { CALC_OP_SIN,
          CALC_OP_COS,
          CALC_OP_TAN,
          CALC_OP_PI,
          CALC_OP_MODEDEG,
          CALC_OP_ASIN,
          CALC_OP_ACOS,
          CALC_OP_ATAN,
          CALC_OP_NULL, // empty slot
          CALC_OP_MODERAD
        }
    },

    { // menu 2
        {" HMS       R>P  ",   
         " Inv  x!   ->D  ", 
         " ->H       P>R  ", 
         " Inv  Sun  DMY  "
        },

        { CALC_OP_HMS,
          CALC_OP_NULL, // spare!
          CALC_OP_R2P,
          CALC_OP_FACTORIAL,
          CALC_OP_DAYS,
          CALC_OP_HOURS,
          CALC_OP_NULL, // inverse spare!
          CALC_OP_P2R,
          CALC_OP_SUNSET,
          CALC_OP_DMY,
        }
    },

    { // menu 3
        {" Rec  Play Conv ",   
         " 2nd  //        ", 
         "                ", 
         " 2nd            "
        },

        { CALC_OP_RECORD,
          CALC_OP_PLAY,
          CALC_OP_CONV,
          CALC_OP_PARALLEL,
          CALC_OP_NULL,
          CALC_OP_NULL,
          CALC_OP_NULL,
          CALC_OP_NULL,
          CALC_OP_NULL,
          CALC_OP_NULL,
        }
    },
};

char LCDline1[MaxLCDdigits+1];  //holds data for the first LCD display line
char LCDline2[MaxLCDdigits+1];  //holds data for the second LCD display line
char LCDhistory1[MaxLCDdigits+1];   //holds a copy of the LCD data for when the LCD is turned off
char LCDhistory2[MaxLCDdigits+1];   //so the program can restore the LCD to were it was when powered back up

BOOL NextMode;          //TRUE if MODE button switches to next mode. FALSE returns to Time/Date mode.
BOOL TwelveHour;        //TRUE if 12 hour mode
BOOL RPNmode;           //TRUE if RPN mode, otherwise Algebraic

//programming mode variables
BOOL ProgPlay;          //TRUE if keystroke playback mode is on
BOOL ProgRec;           //TRUE if keystroke recording mode is on.
unsigned int MemPointer;    //current EEPROM memory location

rtccTime Time;                  //typedef that contains the time
rtccDate Date;                  //typedef that contains the date
unsigned char MoonPhase;
BOOL DST; // are we in daylight saving time?
char dstRegion = 0; // index into TimeZone table

long MJD; // modified julian date (days since 200)

// updated when day changes
unsigned int Year;
unsigned int Month;
unsigned int Day;
unsigned char DayOfWeek;
double Longitude;
double Latitude;

typedef enum
{
    lastSun = 1,
    firstSun = 2,
    secondSun = 3
} DstWhen;

#define MAKE_TZ(_mon, _when)  ((_mon) << 4) | (_when)

/* timezone information */
typedef struct
{ 
    char*               region;
    unsigned char       dstStart;    // when to jump forward
    unsigned char       dstEnd;      // when to fall back
    unsigned char       hour;        // hour of the day to change (BCD)
    char                amount;      // amount to change
} TimeZone;

/* NOTE: amount < 0 means that the DST period is OUTSIDE the
 * start - end range
 *
 * example:
    { "West Europe", MAKE_TZ(3,lastSun), MAKE_TZ(10,lastSun), 0x01, 1 },
    
 * means:
 * 
 * start month = 3 = March
 * DST starts on last sunday
 *
 * end month = 10 = October
 * fall back last sunday
 *
 * trigger at hour = 0x01 = 1:00am (for both cases) NOTE the HEX!
 * amount to shift = 1 hour
 * 
 * southern hemisphere will have negative shifts.
 */
static TimeZone TimeZones[] =
{
    { "NONE", 0, 0, 0, 0 },
    { "West Europe", MAKE_TZ(3,lastSun), MAKE_TZ(10,lastSun), 0x01, 1 },
    { "Cent Europe", MAKE_TZ(3,lastSun), MAKE_TZ(10,lastSun), 0x02, 1 },
    { "East Europe", MAKE_TZ(3,lastSun), MAKE_TZ(10,lastSun), 0x00, 1 },
    { "Australia", MAKE_TZ(3,firstSun), MAKE_TZ(10,firstSun), 0x02, -1 },
    { "USA", MAKE_TZ(3,secondSun), MAKE_TZ(11,firstSun), 0x02, 1 },
    { "Canada", MAKE_TZ(3,secondSun), MAKE_TZ(11,firstSun), 0x02, 1 },
};

//***********************************
//Delay for num milliseconds
//optimised for 250KHz clock
//timing value is only roughly accurate due to overhead
void DelayMs(int num)
{
    int c,d;
    for(d=1;d<15;d++)
        for(c=0;c<num;c++);
}


//************************************
// set the clock rate to 1MHz 
// Used for UART comms
void Clock1MHz(void)
{
    _RCDIV0=1;
    _RCDIV1=1;
    _RCDIV2=0;
}

//************************************
// set the clock rate to 4MHz 
void Clock4MHz(void)
{
    _RCDIV0=1;
    _RCDIV1=0;
    _RCDIV2=0;
}

//************************************
// set the clock rate to 250KHz. 
// High enough speed to give instant calc response,
// but low enough to keep power consumption low
void Clock250KHz(void)
{
    _RCDIV0=1;
    _RCDIV1=0;
    _RCDIV2=1;
}

//************************************
// set the clock rate to 125KHz 
void Clock125KHz(void)
{
    _RCDIV0=0;
    _RCDIV1=1;
    _RCDIV2=1;
}

//************************************
// set the clock rate to 32KHz 
void Clock32KHz(void)
{
    _RCDIV0=1;
    _RCDIV1=1;
    _RCDIV2=1;
}

//************************************
// resets the sleep timer
#define ResetSleepTimer()   TMR1=0

//***********************************
// Insert the LCD code here
#include "uWatch-LCD.c"
//***********************************

//***********************************
// Display the data string passed
// on top line of the LCD display
void UpdateLCDline1(const char* s)
{
    int c;
    strcpy(LCDhistory1,s);      //make a backup copy of the display
    lcd_goto(0);
    lcd_puts(s);
    for (c=strlen(s);c<MaxLCDdigits;c++)
        lcd_write(' ');     //blank the rest of the line
}

//***********************************
// Display the data string passed
// on bottom line of the LCD display
void UpdateLCDline2(const char* s)
{
    int c;
    strcpy(LCDhistory2,s);      //make a backup copy of the display
    lcd_goto(40);               //first charcter on line #2
    lcd_puts(s);
    for (c=strlen(s);c<MaxLCDdigits;c++)
        lcd_write(' ');     //blank the rest of the line
}

void ClearAllRegs(void)
{
    Xreg=0;             
    Yreg=0;
    Zreg=0;
    Treg=0;
    Yreg1=0; Zreg1=0;
    Yreg2=0; Zreg2=0;
    Yreg3=0; Zreg3=0;
    Yreg4=0; Zreg4=0;
    Yreg5=0; Zreg5=0;
    Yreg6=0; Zreg6=0;

    OperatorXY=0;  OperatorYZ=0;
    OperatorXY1=0; OperatorYZ1=0;
    OperatorXY2=0; OperatorYZ2=0;                   
    OperatorXY3=0; OperatorYZ3=0;
    OperatorXY4=0; OperatorYZ4=0;
    OperatorXY5=0; OperatorYZ5=0;
    OperatorXY6=0; OperatorYZ6=0;
}


//***********************************
void i2c_high_sda(void)
{
    _TRISB9=1;      // bring SDA to high impedance input
}

//***********************************
void i2c_low_sda(void)
{
    _TRISB9=0;      // set SDA to a low output
}

//***********************************
void i2c_high_scl(void)
{
    SCL1;
}

//***********************************
void i2c_low_scl(void)
{
    SCL0;
}

//***********************************
unsigned char i2c_in_byte(void)
{
    unsigned char i_byte, n;
    i2c_high_sda();
    for (n=0; n<8; n++)
    {
        i2c_high_scl();

        if (SDA)
        {
            i_byte = (i_byte << 1) | 0x01; // msbit first
        }
        else
        {
            i_byte = i_byte << 1;
        }
        i2c_low_scl();
    }
    return(i_byte);
}

//***********************************
void i2c_out_byte(unsigned char o_byte)
{
    unsigned char n;
    for(n=0; n<8; n++)
    {
        if(o_byte&0x80)
        {
            i2c_high_sda();
        }
        else
        {
            i2c_low_sda();
        }
        i2c_high_scl();
        i2c_low_scl();
        o_byte = o_byte << 1;
    }
    i2c_high_sda();
}

//***********************************
void i2c_nack(void)
{
    i2c_high_sda(); // data at one
    i2c_high_scl(); // clock pulse
    i2c_low_scl();
}

//***********************************
void i2c_ack(void)
{
    i2c_low_sda();  // bring data low and clock
    i2c_high_scl();
    i2c_low_scl();
    i2c_high_sda();
}

//***********************************
void i2c_start(void)
{
    i2c_low_scl();
    i2c_high_sda();
    i2c_high_scl(); // bring SDA low while SCL is high
    i2c_low_sda();
    i2c_low_scl();
}

//***********************************
void i2c_stop(void)
{
    i2c_low_scl();
    i2c_low_sda();
    i2c_high_scl();
    i2c_high_sda();  // bring SDA high while SCL is high
    // idle is SDA high and SCL high
}


//***********************************
//Writes a data byte to the 24LC512 memory chip on the I2C bus
void I2CmemoryWRITE(unsigned int address, unsigned char data)
{
    DisableSleepTimer();
   
    i2c_start();
    i2c_out_byte(0xa0);
    i2c_nack();
    i2c_out_byte((address >> 8) & 0xff);    // high byte of memory address
    i2c_nack();
    i2c_out_byte(address & 0xff);   // low byte of mem address
    i2c_nack();
    i2c_out_byte(data);         // and finally the data
    i2c_nack();
    i2c_stop();
    DelayMs(15);
    
    EnableSleepTimer();
    return;
}

//***********************************
//Reads a data byte from the 24LC512 memory chip on the I2C bus
unsigned char I2CmemoryREAD(unsigned int address)
{
    unsigned char ch;
    DisableSleepTimer();

    i2c_start();
    i2c_out_byte(0xa0);
    i2c_nack();
    i2c_out_byte((address >> 8) & 0xff);
    i2c_nack();
    i2c_out_byte(address & 0xff);
    i2c_nack();
    i2c_start();                // no intermediate stop
    i2c_out_byte(0xa1); // read operation
    i2c_nack();
    ch=i2c_in_byte();
    i2c_stop();
   
    EnableSleepTimer();
    return(ch);
}

//***********************************
// checks for and returns a keystroke from the EEPROM memory
// only if we are in keystroke mode, otherwise returns 0
// Function assumes that MemPointer has been set correctly
unsigned int KeystrokeReplay(void)
{
    unsigned int key;

    //keystroke reply not enabled, so exit without any key press
    if (!ProgPlay) return(0);

    key=I2CmemoryREAD(MemPointer);      //read the keystroke from EEPROM
    if (key==0)                         //check for end of keystroke replay
    {
        ProgPlay=FALSE;                 //disable keystroke replay
        return(0);
    }
    MemPointer++;                       //increment the EEPROM memory pointer
    return(key);                        //return the keystroke from memory
}

//***********************************
// records the keystroke into EEPROM memory
// only if we are in keystroke mode, othrwise does nothing
// Function assumes that MemPointer has been set correctly
void KeystrokeRecord(unsigned char key)
{
    if (ProgPlay==TRUE) return;     //can't record while in play mode

    if (key==KeyMode)                   //check for end of record mode
    {   
        I2CmemoryWRITE(MemPointer, 0);  //store a zero to end programming mode
        ProgRec=FALSE;                  //switch off keystroke record mode  
        UpdateLCDline1("Program Stored.");
        DelayMs(2000);
        return; 
    }

    I2CmemoryWRITE(MemPointer, key);    //store the keystroke into EEPROM
    MemPointer++;                       //increment the EEPROM memory pointer
    return;                     
}

//***********************************
// scans the keypad and returns the key code pressed.
// key codes are from 1 to 25 and start from the upper left
// then proceed across in each row.
// only one pass of the keys is made, looping must be done by the calling function
//   this allows the calling function to do stuff while scaning keys
// returns 0 if no key pressed
unsigned int KeyScan(void)
{
    unsigned int k;

    k=KeystrokeReplay();        //check for keystroke programming
    if (k>0) return(k);         //return the keystroke from memory

    ClearRow1;      //set all rows to inputs
    ClearRow2;
    ClearRow3;
    ClearRow4;
    ClearRow5;
    ClearRow6;
    ClearRow7;
    
    k=0;            //return 0 if no key pressed

    // note that two attempts must be made to access the first column due to read-modify-write
    SetRow1;
    if (Col1==0) k=KeyDiv;
    if (Col1==0) k=KeyDiv;  //need to try again for this column     
    if (Col2==0) k=KeyMult;
    if (Col3==0) k=KeyMinus;
    if (Col4==0) k=KeyPlus;
    ClearRow1;
    SetRow2;
    if (Col1==0) k=KeySign;
    if (Col1==0) k=KeySign;
    if (Col2==0) k=KeyPoint;
    if (Col3==0) k=Key0;
    if (Col4==0) k=KeyMode;
    ClearRow2;
    SetRow3;
    if (Col1==0) k=Key7;
    if (Col1==0) k=Key7;
    if (Col2==0) k=Key4;
    if (Col3==0) k=Key1;
    ClearRow3;
    SetRow4;
    if (Col1==0) k=Key8;
    if (Col1==0) k=Key8;
    if (Col2==0) k=Key5;
    if (Col3==0) k=Key2;
    ClearRow4;
    SetRow5;
    if (Col1==0) k=Key9;
    if (Col1==0) k=Key9;        
    if (Col2==0) k=Key6;
    if (Col3==0) k=Key3;
    ClearRow5;
    SetRow6;
    if (Col1==0) k=KeyXY;
    if (Col1==0) k=KeyXY;
    if (Col2==0) k=KeyLP;
    if (Col3==0) k=KeyEXP;
    if (Col4==0) k=KeyRCL;
    ClearRow6;
    SetRow7;
    if (Col1==0) k=KeyClear;
    if (Col1==0) k=KeyClear;
    if (Col2==0) k=KeyRP;
    if (Col3==0) k=KeyMenu;
    if (Col4==0) k=KeyEnter;
    ClearRow7;          

    if (k!=0) //key was pressed so lets debounce it
    {
        ResetSleepTimer();  //reset the sleep timer every time a key is pressed
        DelayMs(KeyDelay);  //debounce delay
        SetRow1;
        SetRow2;
        SetRow3;
        SetRow4;
        SetRow5;
        SetRow6;
        SetRow7;
        do; while((Col1==0)||(Col2==0)||(Col3==0)||(Col4==0));  //wait for the key to be released
        ClearRow1;
        ClearRow2;
        ClearRow3;
        ClearRow4;
        ClearRow5;
        ClearRow6;
        ClearRow7;          
        if (ProgRec) 
        {
            KeystrokeRecord(k);     //record the keystroke
            if (k==KeyMode) return(0); //don't return to main menu on MODE key
        }
    }
    return(k);      
}

// decls
int moonPhase(int year, int month, int day);
BOOL inDST(int*);


//***********************************
// converts 2 digit BCD to integer
unsigned int BCDtoDEC(unsigned int num)
{
    return (num>>4)*10 + (num&0xf);
}

// convert int to 2 digit BCD
unsigned int DECtoBCD(unsigned int num)
{
    unsigned int a = num/10;
    unsigned int b = num - a * 10;
    return (a<<4)|b;
}

//converts a single digit integer into a char
unsigned char itochar(int i)
{
    if (i >= 0 && i <= 9) return '0' + i;
    return '*';
}

long mjd(int y, int m, int d)
{
    // here is the new Hodges' version 2008
    // rebased to 2000/1/1 (add back 51544 for old version)

    int a = (14-m)/12;
    m += 12*a+1;
    y += 4800-a;
    return d+(153*m)/5+365L*y+y/4-y/100+y/400-2483712L;
}

// 0=sunday, 1=monday etc.
#define DAYOFWEEK(_x)    (((_x)+6)%7)

/* called when we read a date and the day has changed from last read
 * perform calculations based on day here.
 */
static void dayHasChanged()
{
    Year = BCDtoDEC(Date.f.year) + 2000;
    Month = BCDtoDEC(Date.f.mon);
    Day = BCDtoDEC(Date.f.mday);

    // failsafe checks
    if (Month > 12) Month = 0;

    MoonPhase = moonPhase(Year, Month, Day);
    MJD = mjd(Year, Month, Day);

    // 1 = sun, 2 = mon etc.
    DayOfWeek = DAYOFWEEK(MJD) + 1;
}

/// generic menu driver for one line +/- selector
int DriveMenu(const char* title, const char** Menu, int n)
{
    /* display `title' in line 2 and allow `n' items on `Menu'to be cycled
     * with +/- until enter is pressed or another key to exit
     * 
     * return selection index or -1 on escape/cancel.
     */
    int mi = 0;
    int key;

    UpdateLCDline2(title);
    for (;;)
    {
        UpdateLCDline1(Menu[mi]);

        do key=KeyScan(); while(key==0);

        ResetSleepTimer();		
        //start to process the keypress

        // clear key was pressed, exit  mode
        if (key==KeyClear || key == KeyMode) return -1;

        //user pressed some key other than MODE, so ensure that when exit we go back to the time/date display
        NextMode=FALSE; 

        if (key == KeyPlus)
        {
            if (++mi == n) mi = 0;
        }
        else if (key == KeyMinus)
        {
            if (--mi < 0) mi += n;
        }
        else if (key == KeyEnter)
        {
            break;
        }
    }
    return mi;
}

//converts a keypad number key code into a real number 
int ReturnNumber(int key)
{
    if (key==Key1) return(1);
    if (key==Key2) return(2);
    if (key==Key3) return(3);
    if (key==Key4) return(4);
    if (key==Key5) return(5);
    if (key==Key6) return(6);
    if (key==Key7) return(7);
    if (key==Key8) return(8);
    if (key==Key9) return(9);
    if (key==Key0) return(0);
    return -1; // not a number
}

// Two line menu driver with optional inv
// if we escape or change modes, return -keypress
int DriveMenu2(const char* line1, const char* line2,
               const char* inv1, const char* inv2)
{
    int mi;
    int key;
    int inv = 0;
    int invOption = (inv1 != 0); // enabled?

    /* Display `line1' and `line2' consisting of 6 menu items
     * corresponding to the function keys.
     * return the item selected in the pattern, or -1 on cancel/escape.
     *
     *
     *  0  1  2
     *  3  4  5
     *
     * or, when inv is used
     *
     *  0  1  2
     *  X  3  4
     *
     *  5  6  7
     *  X  8  9
     *
     */

    for (;;)
    {
        if (!inv)
        {
            // display normal choices
            UpdateLCDline1(line1);
            UpdateLCDline2(line2);
        }
        else
        {
            // display inv choices
            UpdateLCDline1(inv1);
            if (inv2) UpdateLCDline2(inv2);
        }

        // get a key
        while (!(key = KeyScan())) ;

        // reset timer
        ResetSleepTimer();
        
        // escape?
        if (key==KeyClear || key == KeyMode || key == KeyMenu)
        {
            // < 0 => escape. also return keypress incase useful.
            mi = -key;
            break;
        }

        // look for a function key
        key = ReturnNumber(key);
        if (key >= 4 && key <= 9)
        {
            // is a function key
            if (inv)
            {
                // in "inv" mode, return alternate choice numbers
                if (key == 4)
                {
                    // inv => not inv
                    inv = 0;
                }
                else
                {
                    mi = key - 2; // 789 -> 567
                    if (key < 7)
                    {
                        mi = key + 3; // 456 -> X89
                        if (inv2 == 0)
                            mi = key - 2; // 456 -> X34 
                    }
                    break;
                }
            }
            else
            {
                if (key == 4 && invOption)
                {
                    // go to inv choices
                    inv = 1;
                }
                else
                {
                    mi = key - 7; // 789 -> 012
                    if (key < 7)
                        mi = key - 1 - invOption; // 456 -> 345 (or X34)
                    break;
                }
            }
        }
    }
    return mi;
}


// get a 2 digit BCD, < 0 if fail
int GetNumBCD()
{
    int key, h;

    while (!(key = KeyScan())) ;
    h=ReturnNumber(key);
    if (h >= 0)
    {
        while (!(key = KeyScan())) ;
        return (h<<4)|ReturnNumber(key); // NB: neg if escape.
    }
    return h;
}

//***********************************
// Insert all of the Calculator common code here
#include "uWatch-CalcCommon.c"
//***********************************

//***********************************
// Insert all of the RPN Calculator code here
#include "uWatch-RPN.c"
//***********************************

//***********************************
// Insert all of the ALG Calculator code here
#include "uWatch-ALG.c"
//***********************************

//***********************************
// Insert all of the Setup code here
#include "uWatch-SetupMode.c"
//***********************************


//***********************************
// sets the RTCWREN bit to enable writing to the RTCC registers
void RTCCunlock(void)
{
    asm volatile("disi #5");
    asm volatile("mov #0x55, w7");
    asm volatile("mov w7, _NVMKEY");
    asm volatile("mov #0xAA, w8");
    asm volatile("mov w8, _NVMKEY");
    asm volatile("bset _RCFGCAL, #13");
    asm volatile("nop");
    asm volatile("nop");
}

//***********************************
// switches the LCD power ON
void LCDon(void)
{
    T2CONbits.TON=1;    //turn on timer 2 for PWM
    SetLCD_POWER;
    DelayMs(100);
}

//***********************************
// switches the LCD power OFF
void LCDoff(void)
{
    PORTC=PORTC&0xFF00;         //switch all 8 LCD data pins low.
    ClearLCD_RS;                //switch control lines low
    ClearLCD_EN;
    T2CONbits.TON=0;            //turn off timer 2 for PWM
    ClearLCD_POWER;
}

//***********************************
void IdleI2C1(void)
{
    /* Wait until I2C Bus is Inactive */
    while(I2C1CONbits.SEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT);  
}


// put these in the code segment
static const char months[13][4]={"   ","Jan","Feb","Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const char days[8][4]={"   ","Sun","Mon","Tue", "Wed", "Thu", "Fri", "Sat"};

// N(ew),C(rescent),Q(uarter),W(axing),F(ull),w(aning),q(uarter),c(rescent)
static const char mphase[8] = "NCQWFwqc";


int moonPhase(int year, int month, int day)
{
    /*
      Calculates the moon phase (0-7), accurate to 1 segment.
      0 = > new moon.
      4 => Full moon.

      NOTE: integer math.
    */
    
    int d = 0,g,e;
    if (month == 2) d = 31;
    else if (month > 2)
    {
        d = 59+((month-3)*306 + 5)/10;
        if (year%400 && !year%4) d++;
    }
    d--;
    g = (year-1900)%19 + 1;
    e = (11*g + 18) % 30;
    if ((e == 25 && g > 11) || e == 24) e++;
    return ((((d+ e + day)*6+11)%177)/22 & 7);
}

static BOOL beforeWhenTest(DstWhen w, TimeZone* tz, int* gap)
{
    unsigned int t = 0;
    unsigned int d;

    // if we are close to the limit, set gap to be the number of hours
    *gap = 0;

    switch (w) 
    {
    case lastSun:
        {
            // last day of this month
            t = mjd(Year, Month+1, 1) - 1; // ASSUME month != 12
            d = DAYOFWEEK(t); // day of week, 0=Sun,1=Mon etc.
            t -= d; // t is last sunday in this month
        }
        break;
    case firstSun:
        {
            t = mjd(Year, Month, 1);
            d = DAYOFWEEK(t); // day of week, 0=Sun,1=Mon etc.
            if (d)
                t += 7 - d;
        }
        break;
    case secondSun:
        {
            t = mjd(Year,Month,8);
            d = DAYOFWEEK(t); // day of week, 0=Sun,1=Mon etc.
            if (d)
                t += 7 - d;
        }
        break;
    }

    // NB: hour in BCD
    if (MJD == t - 1)  // day before?
    {
        // near midnight rollover?
        if (!tz->hour)
        {
            if(BCDtoDEC(Time.f.hour) >= 23)
            {
                *gap = 1;
            }
        }
    }

    if (MJD < t)
        return TRUE;
    
    if (MJD == t)
    {
        // we're on the same day as the limit, figure out how many
        // hours before. if so, set gap
        int dt = tz->hour - BCDtoDEC(Time.f.hour);
        if (dt > 0)
        {
            *gap = dt;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL inDST(int* gap)
{
    // ASSUME: MJD, Year, Month, Day. DayOfWeek are updated.
    // ASSUME: `Time' contains current time

    BOOL res = FALSE;

    if (dstRegion > 0)
    {
        TimeZone* tz = TimeZones + dstRegion;    

        unsigned int mStart = tz->dstStart >> 4;
        unsigned int mEnd = tz->dstEnd >> 4;
    
        if (Month >= mStart && Month <= mEnd)
        {
            if (Month > mStart && Month < mEnd)
            {
                res = TRUE;
            }
            else
            {
                // otherwise we are on the start month or the end month
                if (Month == mStart)
                {
                    // in DST if not before start point
                    res = !beforeWhenTest(tz->dstStart & 0xf, tz, gap);
                }
                else // mEnd
                {
                    // in DST if before end point
                    res = beforeWhenTest(tz->dstEnd & 0xf, tz, gap);
                }
            }
        }

        if (tz->amount < 0)
        {
            // when amount is negative, this indicates the range is
            // oppsite, so we invert the result
            res = !res;
        }
    }
    return res;
}

void caldati(long mjd,
             unsigned int* y, unsigned int* m, unsigned int* d)
{
    // here is the new Hodges' version 2008
    // this is the inverse of mjd

    long td = mjd + 2520114L;
    unsigned int ty;
    unsigned int c, a;
    unsigned int tm;

    ty= (td<<2)/146097L;
    td -= (146097L*ty+3)>>2;
    c = (99*(td+1))/36160L;
    td -= ((1461L*c)>>2)-31;
    tm = (5*td)/153;
    a = tm/11;
    *d= td-(367L*tm)/12;
    *m= tm-12*a+2;
    *y= 100*(ty-49)+c+a;
}

void AdjustDateTime(int dh)
{
    // ASSUME: MJD is correct

    BOOL d = FALSE;
    int h = BCDtoDEC(Time.f.hour) + dh;
    if (h >= 24)
    {
        // next day
        h -= 24;

        ++MJD;
        d = TRUE;
    }
    else if (h < 0) 
    {
        // prev day
        h += 24;

        --MJD;
        d = TRUE;
    }

    if (d)
    {
        // day has changed!
        caldati(MJD, &Year, &Month, &Day);
        Date.f.year = DECtoBCD(Year - 2000);
        Date.f.mon = DECtoBCD(Month);
        Date.f.mday = DECtoBCD(Day);
    }

    Time.f.hour = DECtoBCD(h);
}

static BOOL checkDST()
{
    BOOL res = FALSE;
    int gap;
    BOOL dst = inDST(&gap);
    if (DST != dst)
    {
        TimeZone* tz = TimeZones + dstRegion;    
        int a = tz->amount;
        if (a < 0) a = -a;

        if (dst)
        {                
            // time to jump forward!
            if (!gap) 
            {
                // only jump if we're not in proximity to the fall.
                AdjustDateTime(a);
                res = TRUE;
            }
        }
        else
        {
            // fall back!
            AdjustDateTime(-a);
            res = TRUE;
        }

        if (res)
        {
            SetTimeBCD(Time.f.hour, Time.f.min, Time.f.sec);
            SetDateBCD(Date.f.year, Date.f.mon, Date.f.mday);
            DST = dst;
        };
    }
    return res;
}

// display time offset
#define TOFF 2

//**********************************
// displays the time and date on the LCD
void TimeDateDisplay(void)
{
    char s[MaxLCDdigits+1];
    BOOL pm;
    unsigned int temp;
    rtccDate td;
    rtccTime tt;
    int point;

    // copy old time
    tt = Time;

    // read current time
    RtccReadTime(&Time);            //read the RTCC registers

    // if time not changed, we're done
    if (tt.l == Time.l)
    {
        // might reduce LCD flicker.
        return;
    }

    // copy old date
    td = Date;

    // read current date
    RtccReadDate(&Date);

    // has the date changed?
    if (td.l != Date.l)
    {
        // update y,m,d moon phase and day of week
        dayHasChanged();
        td = Date;
    }

    // perform DST check
    if (checkDST())
    {
        // re-read time if changed
        RtccReadTime(&Time);            //read the RTCC registers
        RtccReadDate(&Date);

        if (td.l != Date.l)
            dayHasChanged(); // possible that the day changes
    }

    pm = Time.f.hour>0x12;

    temp=BCDtoDEC(Time.f.hour);

    if (TwelveHour)
        if (temp > 12) temp -= 12;

    // blank string then overwrite
    memset(s, ' ', MaxLCDdigits);
    s[MaxLCDdigits] = 0;

    if (temp >= 10)
        s[TOFF] = itochar(temp/10);
    
    s[TOFF+1]=itochar(temp % 10);
    s[TOFF+2]=':';

    temp=Time.f.min;
    s[TOFF+3]=itochar(temp>>4);
    s[TOFF+4]=itochar(Time.f.min&0x0F);
    s[TOFF+5]=':';

    temp=Time.f.sec;
    s[TOFF+6]=itochar(temp>>4);
    s[TOFF+7]=itochar(Time.f.sec&0x0F);
    point = TOFF+8;

    if (TwelveHour)
    {
        s[TOFF+8]='a';
        if (pm) s[TOFF+8]='p';
        s[TOFF+9]='m';
        point += 2;
    }

    if (DST) 
        s[point] = '.';  // put a little dot after the time if DST

    s[15] = mphase[MoonPhase];
    UpdateLCDline1(s);                  //display the time

    /* process second line */

    // blank line again
    memset(s, ' ', MaxLCDdigits);

    temp=BCDtoDEC(DayOfWeek);
    s[0]=days[temp][0];
    s[1]=days[temp][1];
    s[2]=days[temp][2];

    s[3]=',';

    temp=Date.f.mday;
    if (temp >> 4)
        s[5]=itochar(temp>>4);

    temp &= 0xf;
    s[6]=itochar(temp);
    s[7]='t';
    s[8]='h';
    if (temp == 1) // 1, 21, 31
    {
        s[7]='s';
        s[8]='t';
    }
    else if (temp == 2)
    {
        s[7]='n';
        s[8]='d';
    }
    else if (temp == 3)
    {
        s[7]='r';
        s[8]='d';
    }

    s[10]=months[Month][0];
    s[11]=months[Month][1];
    s[12]=months[Month][2];

    s[14]=itochar(Date.f.year>>4);
    s[15]=itochar(Date.f.year&0x0F);
    UpdateLCDline2(s);                  //display the date
}

//***********************************
// Turns the backlight ON
void BacklightON(void)
{
    _RP3R=18;               //map RP3 pin 24 to OC1 PWM output using Peripheral Pin Select (for backlight power)
    SetLED_POWER;
}

//***********************************
// Turns the backlight OFF
void BacklightOFF(void)
{
    _RP3R=0;                //map RP3 pin 24 to be an I/O pin
    ClearLED_POWER;
}

//***********************************
// Interrupt service routine for Timer1
// This is called by a timeout because the user as not pressed a key in a few minutes
void __attribute__((__interrupt__)) _T1Interrupt( void )
{
    char s[17];
    T1CONbits.TON=0;            //switch the SLEEP timer OFF
    LCDoff();                   //switch OFF LCD power
    BacklightOFF();             //switch off backlight
    _TRISB8=1;          //SCL
    _TRISB9=1;          //SDA
    IFS0bits.T1IF = 0;          //clear Timer1 interrupt
    SetRow2;            //switch on the keypad row so the wake-up key can be activated.
    SetRow6;            //switch on the keypad row so the wake-up key can be activated.
    CNEN1bits.CN3IE=1;  //enable CN3 pin for interrupt in SLEEP mode
    IFS1bits.CNIF=0;    //clear input change interrupt
    IPC4bits.CNIP0=1;   //set CN pin change interrupt priority level
    IPC4bits.CNIP1=0;   //priority is lower than Timer1, so CN vector is not called
    IPC4bits.CNIP2=0;   //operation simply continues from after the sleep instruction
    IEC1bits.CNIE=1;    //enable the pin change interrupt, ready to wake up

    Sleep();                            //put the watch to SLEEP.
    //only the RTCC is kept running at this point.
    //when the watch wakes back up, operation will continue from this point.

    IEC1bits.CNIE=0;            //switch off the pin change interrupt
    IFS1bits.CNIF=0;            //clear input change interrupt
    LCDon();                    //switch on LCD power
    ResetSleepTimer();          //reset the timer
    lcd_init();                 //initialise LCD
    lcd_clear();

    //re-enable the I2C bus
    _TRISB8=0;          //SCL
    _TRISB9=0;          //SDA
    i2c_high_scl();     //clear the I2C bus
    i2c_high_sda();

    //restore the previous display contents, except time mode
    if (WatchMode!=0)
    {
        strcpy(s,LCDhistory1);      
        UpdateLCDline1(s);          
        UpdateLCDline1(s);          
        strcpy(s,LCDhistory2);
        UpdateLCDline2(s);      
    }       
    ResetSleepTimer();
    T1CONbits.TON=1;            //switch the SLEEP timr back on
    DelayMs(500);               //add a key delay
}



//***********************************
void ProgramInit(void)
{
    _ADON=0;        //switch OFF ADC
    AD1PCFG=0xFFFF; //set all ADC pins to digital I/O

    //define all the I/O pins

    CNPU1=0;        //disable all pullups
    CNPU2=0;

    _CN11PUE=1;     //enable pull-up's on port pins for keypad columns
    _CN2PUE=1;
    _CN19PUE=1;
    _CN3PUE=1;
    
    Clock250KHz();      //set the clock freuqnecy to 250KHz
    
    TRISA=0xFFFF;       //set all pins as inputs
    TRISB=0xFFFF;       //set all pins as inputs
    TRISC=0xFFFF;       //set all pins as inputs

    _TRISC0=0;          //set LCD pins as outputs
    _TRISC1=0;
    _TRISC2=0;
    _TRISC3=0;
    _TRISC4=0;
    _TRISC5=0;
    _TRISC6=0;
    _TRISC7=0;
    _TRISA2=0;
    _TRISA3=0;

    _TRISB6=0;          //TX
    _TRISB7=0;          //IRLED
    _TRISC8=0;          //Peripheral I/O
    _TRISB8=0;          //SCL
    _TRISB9=0;          //SDA

    _TRISA8=0;          //LCD_POWER
    _TRISA9=0;          //LED_POWER

    PORTA=0;            //set all keypad output lines low
    PORTB=0;            //set all keypad output lines low

    //setup Timer1 used for the watch SLEEP timeout
    T1CONbits.TCKPS0=1;     //clock prescaler = 256
    T1CONbits.TCKPS1=1;     //timer ticks over at 128 times per sec
    T1CONbits.TCS=1;        //exteral clock select (32.768KHz crystal)
    IPC0bits.T1IP = 4;      //set Timer1 interrupt priority to 4 
    PR1 = 7680;             //set the timeout value in the period register
    //7680 = 1 minute
    //128 counts for every second
    TMR1=0;
    T1CONbits.TON=1;        //switch on sleep timer
    IEC0bits.T1IE = 1;      //enable Timer1 interrupt

    //setup Timer2 used for the PWM LCD Display Power & backlight display
    T2CONbits.T32=0;
    T2CONbits.TON=1;
    T2CONbits.TCKPS0=0;     // /DIV prescaler closk for Timer2
    T2CONbits.TCKPS1=0;
    PR2=0x0001;
    TMR2=0x0;
    OC1RS=0x01;

    OC1CONbits.OCM0=0;      //switch PWM mode ON on OC1
    OC1CONbits.OCM1=1;
    OC1CONbits.OCM2=1;
    OC1CONbits.OCTSEL=0;    //output compare registers use Timer2
    
    _RP2R=18;               //map RP2 pin 23 to OC1 PWM output using Peripheral Pin Select (for LCD power)


    //initialise the RTCC
    __builtin_write_OSCCONL(2);     // enable the Secondary Oscillator
    RTCCunlock();           //allow write access to the RTCC
    mRtccOn();              //switch on the RTCC

    //initialise the RTCC with something on first start, just in case it's corrupted
    Date.f.wday=1;
    Date.f.mday=1;
    Date.f.mon=1;
    Date.f.year=1;
    Time.f.hour=1;
    Time.f.min=1;
    Time.f.sec=1;

    //disable voltage regulator in SLEEP mode to save power
    RCONbits.VREGS=0;

    //UART setup
    _RP6R=3;                //map RP6 pin 42 to UART1 TX function
    RPINR18bits.U1RXR=5;    //map RP5 pin 41 to UART1 RX function
    U1MODEbits.UARTEN=1;    //enable UART
    U1STAbits.UTXEN=1;      //enable transmitter
    U1MODEbits.BRGH=0;      //low baud rate select
    U1BRG=12;               //buad rate = 2400 with BRGH=0 and 1MHz clock
    U1MODEbits.LPBACK=0;

    //I2C setup
    //  I2C1BRG=2;              //buad rate reg must be a minimum of 2
    I2C1CONbits.I2CEN=0;    //disable the I2C bus

    // Power Down mode enabled, 11 bit sampling
    TwelveHour=TRUE;        //TRUE if 12 hour mode
    RPNmode=TRUE;               // default to RPN!!
    ProgPlay=FALSE;         //turn off keystroke playback
    ProgRec=FALSE;          //turn off keystroke record
    DegreesMode=FALSE;      //default is radians mode

    // greenwich, UK
    Longitude = 0;
    Latitude = 51.5; 
}


//************************************************
void OpenTimer1(unsigned int config,unsigned int period)
{
    TMR1  = 0;          /* Reset Timer1 to 0x0000 */
    PR1   = period;     /* assigning Period to Timer period register */
    T1CON = config;     /* Configure timer control reg */
}

//***********************************
int main(void)
{
    char Key;
    int c;
    ProgramInit();  //initialse everything

    lcd_init();
    lcd_clear();

    Clock250KHz();
    PeripheralIOOFF();

    i2c_high_scl();     //clear the I2C bus
    i2c_high_sda();

    //retrieve calibration value from last byte
    RCFGCALbits.CAL=I2CmemoryREAD(63535);

    BacklightOFF();

    //clear all the calculator registers
    ClearAllRegs(); 

    while(TRUE)     //The main endless loop
    {
        //the MODE key switches between apps
        NextMode=TRUE;

        do {
            TimeDateDisplay();
            WatchMode=0;
            for(c=0;c<50;c++)
            {
                Key=KeyScan();
                if (Key) break;
            }
            
            if (Key==KeyRCL)
            {
                BacklightON();
                for(c=0;c<3000;c++)
                {
                    Key=KeyScan();

                    //if RCL key is pressed again within a few seconds
                    if (Key==KeyRCL) break;
                }

                //if key was not pressed a second time then switch off backlight
                if (Key!=KeyRCL) BacklightOFF();   
            } //otherwise leave the backlight on so user can use other modes
                
        } while (Key!=KeyMode); //loop until a key has been pressed

        WatchMode=1;    
        if (RPNmode)
        { 
            RPNcalculator();
        }
        else 
        {
            // XX !!
            RPNcalculator();
            // ALGcalculator();
        }

        //user did something in the calc mode, so return to time/date
        if (NextMode==FALSE) continue; 
    
        WatchMode=2;
        SetupMode();
    }
}
