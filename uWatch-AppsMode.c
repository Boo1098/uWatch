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
    "Setup",
};

extern void ToolsMode(void);
extern void GamesMode(void);
extern void SetupMode(void);

//***********************************
// The main apps mode routine
// Note that all variables are global
void AppsMode(void)
{
    int Mode= DriveMenu("APPS: +/- & ENT", AppsMenu, DIM(AppsMenu));

    switch(Mode)                
    {
    case 0:     //tools
        {
            ToolsMode();
        }
        break;
    case 1:     //games
        {
            GamesMode();
        }
        break;
    case 2:     // setup
        {
            SetupMode();
        }
        break;
    }
}

