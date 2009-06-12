
#include "def.h"
#include "stopwatch.h"
#include "menu.h"
#include "uWatch-LCD.h"
#include "characterset.h"

#define STOPWATCH_START -1
#define STOPWATCH_LAP -2
#define STOPWATCH_SPLIT -3


BOOL stopWatchActive = FALSE;

unsigned long stopWatchStart = 0;             // starting seconds
unsigned long stopWatchLapTime = 0;
unsigned long swTenths = 0;

BOOL firstSecond;


char *displayTime( unsigned long secs ) {

    // Buried somewhere in here was the stopwatch weird time display issue
    // this was fixed by moving to integer arithmetic AND using LONG values.  Probably an overflow/underflow.
    // This method is smaller and also avoids the %02.1f floating point bug which DOESN'T display a leading 0
    unsigned long secs2 = secs/10;
    unsigned int hours = ( secs2 / 3600L );
    secs2 -= hours * 3600L;
    unsigned int minutes = ( secs2 / 60L );
    secs -= (( hours * 3600L ) + ( minutes * 60L )) * 10L;
    unsigned int s1 = secs/10;
    unsigned int s2 = secs-s1*10;
    sprintf( displayBuffer, "%02d:%02d:%02d.%d", hours, minutes, s1, s2 );
    return displayBuffer;
}


extern int seconds( rtccTime *t );

int getRT() {
    rtccTime time;
    RtccReadTime(&time);
    return 10 * seconds( &time );
}


long lastNow = 0;
long currentTime;
int delay = 2200;

char *stopWatchPrintMenu( int *item, const menuItem *menu ) {

    Clock4MHz();

    switch ( menu[*item].op ) {

        case STOPWATCH_START:
            strcpy( out, stopWatchActive ? "STOP" : "START" );
            break;

        case STOPWATCH_LAP:
            sprintf( out, "LAP  %s", displayTime( stopWatchLapTime ));
            break;
    }

    DelayMs( delay );

    if ( stopWatchActive ) {

        currentTime = getRT();

        if ( ++swTenths > 9 ) {
            delay += 200 * ( swTenths - 9 );
            swTenths--;
        }

        if ( currentTime != lastNow ) {

            if ( firstSecond ) {
                stopWatchStart += 9 - swTenths;
                firstSecond = FALSE;
            } else {

                if ( swTenths < 9 ) {
                    delay -= 200 * ( 9 - swTenths );
                }    

            }

            swTenths = 0;
            lastNow = currentTime;
        }

    }

    sprintf( displayBuffer+50, "Time %s", displayTime( currentTime - stopWatchStart + swTenths ));

    Clock250KHz();
    
    UpdateLCDline1( displayBuffer+50 );
    return out;
}

int stopWatchStartStop( int op ) {

    if ( !stopWatchActive ) {

        firstSecond = TRUE;
        swTenths = 0;
        stopWatchStart = getRT();
        lastNow = stopWatchStart;
        currentTime = stopWatchStart;
    }

    stopWatchActive = !stopWatchActive;

    return MODE_EXIT;
}


int stopWatchLap( int op ) {
    if ( stopWatchActive ) {
        stopWatchLapTime = lastNow - stopWatchStart + swTenths;
    }
    return 0;
}


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

    while ( genericMenu2( &stopWatchMenu ) != MODE_KEYMODE ) {}
    return MODE_KEYMODE;
}


int StopWatch( int p ) {

    int status = StopWatchMode();
    while ( KeyScan2( FALSE ));     // wait for mode release
    return status;
}




