#ifndef __UWATCH_SETUPMODE_H
#define __UWATCH_SETUPMODE_H

int changeTime( int p );
int changeDate();
int changeCalibration();
int change1224();
int changeDST();
int changeLocation();

void SetTimeBCD( unsigned int h, unsigned int m, unsigned int s );
int SetDateBCD( unsigned int y, unsigned int m, unsigned int d );


#endif
