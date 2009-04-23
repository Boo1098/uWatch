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

    //quadratic (Solve ax^2 + bx + c = 0 given B*B-4*A*C >= 0)

    float a,b,c;
    float d;
    float root1,root2;
    
    custom_character( 2, character_squaring );
    UpdateLCDline1( "[?]x\2 + bx + c" );

    Xreg = 0;

    IFEXIT( OneLineNumberEntry() );

    a = Xreg;

    UpdateLCDline1( "ax\2 + [?]x + c" );
    Xreg = 0;

    IFEXIT( OneLineNumberEntry() );

    b = Xreg;
    UpdateLCDline1( "ax\2 + bx + [?]" );
    Xreg = 0;

    IFEXIT( OneLineNumberEntry() );

    c = Xreg;
//    Xreg = 0;

    if ( a == (int)a )
        sprintf( displayBuffer, "%d", (int)a );
    else
        sprintf( displayBuffer, "%f", a );

    strcat( displayBuffer, "x\2+" );

    if ( b == (int)b )
        sprintf( out, "%d", (int)b );
    else
        sprintf( out, "%f", b );
   
    strcat( displayBuffer, out );
    strcat( displayBuffer, "x+" );

    if ( c == (int)c )
        sprintf( out, "%d ", (int)c );
    else
        sprintf( out, "%f ", c );

    strcat( displayBuffer, out );

    
    if ( a == 0 )
        strcat( displayBuffer, "has no roots." );
    else {
    
        d = b*b - 4*a*c;
    
        if ( d < 0 )
            strcat( displayBuffer, "is invalid (b\2-4ac < 0)." );
        else {
    
            d  = sqrt(d);   //compute the square root of discriminant d
            root1 = (-b + d)/(2.0*a);   //first root
            root2 = (-b - d)/(2.0*a);   //second root
    
            char buffer[64];
            sprintf( buffer, " has roots %f and %f.", root1, root2 );
            strcat( displayBuffer, buffer );
        }
    }
    
    return viewString( "Equation", displayBuffer, 0, VIEW_AUTOSCROLL );
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


