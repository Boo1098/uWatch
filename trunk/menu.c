
#include "menu.h"
#include "characterset.h"
#include "uWatch-LCD.h"
#include "def.h"


void increment( int *selection, int max )
{
    ( *selection )++;
    if (( *selection ) >= max )
        ( *selection ) = 0;
}

void decrement( int *selection, int max )
{
    ( *selection )--;
    if (( *selection ) < 0 )
        ( *selection ) = max - 1;
}


int calculatorMenu( const packedMenu *menu[], int size ) {

    int menuNum = 0;
    int mode = 0;

    while ( mode != MODE_EXIT && mode != MODE_KEYMODE ) {

        mode = genericMenu2( menu[ menuNum ], &menuNum );   // last param "anywhere"

        switch ( mode ) {
            case MODE_KEY_NEXT:
                increment( &menuNum, size );
                break;
            case MODE_KEY_PREVIOUS:
                decrement( &menuNum, size );
                break;
        }
    }
    return mode;
}


int genericMenu2( const packedMenu *menu, int *selection )
{
    int mode = MODE_EXIT;

    int sel = selection ? ( *selection ) : 0;

    // Draw the menu indicators
    custom_character( 0, character_arrow_updown );

    // Menu may itself define custom characters it requires.  Character #s are from 2-7
    int i;
    for ( i = 0; i < MENU_CUSTOM_CHAR_COUNT; i++ )
        if ( menu->customCharacter[i] )
            custom_character( i + 2, menu->customCharacter[i] );


    if ( menu->title && *menu->title)
        UpdateLCDline1( menu->title );


    int key;
    int mask = 0;

    do {

        if ( menu->print ) {

            if ( !menu->title ) {

                // we're drawing a FUNCTION KEY MENU -- aka calculator menu
                // and the OP values are associated with function keypresses.  Here we
                // draw BOTH LCD lines and we also highlight the current selection

                sprintf( out, "%s%s%s", menu->menu[0].name, menu->menu[1].name, menu->menu[2].name );
                UpdateLCDline1( out );
                sprintf( out, "%s%s%s", menu->menu[3].name, menu->menu[4].name, menu->menu[5].name );
                UpdateLCDline2( out );

            } else {

                char out2[17];
                sprintf( out2, "%-15s\010", ( *( menu->print ) )( &sel, (menuItem *)(menu->menu) ) );
                UpdateLCDline2( out2 );
            }
        }

        if ( menu->menu[sel].op < 0 ) { // NO DEBOUNCE? .. ???
            key = KeyScan2();
            if ( !key )
                mask = 0xFFFF;
            key &= mask;
        } else
            key = GetDebouncedKey();

        if ( key ) {
            IFEXIT( key );

            // Calculator-style menus need to cycle from menu to menu -- this is
            // done by using the MENU/RP keys, and passing appropriate values for each

            if ( !menu->title ) {
                if ( NEXT(key) )
                    return MODE_KEY_NEXT;
                if ( PREVIOUS( key ) )
                    return MODE_KEY_PREVIOUS;
    
                // auto-selection only available in calculator mode   

                int autoSel = ReturnNumber( key );
                if ( autoSel >= 0 && autoSel <= 9 ) {
    
                    // scan menu items to find equivalent function for key
                    // This is how function keys work in calculator mode -- finding the equivalent # in the op in the menu
    
                    static const int fkey[] = { -1,-1,-1,-1, 3, 4, 5, 0, 1, 2 };
                    if ( !menu->title ) {
                        if ( fkey[autoSel]>=0 ) {
                            sel = fkey[autoSel];
                            if ( menu->menu[sel].op != CALC_OP_NULL )
                                break;
                        } 
                    }
                }
            }    
    
            if ( menu->inc && NEXT( key ) ) {
                ( *( menu->inc ) )( &sel, menu->max );
                mask = 0;
            }    
    
            if ( menu->dec && PREVIOUS( key ) ) {
                ( *( menu->dec ) )( &sel, menu->max );
                mask = 0;
            }
        }

    } while ( !ENTER(key) );


    if ( selection )
        ( *selection ) = sel;

    if ( menu->menu[sel].run )
        mode = ( menu->menu[sel].run )( menu->menu[sel].op );

    return mode;
}



int genericMenu( char *title,
                 char *( *printFunc )( int *num, int max ),
//                 int ( *idleFunc )( int *halt ),
                 void ( *incrementFunc )( int *num, int max ),
                 void ( *decrementFunc )( int *num, int max ),
                 int max, int *selection )
{

    int sel = selection ? ( *selection ) : 0;

    custom_character( 0, character_arrow_updown );
//    custom_character( 1, right_menu );


    if ( title )
        UpdateLCDline1( title );

    int key = 0;
    do {

        if ( printFunc ) {
            sprintf( displayBuffer, "%-15s\010", ( *printFunc )( &sel, max ));
            UpdateLCDline2( displayBuffer );
        }

        key = GetDebouncedKey();
        IFEXIT( key );

/*

        // we DON'T have autokey here anymore, as it causes problems in time selection

        int autoSel = ReturnNumber( key );
        if ( autoSel >= 0 && autoSel < max ) {
            sel = autoSel;
            break;
        }
*/
        if ( incrementFunc && NEXT( key ) )
            ( *incrementFunc )( &sel, max );

        if ( decrementFunc && PREVIOUS( key ) )
            ( *decrementFunc )( &sel, max );

    } while ( !ENTER(key) );


    if ( selection )
        ( *selection ) = sel;

    return MODE_EXIT;

}



int viewString( char *title, char *string,
                 int *selection, int viewControl )
{

// Values of viewControl...
//  VIEW_ONESHOT causes display of the string and immediate exit
//  VIEW_AUTOSCROLL keypress auto-repeats, giving auto-scroll.  Without this, user has
//  to continuously press the key to scroll.


    int max = strlen( string );;

    if ( title )
        UpdateLCDline1( title );

    int sel = 0;
    if ( selection ) sel = *selection;

    custom_character( 0, character_left_menu );
    custom_character( 1, character_right_menu );

    while ( KeyScan2() );            // wait for key release so no ENTER auto-press!


    int key = 0;
    do {

        char t1[18];
        char t2[17];
        char t3[17];

        *t2 = 0;
        *t3 = 0;

        strncpy( t1, &(string[sel]), 17 );
        t1[17] = 0;

        char *s = t1;

        int available = 16;
        if ( sel ) {
            available--;
            strcpy( t2, "\010" );
        }

        int slen = strlen(s);
        if ( slen > available ) {
            available--;
            strcpy( t3, "\1" );
        }

        strncat( t2, s, available );

        int desiredlen = 16 - strlen( t3);
        while ( strlen( t2 ) < desiredlen )
            strcat( t2, " " );



        sprintf( out, "%s%s", t2, t3 );

        UpdateLCDline2( out );

        if ( viewControl == VIEW_ONESHOT )
            break;


        if ( viewControl == VIEW_AUTOSCROLL ) {

            if ( key )
                DelayMs(350);
            key = KeyScan2();


        } else
            key = GetDebouncedKey();

        IFEXIT( key );

        if ( NEXT( key ) ) {
            if ( !sel ) sel++;      // caters for arrow appearing
            sel++;
            if ( sel >= max )
                sel = max-1;
        }

        if ( PREVIOUS( key ) ) {
            if ( sel == 2 )
                sel--;
            sel--;
            if ( sel <= 0 )
                sel = 0;
        }    

    } while ( !ENTER(key) );


    if ( selection )
        ( *selection ) = sel;

    return MODE_EXIT;

}


char *printMenu( int *item, menuItem *menu )
{
    return (char *) menu[*item].name;
}
