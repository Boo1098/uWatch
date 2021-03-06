
/*-
 * Copyright (c) 2008 Voidware Ltd.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of the source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Any redistribution solely in binary form must conspicuously
 *    reproduce the following disclaimer in documentation provided with the
 *    binary redistribution.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'', WITHOUT ANY WARRANTIES, EXPRESS
 * OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  LICENSOR SHALL
 * NOT BE LIABLE FOR ANY LOSS OR DAMAGES RESULTING FROM THE USE OF THIS
 * SOFTWARE, EITHER ALONE OR IN COMBINATION WITH ANY OTHER SOFTWARE.
 * 
 */

/* Astromony Functions for uWatch */

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include "def.h"
#include        "uWatch-astro.h"


#define PI2 6.283185307179586476925286766559006
#define DPR 57.29577951308232087679815481410517
#define RPD 0.017453292519943295769236907684886

static double frac(double x)
{
    return x - floor(x);
}

#define SRATE   0.9972696
#define EPS  (1.0/120.0)

void m24( double *v ) {
    *v += 24;
    while ( *v >= 24 )
        *v -= 24;
}


static double daytime(double t)
{
    m24(&t);
    return t * SRATE;
}

static double lmst(double mjd, double lambda)
{
/*-----------------------------------------------------------------------*/
/* LMST: local mean sidereal time                                        */
/*-----------------------------------------------------------------------*/
    double mjd0,t,ut,gmst;

    mjd0= floor(mjd);
    ut=(mjd-mjd0)*24;
    t=(mjd0-51544.5)/36525.0;
    gmst=6.697374558 + 1.0027379093*ut
            +(8640184.812866+(0.093104-6.2e-6*t)*t)*t/3600.0;
    return 24.0*frac( (gmst-lambda/15.0) / 24.0 );
}

static double minisun(double t, double* ra, double* dec)
{
/*-----------------------------------------------------------------------*/
/* MINI_SUN: low precision solar coordinates (approx. 1')                */
/*           T  : time in MJD                                            */
/*           RA : right ascension (in h; equinox of date)                */
/*           DEC: declination (in deg; equinox of date)                  */
/*-----------------------------------------------------------------------*/

    double coseps=0.91748;
    double sineps=0.39778;
    double l,m,dl,sl,x,y,z,rho;

    t  = (t-51544.5)/36525.0;
    m  = PI2*frac(0.993133+99.997361*t); 
    dl = 6893.0*sin(m)+72.0*sin(2*m);
    l  = PI2*frac(0.7859453 + m/PI2 + (6191.2*t+dl)/1296e3);
    sl = sin(l);
    x=cos(l);
    y=coseps*sl;
    z=sineps*sl;
    rho=sqrt(1.0-z*z);
    *dec = (360.0/PI2)*atan(z/rho); 
    *ra  = ( 48.0/PI2)*atan(y/(x+rho));
    m24(ra);
    return l * DPR;
}

static void SunRiseSet(double t, double lambda, double phi,
                       double sinh0, double* rise, double* set)
{
    double sphi, cphi;
    double ra, dec;
    double ct, srt, sst, tau, r, s;
    double lst0;

    double DPR15 = DPR/15.0;

    phi *= RPD;
    sphi = sin(phi);
    cphi = cos(phi);

    lst0 = lmst(t, lambda);
    *rise = -1;
    *set = -1;

    minisun(t + 0.5, &ra, &dec);
    dec *= RPD;
    ct = (sinh0 - sphi*sin(dec))/(cphi*cos(dec));
    
    tau = acos(ct) * DPR15;
    srt = ra - tau;
    sst = ra + tau;

    r = daytime(srt - lst0);
    s = daytime(sst - lst0);

    /* Find rise time */
    minisun(t + r/24, &ra, &dec);
    dec *= RPD;
    ct = (sinh0 - sphi*sin(dec))/(cphi*cos(dec));
    tau = acos(ct) * DPR15;
    srt = ra - tau;
    *rise = daytime(srt - lst0);

    minisun(t + s/24, &ra, &dec);
    dec *= RPD;
    ct = (sinh0 - sphi*sin(dec))/(cphi*cos(dec));
    tau = acos(ct) * DPR15;
    sst = ra + tau;
    *set = daytime(sst - lst0);
}

void CalcRiseAndSet(double* r, double* s)
{
    //double lambda, phi, t;
    //double t;

    //sinh0 = sin(-50.0/60.0 * RPD); /* sunrise           at h=-50'        */
    //double sinh0 = -1.454389765158265677038484573310e-2;

    // get global position
    //lambda = Longitude; 
    //phi = Latitude;

    SunRiseSet( MJD + 51544, Longitude, Latitude,
        -1.454389765158265677038484573310e-2, r, s);

    if (DST)
    {
        *r += 1.0;
        *s += 1.0;
    }

    *r += GMTOffset; //[ activeTimezone ];
    *s += GMTOffset; //[ activeTimezone ];

    m24(r);
    m24(s);

    //while (*r < 0 )
    //    *r += 24;
    //while ( *s < 0 )
    //    *s += 24;

}

