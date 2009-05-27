
//********************************************************
// uWatch
// Setup Mode Functions
// Version 1.6
// Last Update: 16th March 09
// Copyright(c) 2008 David L. Jones
// Written for the Microchip C30 Compiler
// Target Device: PIC24FJ64GA004 (44pin)
// http://www.calcwatch.com
// EMAIL: david@alternatezone.com

// NOTE: This code is designed to be inserted inline into the uWatch-Main.C function
//       Most variables are passed as globals and are defined in uWatch-Main.C
//********************************************************

//global variables for Setup Mode

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

#include "uWatch-SetupMode.h"
#include "uWatch-LCD.h"

#include "def.h"
#include "calculator.h"
#include "characterset.h"
#include "menu.h"




void SetTimeBCD( unsigned int h, unsigned int m, unsigned int s )
{
    //if ( h < 0x24 && m < 0x60 && s < 0x60 ) {
        RCFGCALbits.RTCPTR = 1;     //select correct RTC register
        RTCVAL = h;

        //the result is now a 16bit BCD number
        //m = ( m << 8 ) + s;
        RCFGCALbits.RTCPTR = 0;     //select correct RTC register
        RTCVAL = ( m << 8 ) + s;
    //}
}

int SetDateBCD( unsigned int y, unsigned int m, unsigned int d )
{
    int v = ( m && m < 0x13 && d && d < 0x32 && y < 0x100 );
    if ( v ) {
        //month and day are combined in the one word
        m = ( m << 8 ) + d;

        //the result is now a 16bit BCD number
        RCFGCALbits.RTCPTR = 2;     //select correct RTC register
        RTCVAL = m;

        //the result is now an 8bit BCD number
        RCFGCALbits.RTCPTR = 3;     //select correct RTC register
        RTCVAL = y;

    }
    return v;
}

void SetDateBCDandUpdate( int y, int m, int d )
{
    if ( SetDateBCD( y, m, d ) ) {

        // read accepted date and time back
        RtccReadDate( &Date );
        RtccReadTime( &Time );

        // recalculate DoW and MJD
        dayHasChanged();

        // infer whether we are in DST
        DST = inDST( &y ); // y is dummy
    }
}



// enter a number on line return in Xreg
// return final key pressed
int OneLineNumberEntry()
{
    // Assume Xreg contains old value
    // display old value
    UpdateXregDisplay();
    UpdateLCDline2( DisplayXreg );

    // process input
    ResetFlags();
    EnableXregOverwrite = TRUE;

    int key;
    do {

        key = GetDebouncedKey();

        switch (key ) {
            case KeyMenu:
                return MODE_KEY_NEXT;
            case KeyMode:
                return MODE_KEYMODE;
        }        

        int c = EnterNumber( key );

        if ( c == KeyClear )
            Xreg = 0;
            

    } while ( !ENTER( key ) );

    CompleteXreg();
    UpdateXregDisplay();
    UpdateLCDline2( DisplayXreg );
    return MODE_EXIT;
}







char *printNumber( int *number, int max ) {
    // decimal to string is VERY slow -- hence the turbo
    //Clock4MHz();
    sprintf( out, "%d", *number );
    //Clock250KHz();
    return out;
}

char *printHour( int *hour, int max )
{
    //Clock4MHz();

    // 12AM, 1AM... 12PM.. etc
    // OR 24 hour

    if ( TwelveHour ) {

        int cbase = ( *hour ) > 11 ? 3 : 2;
        int h = ( *hour ) % 12;
        if ( !h ) h = 12;

        sprintf( out, "%d%c\4", h, cbase );

    } else
        sprintf( out, "%02d", ( *hour ) );

    //Clock250KHz();
    return out;
}

char *printMinSec( int *n, int max ) {
    sprintf( out, "%02d", *n );
    return out;
}

int changeTime( int p )
{

    RtccReadTime( &Time );          //read the RTCC registers

    int h = BCDtoDEC( Time.f.hour );
    int m = BCDtoDEC( Time.f.min );
    int s = BCDtoDEC( Time.f.sec );

    custom_character( 2, &( AMPM[ 0 ][0] ) );
    custom_character( 3, &( AMPM[ 1 ][0] ) );
    custom_character( 4, &( AMPM[ 2 ][0] ) );

    if ( genericMenu( "Hour", &printHour, &decrement, &increment, chooseExact, 24, &h ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    if ( genericMenu( "Minute", &printMinSec, &decrement, &increment, chooseExact, 60, &m ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    if ( genericMenu( "Second", &printMinSec, &decrement, &increment, chooseExact, 60, &s ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    SetTimeBCD( DECtoBCD( h ), DECtoBCD( m ), DECtoBCD( s ) );
    return MODE_EXIT;
}


char *printMonth( int *month, int max ) {
    if ( mChar[*month] )
        custom_character(5,mChar[*month]);
    return (char *) monthName[ *month ];
}

const unsigned char *qday[] = {
    character_Sunday,
    character_Monday,
    character_Tuesday,
    character_Wednesday,
    character_Thursday,
    character_Friday,
    character_Saturday,
};


inline int leap( int year ) {
    return ( !( year % 4 ) ) && ( ( year % 100 ) || ( !( year % 400 )));
}

int daysInMonth( int year, int month ) {
    int dom[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; 
    int dim = dom[ month - 1];
    if ( leap(year) && (month == 2))
        dim++;
    return dim;
}

static int gYear, gMonth;
static int dim;
static BOOL gModify;

char *processCalendar( int *pDay, int max )
{

    int dayOfWeek = DAYOFWEEK( mjd( gYear, gMonth, *pDay ) );
    
    char cc = 2;                     // custom char #


    // preload custom chars as we can't do it in 1MHz mode...
    int i;
    for ( i = 0; i < 3; i++ )
        custom_character( i+cc, qday[ ( dayOfWeek + i ) % 7 ] );

    Clock4MHz();
    strcpy(out, "(" );

    int limit = (*pDay) + 2;        // MAX DAYS?
    if ( limit > dim )
        limit = dim;

    char hc = ')';

    int dd;
    for ( dd = *pDay; dd <= limit; dd++ ) {
        sprintf( out + strlen( out ), "%c%-2d%c", cc++, dd, hc );
        hc = ' ';        
    }

    Clock250KHz();

    return out;
}

void fixTitle( int year, int month ) {
    sprintf( out, "%d, %s", year, monthName[ month - 1 ] );
    UpdateLCDline1( out );

    // Note: it is IMPORTANT that the character flip comes AFTER
    // the screen update, even though logically wrong.  The problem is
    // the timing of UpdateLCDline is slow, so we see what WAS on the screen
    // for a fair while before it changes.  If we've already changed the character
    // then we see stuff like "Segtember" when switching August/September.
    // However, the character custom draw is FAST, so we can do this pretty
    // damn quick after the screen draw returns.  So even though it's wrong
    // the eye doesn't notice it.  In short, this code is well thought out... don't change

    if ( mChar[month-1])
        custom_character(5,mChar[month-1]);
}


void decrementDay( int *day, int max )
{
    ( *day)--;
    if (( *day) < 1 ) {

        if ( !gModify ) {
            gMonth--;
            if ( gMonth < 1 ) {
                gYear--;
                gMonth = 12;
            }
            dim = daysInMonth( gYear, gMonth );
        }

        (*day) = dim;

        fixTitle( gYear, gMonth );

    }
}

void incrementDay( int *day, int max )
{
    ( *day)++;

    if (( *day) > dim ) {

        ( *day ) = 1;

        if ( !gModify ) {
            gMonth++;
            if ( gMonth > 12 ) {
                gYear++;
                gMonth = 1;
            }
            dim = daysInMonth( gYear, gMonth );
        }

        fixTitle( gYear, gMonth );
    }
}


char *choosePlusOne( int *sel, int kpv, int max ) {
    if ( kpv < max + 1 )
        *sel = kpv + 1;
    return 0;
}

char *chooseMinusOne( int *sel, int kpv, int max ) {
    if ( kpv > 0 && kpv <= max )
        *sel = kpv - 1;
    return 0;
}




int doCal( BOOL modify ) {

    gModify = modify;

    int year = BCDtoDEC( Date.f.year ) + 2000;
    int month = BCDtoDEC( Date.f.mon ) - 1;
    int day = BCDtoDEC( Date.f.mday );

    int oldYear = year;
    int oldMonth = month;

    if ( genericMenu( "Year", &printNumber, &decrement, &increment, chooseExact, 9999, &year ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    sprintf( out, "%d", year );
    if ( genericMenu( out, &printMonth, &decrement, &increment, chooseMinusOne, 12, &month ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    gYear = year;
    gMonth = month + 1;         // 1-based

    sprintf( out, "%d, %s", year, monthName[ month ] );
    dim = daysInMonth( gYear, gMonth );
    //if ( day > dim )
    //    day = 1;

    if ( !modify && ( oldMonth != month || oldYear != year ))
        day = 1;

    if ( mChar[month])
        custom_character(5,mChar[month]);

    if ( genericMenu( out, processCalendar, decrementDay, incrementDay, chooseExact, dim, &day ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    //TODO: year should be absolute, not limited from 2000...

    if ( modify )
        SetDateBCDandUpdate( DECtoBCD( year - 2000 ), DECtoBCD( month + 1 ), DECtoBCD( day ) );

    return MODE_EXIT;
}


char *chooseExact( int *sel, int kp, int max ) {
    if ( kp < max )
        *sel = kp;
    return 0;
}


int changeCalibration() {
    int cal = RCFGCALbits.CAL;
    if ( genericMenu( "Calibration", printNumber, decrement, increment, chooseExact, 256, &cal ) == MODE_KEYMODE )
        return MODE_KEYMODE;
    RCFGCALbits.CAL = ( char ) cal;
    I2CmemoryWRITE( 63535, RCFGCALbits.CAL );     //store value in last byte
    return MODE_EXIT;
}



int change1224()
{

    char *print1224( int *sel, int max ) {


        if ( *sel ) {
            custom_character( 2, &( AMPM[1][0] ) );
            custom_character( 3, &( AMPM[2][0] ) );
            sprintf( out, "12h 8:34:00\2\3" );
        } else
            sprintf( out, "24h 20:34:00" );

        return out;
    }

    void sel1224( int *sel, int max ) {
        ( *sel ) = !( *sel );
    }

    int mode1224 = TwelveHour ? 1 : 0;
    if ( genericMenu( "Format", print1224, sel1224, sel1224, 0, -2, &mode1224 ) == MODE_KEYMODE )
        return MODE_KEYMODE;
    TwelveHour = mode1224; // ? TRUE : FALSE;
    return MODE_EXIT;
}

int changeDST()
{
    extern char *TimeZone_name[];
    char *printDST( int *zone, int max ) {
        return TimeZone_name[ *zone ];
    }

    int region = dstRegion;
    if ( genericMenu( "DST Zone", printDST, increment, decrement, 0, -5, &region ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    dstRegion = region;
    DST = inDST( &region ); // region is dummy
    return MODE_EXIT;
}


int changeLocation()
{

    int c;
    /* The location feature will be expanded to provide a
     * selectable menu of major world cities. These will feed
     * the world time, the DST and the long/lat.
     *
     * However, in any case there should be a CUSTOM selector
     */

    /* only implement the CUSTOM for now.. */

    //custom_character( 0, characterEnter );
    //custom_character( 1, characterEllipsis );
    //custom_character( 2, characterEllipsis2 );


    double originalLongitude = Longitude;

    BOOL ok = FALSE;
    do {
        UpdateLCDline1( "Longitude (E<0)?" );

        // Display old Long and enter new value
        Xreg = hms( Longitude );

        c = OneLineNumberEntry();
        if ( c != MODE_EXIT ) return c; // escape

        // validate longitude -180 <= long <= +180
        if ( Xreg < -180 || Xreg > 180 ) {
            UpdateLCDline1( "Range" );
            UpdateLCDline2( "-180\xDF - +180\xDF   \010" );
            GetDebouncedKey();
        } else {
            // assume HMS format
            Longitude = hr( Xreg );
//            UpdateLCDline1("Longitude SET   ");
            ok = TRUE;
        }
//        DelayMs(2000); // visual delay
    } while ( !ok );

    ok = FALSE;
    do {
        // display old lat and enter new value
        UpdateLCDline1( "Latitude?" );

        Xreg = hms( Latitude );

        c = OneLineNumberEntry();
        if ( c != MODE_EXIT ) {
            Longitude = originalLongitude;      // undo any longitude change
            return c; // escape
        }

        // validate lat -90 <= lat <= +90

        if ( Xreg < -90 || Xreg > 90 ) {
            UpdateLCDline1( "Range" );
            UpdateLCDline2( "-90\xDF - +90\xDF     \010" );
            GetDebouncedKey();
        } else {
            Latitude = hr( Xreg );
            ok = TRUE;
        }
    } while ( !ok );

    return MODE_EXIT;
}




char *printCalc( int *type, int max )
{
    return ( *type ) ? "RPN" : "Al\5ebraic";
//    if ( *type ) return "RPN";
//    else return "Al\5ebraic";
}

int appCalculatorMode()
{
    custom_character( 5, character_g );
    return genericMenu( "Calculator", &printCalc, &increment, &decrement, 0, -2, &RPNmode );
}

char *printErase( int *n, int max ) {
    return (*n) ? "Yes":"No";
}

int appClearEEPROM()
{
    int erase = 0;
    genericMenu( "Erase?", printErase, increment, decrement, 0, -2, &erase );
    if ( erase ) {

        custom_character(5,character_g);
        UpdateLCDline1( "Erasin\5" );

        unsigned int c2 = 0;
        while ( c2 < 65535 ) {

            ResetSleepTimer();          //we don't want to timeout while doing this
            if ( KeyScan2( FALSE ) == KeyClear ) break;

            I2CmemoryWRITE( c2, 0 );

            if ( !( c2 & 63 )) {
                sprintf( out, "%5u of 65535", c2 );
                UpdateLCDline2( out );
            }

            c2++;
        }
    }
    return MODE_EXIT;
}


static const char *k2c[] = { 0, "/", "+-", "7", "8", "9", "X<>Y", "Clear", "*", ".",
    "4", "5", "6", "(", ")", "-", "0", "1", "2", "3", "EXP", "Menu", "+", "", "STO", "ENT" 
};



int appSelfTest()
{


    I2CmemoryWRITE( 65534, 0xAA );
    int c = I2CmemoryREAD( 65534 );
    I2CmemoryWRITE( 65534, 0 );
    int c2 = I2CmemoryREAD( 65534 );
    if (( c != 0xAA ) || ( c2 != 0 ) ) {
        UpdateLCDline2( "EEPROM fail" );
        GetDebouncedKey();
    }

    custom_character(5,character_y);
    UpdateLCDline2( "Ke\5board Test" );
    //UpdateLCDline2( "MODE to exit" );

    int KeyPress2;
    do {
        KeyPress2 = GetDebouncedKey();
        UpdateLCDline2( k2c[ KeyPress2 ] );
    } while ( KeyPress2 != KeyMode );

    return MODE_EXIT;
}



char *printTimeout( int *timeout, int max )
{
    sprintf( out, "%3d seconds", (*timeout) );
    return out;
}

int appLCDTimeout()
{
    int sel = PR1 >> 7;
    if ( genericMenu( "LCD Timeout", &printTimeout, &decrement, &increment, chooseExact, 420, &sel ) != MODE_KEYMODE )
        PR1 = sel << 7;
    return MODE_EXIT;
}


int appAbout()
{
    custom_character(3, character_g );
    sprintf( out, "\xE4Watch %s", RevString );
    viewString( out, "(c)David L. Jones, zowki, Hu\3h Steers, Andrew Davie", 0, 2 );
    return MODE_EXIT;
}


//***********************************
// The main setup mode routine

int SetupMode( int p )
{

    extern const charSet appCharset[];

    const menuItem setupMenuMenu[] = {
        { "Calculator", &appCalculatorMode, 0 },
        { "Clear EEPROM", &appClearEEPROM, 0 },
        { "Test", &appSelfTest, 0 },
        { "LCD timeout", &appLCDTimeout, 0 },
        { "About", &appAbout, 0 },
    };

    const packedMenu2 setupMenu = {
        "Setu\4",
        printMenu,
        2, appCharset, 5, setupMenuMenu
    };

    return genericMenu2( &setupMenu );
}


