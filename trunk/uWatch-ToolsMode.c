//********************************************************
// uWatch
// Tools Mode Functions
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

//global variables for Tools Mode

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

#include "def.h"
#include "menu.h"
#include "characterset.h"
#include "uWatch-LCD.h"
#include "tool-factor.h"



int StopWatch(void) {
	unsigned int KeyPress2;
    char s[MaxLCDdigits + 1];


	rtccTime before;
	
	int startSeconds;
	int mask;
	int hours;
	int minutes;
	int seconds;
    int basetime;


	
	void displayElasped( int displaySeconds )
	{
			hours = (int) ( displaySeconds / ( 60 * 60 ));
			displaySeconds -= hours * 60 * 60;
			minutes = (int) ( displaySeconds / 60 );
			displaySeconds -= minutes * 60;
			
			sprintf( s, "        %02d:%02d:%02d", hours, minutes, displaySeconds );
			
			UpdateLCDline2(s);
	}
	
	UpdateLCDline1("Stopwatch");
	UpdateLCDline2("Paused  00:00:00");

	
	RtccReadTime(&before);
	Time = before;
	startSeconds = BCDtoDEC(before.f.hour) * 60 * 60 + BCDtoDEC(before.f.min) * 60 + BCDtoDEC(before.f.sec);


	mask = 0;
    basetime = 0;

	BOOL displayLap = FALSE;
	BOOL displaySplit = FALSE;

	int lapSeconds = 0;
	int splitSeconds = 0;
	int displaySeconds;
    BOOL active = FALSE;


//	sprintf( s, "%s%sZERO", displayLap ? "[LAP]":" LAP ", displaySplit ? "[SPLIT]" : " SPLIT " );
//	UpdateLCDline1( s );

	do {

    	RtccReadTime( &Time );
		seconds = BCDtoDEC(Time.f.hour) * 60 * 60 + BCDtoDEC(Time.f.min) * 60 + BCDtoDEC(Time.f.sec);
        displaySeconds = seconds - startSeconds;

		KeyPress2 = KeyScan2( FALSE );

        // debounce enter via masking
		if ( !mask && ( KeyPress2 != KeyEnter ))
			mask = 0xFFFF;
		KeyPress2 &= mask;

		if ( KeyPress2==KeyMode )
            return MODE_KEYMODE;

        if ( KeyPress2 == KeyClear )
            return MODE_KEYCLEAR;


        // Handle Pause via Enter key
        // elapsed times can bracket these paused moments

        if ( KeyPress2 == KeyEnter ) {

            if ( !active ) {
                startSeconds = seconds;
            } else {
                basetime += displaySeconds;
            }

            active = !active;


        } else {


            if ( active ) {
    
        		if ( KeyPress2 == Key7 ) {	// LAP
        
        			displayLap = !displayLap;
        
        			if ( displayLap ) {
        				lapSeconds = displaySeconds;
        				displaySplit = FALSE;
        			}
        
        			sprintf( s, "%s%sZERO", displayLap ? "[LAP]":" LAP ", displaySplit ? "[SPLIT]" : " SPLIT " );
        			UpdateLCDline1( s );
        
        			mask = 0;
        
        		} else if ( KeyPress2 == Key8 ) { // SPLIT
        			
        			displaySplit = !displaySplit;
        
        			if ( displaySplit ) {
        				splitSeconds = displaySeconds - lapSeconds;
        				displayLap = FALSE;
        			}
        
        			sprintf( s, "%s%sZERO", displayLap ? "[LAP]":" LAP ", displaySplit ? "[SPLIT]" : " SPLIT " );
        			UpdateLCDline1( s );
                }
          
    			mask  = 0;
    
    		} else if ( KeyPress2 == Key9 ) { // ZERO
    			startSeconds = seconds;
    			lapSeconds = seconds;
    			splitSeconds = 0;
    			displaySeconds = 0;
    			mask = 0;
    		}
        }
    

//		if ( displayLap )
//			displaySeconds = lapSeconds;
//		else if ( displaySplit )
//			displaySeconds = splitSeconds;


        displayElasped( displaySeconds );



	
	} while ( TRUE );
	
	
//	UpdateLCDline1("Any key to exit");
//	KeyPress2 = GetDebouncedKey();


    return MODE_EXIT;

}




int quadratic( int p ) {

    int KeyPress2;

    //quadratic (Solve ax^2 + bx + c = 0 given B*B-4*A*C >= 0)

    float a,b,c;
    float d;
    float root1,root2;
    float tmp;
    char s2[MaxLCDdigits + 1];
    
    /*receive coefficients a, b and c from user*/
    UpdateLCDline1("Enter a:");
    Xreg = 0;
    tmp = OneLineNumberEntry();
    a = Xreg;
    UpdateLCDline1("Enter b:");
    Xreg = 0;
    tmp = OneLineNumberEntry();
    b = Xreg;
    UpdateLCDline1("Enter c:");
    Xreg = 0;
    tmp = OneLineNumberEntry();
    c = Xreg;
    Xreg = 0;
    
    d  = sqrt(b*b - 4.0*a*c);   //compute the square root of discriminant d
    root1 = (-b + d)/(2.0*a);   //first root
    root2 = (-b - d)/(2.0*a);   //second root
    
    if (b*b-4*a*c>=0) //check for valid equation
    {
        memset( out, '\0', MaxLCDdigits+1 ); //clear string
        sprintf(s2, "%f", root1);
        strcat( out, "x=");
        strcat(out, s2);
        UpdateLCDline1(out);
    
        memset(out, '\0', MaxLCDdigits+1 );
        strcat(out, "or x=");
        sprintf(s2, "%f", root2);
        strcat(out, s2);
        UpdateLCDline2(out);
        KeyPress2 = GetDebouncedKey();
    }
    
    else
    {
        UpdateLCDline1("Error:");
        UpdateLCDline2("Invalid Equation");
        KeyPress2 = GetDebouncedKey();
    }

    return MODE_EXIT;
}
    


//***********************************
// The main tools mode routine

int ToolsMode( int p )
{
    const packedMenu toolsMenu = {
        "Applications:",
        printMenu,
        increment, decrement, 2,
        {   0,0,0,0,
        },
        {   { "Quadratic",  quadratic, 0 },
            { "Factorise", factor, 0 },
        },
    };

    return genericMenu2( &toolsMenu, 0 );
}


