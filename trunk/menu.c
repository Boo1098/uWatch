
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




int genericMenu2( const packedMenu *menu, int *selection )
{

    int sel = selection ? ( *selection ) : 0;

    // Draw the menu indicators
    custom_character( 0, left_menu );
    custom_character( 1, right_menu );

    // Menu may itself define custom characters it requires.  Character #s are from 2-7
    int i;
    for ( i = 0; i < MENU_CUSTOM_CHAR_COUNT; i++ )
        if ( menu->customCharacter[i] )
            custom_character( i + 2, menu->customCharacter[i] );


    if ( menu->title )
        UpdateLCDline1( menu->title );


    int key;
    do {

        if ( menu->print ) {
            char out2[17];
            sprintf( out2, "%-13s\010%d\1", ( *( menu->print ) )( &sel, (menuItem *)(menu->menu) ), sel);
            UpdateLCDline2( out2 );
        }

        key = GetDebouncedKey();

        IFEXIT( key );

        int autoSel = ReturnNumber( key );
        if ( autoSel >= 0 && autoSel < menu->max ) {
            sel = autoSel;
            break;
        }

        if ( menu->inc && NEXT( key ) )
            ( *( menu->inc ) )( &sel, menu->max );

        if ( menu->dec && PREVIOUS( key ) )
            ( *( menu->dec ) )( &sel, menu->max );

    } while ( key != KeyEnter );


    if ( selection )
        ( *selection ) = sel;

    if ( menu->menu[sel].run )
        ( *selection ) = ( menu->menu[sel].run )( menu->menu[sel].op );

    return MODE_EXIT;
}



int genericMenu( char *title,
                 char *( *printFunc )( int *num, int max ),
                 int ( *idleFunc )( int *halt ),
                 void ( *incrementFunc )( int *num, int max ),
                 void ( *decrementFunc )( int *num, int max ),
                 int max, int *selection )
{

    int sel = selection ? ( *selection ) : 0;

    custom_character( 0, left_menu );
    custom_character( 1, right_menu );

    if ( title )
        UpdateLCDline1( title );

    int key = 0;
    do {

        if ( printFunc ) {
            char out2[17];
            sprintf( out2, "%-14s\010\1", ( *printFunc )( &sel, max ) );
            UpdateLCDline2( out2 );
        }

        int lastMax = max;
        if ( idleFunc && ( KeyScan2(FALSE) == 0 ) ) {
            int h = 0;
            max = (*idleFunc)( &h );
            if ( h )
                return MODE_EXIT;
        }            

        if ( lastMax != max )
            continue;        

        if ( !idleFunc ) {
            key = GetDebouncedKey();
        } else {

            if ( key )
                DelayMs( 500 );

            key = KeyScan2( FALSE );
        }



        IFEXIT( key );

        int autoSel = ReturnNumber( key );
        if ( autoSel >= 0 && autoSel < max ) {
            sel = autoSel;
            break;
        }

        if ( incrementFunc && NEXT( key ) )
            ( *incrementFunc )( &sel, max );

        if ( decrementFunc && PREVIOUS( key ) )
            ( *decrementFunc )( &sel, max );

    } while ( key != KeyEnter );


    if ( selection )
        ( *selection ) = sel;

    return MODE_EXIT;

}




int viewString( char *title, char *string,
                 int *selection, int oneshot )
{
    int max = strlen( string );;

    if ( title )
        UpdateLCDline1( title );

    int sel = 0;
    if ( selection ) sel = *selection;

    custom_character( 0, left_menu );
    custom_character( 1, right_menu );

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

        if ( oneshot == 1 )
            break;


        if ( oneshot == 2 ) {

            if ( key )
                DelayMs(400);
            key = KeyScan2( FALSE );


        } //else
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

    } while ( key != KeyEnter );


    if ( selection )
        ( *selection ) = sel;

    return MODE_EXIT;

}


char *printMenu( int *item, menuItem *menu )
{
    return (char *) menu[*item].name;
}
