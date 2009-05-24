
//********************************************************
// uWatch
// Games Mode Functions
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

#include "def.h"
#include "menu.h"
#include "uWatch-vchess.h"
#include "uWatch-LCD.h"

#if LUNAR_LANDER

int height;
int velocity;
int fuelRemaining;
int elapsedTime;

char *printStat( int *n, int max ) {
    switch (*n ) {
    case 3:
        sprintf( out, "Time %i s", elapsedTime );
        break;
    case 0:
        sprintf( out, "Height %i m", height );
        break;
    case 1:
        sprintf( out, "Speed %d m/s", velocity );
        break;
    case 2:
        sprintf( out, "Fuel %d", fuelRemaining );
        break;
    }
    return out;
}


int lunarLander( int p )
{


//    UpdateLCDline1( "--LUNAR LANDER--" );
//    UpdateLCDline2( "    By zowki" );
//    GetDebouncedKey();


    elapsedTime   = 0;
    height        = 1000;
    velocity      = 50;
    fuelRemaining = 150;

    int burnAmount  = 0;
    int newVelocity = 0;
    int delta = 0;


    while ( height > 0 ) {

        if ( burnAmount > fuelRemaining )
            burnAmount = fuelRemaining;

        if ( fuelRemaining > 0 ) {

            int sel = 0;
            if ( genericMenu( "Status", printStat, increment, decrement, 4, &sel ) == MODE_KEYMODE )
                return MODE_KEYMODE;


            if ( genericMenu( "Burn fuel?", printNumber, increment, decrement, fuelRemaining+1, &burnAmount ) == MODE_KEYMODE )
               return MODE_KEYMODE;
        }

        newVelocity   = velocity - burnAmount + 5;
        fuelRemaining = fuelRemaining - burnAmount;
        height        = height - ( velocity + newVelocity ) * 0.5;
        elapsedTime++;
        velocity      = newVelocity;

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
    sprintf( out, "Time %i s", elapsedTime + delta );
    UpdateLCDline2( out );

    int KeyPress2 = GetDebouncedKey();
    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;

    sprintf( out, "Speed %i m/s", newVelocity );
    UpdateLCDline1( out );
    sprintf( out, "Fuel %i", fuelRemaining );
    UpdateLCDline2( out );

    KeyPress2 = GetDebouncedKey();
    if ( KeyPress2 == KeyMode ) return MODE_KEYMODE;

    UpdateLCDline1( "Result:" );
    if ( newVelocity <= 0 ) {
        UpdateLCDline2( "Perfect!" );
    } else if ( newVelocity < 2 ) {
        UpdateLCDline2( "Bumpy!" );
    } else {
        UpdateLCDline1( "You crashed!" );
    }

//    UpdateLCDline2( "ENT to cont." );
//    Xreg = 0;
    GetDebouncedKey();

    return MODE_EXIT;
}

#endif


const unsigned char character_heart[] =     { 0x00, 0x0A, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00 };
const unsigned char character_spade[] =     { 0x04, 0x04, 0x0E, 0x1F, 0x1F, 0x1B, 0x04, 0x0E };
const unsigned char character_diamond[] =   { 0x00, 0x04, 0x0E, 0x1F, 0x1F, 0x0E, 0x04, 0x00 };
const unsigned char character_club[] =      { 0x0E, 0x0E, 0x04, 0x1F, 0x1F, 0x1B, 0x04, 0x0E };

void drawCard( char *dest, int card ) {
    char *c = "A234567890JQK";
    if (( card>>2) == 9 )   // a 10?
        *dest++ = '1';
    int suit = ( card & 3 ) + 2;
    *dest++ = c[card>>2];
    *dest++ = suit|8;
    *dest = 0;
}

void shuffle( int *deck ) {
    int card, card2;
    for ( card = 0; card < 52; card++ ) {
        card2 = rand32() % 52;
        int temp = deck[card];
        deck[card] = deck[card2];
        deck[card2] = temp;
    }
}

int valueof( int card ) 
{
    int val = card >> 2;
    if (!val) val = 11; // ACE
    else
    {
        ++val; // to value
        if (val > 10) val = 10; // max 10
    }
    return val;
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
    for ( i = 0; i < count; i++ )
        drawCard( dest + strlen( dest ), hand[i] );

    int total = countTotal( hand, count );
    sprintf( dest + strlen(dest), " =%d", total );
    return total;
}    

char *printHitStand(int *n, int max ) {
    if (*n)
        return "Stand";
    else
        return "Hit!";
}


static void shuffling(int* deck)
{
    shuffle( deck );
    UpdateLCDline2( "Shuffling..." );
    DelayMs(1000);
}

void blackjack() {
    UpdateLCDline2( "Blackjack!" );
    DelayMs( 3000 );
}

int twenty1( int p )
{
#if 1

    custom_character( 2, character_heart );
    custom_character( 3, character_spade );
    custom_character( 4, character_diamond );
    custom_character( 5, character_club );

    int deck[52];
    int player[20];
    int dealer[20];
    
    int card;
    for ( card = 0; card < 52; card++ ) deck[card] = card;

    int key;
    while ( TRUE ) {

        key = 0;

        // Shuffling is done here for two reasons...
        // 1) this will do an initial shuffle
        // 2) this will do a mid-game shuffle, but NEVER shuffle while hands are in play
        // 3) note we don't let the deck 'run out' before shuffling.  important!!

        if ( card > 40 ) {            // it WILL be on 1st time through!
            shuffling( deck );
            card = 0;
        }    

        UpdateLCDline2( "" );

        int cardn = 0;
        int total = 0;
        int hitStand = 0;

        while ( total < 21 && !hitStand ) {

            player[cardn++] = deck[card++];

            total = drawHand( "", displayBuffer, player, cardn );
            UpdateLCDline1( displayBuffer );

            if ( cardn < 2 ) {
                DelayMs(2000);
            } else if ( total < 21 ) { // wont hit on 21!

                if ( genericMenu( displayBuffer, &printHitStand, &increment, &decrement, 2, &hitStand ) == MODE_KEYMODE )
                    return MODE_KEYMODE;
            }
        }

        
        if ( total > 21 ) {
            UpdateLCDline2( "Bust!" );
        }
        else {

            if ( total == 21 && cardn == 2 )
                blackjack();

            char *dout = displayBuffer + strlen(displayBuffer) + 20;
            int dealertotal = 0;
            int dcardn = 0;


/*

The dealer must play his hand in a specific way, with no choices allowed. There are two popular rule variations that
determine what totals the dealer must draw to. In any given casino, you can tell which rule is in effect by looking
at the blackjack tabletop. It should be clearly labeled with one of these rules: 

"Dealer stands on all 17s": This is the most common rule. In this case, the dealer must continue to take cards ("hit")
until his total is 17 or greater. An Ace in the dealer's hand is always counted as 11 if possible without the dealer
going over 21. For example, (Ace,8) would be 19 and the dealer would stop drawing cards ("stand"). Also, (Ace,6) is 17
and again the dealer will stand. (Ace,5) is only 16, so the dealer would hit. He will continue to draw cards until the
hand's value is 17 or more. For example, (Ace,5,7) is only 13 so he hits again. (Ace,5,7,5) makes 18 so he would stop
("stand") at that point.

"Dealer hits soft 17": Some casinos use this rule variation instead. This rule is identical except for what happens
when the dealer has a soft total of 17. Hands such as (Ace,6), (Ace,5,Ace), and (Ace, 2, 4) are all examples of soft
17. The dealer hits these hands, and stands on soft 18 or higher, or hard 17 or higher. When this rule is used, the
house advantage against the players is slightly increased. 

Again, the dealer has no choices to make in the play of his hand. He cannot split pairs, but must instead simply hit
until he reaches at least 17 or busts by going over 21. 

*/

            while ( dealertotal < 17 ) {  // dealer hits while under 17.  DO NOT CHANGE THIS WITHOUT DISCUSSION!!!!
        
                dealer[dcardn++] = deck[card++];
                
                dealertotal = drawHand( "D: ", dout, dealer, dcardn  );
                UpdateLCDline2( dout );
                DelayMs(2500);
            }

            if ( dealertotal == 21 ) {
                if ( dcardn == 2 ) {
                    blackjack();
                    if ( total == 21 && cardn > 2 )
                        total--;                        // dealer wins with 21 blackjack vs 21 without
                } else {
                    if ( total == 21 && cardn == 2 )   // player blackjack
                        dealertotal--;                 // player wins if 21 with blackjack vs 21 without
                }
            }


            char *result = "Dealer Wins!";
            if ( dealertotal > 21 ) {
                result = "Dealer Bust!";
            } else if ( dealertotal == total ) {
                result = "Push!";
            } else if ( dealertotal < total ) {
                result = "You Win!";
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

        const menuItem gameMenu[] = {
            { "Chess",  chessGame, 0 },
            { "21", twenty1, 0 },
#if LUNAR_LANDER
            { "Lunar Lander", lunarLander, 0 },
#endif
        };

        const packedMenu2 gamesMenu = {
            "Game",
            printMenu,
            0, 0,
#if LUNAR_LANDER
            3,
#else
            2,
#endif
            gameMenu
        };


        return genericMenu2( &gamesMenu, 0 );
    }
