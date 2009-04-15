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

static const char* ToolsMenu[] = 
{
    "Quadratic",
    "Factor",
    "Stopwatch",
};

void StopWatch(void) {
	unsigned int KeyPress2;
    char s[MaxLCDdigits + 1];


	rtccTime before;
	
	int startSeconds;
	int mask;
	int hours;
	int minutes;
	int seconds;


	
	void displayElasped( int displaySeconds )
	{
			hours = (int) ( displaySeconds / ( 60 * 60 ));
			displaySeconds -= hours * 60 * 60;
			minutes = (int) ( displaySeconds / 60 );
			displaySeconds -= minutes * 60;
			
			sprintf( s, "        %02d:%02d:%02d", hours, minutes, displaySeconds );
			
			UpdateLCDline2(s);
	}
	
	UpdateLCDline1("Stopwatch \x7E[ENT]");
	UpdateLCDline2("        00:00:00");

	do {
		KeyPress2 = GetDebouncedKey();
		if (KeyPress2==KeyMode || KeyPress2 == KeyClear || KeyPress2 == KeyMenu ) return;
	} while ( KeyPress2 != KeyEnter );
	
	RtccReadTime(&before);
	Time = before;
	startSeconds = BCDtoDEC(before.f.hour) * 60 * 60 + BCDtoDEC(before.f.min) * 60 + BCDtoDEC(before.f.sec);
	


	mask = 0;

	BOOL displayLap = FALSE;
	BOOL displaySplit = FALSE;

	int lapSeconds = 0;
	int splitSeconds = 0;
	int displaySeconds;


			sprintf( s, "%s%sZERO", displayLap ? "[LAP]":" LAP ", displaySplit ? "[SPLIT]" : " SPLIT " );
			UpdateLCDline1( s );

	do {

    	RtccReadTime( &Time );
		seconds = BCDtoDEC(Time.f.hour) * 60 * 60 + BCDtoDEC(Time.f.min) * 60 + BCDtoDEC(Time.f.sec);
        displaySeconds = seconds - startSeconds;

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

			mask  = 0;

		} else if ( KeyPress2 == Key9 ) { // ZERO
			startSeconds = seconds;
			lapSeconds = seconds;
			splitSeconds = 0;
			displaySeconds = 0;
			mask = 0;
		}


		if ( displayLap )
			displaySeconds = lapSeconds;
		else if ( displaySplit )
			displaySeconds = splitSeconds;


        displayElasped( displaySeconds );


		KeyPress2 = KeyScan2( FALSE );
		if ( !mask && ( KeyPress2 != KeyEnter ))
			mask = 0xFFFF;
		KeyPress2 &= mask;

		if (KeyPress2==KeyMode) return;
	
	} while ( KeyPress2 != KeyEnter );
	
	
	UpdateLCDline1("Any key to exit");
	KeyPress2 = GetDebouncedKey();

}


void factor()
{
	unsigned int KeyPress2;
    char s[MaxLCDdigits + 1];
    char s2[MaxLCDdigits*2 + 1];

    unsigned long int i;
    unsigned long int n;
    int tmp;
    int factors=0;

    
    UpdateLCDline1("Type number:");
    Xreg = 0;
    tmp = OneLineNumberEntry();
    n = Xreg;
    Xreg = 0;

    if (n> (unsigned long int)(long int)(-1)) //checks if number > max long int
    {
        UpdateLCDline1("Number too large");
        UpdateLCDline2("Ent to continue");
        KeyPress2=wait();
        if (KeyPress2!=KeyMode || KeyPress2!=KeyEnter) return;
    }
    
    if (n<0) //checks for negative
    {
        UpdateLCDline1("Negative number");
        UpdateLCDline2("Ent to continue");
        KeyPress2=wait();
        if (KeyPress2!=KeyMode || KeyPress2!=KeyEnter) return;
    }
    
    if (n==1 || n==2)   //skips calculations if n=1 or 2
    {
        UpdateLCDline1("Prime number");
        UpdateLCDline2("Ent to continue");
        KeyPress2=wait();
        if (KeyPress2!=KeyMode || KeyPress2!=KeyEnter) return;
    }
    
    memset(s, '\0', sizeof(s));
	  while ( strlen(s) < MaxLCDdigits )
	               strcat( s, " " );
    *s2 = 0;



    static const unsigned char roller[][8] = 
        {
	
		{ 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00 },
		{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00 },
	};
	
	for ( i = 0; i < 4; i++ )
		custom_character( i+1, &( roller[i][0] ));
	

    int strobe = 0;
	unsigned int akey = 0;


	// sqrt moved outside loop so it's not calculated every time!

	unsigned long int sqr = sqrt( n );
    char fct[MaxLCDdigits];

    for( i=2; i <= sqr; i += 2 ) {

		if ( i == 4 ) i--;

       // allow an early abort, but only check periodically for speed purposes
       //if ( !(i % 100)) {

		    akey = KeyScan2( FALSE );

		    if ( akey == KeyClear )
			    break;

		    else if ( akey ) {
				s[15]='P';
				UpdateLCDline1(s);
			    while ( KeyScan2(FALSE ) );		// pause under key control
				s[ 15 ] = (strobe&3)+1;
				UpdateLCDline1(s);

		  }
	  //}


		unsigned long int fact = n / i;
        if ( fact * i == n ) {

          sprintf(fct, "%lu", i );

          if ( strlen( s2 ) + strlen( fct ) >= MaxLCDdigits ) {
              strcpy( s, s2 );
			  while ( strlen(s) < MaxLCDdigits )
                  strcat( s, " " );
              s[ 15 ] = (strobe&3)+1;
              UpdateLCDline1( s );
              *s2 = 0;
          }

	      strcat( s2, fct );

		  if ( fact != i ) {

	          sprintf(fct, " %lu", fact );
	
	          if ( strlen( s2 ) + strlen( fct ) >= MaxLCDdigits ) {
	              strcpy( s, s2 );
				  while ( strlen(s) < MaxLCDdigits )
    	              strcat( s, " " );
	              s[ 15 ] = (strobe&3)+1;
	              UpdateLCDline1( s );
	              *s2 = 0;
	          }
		      strcat( s2, fct );
	          factors++;
		  }
       
          if ( strlen( s2 ) < MaxLCDdigits-1 )
				strcat( s2, " " );

		  UpdateLCDline2( s2 );

          factors++;
       }


		if ( !( (i-1)%20 )) {
            s[ 15 ] = (strobe++&3)+1;
	        UpdateLCDline1( s );
		}

    }
    
	if ( akey != KeyClear ) {

	    if (factors == 0 )
	    {
	        UpdateLCDline1("Prime number");
	        UpdateLCDline2("Ent to continue");
	        KeyPress2=wait();
	        if (KeyPress2!=KeyMode || KeyPress2!=KeyEnter) return;
	
	    } else {
	
			s[ 15 ] = ' ';
			UpdateLCDline1( s );
		
			sprintf( fct, "= %d factors", factors );
		
			if ( strlen( s2 ) + strlen( fct ) >= MaxLCDdigits ) {
			    strcpy( s, s2 );
			    UpdateLCDline1( s );
			    *s2 = 0;
			}
		
			strcat( s2, fct );
		    UpdateLCDline2( s2 );
		}
	} else {

		UpdateLCDline1( s2 );
        UpdateLCDline2( "HALTED" );
	}

	// one should never wait for a key release at the end of stuff -- this causes delays in UI responsiveness
	// this release is automatically catered for when the next keypress is detected.
	KeyPress2=wait();

}


//***********************************
// The main tools mode routine
// Note that all variables are global
void ToolsMode(void)
{
    unsigned int KeyPress2;        //keypress variables
    char s[MaxLCDdigits + 1];
    int Mode;

    Mode= DriveMenu("TOOLS: +/- & ENT", ToolsMenu, DIM(ToolsMenu));

    switch(Mode)
    {
        case 0: //quadratic (Solve ax^2 + bx + c = 0 given B*B-4*A*C >= 0)
        {
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
               memset(s, '\0', sizeof(s)); //clear string
               sprintf(s2, "%f", root1);
               strcat(s, "x=");
               strcat(s, s2);
               UpdateLCDline1(s);
            
               memset(s, '\0', sizeof(s));
               strcat(s, "or x=");
               sprintf(s2, "%f", root2);
               strcat(s, s2);
               UpdateLCDline2(s);
               KeyPress2 = GetDebouncedKey();
               return;
            }
            
            else
            {
               UpdateLCDline1("Error:");
               UpdateLCDline2("Invalid Equation");
               KeyPress2 = GetDebouncedKey();
               return;
            }
        } break;

        case 1:   //factor
			factor();
			break;

        case 2:   //stopwatch
        {
			StopWatch();
		}
		break;
    }
}

