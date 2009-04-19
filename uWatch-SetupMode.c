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

void SetTimeBCD(unsigned int h, unsigned int m, unsigned int s)
{
    if (h < 0x24 && m < 0x60 && s < 0x60)
    {
        RCFGCALbits.RTCPTR=1;       //select correct RTC register
        RTCVAL=h;

        //the result is now a 16bit BCD number
        m = (m << 8) + s;
        RCFGCALbits.RTCPTR=0;       //select correct RTC register
        RTCVAL=m;
    }
}

int SetDateBCD(unsigned int y, unsigned int m, unsigned int d)
{
    int v = (m && m < 0x13 && d && d < 0x32 && y < 0x100);
    if (v)
    {
        //month and day are combined in the one word
        m = (m << 8) + d;

        //the result is now a 16bit BCD number
        RCFGCALbits.RTCPTR=2;       //select correct RTC register
        RTCVAL=m;
        
        //the result is now an 8bit BCD number
        RCFGCALbits.RTCPTR=3;       //select correct RTC register
        RTCVAL=y;

    }
    return v;
}

void SetDateBCDandUpdate(int y, int m, int d)
{
    if (SetDateBCD(y, m, d))
    {

        // read accepted date and time back
        RtccReadDate(&Date);
        RtccReadTime(&Time);       

        // recalculate DoW and MJD
        dayHasChanged();
    
        // infer whether we are in DST
        DST = inDST(&y); // dummy
    }
}


static const char* SetupMenu[] = 
{
    "Calc Mode",
    "Clear EEPROM",
    "Self Test",
    "LCD timeout",
    "About",
};

// enter a number on line return in Xreg
// return final key pressed
int OneLineNumberEntry()
{
    // Assume Xreg contains old value
    // display old value
    UpdateXregDisplay();
    UpdateLCDline2(DisplayXreg);

    // process input
    ResetFlags();
    EnableXregOverwrite = TRUE;

    for (;;)
    {
        int c;
        int key;

		key = GetDebouncedKey();
        // ??? while ((key = KeyScan(TRUE)) == 0) ;
        ResetSleepTimer();		
                
        c = EnterNumber(key);

        IFEXIT( c );

        if (c == KeyEnter)
        {
            CompleteXreg();
            
            UpdateXregDisplay();
            UpdateLCDline2(DisplayXreg);
            return MODE_EXIT;
        }
    }
}







char *printNumber( int *number, int max ) {
    sprintf( out, "%d", *number );
    return out;
}

char *printHour( int *hour, int max ) {

    // 12AM, 1AM... 12PM.. etc
    // OR 24 hour 

    if ( TwelveHour ) {

		int cbase = (*hour) > 11 ? 1 : 0;
        int h = (*hour) % 12;
        if ( !h ) h = 12;

        sprintf( out, "%2d%c%c", h,
            custom_character( 2, &( AMPM[ cbase ][0] ) ),
            custom_character( 3, &( AMPM[ 2 ][0] ) ));

    } else
        sprintf( out, "%2d", (*hour) );

    return out;
}


int changeTime() {

    RtccReadTime(&Time);            //read the RTCC registers

    int h = BCDtoDEC( Time.f.hour );
    int m = BCDtoDEC( Time.f.min );
    int s = BCDtoDEC( Time.f.sec );

    if ( genericMenu( "Hour:", &printHour, &increment, &decrement, 24, &h ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    if ( genericMenu( "Minute:", &printNumber, &increment, &decrement, 60, &m ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    if ( genericMenu( "Second:", &printNumber, &increment, &decrement, 60, &s ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    SetTimeBCD( DECtoBCD(h), DECtoBCD(m), DECtoBCD(s) );
    return MODE_EXIT;
}

void incYear( int *year, int max ) {
    (*year)++;    
}

void decYear( int *year, int max ) {
    (*year)--;
}


char *printMonth( int *month, int max ) {
    strcpy( out, monthName[ *month ] );         // make a COPY so we don't have ROM limitation
    return out;
}

void drawDay( char *s, int cc, int day, int dayOfWeek, char highlight, BOOL bold ) {

    char q[5];

    Clock250KHz();
    custom_character( cc, qday[ dayOfWeek ] );
    Clock4MHz();

    day++;               // 1-based

    if ( bold ) {

        int bd1 = day / 10;
        if ( !bd1 )
            bd1 = 10;

        int bd2 = day % 10;

        Clock250KHz();
        custom_character( 5, boldDigit[ bd1 ] );
        custom_character( 6, boldDigit[ bd2 ] );
        Clock4MHz();

        sprintf( q, "%c%c\005\006", highlight, cc );

    } else
        sprintf( q, "%c%c%-2d", highlight, cc, day );

    strcat( s, q );
}

char *processCalendar( int *pDay, int max ) {

// day 0-based

    int day = *pDay;
    int today = 12;


    Clock4MHz();

    BOOL selecting = TRUE;     //temp
    if ( selecting )
        today = day;

    int cc = 2;                     // custom char #
    int dayOfWeek = day%7;

    int dd;

    *out = 0;

    int limit = day + 3;
    for ( dd = day; dd < limit && dd < max ; dd++ ) {


        BOOL bold = ( dd == today ) || ( dd == day && selecting );

        char highlight = ' ';
        if ( dd == today )
            highlight = '(';
        else if ( dd == today+1 )
            highlight = ')';
        
        drawDay( out, cc, dd, dayOfWeek, highlight, bold );

        cc++;

        dayOfWeek = ( dayOfWeek + 1 ) % 7;


        if ( dd == limit && dd == today )
            strcat( out, ")" );
    }

    Clock250KHz();

    return out;
}


int changeDate() {

    int year = BCDtoDEC( Date.f.year ) + 2000;
    int month = BCDtoDEC( Date.f.mon );
    int day = BCDtoDEC( Date.f.mday ) - 1;          // 0-based

    if ( genericMenu( "Year:", &printNumber, &incYear, &decYear, 0, &year ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    sprintf( out, "%d, Month:", year );
    if ( genericMenu( out, &printMonth, &increment, &decrement, 12, &month ) == MODE_KEYMODE )
        return MODE_KEYMODE;
    
    sprintf( out, "%d, %s", year, monthName[ month ] );
    if ( genericMenu( out, &processCalendar, &increment, &decrement, 32, &day ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    //TODO: year should be absolute, not limited from 2000...

    SetDateBCDandUpdate( DECtoBCD( year - 2000 ), DECtoBCD( month ), DECtoBCD( day + 1 ) );

    return MODE_EXIT;
}


char *printCal( int *selection, int max ) {
    sprintf( out, "CAL=%d", RCFGCALbits.CAL );
    return out;
}
        
void incCal( int *selection, int max ) {
    RCFGCALbits.CAL++;
}

void decCal( int *selection, int max ) {
    RCFGCALbits.CAL--;
}

int changeCalibration() {
    if ( genericMenu( "Calibration", &printCal, &incCal, &decCal, 0, 0 ) == MODE_KEYMODE )
        return MODE_KEYMODE;
    I2CmemoryWRITE( 63535,RCFGCALbits.CAL );      //store value in last byte
    return MODE_EXIT;
}


char *print1224( int *sel, int max ) {
    if ( *sel ) {
        sprintf( out, "12h 8:34:00%c%c",
            custom_character( 2, &( AMPM[1][0] )),
            custom_character( 3, &( AMPM[2][0] )) );
    }
    else
        sprintf( out, "24h 20:34:00" );

    return out;
}

void sel1224( int *sel, int max ) {
    (*sel) = !(*sel);
}

int change1224() {
    int mode1224 = TwelveHour ? 1 : 0;
    if ( genericMenu( "Set Time Format", &print1224, &sel1224, &sel1224, 0, &mode1224 ) == MODE_KEYMODE )
        return MODE_KEYMODE;
    TwelveHour = mode1224 ? TRUE : FALSE;
    return MODE_EXIT;
}

int changeDST() {

    char *printDST( int *zone, int max ) {
        return (char * ) TimeZones[ *zone ].region;
    }

    void incDST( int *zone, int max ) {
        (*zone)++;
        if ( (*zone) >= max )
            (*zone) = 0;
    }

    int Mode = 0;
    if ( genericMenu( "DST Zone:", printDST, &increment, &decrement, DIM( TimeZones ), &dstRegion ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    DST = inDST( &dstRegion );
    return MODE_EXIT;
}


int changeLocation() {

    int c;
    /* The location feature will be expanded to provide a 
     * selectable menu of major world cities. These will feed
     * the world time, the DST and the long/lat.
     *
     * However, in any case there should be a CUSTOM selector
     */

    /* only implement the CUSTOM for now.. */

    custom_character( 0, characterEnter );
    custom_character( 1, characterEllipsis );
    custom_character( 2, characterEllipsis2 );


    double originalLongitude = Longitude;

    BOOL ok = FALSE;
    do
    {
        UpdateLCDline1("Longitude (E<0)?");
        
        // Display old Long and enter new value
        Xreg = hms(Longitude);
        
        c = OneLineNumberEntry();
        if (c != MODE_EXIT) return c; // escape
        
        // validate longitude -180 <= long <= +180
        if (Xreg < -180 || Xreg > 180)
        {
            UpdateLCDline1("Longitude range:" );
            UpdateLCDline2("-180\xDF\001\002+180\xDF   \010");                
            GetDebouncedKey();
        }
        else
        {
            // assume HMS format
            Longitude = hr(Xreg);
//            UpdateLCDline1("Longitude SET   ");
            ok = TRUE;
        }
//        DelayMs(2000); // visual delay
    } while (!ok);
    
    ok = FALSE;
    do
    {
        // display old lat and enter new value
        UpdateLCDline1("Latitude?");

        Xreg = hms(Latitude);

        c = OneLineNumberEntry();            
        if (c != MODE_EXIT) {
            Longitude = originalLongitude;      // undo any longitude change
            return c; // escape
        }

        // validate lat -90 <= lat <= +90

        if (Xreg < -90 || Xreg > 90)
        {
            UpdateLCDline1("Latitude range:" );
            UpdateLCDline2("-90\xDF\001\002+90\xDF     \010");  
            GetDebouncedKey();
        }
        else
        {
            Latitude = hr(Xreg);
            ok = TRUE;
        }
    } while (!ok);

    return MODE_EXIT;
}





//***********************************
// The main setup mode routine
// Note that all variables are global
int SetupMode(void)
{
    unsigned int KeyPress2;        //keypress variables
    int c,c2;

    char *printSet( int *sel, int max ) {
        strcpy( out, SetupMenu[ *sel ] );
        return out;
    }

    int Mode = 0;
    if ( genericMenu( "Apps Option:", &printSet, &increment, &decrement, DIM( SetupMenu ), &Mode ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    custom_character( 1, left_menu );
    custom_character( 2, right_menu );


    switch(Mode)                
    {
    case 0: // calc mode
        {
            //TODO: genericMenu
            UpdateLCDline1( "Calculator type:" );
            do {

                if (RPNmode) 
                    UpdateLCDline2(  "RPN           \1\2" );
                else UpdateLCDline2( "Algebraic     \1\2" );

                KeyPress2 = GetDebouncedKey();

                if ( NEXT( KeyPress2 ) || PREVIOUS( KeyPress2 ))
                    RPNmode = !RPNmode;

            } while ( KeyPress2 != KeyEnter );

        }
        break;
    case 1:                 //clear the EEPROM contents
        {
            UpdateLCDline1("Erase EEPROM ?");
            UpdateLCDline2("ENTER or Cancel");
            KeyPress2 = GetDebouncedKey();
            if (KeyPress2==KeyEnter)
            {
                unsigned int c;
                UpdateLCDline1("Erasing EEPROM");
                c2=0;
                for(c=0;c<65534;c++)        //don't overwrite the last byte, it's used for the calibration value
                {
                    ResetSleepTimer();          //we don't want to timeout while doing this
                    
					if ( GetDebouncedKey() == KeyClear ) break;

                    I2CmemoryWRITE(c,0);
                    c2++;
                    if (c2>=100)    
                    {
                        sprintf( out,"%5u of 65535",c);    
                        UpdateLCDline2(out);
                        c2=0;
                    }
                }
            }
        }
        break;
    case 2: // self test
        {
            I2CmemoryWRITE(65530, 0xAA);
            c=I2CmemoryREAD(65530);
            I2CmemoryWRITE(65530, 0);
            c2=I2CmemoryREAD(65530);
            if ((c!=0xAA)||(c2!=0))
            {
                UpdateLCDline1("EEPROM failed!");
                UpdateLCDline2("Press ENTER");
                KeyPress2 = GetDebouncedKey();
                return MODE_EXIT;
            }
            UpdateLCDline1("EEPROM passed");
            UpdateLCDline2("Press ENTER");
            KeyPress2 = GetDebouncedKey();
            UpdateLCDline1("Keyboard Test");
            UpdateLCDline2("MODE to exit");
                    
            while(TRUE)
            {
                KeyPress2 = GetDebouncedKey();
                switch(KeyPress2)
                {
                case Key1: UpdateLCDline2("1"); break;
                case Key2: UpdateLCDline2("2"); break;
                case Key3: UpdateLCDline2("3");  break;
                case Key4: UpdateLCDline2("4"); break;
                case Key5: UpdateLCDline2("5");  break;
                case Key6: UpdateLCDline2("6"); break;
                case Key7: UpdateLCDline2("7");  break;
                case Key8: UpdateLCDline2("8");  break;
                case Key9: UpdateLCDline2("9");  break;
                case Key0: UpdateLCDline2("0");  break;
                case KeyPoint: UpdateLCDline2(".");  break;
                case KeyPlus: UpdateLCDline2("+");  break;
                case KeyMinus: UpdateLCDline2("-"); break;
                case KeyMult: UpdateLCDline2("X"); break;
                case KeyDiv: UpdateLCDline2("/"); break;
                case KeyMenu: UpdateLCDline2("Menu"); break;
                case KeyEnter: UpdateLCDline2("Enter"); break;
                case KeyClear: UpdateLCDline2("Clear"); break;
                case KeySign: UpdateLCDline2("+/-"); break;
                case KeyEXP: UpdateLCDline2("EXP"); break;
                case KeyRCL: UpdateLCDline2("STO/RCL"); break;
                case KeyLP: UpdateLCDline2("(ROLL"); break;
                case KeyRP: UpdateLCDline2(")"); break;
                case KeyXY: UpdateLCDline2("X-Y"); break;
                case KeyMode: return MODE_KEYMODE;
                }
            }
        }
        break;

        case 3: // LCD timeout

            UpdateLCDline1("LCD Timeout:");

            do {

                sprintf(out,"%3d seconds   \001\002",PR1/128);
                UpdateLCDline2(out);

                KeyPress2 = GetDebouncedKey();

                if ( NEXT( KeyPress2 ) && PR1<53760 )
                    PR1 = PR1 + 1280;                           //increment by 10 seconds

                if ( PREVIOUS( KeyPress2 ) && PR1>1280 )
                    PR1 = PR1 - 1280;                           //decrement by 10 seconds

                IFEXIT( KeyPress2 );

            } while( KeyPress2 != KeyEnter );   

            break;


        case 4:     //about

            sprintf( out, "\xE4Watch %s", RevString );
            UpdateLCDline1( out );
            UpdateLCDline2("(c)David L Jones");
    		GetDebouncedKey();
            break;

    }

    return MODE_EXIT;
}

