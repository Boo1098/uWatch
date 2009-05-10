
#include "tool-factor.h"

#include "def.h"
#include "uWatch-LCD.h"
#include "menu.h"
#include "characterset.h"

char displayBuffer[400];


int check( unsigned long* nx, unsigned long int in, int* factors ) 
{
    // return 1 if factor and reduce nx
    unsigned long int fact = *nx / in;
    if ( fact * in == *nx ) {

        if ( *factors )
            strcat( displayBuffer, "," );
    
        sprintf( out, "%lu", in );
        strcat( displayBuffer, out );
        *nx = fact;
        ++*factors;
        return 1;
    }
    return 0;
}

int factor( int p )
{
    unsigned long int in;
    unsigned long int nx;
    unsigned long int idleEnd;
    int factors=0;

    UpdateLCDline1("Factor this");
    //Xreg = 0; allow number to be carried in from calculator.
    OneLineNumberEntry();
    Xreg = fabs(Xreg);

    const char *nb = "Result";

    if (Xreg > 4294967296.0) {
        sprintf( displayBuffer, "too large");
    }
    else {

        nx = Xreg; Xreg = 0;
        sprintf( displayBuffer, "%lu = (", nx );
        viewString( "Factors...", displayBuffer, 0, 1 );

        if (nx > 0)
        {
            // elminate 2s
            while (check(&nx, 2, &factors)) ;

            // elminate 3s
            while (check(&nx, 3, &factors)) ;

            idleEnd = sqrt( nx );
            
            for (in = 5; in <= idleEnd; in += 4)
            {
                unsigned long t = nx;
                while (check( &nx, in, &factors )) ;
                in += 2;
                while (check( &nx, in, &factors )) ;

                if (t != nx)
                {
                    // we've factored, so reduce the limit
                    idleEnd = sqrt( nx );                    
                }

                int key = KeyScan2();
                if ( key == KeyClear ) {
                    strcat( displayBuffer, " ..." );
                    nb = "Halted!";
                    break;
                }
                if ( key == KeyMode ) return MODE_KEYMODE;
            }
            if (nx > 1) check(&nx, nx, &factors); // put remainder
        }
        strcat( displayBuffer, ")" );
    }

    viewString( (char *) nb, displayBuffer, 0, 2 );

    return MODE_EXIT;

}
