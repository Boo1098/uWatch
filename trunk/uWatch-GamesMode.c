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
            UpdateLCDline1( "ENT - burn fuel" );
            UpdateLCDline2( "MENU - stats" );
            KeyPress2 = GetDebouncedKey();
            if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;
            if ( ENTER(KeyPress2) ) {  //enter fuel to burn
                UpdateLCDline1( "Burn fuel(0-30):" );
                Xreg = burnAmount;
                c = OneLineNumberEntry();
                burnAmount = Xreg;
            }
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



const unsigned char character_heart[] = { 0x0A, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00 };
const unsigned char character_spade[] = { 0x04, 0x0E, 0x1F, 0x1F, 0x0A, 0x04, 0x04, 0 };
const unsigned char character_diamond[] = { 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04, 0x00, 0x00 };
const unsigned char character_club[] = { 0x0E, 0x0E, 0x04, 0x1F, 0x1B, 0x04, 0x04, 0x00 };

void drawCard( char *dest, int card ) {
    char *c[] = { "A","2","3","4","5","6","7","8","9","10","J","Q","K"  };
    int suit = ( card % 4 ) + 2;
    int val = (int) (card / 4);
    sprintf( dest, "%s%c", c[val], suit|8 ); 
}

void shuffle( int *deck ) {
    Clock1MHz();
    int card, card2, shuf;
    for ( shuf = 0; shuf < 1000; shuf++ ) {
        card = rand() % 52;
        card2 = rand() % 52;
        int temp = deck[card];
        deck[card] = deck[card2];
        deck[card2] = temp;
    }
    Clock250KHz();
}


int valueof( int card ) {
    int val = card / 4;
    switch (val ) {
        case 0:
            return 11;
        case 10:
        case 11:
        case 12:
        case 13:
            return 10;
    }
    return val+1;
}


int countTotal( int *hand, int cards ) {
    // find HIGHEST total < 22
    // aces count as 1 or 11

    int total = 0;
    int i;
    for ( i = 0; i < cards; i++ ) {
        total += valueof( hand[i] );        
    }

    // change any/all aces down to value of 1, reducing total
    i = 0;
    while ( i < cards && total > 21 ) {
        if ( valueof( hand[i]) == 11 )  // an ace?
            total -= 10;
        i++;
    }

    return total;
}

int drawHand( char *prefix, char *dest, int *hand, int count ) {

    strcpy( dest, prefix );
    int i;
    for ( i = 0; i < count; i++ ) {
        drawCard( dest + strlen( dest ), hand[i] );
        //if ( i == count-2 )
        //    strcat( dest, " " );
    }    
    int total = countTotal( hand, count );
    sprintf( dest + strlen(dest), " =%2d", total );
    return total;
}    

char *printHitStand(int *n, int max ) {
    if (*n)
        return "Stand";
    else
        return "Hit!";
}

void seedRandom(void) {
    int seed;
    
    RtccReadTime( &Time );
    RtccReadDate( &Date );
    
    seed = BCDtoDEC( Time.f.sec ) + BCDtoDEC( Time.f.min ) + BCDtoDEC( Time.f.hour ) + BCDtoDEC( Date.f.year ) + BCDtoDEC( Date.f.mon ) + BCDtoDEC( Date.f.mday );
    
    srand( seed );
}

int twenty1( int p )
{
#if 1


    custom_character( 2, character_heart );
    custom_character( 3, character_spade );
    custom_character( 4, character_diamond );
    custom_character( 5, character_club );



    int deck[52];
    int card;
    for ( card = 0; card < 52; card++ )
        deck[card] = card;


    int player[20];
    int dealer[20];

    int key;
    while ( TRUE ) {
    
        seedRandom();   //seed random number using time and date

        key = 0;

        shuffle( deck );

        UpdateLCDline2( "" );

        card = 0;
        int cardn = 0;
        int total = 0;
        while ( total < 22 ) {
    
            player[cardn++] = deck[card++];
    
            total = drawHand( "", displayBuffer, player, cardn );
            UpdateLCDline1( displayBuffer );

            if ( card < 2 ) {
                DelayMs(2000);
            } else if ( total < 22 ) {

                int hitStand = 0;
                if ( genericMenu( displayBuffer, &printHitStand, &increment, &decrement, 2, &hitStand ) == MODE_KEYMODE )
                    return MODE_KEYMODE;
    
                if ( hitStand == 1 )     // stand
                    break;
            }
        }

        if ( total > 21 ) {
            //UpdateLCDline1( displayBuffer + strlen(displayBuffer) - 16 );
            UpdateLCDline2( "BUST!" );
        }
        else {

            char *dout = displayBuffer + strlen(displayBuffer) + 20;
    
            int dealertotal = 0;
            cardn = 0;
            while ( dealertotal < 17 ) {
        
                dealer[cardn++] = deck[card++];
        
                dealertotal = drawHand( "D: ", dout, dealer, cardn  );
                UpdateLCDline2( dout );
        
                DelayMs(2500);
        
            }

            char *result = "DEALER WINS!";
            if ( dealertotal > 21 ) {
                result = "DEALER BUST!";
            } else if ( dealertotal == total ) {
                result = "PUSH!";
            } else if ( dealertotal < total ) {
                result = "YOU WIN!";
            }

            int phase = 0;
            while ( ! (key = KeyScan2( FALSE ))) {
                phase++;
                if ( phase & 0x100 )
                    UpdateLCDline2( dout );
                else
                    UpdateLCDline2( result );
            }
            while ( KeyScan2(FALSE ));

        }
    
        if ( !key )
            key = GetDebouncedKey();
        IFEXIT( key );
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
            {},
            {   { "Chess",  chessGame, 0 },
                { "21", twenty1, 0 },
                { "Lunar Lander", lunarLander, 0 },
            },
        };

        return genericMenu2( &gamesMenu, 0 );
    }
