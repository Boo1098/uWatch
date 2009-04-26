//********************************************************
// uWatch
// Setup Mode Functions
// Version 1.5
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
    if ( h < 0x24 && m < 0x60 && s < 0x60 ) {
        RCFGCALbits.RTCPTR = 1;     //select correct RTC register
        RTCVAL = h;

        //the result is now a 16bit BCD number
        m = ( m << 8 ) + s;
        RCFGCALbits.RTCPTR = 0;     //select correct RTC register
        RTCVAL = m;
    }
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
        DST = inDST( &y ); // dummy
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

    for ( ;; ) {
        int c;
        int key;

        key = GetDebouncedKey();
        // ??? while ((key = KeyScan(TRUE)) == 0) ;
        ResetSleepTimer();

        c = EnterNumber( key );

        IFEXIT( c );

        if ( c == KeyEnter ) {
            CompleteXreg();

            UpdateXregDisplay();
            UpdateLCDline2( DisplayXreg );
            return MODE_EXIT;
        }
    }
}







char *printNumber( int *number, int max )
{
    sprintf( out, "%d", *number );
    return out;
}

char *printHour( int *hour, int max )
{

    // 12AM, 1AM... 12PM.. etc
    // OR 24 hour

    if ( TwelveHour ) {

        int cbase = ( *hour ) > 11 ? 1 : 0;
        int h = ( *hour ) % 12;
        if ( !h ) h = 12;

        sprintf( out, "%2d%c%c", h,
                 custom_character( 2, &( AMPM[ cbase ][0] ) ),
                 custom_character( 3, &( AMPM[ 2 ][0] ) ) );

    } else
        sprintf( out, "%2d", ( *hour ) );

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

    if ( genericMenu( "Hour", &printHour, &increment, &decrement, 24, &h ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    if ( genericMenu( "Minute", &printMinSec, &increment, &decrement, 60, &m ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    if ( genericMenu( "Second", &printMinSec, &increment, &decrement, 60, &s ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    SetTimeBCD( DECtoBCD( h ), DECtoBCD( m ), DECtoBCD( s ) );
    return MODE_EXIT;
}


char *printMonth( int *month, int max )
{
    strcpy( out, monthName[ *month ] );         // make a COPY so we don't have ROM limitation
    return out;
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

/*const unsigned char *boldDigit[] = {
    character_bold0,
    character_bold1,
    character_bold2,
    character_bold3,
    character_bold4,
    character_bold5,
    character_bold6,
    character_bold7,
    character_bold8,
    character_bold9,
    character_boldSpace
};
*/

int leap( int year ) {
    return ( ( year % 4 ) == 0 )
        && (    (( year % 100 ) != 0 )
             || (( year % 400 ) == 0 )
           )
        ? 1 : 0;
}

int daysInMonth( int year, int month ) {
    int dom[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; 
    int dim = dom[ month - 1];
    if ( month == 2 && leap( year ))
        dim++;
    return dim;
}

int gYear, gMonth;
char *processCalendar( int *pDay, int max )
{
    int dayOfWeek = DAYOFWEEK( mjd( gYear, gMonth, *pDay ) );
    
    char cc = 2;                     // custom char #

    // preload custom chars as we can't do it in 1MHz mode...
    int i;
    for ( i = 0; i < 3; i++ )
        custom_character( i+cc, qday[ ( dayOfWeek + i ) % 7 ] );

    Clock4MHz();

    *out = 0;

    int limit = (*pDay) + 2;        // MAX DAYS?
    if ( limit > max )
        limit = max;

    int dd;
    for ( dd = *pDay; dd <= limit; dd++ ) {

        char highlight = ' ';
        if ( dd == *pDay )
            highlight = '(';
        else if ( dd == (*pDay ) + 1 )
            highlight = ')';

        char *ps = out + strlen( out );
        sprintf( ps, "%c%c%-2d", highlight, cc++, dd );

        dayOfWeek = ( dayOfWeek + 1 ) % 7;

        if ( dd == limit && dd == *pDay )
            strcat( out, ")" );
    }

    Clock250KHz();

    return out;
}


void decrementDay( int *day, int max )
{
    ( *day)--;
    if (( *day) < 1 )
        ( *day ) = max;
}

void incrementDay( int *day, int max )
{
    ( *day)++;
    if (( *day) > max )
        ( *day ) = 1;
}



int doCal( BOOL modify ) {

    int year = BCDtoDEC( Date.f.year ) + 2000;
    int month = BCDtoDEC( Date.f.mon );
    int day = BCDtoDEC( Date.f.mday );

    if ( genericMenu( "Year", &printNumber, &increment, &decrement, 2100, &year ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    sprintf( out, "%d", year );
    if ( genericMenu( out, &printMonth, &increment, &decrement, 12, &month ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    gYear = year;
    gMonth = month + 1;         // 1-based

    sprintf( out, "%d, %s", year, monthName[ month ] );
    int dim = daysInMonth( gYear, gMonth );

    if ( genericMenu( out, processCalendar, incrementDay, decrementDay, dim, &day ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    //TODO: year should be absolute, not limited from 2000...

    if ( modify )
        SetDateBCDandUpdate( DECtoBCD( year - 2000 ), DECtoBCD( month ), DECtoBCD( day ) );

    return MODE_EXIT;
}


int changeDate()
{
    return doCal( TRUE );
}

int doCalendar() {

    return doCal( FALSE );
}


int changeCalibration()
{

    char *printCal( int *cal, int max ) {
        sprintf( out, "CAL=%d", *cal );
        return out;
    }

    int cal = RCFGCALbits.CAL;
    if ( genericMenu( "Calibration", &printCal, increment, decrement, 256, &cal ) == MODE_KEYMODE )
        return MODE_KEYMODE;
    RCFGCALbits.CAL = ( char ) cal;
    I2CmemoryWRITE( 63535, RCFGCALbits.CAL );     //store value in last byte
    return MODE_EXIT;
}



int change1224()
{

    char *print1224( int *sel, int max ) {
        if ( *sel ) {
            sprintf( out, "12h 8:34:00%c%c",
                     custom_character( 2, &( AMPM[1][0] ) ),
                     custom_character( 3, &( AMPM[2][0] ) ) );
        } else
            sprintf( out, "24h 20:34:00" );

        return out;
    }

    void sel1224( int *sel, int max ) {
        ( *sel ) = !( *sel );
    }

    int mode1224 = TwelveHour ? 1 : 0;
    if ( genericMenu( "Time Format", print1224, sel1224, sel1224, 0, &mode1224 ) == MODE_KEYMODE )
        return MODE_KEYMODE;
    TwelveHour = mode1224 ? TRUE : FALSE;
    return MODE_EXIT;
}

int changeDST()
{

    char *printDST( int *zone, int max ) {
        return ( char * ) TimeZones[ *zone ].region;
    }

    int region = dstRegion;
    if ( genericMenu( "DST Zone", printDST, increment, decrement, DIM( TimeZones ), &region ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    dstRegion = region;
    DST = inDST( &dstRegion );
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
            UpdateLCDline1( "Longitude range" );
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
            UpdateLCDline1( "Latitude range" );
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
    if ( *type ) return "RPN";
    else return "Algebraic";
}

int appCalculatorMode()
{
    return genericMenu( "Calculator", &printCalc, &increment, &decrement, 2, &RPNmode );
}


int appClearEEPROM()
{

    UpdateLCDline1( "Erase EEPROM ?" );
    UpdateLCDline2( "ENTER or Cancel" );
    int KeyPress2 = GetDebouncedKey();
    if ( KeyPress2 == KeyEnter ) {
        unsigned int c;
        int c2;
        UpdateLCDline1( "Erasing EEPROM" );
        c2 = 0;
        for ( c = 0;c < 65534;c++ ) { //don't overwrite the last byte, it's used for the calibration value
            ResetSleepTimer();          //we don't want to timeout while doing this

            if ( GetDebouncedKey() == KeyClear ) break;

            I2CmemoryWRITE( c, 0 );
            c2++;
            if ( c2 >= 100 ) {
                sprintf( out, "%5u of 65535", c );
                UpdateLCDline2( out );
                c2 = 0;
            }
        }
    }
    return MODE_EXIT;

}

int appSelfTest()
{
    int c, c2;
    int KeyPress2;

    I2CmemoryWRITE( 65530, 0xAA );
    c = I2CmemoryREAD( 65530 );
    I2CmemoryWRITE( 65530, 0 );
    c2 = I2CmemoryREAD( 65530 );
    if (( c != 0xAA ) || ( c2 != 0 ) ) {
        UpdateLCDline1( "EEPROM failed!" );
        UpdateLCDline2( "Press ENTER" );
        KeyPress2 = GetDebouncedKey();
        return MODE_EXIT;
    }
    UpdateLCDline1( "EEPROM passed" );
    UpdateLCDline2( "Press ENTER" );
    KeyPress2 = GetDebouncedKey();
    UpdateLCDline1( "Keyboard Test" );
    UpdateLCDline2( "MODE to exit" );

    while ( TRUE ) {

        char *s = 0;

        int KeyPress2 = GetDebouncedKey();
        switch ( KeyPress2 ) {

        case Key1:
            s = "1";
            break;
        case Key2:
            s = "2";
            break;
        case Key3:
            s = "3";
            break;
        case Key4:
            s = "4";
            break;
        case Key5:
            s = "5";
            break;
        case Key6:
            s = "6";
            break;
        case Key7:
            s = "7";
            break;
        case Key8:
            s = "8";
            break;
        case Key9:
            s = "9";
            break;
        case Key0:
            s = "0";
            break;
        case KeyPoint:
            s = ".";
            break;
        case KeyPlus:
            s = "+";
            break;
        case KeyMinus:
            s = "-";
            break;
        case KeyMult:
            s = "X";
            break;
        case KeyDiv:
            s = "/";
            break;
        case KeyMenu:
            s = "Menu";
            break;
        case KeyEnter:
            s = "Enter";
            break;
        case KeyClear:
            s = "C";
            break;
        case KeySign:
            s = "+/-";
            break;
        case KeyEXP:
            s = "EXP";
            break;
        case KeyRCL:
            s = "STO";
            break;
        case KeyLP:
            s = "(";
            break;
        case KeyRP:
            s = ")";
            break;
        case KeyXY:
            s = "X-Y";
            break;
        case KeyMode:
            return MODE_KEYMODE;
        }

        if ( s )
            UpdateLCDline2( s );


    }
    return MODE_EXIT;
}


char *printTimeout( int *timeout, int max )
{
    sprintf( out, "%3d seconds", *timeout / 128 );
    return out;
}

void incTimeout( int *timeout, int max ) {
    if ( ((unsigned int)*timeout) < 53760 )
        *timeout += 1280;
}

void decTimeout( int *timeout, int max ) {
    if ( ((unsigned int)*timeout) > 1280 )
        *timeout -= 1280;
}

int appLCDTimeout()
{
    return genericMenu( "LCD Timeout", &printTimeout, &incTimeout, &decTimeout, 2, (int*)&PR1 );
}


int appAbout()
{

    sprintf( out, "\xE4Watch %s", RevString );
    viewString( out, "(c)David L. Jones, zowki, hugh, Andrew Davie", 0, 2 );
    return MODE_EXIT;
}


//***********************************
// The main setup mode routine

int SetupMode( int p )
{
    const packedMenu setupMenu = {
        "Apps Option",
        printMenu,
        increment, decrement, 5,
        {   0,0,0,0,
        },
        {   { "Calculator", &appCalculatorMode, 0 },
            { "Clear EEPROM", &appClearEEPROM, 0 },
            { "Self Test", &appSelfTest, 0 },
            { "LCD timeout", &appLCDTimeout, 0 },
            { "About", &appAbout, 0 },
        },
    };

    return genericMenu2( &setupMenu, 0 );
}


