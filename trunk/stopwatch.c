#include "def.h"
#include "stopwatch.h"
#include "menu.h"
#include "uWatch-LCD.h"
#include "characterset.h"

#define STOPWATCH_START -1
#define STOPWATCH_LAP -2
#define STOPWATCH_SPLIT -3


BOOL stopWatchActive;
int stopWatchStart;         // starting seconds
int stopWatchLapTime;


char *displayTime( int seconds ) {

    int hours = (int) ( seconds/ ( 60 * 60 ));
    seconds -= hours * 60 * 60;
    int minutes = (int) ( seconds / 60 );
    seconds -= minutes * 60;
    
    if ( hours > 0 )
        sprintf( out, "%02d:%02d:%02d", hours, minutes, seconds );
    else
        sprintf( out, "   %02d:%02d", minutes, seconds );

    return out;
}

extern int seconds( rtccTime *t );

int getRT() {

    rtccTime time;
    RtccReadTime(&time);
    return seconds( &time );
}


char *stopWatchPrintMenu( int *item, menuItem *menu ) {

    Clock1MHz();

    char out2[17];
    char out3[17];

    int now = getRT();



    switch ( menu[*item].op ) {

        case STOPWATCH_START:

            if ( stopWatchActive )
                strcpy( out2, "STOP" );
            else
                strcpy( out2, "START" );
            break;

        case STOPWATCH_LAP:

            sprintf( out2, "LAP   %s", displayTime( stopWatchLapTime ));
            break;

//        case STOPWATCH_SPLIT:
//        {
//            getRT();
//            sprintf( out2, "SPLIT: %s", displayTime( now - stopWatchLapTime ) );
//        }
//        break;
        
    }

    if ( stopWatchActive ) {

        sprintf( out3, "Time  %s", displayTime( now - stopWatchStart ));


    } else {
        sprintf( out3, "Time  %s\2", displayTime( stopWatchStart ) ); 

    }

    Clock250KHz();
    

    UpdateLCDline1( out3 );
   // Clock1MHz();

    strcpy( out, out2 );
    return out;
}

int stopWatchStartStop( int op ) {

    if ( stopWatchActive ) {
        stopWatchActive = FALSE;
        stopWatchStart = getRT() - stopWatchStart;

    } else {

        stopWatchActive = TRUE;
        stopWatchStart = getRT();
    }
    return MODE_EXIT;
}


int stopWatchLap( int op ) {
    if ( stopWatchActive ) {
        stopWatchLapTime = getRT() - stopWatchStart;
    }
    return MODE_EXIT;
}

//int stopWatchSplit( int op ) {
//    return MODE_EXIT;
//}

int StopWatchMode() {

    const packedMenu stopWatchMenu = {
        "",
        stopWatchPrintMenu,
        increment, decrement, 2,
        {},
        {   { "Start/Stop",  stopWatchStartStop, STOPWATCH_START },
            { "Lap", stopWatchLap, STOPWATCH_LAP },
            //{ "Split", stopWatchSplit, STOPWATCH_SPLIT },
        },
    };

    int mode;
    int sel = 0;
    do {

        mode = genericMenu2( &stopWatchMenu, &sel );
    } while ( mode != MODE_KEYMODE );

    return mode;
}




int StopWatch( int p ) {

    custom_character( 2, characterDST );

    stopWatchActive = FALSE;
    stopWatchStart = 0;
    stopWatchLapTime = 0;

    int status = StopWatchMode();

    while ( KeyScan2( FALSE ));     // wait for mode release

    return status;
}




