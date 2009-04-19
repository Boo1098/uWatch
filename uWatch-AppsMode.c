//********************************************************
// uWatch
// Apps Mode Functions
// Version 1.5.X
// Last Update: 12th June 08
// Copyright(c) 2008 David L. Jones
// Written for the Microchip C30 Compiler
// Target Device: PIC24FJ64GA004 (44pin)
// http://www.calcwatch.com
// EMAIL: david@alternatezone.com

// NOTE: This code is designed to be inserted inline into the uWatch-Main.C function
//       Most variables are passed as globals and are defined in uWatch-Main.C
//********************************************************

//global variables for Apps Mode

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

static const char* AppsMenu[] = 
{
    "Tools",
    "Games",
    "Options",
};

extern int ToolsMode(void);
extern int GamesMode(void);
extern int SetupMode(void);

//***********************************
// The main apps mode routine
// Note that all variables are global
int AppsMode(void) {

    char *printApp( int *app, int max ) {
        return (char *) AppsMenu[ *app ];
    }

    int mode = 0;
    if ( genericMenu( "Applications:", &printApp, &increment, &decrement, DIM( AppsMenu ), &mode ) == MODE_KEYMODE )
        return MODE_KEYMODE;

    switch( mode ) {
       
        case 0:
            ToolsMode();
            break;
    
        case 1:
            GamesMode();
            break;
    
        case 2:
            SetupMode();
            break;
    }

    return MODE_EXIT;
}

