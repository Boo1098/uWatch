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
    "Set Time",
    "Set Date",
    "Calc Mode",
    "Clear EEPROM",
    "Self Test",
    "LCD timeout",
    "ClockCalibration",
    "12/24hr Time",
    "DST Zone",
    "Location",
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

        if (c == KeyMode || c == KeyClear || c == KeyMenu)
            return c;

        if (c == KeyEnter)
        {
            CompleteXreg();
            
            UpdateXregDisplay();
            UpdateLCDline2(DisplayXreg);
            return c;
        }
    }
}

//***********************************
// The main setup mode routine
// Note that all variables are global
void SetupMode(void)
{
    unsigned int KeyPress2;        //keypress variables
    char s[MaxLCDdigits + 1];
    int Mode;
    int c,c2;

    Mode= DriveMenu("SETUP: +/- & ENT", SetupMenu, DIM(SetupMenu));

    switch(Mode)                
    {
    case 0:                 //change time
        {
            int h, m, s;
            UpdateLCDline2("(Press 2 Digits)");
            UpdateLCDline1("Enter Hours: ");
            h = GetNumBCD();

            if (h < 0) return; // escape

            UpdateLCDline1("Enter Minutes: ");
            m = GetNumBCD();
        
            UpdateLCDline1("Enter Seconds: ");
            s = GetNumBCD();
                    
            SetTimeBCD(h, m, s);
        }
        break;
    case 1:                 //change date
        {
            int y, m, d;
            UpdateLCDline2("(Press 2 Digits)");
            UpdateLCDline1("Enter Month: ");

            m = GetNumBCD();
            if (m < 0) return; // escape

            UpdateLCDline1("Enter Day: ");
            d = GetNumBCD();

            UpdateLCDline1("Enter Year: ");
            y = GetNumBCD();

            SetDateBCDandUpdate(y, m, d);
        }
        break;
    case 2: // calc mode
        {
            UpdateLCDline2("  +/- & ENTER");
            while(TRUE)
            {
                if (RPNmode) 
                    UpdateLCDline1("Calc Mode = RPN");
                else UpdateLCDline1("Calc Mode = ALG");
                KeyPress2 = GetDebouncedKey();
                if (KeyPress2==KeyEnter) return;
                if ((KeyPress2==KeyPlus)||(KeyPress2==KeyMinus))
                    RPNmode=!RPNmode;
            }
        }
        break;
    case 3:                 //clear the EEPROM contents
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
                        sprintf(s,"%5u of 65535",c);    
                        UpdateLCDline2(s);
                        c2=0;
                    }
                }
            }
        }
        break;
    case 4: // self test
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
                return;
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
                case KeyMode: return;
                }
            }
        }
        break;
    case 5: // LCD timeout
        {
            UpdateLCDline1("+/- Adj Timeout");
            do
            {
                sprintf(s,"%3i Seconds",PR1/128);
                UpdateLCDline2(s);
                KeyPress2 = GetDebouncedKey();
                if ((KeyPress2==KeyPlus)&&(PR1<53760)) PR1=PR1+1280;        //increment by 10 seconds
                if ((KeyPress2==KeyMinus)&&(PR1>1280)) PR1=PR1-1280;        //decrement by 10 seconds
                if (KeyPress2==KeyEnter) break;
            }
            while(1);   
        }
        break; // Clock calibration
    case 6:
        {
            UpdateLCDline1("+/- Adjust CAL");
            do
            {
                sprintf(s,"CAL=%4i",RCFGCALbits.CAL);
                UpdateLCDline2(s);
                KeyPress2 = GetDebouncedKey();
                if (KeyPress2==KeyPlus) RCFGCALbits.CAL++;
                if (KeyPress2==KeyMinus) RCFGCALbits.CAL--;
                if (KeyPress2==KeyEnter) break;
            }
            while(1);   
            I2CmemoryWRITE(63535,RCFGCALbits.CAL);      //store value in last byte
        }
        break;
    case 7: // 12/24 hour mode
        {
            UpdateLCDline2("  +/- & ENTER");
            while(TRUE)
            {
                if (TwelveHour) 
                    UpdateLCDline1("Time Mode = 12h");
                else UpdateLCDline1("Time Mode = 24h");
                KeyPress2 = GetDebouncedKey();
                if (KeyPress2==KeyEnter) return;
                if (KeyPress2==KeyPlus||KeyPress2==KeyMinus)
                    TwelveHour=!TwelveHour;
            }
        }
        break;
    case 8: // DST Zone
        {
            UpdateLCDline2("  +/- & ENTER");
            while(TRUE)
            {
                UpdateLCDline1(TimeZones[(int)dstRegion].region);
                        
                KeyPress2 = GetDebouncedKey();
                if (KeyPress2==KeyEnter) 
                {
                    DST = inDST(&Mode); // dummy
                    return;
                }

                if (KeyPress2 == KeyPlus)
                {
                    if (++dstRegion >= DIM(TimeZones))
                        dstRegion = 0;
                }
                else if (KeyPress2 == KeyMinus)
                {
                    if (--dstRegion < 0)
                        dstRegion += DIM(TimeZones);
                }
            }
        }
        break;
    case 9: // location
        {
            int c;
            /* The location feature will be expanded to provide a 
             * selectable menu of major world cities. These will feed
             * the world time, the DST and the long/lat.
             *
             * However, in any case there should be a CUSTOM selector
             */

            /* only implement the CUSTOM for now.. */

            BOOL ok = FALSE;
            do
            {
                UpdateLCDline1("Longitude (E<0) ");
                
                // Display old Long and enter new value
                Xreg = hms(Longitude);
                
                c = OneLineNumberEntry();
                if (c != KeyEnter) return; // escape
                
                // validate longitude -180 <= long <= +180
                if (Xreg < -180 || Xreg > 180)
                {
                    UpdateLCDline1("long [-180,+180]");                
                }
                else
                {
                    // assume HMS format
                    Longitude = hr(Xreg);
                    UpdateLCDline1("Longitude SET   ");
                    ok = TRUE;
                }
                DelayMs(2000); // visual delay
            } while (!ok);
            
            ok = FALSE;
            do
            {
                // display old lat and enter new value
                UpdateLCDline1("Latitude        ");

                Xreg = hms(Latitude);

                c = OneLineNumberEntry();            
                if (c != KeyEnter) return; // escape

                // validate lat -90 <= lat <= +90

                if (Xreg < -90 || Xreg > 90)
                {
                    UpdateLCDline1("lat [-90,+90]   ");
                }
                else
                {
                    Latitude = hr(Xreg);
                    UpdateLCDline1("Latitude SET    ");
                    ok = TRUE;
                }
                DelayMs(2000); // visual delay
            } while (!ok);
        }
        break;
    case 10: // about
        {
            strcpy(s," Watch ");
            strcat(s,RevString);
            s[0]=228;               //mu symbol
            UpdateLCDline1(s);
            UpdateLCDline2("(c)David L Jones");
			KeyPress2 = GetDebouncedKey();
            //do KeyPress2=KeyScan(); while(KeyPress2==0);
        }
        break;

    }
}

