//********************************************************
// uWatch
// Tools Mode Functions
// Version 1.6
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





int quadratic( int p ) {

    //quadratic (Solve ax^2 + bx + c = 0 given B*B-4*A*C >= 0)

    float a,b,c;
    float d;
    float root1,root2;
    
    custom_character( 2, character_squaring );
    UpdateLCDline1( "a x\2?" );

    Xreg = 0;

    IFEXIT( OneLineNumberEntry() );

    a = Xreg;

    UpdateLCDline1( "b x?" );
    Xreg = 0;

    IFEXIT( OneLineNumberEntry() );

    b = Xreg;
    UpdateLCDline1( "c?" );
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
        strcat( displayBuffer, ": no roots." );
    else {
    
        d = b*b - 4*a*c;
    
        if ( d < 0 )
            strcat( displayBuffer, ": invalid (b\2-4ac < 0)." );
        else {
    
            d  = sqrt(d);   //compute the square root of discriminant d
            root1 = (-b + d)/(2.0*a);   //first root
            root2 = (-b - d)/(2.0*a);   //second root
    
            char buffer[64];
            sprintf( buffer, ": roots %f, %f.", root1, root2 );
            strcat( displayBuffer, buffer );
        }
    }
    
    return viewString( "Equation", displayBuffer, 0, VIEW_AUTOSCROLL );
    }
    


//***********************************
// The main tools mode routine

int ToolsMode( int p )
{
    const menuItem toolsMenuMenu[] = {
        { "Quadratic",  quadratic, 0 },
        { "Factorise", factor, 0 },
    };

    const packedMenu2 toolsMenu = {
        "Apps",
        printMenu,
        0, 0, 2, toolsMenuMenu
    };

    return genericMenu2( &toolsMenu, 0 );
}


