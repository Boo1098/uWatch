
#include "tool-factor.h"

#include "def.h"
#include "uWatch-LCD.h"
#include "menu.h"
#include "characterset.h"

char displayBuffer[400];



void check( unsigned long int nx, unsigned long int in, int *factors ) {
           
    unsigned long int fact = nx / in;
    if ( fact * in == nx ) {

        if ( *factors )
            strcat( displayBuffer, "," );
    
        sprintf( out, "%lu", in );
        strcat( displayBuffer, out );
        if ( fact != in ) {
            sprintf( out, ",%lu", fact );
            strcat( displayBuffer, out );
            (*factors)++;
        }
        (*factors)++;
    }
}

int factor( int p )
{
    unsigned long int in;
    unsigned long int nx;
    unsigned long int idleEnd;
    int factors=0;

    //notebook();

//        viewString( "Address:", view, 0, 0 );

    UpdateLCDline1("Factor this");
    Xreg = 0;
    OneLineNumberEntry();
    Xreg = fabs(Xreg);

    const char *nb = "Result";

    if (Xreg > 4294967296.0) {
        sprintf( displayBuffer, "too large");
    }
    else {

        nx = Xreg; Xreg = 0;
        idleEnd = sqrt( nx );
        sprintf( displayBuffer, "%lu = (", nx );
        viewString( "Factors...", displayBuffer, 0, 1 );
    
        for ( in = 2; in <= idleEnd; in++ ) {
    
            int key = KeyScan2();
    
            if ( key == KeyClear ) {
                strcat( displayBuffer, " ..." );
                nb = "Halted!";
                break;
            }
    
            if ( key == KeyMode )
                return MODE_KEYMODE;
    
            check( nx, in, &factors );
        }
    
        strcat( displayBuffer, ")" );
    }

    viewString( (char *) nb, displayBuffer, 0, 2 );

    return MODE_EXIT;

}
