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
    "Stopwatch",
};

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
        case 0: //quadratic (Solve  ax^2 + bx + c = 0 given B*B-4*A*C >= 0)
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
               do KeyPress2=KeyScan(); while(KeyPress2==0);
               return;
            }
            
            else
            {
               UpdateLCDline1("Error:");
               UpdateLCDline2("Invalid Equation");
               do KeyPress2=KeyScan(); while(KeyPress2==0);
               return;
            }
        } break;
        
        case 1:   //stopwatch
        {
            rtccTime before;
            rtccTime after;
            int hours;
            int minutes;
            int seconds;
            int c;
            char s2[MaxLCDdigits + 1];
            
            void displayElasped(void)
            {
               memset(s, ' ', MaxLCDdigits);
               after=Time;
               RtccReadTime(&Time);
               if (after.l == Time.l)
               {
                  // no need to update
                  return;
               }
               hours=BCDtoDEC(after.f.hour)-BCDtoDEC(before.f.hour);
               minutes=BCDtoDEC(after.f.min)-BCDtoDEC(before.f.min);
               seconds=BCDtoDEC(after.f.sec)-BCDtoDEC(before.f.sec);
               if (hours!=0)
               {
                  if (hours<10) strcat(s, "0");
                  sprintf(s2, "%d", hours);
                  strcat(s, s2);
                  strcat(s, ":");
               } else strcat(s, "00:");
            
               if (minutes!=0)
               {
                  if (minutes<10) strcat(s, "0");
                  sprintf(s2, "%d", minutes);
                  strcat(s, s2);
                  strcat(s, ":");
               } else strcat(s, "00:");
               
               if (seconds!=0)
               {
                  if (seconds<10) strcat(s, "0");
                  sprintf(s2, "%d", seconds);
                  strcat(s, s2);
               } else strcat(s, "00");
               
               UpdateLCDline2(s);
               memset(s, '\0', sizeof(s));
            }
            
            UpdateLCDline1("Stopwatch");
            UpdateLCDline2("ENT to start");
            do KeyPress2=KeyScan(); while(KeyPress2==0);
            if (KeyPress2==KeyMode) return;
            RtccReadTime(&before);
            UpdateLCDline1("ENT to stop");
            
            while (TRUE)
            {
               displayElasped();
               for(c=0;c<50;c++)
               {
                  KeyPress2=KeyScan();
                  if (KeyPress2!=KeyMode || KeyPress2!=KeyEnter) break;
               }
               
               if (KeyPress2==KeyMode) return;
               if (KeyPress2==KeyEnter) break;
            }
            
            UpdateLCDline1("ENT to exit");
            do KeyPress2=KeyScan(); while(KeyPress2==0);
        } break;
    }
}

