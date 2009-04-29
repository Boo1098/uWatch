#include "def.h"
#include "stopwatch.h"
#include "menu.h"

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
    
    sprintf( out, "%02d:%02d:%02d", hours, minutes, seconds );
    return out;
}

int getRT() {

    rtccTime time;
    RtccReadTime(&time);
    return BCDtoDEC(time.f.hour) * 60 * 60 + BCDtoDEC(time.f.min) * 60 + BCDtoDEC(time.f.sec);
}


char *stopWatchPrintMenu( int *item, menuItem *menu ) {

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

            sprintf( out2, "LAP  %s", displayTime( stopWatchLapTime ));
            break;

//        case STOPWATCH_SPLIT:
//        {
//            getRT();
//            sprintf( out2, "SPLIT: %s", displayTime( now - stopWatchLapTime ) );
//        }
//        break;
        
    }

    if ( !stopWatchActive )
        sprintf( out3, "TIME %s", displayTime( now - stopWatchStart ));
    else
        sprintf( out3, "TIME %s", displayTime( stopWatchStart ));

    UpdateLCDline1( out3 );

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
    stopWatchLapTime = getRT();
    return MODE_EXIT;
}

//int stopWatchSplit( int op ) {
//    return MODE_EXIT;
//}

int StopWatchMode() {

    const packedMenu stopWatchMenu = {
        "ToDO",
        stopWatchPrintMenu,
        increment, decrement, 2,
        {   0,
            0,
            0,
            0,
        },
        {   { "Start/Stop",  stopWatchStartStop, STOPWATCH_START },
            { "Lap", stopWatchLap, STOPWATCH_LAP },
            //{ "Split", stopWatchSplit, STOPWATCH_SPLIT },
        },
    };

    int key;
    do {

        key = genericMenu2( &stopWatchMenu, 0 );


    } while ( key != MODE_KEYMODE );

    return key;
}




int StopWatch( int p ) {

    Clock1MHz();
    DisableSleepTimer();

    stopWatchActive = FALSE;
    stopWatchLapTime = 0;

    int status = StopWatchMode();

    Clock250KHz();
    EnableSleepTimer();
    return status;
}




