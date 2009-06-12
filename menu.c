

#include "menu.h"
#include "characterset.h"
#include "uWatch-LCD.h"
#include "uWatch-op.h"

#include "def.h"


void increment( int *selection, int max ) {
    if ( ++( *selection ) >= max )
        ( *selection ) = 0;
}


void decrement( int *selection, int max ) {
    if ( --( *selection ) < 0 )
        ( *selection ) = max - 1;
}


void calculatorMenu( const packedMenu2 *menu[], int size ) {

    int mode = 0;

    while ( mode != MODE_EXIT && mode != MODE_KEYMODE ) {

        mode = genericMenu2( menu[ CurrentMenu ] );

        switch ( mode ) {
            case MODE_KEY_NEXT:
                increment( &CurrentMenu, size );
                break;
            case MODE_KEY_PREVIOUS:
                decrement( &CurrentMenu, size );
                break;
        }
    }
}


int genericMenu2( const packedMenu2 *menu )
{
    int mode = 0;
    int sel = 0;

    // Draw the menu indicators
    custom_character( 0, character_arrow_updown );

    // Menu may itself define custom characters it requires.  Character #s are from 4-7

    int i;
    for ( i = 0; i < menu->csetsize; i++ )
        custom_character( i + 3, (menu->charset)[i] );

    if ( menu->title && *menu->title )
        UpdateLCDline1( menu->title );

    int key;
    extern unsigned int mask;
    mask = 0;

    const menuItem *pmenu = menu->menu;
    do {

        do {
            if ( menu->print ) {
                if ( !menu->title ) {
    
                    // we're drawing a FUNCTION KEY MENU -- aka calculator menu
                    // and the OP values are associated with function keypresses.  Here we
                    // draw BOTH LCD lines and we also highlight the current selection
    
                    sprintf( out, "%s%s%s", pmenu[0].name, pmenu[1].name, pmenu[2].name );
                    char *s = pmenu[0].name;
                    if ( s[1] == 's' && s[2] == 'i' &&  DegreesMode )
                        *out = '\377';
                    UpdateLCDline1( out );
                    sprintf( out, "%s%s%s", pmenu[3].name, pmenu[4].name, pmenu[5].name );
                    UpdateLCDline2( out );
    
                } else {
                    char out2[17];
                    sprintf( out2, "%-15s\010", ( *( menu->print ) )( &sel, pmenu ) );
                    UpdateLCDline2( out2 );
                }
            }
    
            if ( pmenu[sel].op < 0 ) { // NO DEBOUNCE? .. USED IN STOPWATCH MODE
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
                    if ( autoSel >= 0 ) {
        
                        // scan menu items to find equivalent function for key
                        // This is how function keys work in calculator mode -- finding the equivalent # in the op in the menu
        
                        static const int fkey[] = { -1,-1,-1,-1, 3, 4, 5, 0, 1, 2 };
                        //if ( !menu->title ) {
                            if ( fkey[autoSel]>=0 ) {
                                sel = fkey[autoSel];
                                if ( pmenu[sel].op != CALC_OP_NULL )
                                    break;
                            } 
                        //}
                    }
                }
    
                if ( NEXT( key ) ) {
                    increment( &sel, menu->menusize );
                    mask = 0;
                }    
        
                if ( PREVIOUS( key ) ) {
                    decrement( &sel, menu->menusize );
                    mask = 0;
                }
            }
    
        } while ( !ENTER(key) );
    
    
    //    if ( selection )
    //        ( *selection ) = sel;
    
        if ( pmenu[sel].run )
            mode = ( pmenu[sel].run )( pmenu[sel].op );

    } while ( !mode );


    return mode;
}


void showEntry( char *title, char *qkey, char c ) {
    sprintf( out, "%s %s%c", title, qkey, c );
    UpdateLCDline1( out );
}

extern char gTitle[];
extern int gDim;

int genericMenu( char *title,
                 char *( *printFunc )( int *num, int max ),
                 void ( *incrementFunc )( int *num, int max ),
                 void ( *decrementFunc )( int *num, int max ),
                 char *( *quickKeyFunc ) ( int *sel, int kpv, int max ),
                 int max,               // if max is negative, indicate we CANNOT use quick-number-entry
                 int *selection )
{
    strcpy( gTitle, title );

    int sel = selection ? ( *selection ) : 0;

    custom_character( 0, character_arrow_updown );


    char qkeyBuffer[6];
    char *qkey = qkeyBuffer;
    *qkey = 0;

    int qkv = 0;


    int key = 0;
    do {

        showEntry( gTitle, qkeyBuffer, quickKeyFunc ? '_' : ' ' );

        char out[32];
        sprintf( out, "%-15s\010", ( *printFunc )( &sel, max ));
        UpdateLCDline2( out );


        key = GetDebouncedKey();
        IFEXIT( key );


        // Neato turbo-menu keyboard enhancement!

        if (  quickKeyFunc ) {

            // Allow numbers to be input from keyboard -- jumps to first compatible
            // menu option starting with the same number substring.  The substring is
            // appended to the title

            int num = ReturnNumber( key );
            if ( num >= 0 ) {

                if ( qkey - qkeyBuffer < 4 ) {
    
                    qkv = 10 * qkv + num;
                    *(qkey++) = '0' + num;
                    *qkey = 0;

                    // Try and map the quick keypresses to a menu option. The passed-in function is responsible
                    // for this matching, and if it can, it will change "sel" to the correct selection.  This is 
                    // pretty neat, as it allows you to type in "10" and have "October" selected in the menu, for
                    // example.

                    char *newt = (*quickKeyFunc)( &sel, qkv, max );
                    if ( newt )
                        title = newt;
                }

            } else {
                
                qkey = qkeyBuffer;
                *qkey = 0;
                qkv=0;
            }
        }


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


    int max = strlen( string );

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

        if ( PREVIOUS( key ) )
            if ( --sel < 2 )
                sel = 0;
    
    } while ( !ENTER(key) );


    if ( selection )
        ( *selection ) = sel;

    return MODE_EXIT;

}


char *printMenu( int *item, const menuItem *menu )
{
    return (char *) menu[*item].name;
}
