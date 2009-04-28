//********************************************************
// uWatch
// Games Mode Functions
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

//global variables for Games Mode

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

/*
static unsigned int GetDebouncedKey()
{
    unsigned int KeyPress2;        //keypress variables
    KeyPress2 = GetDebouncedKey();
    return KeyPress2;
}
*/

#include "def.h"
#include "menu.h"
#include "uWatch-vchess.h"
#include "uWatch-LCD.h"



int lunarLander( int p )
{
#if 1
    int KeyPress2;

    UpdateLCDline1( "--LUNAR LANDAR--" );
    UpdateLCDline2( "    By zowki    " );

    if ( GetDebouncedKey() == KeyMode )
        return MODE_KEYMODE;

    int elapsedTime   = 0;
    int height        = 1000;
    int velocity      = 50;
    int fuelRemaining = 150;
    int c;

    int burnAmount  = 0;
    int newVelocity = 0;
    int delta = 0;

    while ( height > 0 ) {
        if ( fuelRemaining > 0 ) {
lunar:
            UpdateLCDline1( "ENT - burn fuel" );
            UpdateLCDline2( "MENU - stats" );
            KeyPress2 = GetDebouncedKey();
            if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
            if ( KeyPress2 == KeyMenu ) { //stats menu
                UpdateLCDline1( "1=Time  2=Height" );
                UpdateLCDline2( "3=Speed 4=Fuel" );
                KeyPress2 = GetDebouncedKey();
                if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
                if ( KeyPress2 == Key1 ) {
                    UpdateLCDline1( "Time (seconds):" );
                    sprintf( out, "%i", elapsedTime );
                    UpdateLCDline2( out );
                    KeyPress2 = GetDebouncedKey();
                    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
                }
                if ( KeyPress2 == Key2 ) {
                    UpdateLCDline1( "Height (feet):" );
                    sprintf( out, "%i", height );
                    UpdateLCDline2( out );
                    KeyPress2 = GetDebouncedKey();
                    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
                }
                if ( KeyPress2 == Key3 ) {
                    UpdateLCDline1( "Speed (feet/s):" );
                    sprintf( out, "%i", velocity );
                    UpdateLCDline2( out );
                    KeyPress2 = GetDebouncedKey();
                    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
                }
                if ( KeyPress2 == Key4 ) {
                    UpdateLCDline1( "Fuel:" );
                    sprintf( out, "%i", fuelRemaining );
                    UpdateLCDline2( out );
                    KeyPress2 = GetDebouncedKey();
                    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
                }
                goto lunar;
            }
            if ( ENTER(KeyPress2) ) {  //enter fuel to burn
                UpdateLCDline1( "Burn fuel(0-30):" );
                Xreg = burnAmount;
                c = OneLineNumberEntry();
                burnAmount = Xreg;
            }
        }

        if ( burnAmount < 0 ) burnAmount = 0;
        if ( burnAmount > 30 )    burnAmount = 30;
        if ( burnAmount > fuelRemaining ) burnAmount = fuelRemaining;

        newVelocity   = velocity - burnAmount + 5;
        fuelRemaining = fuelRemaining - burnAmount;
        height        = height - ( velocity + newVelocity ) * 0.5;
        elapsedTime   = elapsedTime + 1;
        velocity      = newVelocity;

        sprintf( out, "%i", burnAmount );
        strcat( out, " fuel burnt" );
        UpdateLCDline1( out );
        sprintf( out, "%i", fuelRemaining );
        strcat( out, " fuel left" );
        UpdateLCDline2( out );
        KeyPress2 = GetDebouncedKey();
        if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
    }

    /* Touchdown. Calculate landing parameters. */
    elapsedTime = elapsedTime - 1;
    velocity = velocity - 5 + burnAmount;
    height = height + ( velocity + newVelocity ) * 0.5;

    if ( burnAmount == 5 )
        delta = height / velocity;
    else
        delta = ( sqrt( velocity * velocity + height * ( 10 - burnAmount * 2 ) ) - velocity ) / ( 5 - burnAmount );
    newVelocity = velocity + ( 5 - burnAmount ) * delta;

    UpdateLCDline1( "Touchdown!" );
    UpdateLCDline2( "ENT to see stats" );
    KeyPress2 = GetDebouncedKey();
    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;

    UpdateLCDline1( "Time taken(sec):" );
    sprintf( out, "%i", elapsedTime + delta );
    UpdateLCDline2( out );
    KeyPress2 = GetDebouncedKey();
    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;

    UpdateLCDline1( "Speed (feet/s):" );
    sprintf( out, "%i", newVelocity );
    UpdateLCDline2( out );
    KeyPress2 = GetDebouncedKey();
    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;

    UpdateLCDline1( "Fuel left:" );
    sprintf( out, "%i", fuelRemaining );
    UpdateLCDline2( out );
    KeyPress2 = GetDebouncedKey();
    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;

    if ( newVelocity <= 0 ) {
        UpdateLCDline1( "Perfect landing!" );
        UpdateLCDline2( "ENT to cont." );
        Xreg = 0;
        GetDebouncedKey();
        return MODE_EXIT;
    } else if ( newVelocity < 2 ) {
        UpdateLCDline1( "Bumpy landing." );
        UpdateLCDline2( "ENT to cont." );
        Xreg = 0;
        GetDebouncedKey();
        return MODE_EXIT;
    } else {
        UpdateLCDline1( "You crashed!" );
        UpdateLCDline2( "ENT to cont." );
        Xreg = 0;
        GetDebouncedKey();
        return MODE_EXIT;
    }
#endif

    return MODE_EXIT;
}






int twenty1( int p )
{
#if 1

    //char s2[MaxLCDdigits+1];
    int player_total = 0;
    int dealer_total = 0;
    int done = 0;
    int dealerBust = 0;
    int KeyPress2;


void bust_player( void )
{
    done = 1;
    sprintf( out, "You:%d Dealer:%d", player_total, dealer_total );
    UpdateLCDline1( out );
    UpdateLCDline2( "Player bust!" );
    GetDebouncedKey();
}

void bust_dealer( void )
{
    done = 1;
    sprintf( out, "You:%d Dealer:%d", player_total, dealer_total );
    UpdateLCDline1( out );
    UpdateLCDline2( "Dealer bust!" );
    GetDebouncedKey();
}

void hit_player( void )
{
    player_total += rand() % 10 + 1;
    if ( player_total > 21 ) bust_player();
}

void hit_dealer( void )
{
    dealer_total += rand() % 10 + 1;
    if ( dealer_total > 21 ) {
        dealerBust = 1;
        bust_dealer();
    }
}

void prepare( void )
{
    player_total = 0;
    dealer_total = 0; //reset card totals
    hit_player();
    hit_player();
    hit_dealer();
    hit_dealer();  //hit player and dealer twice for initial cards
    done = 0;
}

void stand( void )
{
    while ( dealer_total < 17 ) hit_dealer(); //The dealer's play, always hits when lower than 17 (vegas rules)
    done = 1;

    sprintf( out, "You:%d Dealer:%d", player_total, dealer_total );
    UpdateLCDline1( out );
    if ( player_total > dealer_total )
        UpdateLCDline2( "Player wins!" );
    else if ( player_total == dealer_total )
        UpdateLCDline2( "Push!" );
    else if ( player_total < dealer_total || dealerBust == 0 )     // must be else, otherwise will call wrong winner
        UpdateLCDline2( "Dealer wins!" );
    GetDebouncedKey();
}


    const unsigned char character_heart[] = { 0x0A, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00 };
    const unsigned char character_spade[] = { 0x04, 0x04, 0x0E, 0x1F, 0x1F, 0x0A, 0x04, 0x0E };
    const unsigned char character_diamond[] = { 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04, 0x00, 0x00 };
    const unsigned char character_club[] = { 0x0E, 0x0E, 0x04, 0x1F, 0x1B, 0x04, 0x04, 0x00 };

    custom_character( 0, character_heart );
    custom_character( 1, character_spade );
    custom_character( 2, character_diamond );
    custom_character( 3, character_club );


    UpdateLCDline1( "5\010 Q\1 J\2 2\3 7\1" );
    UpdateLCDline2( "    By zowki    " );
    KeyPress2 = GetDebouncedKey();
    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;


    prepare();

    while ( done == 0 ) {
        sprintf( out, "You:%d Dealer:?", player_total );
        UpdateLCDline1( out );
        UpdateLCDline2( "Hit=1 Stand=2" );
        KeyPress2 = GetDebouncedKey();
        if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
        if ( KeyPress2 == Key1 ) hit_player();
        if ( KeyPress2 == Key2 ) stand();
        if ( done == 1 ) {
            UpdateLCDline1( "Deal again?" );
            UpdateLCDline2( "Yes=1 No=2" );
            KeyPress2 = GetDebouncedKey();
            if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
            if ( KeyPress2 == Key1 ) prepare();
            if ( KeyPress2 == Key2 ) break;
        }
    }
#endif
    return MODE_EXIT;
    }

//***********************************
// The main games mode routine

    int GamesMode( int p ) {
        const packedMenu gamesMenu = {
            "Game",
            printMenu,
            increment, decrement, 3,
            {   0,
                0,
                0,
                0,
            },
            {   { "Chess",  chessGame, 0 },
                { "21", twenty1, 0 },
                { "Lunar Lander", lunarLander, 0 },
            },
        };

        return genericMenu2( &gamesMenu, 0 );
    }
