
#include "def.h"
#include "stopwatch.h"
#include "menu.h"
#include "uWatch-LCD.h"
#include "characterset.h"

#define STOPWATCH_START -1
#define STOPWATCH_LAP -2
#define STOPWATCH_SPLIT -3


BOOL stopWatchActive = FALSE;

double stopWatchStart = 0;             // starting seconds
double stopWatchLapTime = 0;
double swTenths = 0;

BOOL firstSecond;


char *displayTime( double seconds ) {

    int hours = (int) ( seconds/ ( 60 * 60 ));
    seconds -= hours * 60 * 60;
    int minutes = (int) ( seconds / 60 );
    seconds -= minutes * 60;
    
    //if ( hours > 0 )
        if ( seconds < 10 )
            sprintf( out, "%02d:%02d:0%2.1f", hours, minutes, seconds );
        else
            sprintf( out, "%02d:%02d:%2.1f", hours, minutes, seconds );
    //else
    //    if ( seconds < 10 )
    //        sprintf( out, "   %02d:0%2.1f", minutes, seconds );
    //    else
    //        sprintf( out, "   %02d:%2.1f", minutes, seconds );

    return out;
}

extern int seconds( rtccTime *t );

int getRT() {

    rtccTime time;
    RtccReadTime(&time);
    return seconds( &time );
}


static int lastNow = 0;
static int currentTime;
int delay = 2200;

char *stopWatchPrintMenu( int *item, const menuItem *menu ) {

    Clock4MHz();

    char out2[20];


    switch ( menu[*item].op ) {

        case STOPWATCH_START:


            if ( stopWatchActive )
                strcpy( out2, "STOP" );
            else
                strcpy( out2, "START" );
            break;

        case STOPWATCH_LAP:

            sprintf( out2, "LAP  %s", displayTime( stopWatchLapTime ));
            break;

//        case STOPWATCH_SPLIT:
//        {
//            getRT();
//            sprintf( out2, "SPLIT: %s", displayTime( now - stopWatchLapTime ) );
//        }
//        break;
        
    }

    DelayMs( delay );

    if ( stopWatchActive ) {

        currentTime = getRT();

        swTenths += 0.1;

        if ( swTenths > 0.9 ) {
            delay += 2000 * ( swTenths - 0.9 );
            swTenths = 0.9;
        }

        if ( currentTime != lastNow ) {


            if ( firstSecond ) {
                stopWatchStart += 0.9 - swTenths;
                firstSecond = FALSE;
            } else {

                if ( swTenths < 0.9 ) {
                    delay -= 2000 * ( 0.9 - swTenths );
                }    

            }

            swTenths = 0;
            lastNow = currentTime;
        }

    }


    sprintf( displayBuffer, "Time %s%c", displayTime( currentTime - stopWatchStart + swTenths ),
        stopWatchActive ? ' ':'\2' );

    Clock250KHz();
    
    UpdateLCDline1( displayBuffer );

    strcpy( out, out2 );
    return out;
}

int stopWatchStartStop( int op ) {

    if ( stopWatchActive ) {
        stopWatchActive = FALSE;
        //stopWatchStart = getRT() - stopWatchStart;

    } else {

        stopWatchActive = TRUE;
        firstSecond = TRUE;
        swTenths = 0;
        stopWatchStart = getRT();
        lastNow = stopWatchStart;
        currentTime = stopWatchStart;
    }
    return MODE_EXIT;
}


int stopWatchLap( int op ) {
    if ( stopWatchActive ) {
        stopWatchLapTime = lastNow - stopWatchStart + swTenths;
    }
    return MODE_EXIT;
}

//int stopWatchSplit( int op ) {
//    return MODE_EXIT;
//}

int StopWatchMode() {


    const menuItem stopWatchMenuMenu[] = {
        { "",  stopWatchStartStop, STOPWATCH_START },
        { "", stopWatchLap, STOPWATCH_LAP },
    };

    const packedMenu2 stopWatchMenu = {
        "",
        stopWatchPrintMenu,
        0, 0, 2, stopWatchMenuMenu
    };


    int mode;
    //int sel = 0;
    do {

        mode = genericMenu2( &stopWatchMenu );
    } while ( mode != MODE_KEYMODE );

    return mode;
}




int StopWatch( int p ) {

    custom_character( 2, characterDST );


    int status = StopWatchMode();

    while ( KeyScan2( FALSE ));     // wait for mode release

    return status;
}




