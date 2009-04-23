#include "tool-factor.h"

#include "def.h"
#include "uWatch-LCD.h"
#include "menu.h"
#include "characterset.h"

char displayBuffer[512];



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
    
        //int sel = strlen( displayBuffer ) - 4;
        //viewString( "Calculating:", displayBuffer, &sel, 1 );
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

    UpdateLCDline1("Factor this:");
    Xreg = 0;
    OneLineNumberEntry();
    nx = Xreg;
    Xreg = 0;

    if (nx > (unsigned long int)(long int)(-1)) //checks if number > max long int??
        sprintf( displayBuffer, "%lu is too large to factorise.", nx );


    else if ( Xreg < 0 ) //checks for negative,0
        sprintf( displayBuffer, "Please enter positive numbers only." );
    
    else {

        idleEnd = sqrt( nx );
        sprintf( displayBuffer, "%lu = (", nx );
        viewString( "Factorising:", displayBuffer, 0, 1 );
    
        for ( in = 2; in <= idleEnd; in++ )
            check( nx, in, &factors );

        strcat( displayBuffer, ")" );
    
        if (factors == 0 )
            sprintf( displayBuffer, "%lu is prime.", nx );

        viewString( "Factorised:", displayBuffer, 0, 2 );

    }

    return MODE_EXIT;

}
