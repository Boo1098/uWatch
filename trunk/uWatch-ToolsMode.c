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
	rtccTime after;
	
	int startSeconds;
	int mask;
	int hours;
	int minutes;
	int seconds;
	int c;
	char s2[MaxLCDdigits + 1];


	
	void displayElasped( int displaySeconds )
	{

//   		after=Time;
//    	RtccReadTime(&Time);

//		if ( displayL && after.l != Time.l) {
//			seconds = BCDtoDEC(Time.f.hour) * 60 * 60 + BCDtoDEC(Time.f.min) * 60 + BCDtoDEC(Time.f.sec) - startSeconds;
			
			hours = (int) ( displaySeconds / ( 60 * 60 ));
			displaySeconds -= hours * 60 * 60;
			minutes = (int) ( displaySeconds / 60 );
			displaySeconds -= minutes * 60;
			
			sprintf( s, "        %02d:%02d:%02d", hours, minutes, displaySeconds );
			
			UpdateLCDline2(s);
		//}
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

	BOOL displayCurrent = TRUE;
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
        {
            int i;
            int num;
            int tmp;
            int factors=0;
            char s2[MaxLCDdigits + 1];
            
            UpdateLCDline1("Type number:");
            Xreg = 0;
            tmp = OneLineNumberEntry();
            num = Xreg;
            Xreg = 0;
            
            memset(s, '\0', sizeof(s));
            
            for(i=1;i<num;i++)
            {
               if(num%i==0)
               {
                  sprintf(s2, "%d", i);
                  strcat(s, s2);
                  strcat(s, " ");
                  factors++;
               }
            }
            
            if (factors!=0)
            {
              UpdateLCDline1("Factors:");
              UpdateLCDline2(s);
            }
            
            if (factors==0)
            {
              UpdateLCDline1("No Factors");
              UpdateLCDline2("Number is prime");
            }
            KeyPress2 = GetDebouncedKey();
        } break;
        case 2:   //stopwatch
        {
			StopWatch();
		}
		break;
    }
}

