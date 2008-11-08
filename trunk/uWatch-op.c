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

/* implementation of the main calculator functions */

#include <stdlib.h>
#include <math.h>
#include "uWatch-op.h"
#include "uWatch-astro.h"
#include "uWatch-dp.h"

#define LN10 2.302585092994045684017991454683

double hms(double h)
{
    return (90*h+100*(int)(h)+(int)(60*h))/250;
}

double hr(double x)
{
    return (250*x-60*(int)(x)-(int)(100*x))/90;
}

void caldati(long mjd,
             unsigned int* y, unsigned int* m, unsigned int* d)
{
    // here is the new Hodges' version 2008
    // this is the inverse of mjd

    long td = mjd + 2520114L;
    unsigned int ty;
    unsigned int c, a;
    unsigned int tm;

    ty= (td<<2)/146097L;
    td -= (146097L*ty+3)>>2;
    c = (99*(td+1))/36160L;
    td -= ((1461L*c)>>2)-31;
    tm = (5*td)/153;
    a = tm/11;
    *d= td-(367L*tm)/12;
    *m= tm-12*a+2;
    *y= 100*(ty-49)+c+a;
}

long mjd(int y, int m, int d)
{
    // here is the new Hodges' version 2008
    // rebased to 2000/1/1 (add back 51544 for old version)

    int a = (14-m)/12;
    m += 12*a+1;
    y += 4800-a;
    return d+(153*m)/5+365L*y+y/4-y/100+y/400-2483712L;
}

int moonPhase(int year,int month,int day)
{
    /*
      Calculates the moon phase (0-7), accurate to 1 segment.
      0 = > new moon.
      4 => Full moon.

      NOTE: integer math.
    */
    
    int g, e;

    if (month == 1) --day;
    else if (month == 2) day += 30;
    else // m >= 3
    {
        day += 28 + (month-2)*3059/100;

        // adjust for leap years
        if (!(year & 3)) ++day;
        if ((year%100) == 0) --day;
    }
    
    g = (year-1900)%19 + 1;
    e = (11*g + 18) % 30;
    if ((e == 25 && g > 11) || e == 24) e++;
    return ((((e + day)*6+11)%177)/22 & 7);
}

// complex fabs
static double fabsC(double* a, double* b)
{
    // use numerically stable method, Numerical Recipies 3rd p226
    double fa, fb, t;

    fa = fabs(*a);
    fb = fabs(*b);

    if (!*a) return fb;
    if (!*b) return fa;

    if (fa >= fb)
    {
        t = (*b)/(*a);
        return fa*sqrt(t*t+1.0);
    }
    else
    {
        t = (*a)/(*b);
        return fb*sqrt(t*t+1.0);
    }
}

static void sinandcos(double v, double* sa, double* ca)
{
    // need to find a more efficient way to calculate both sin and cos
    *sa = sin(v);
    *ca = cos(v);
}

static void sinhandcosh(double v, double* sha, double* cha)
{
    if (fabs(v) >= 1)
    {
        double t = exp(v);
        double t1 = 1/t;
        *sha = (t - t1)/2;
        *cha = (t + t1)/2;
    }
    else
    {
        // sinh(x) requires non-exp method for small |x|.
        // noticed that this is already implemented in the mplib
        // so use it!
        *cha = cosh(v);
        *sha = sinh(v);
    }
}

static void RtoP(double* x, double* y)
{
    double t = fabsC(x, y);
    *y=atan2(*y, *x);
    *x = t;
}

static void PtoR(double* x, double* y)
{
    double s, c;
    sinandcos(*y, &s, &c);
    *y = (*x)*s;
    *x = (*x)*c;
}

static double dgamma(double x)
{
    /// gamma function baked for IEEE754
    int k, n;
    double w, y;

    n = x < 1.5 ? -((int) (2.5 - x)) : (int) (x - 1.5);
    w = x - (n + 2);
    y = ((((((((((((-1.99542863674e-7 * w + 1.337767384067e-6) * w - 
        2.591225267689e-6) * w - 1.7545539395205e-5) * w + 
        1.45596568617526e-4) * w - 3.60837876648255e-4) * w - 
        8.04329819255744e-4) * w + 0.008023273027855346) * w - 
        0.017645244547851414) * w - 0.024552490005641278) * w + 
        0.19109110138763841) * w - 0.233093736421782878) * w - 
        0.422784335098466784) * w + 0.99999999999999999;
    if (n > 0) {
        w = x - 1;
        for (k = 2; k <= n; k++) {
            w *= x - k;
        }
    } else {
        w = 1;
        for (k = 0; k > n; k--) {
            y *= x - k;
        }
    }
    return w / y;
}

//perform the factorial operation and return the result
static double factorial(double num)
{
    if (num > 171 || num < -171)
    {
        return 1.0/0.0;  // inf
    }

    int n = (int)floor(num);
    if (n == num)
    {
        if (n < 0)
        {
            // negative integer is NaN
            return 0.0/0.0; // NAN
        }

        // is integer
        double temp=1;
        while(n>=1)     //perform the factorial algorithm
        {
            temp=temp*n;
            --n;
        }
        return temp;
    }
    else
    {
        // calculate non-integer factorials with the gamma function
        // as a treat to HP fans.
        return dgamma(num + 1);
    }
}

static void opLn(double* rp, double* irp)
{
    if (*rp >= 0 && !*irp)
        *rp=log(*rp);           
    else
    {
        RtoP(rp, irp);
        *rp = log(*rp);
    }
}

#define MULC(_a, _b, _c, _d)                    \
{                                               \
    double t = (_a)*(_c) - (_b)*(_d);           \
    _b = (_a)*(_d) + (_b)*(_c);                 \
    _a = t;                                     \
}

#define SQUAREC(_a, _b)                         \
{                                               \
    double t = (_a)*(_a) - (_b)*(_b);           \
    _b = 2*(_a)*(_b);                           \
    _a = t;                                     \
}

static void opInv(double* rp, double* irp)
{
    if (!*irp)
        *rp = 1.0/(*rp);
    else
    {
        // 1 / (c + i d) = c / (c*c + d*d) - i d / (c*c + d*d)
        double d = (*rp)*(*rp) + (*irp)*(*irp);
        *rp = (*rp)/d;
        *irp = -(*irp)/d;
    }
}

static void powInt(double* rp, double* irp, unsigned long m)
{
    // assume m > 0

    double x, y;

    x = 1;
    
    /* Use binary exponentiation */
    if (!*irp)
    {
        for (;;)
        {
            if (m & 1) x *= (*rp);
            m >>= 1;
            if (!m) break;
            (*rp) *= (*rp);
        }
    }
    else
    {
        y = 0;
        for (;;)
        {
            if (m & 1) MULC(x, y, *rp, *irp);
            m >>= 1;
            if (!m) break;
            SQUAREC(*rp, *irp);
        }
        *irp = y;
    }
    
    *rp = x;
}

static void powC(double* rp, double* irp, double a, double b)
{
    double x, y;

    if (!b)
    {
        unsigned long ni;

        if (a == 1) return;
        if (!a)
        {
            if (!*rp && !*irp)
            {
                // 0^0 = nan
                *rp = 0.0/0.0;
            }
            else *rp = 1;
            *irp = 0;
            return;
        }

        if (a < 0)
        {
            // power the inverse
            opInv(rp, irp);
            a = -a;
        }

        // convert to int and see if we have the same number.
        ni = (unsigned long)a;

        // if we have an integer that fits in a long, use ladder
        if (ni == a)
        {
            powInt(rp, irp, ni);
            return;
        }

        if (!*irp)
        {
            if (!*rp) return; // 0^positive-real = 0
            if (*rp > 0)
            {
                *rp=pow(*rp, a);
                return;
            }
        }
    }

    // general case

    RtoP(rp, irp);
    x = pow(*rp, a);
    y = a * (*irp);

    if (b)
    {
        x *= exp(-b*(*irp));
        y += b*log(*rp);
    }

    *rp = x;
    *irp = y;
    
    PtoR(rp, irp);
}

// Raw operation
void Operation(int op)
{
    double* rp = Regs;
    double* irp = iRegs;
    switch(op)
    {
    case CALC_OP_RECIPROCAL:
        opInv(rp, irp);
        break;
    case CALC_OP_SQUARE:
        if (!*irp)
            *rp *= (*rp);
        else
            SQUAREC(*rp, *irp);
        break;
    case CALC_OP_SQRT:
        {
            if (!*irp)
            {
                if (*rp < 0)
                {
                    double x = sqrt(-*rp);
                    *rp = 0;
                    *irp = x;
                }
                else *rp=sqrt(*rp);
            }
            else 
            {
                // see Numerical Recipes, 3rd, p226
                double fc = fabs(*rp);
                double fd = fabs(*irp);
                double w;
                if (fc >= fd)
                {
                    double t = (*irp)/(*rp);
                    w = sqrt(fc)*sqrt((sqrt(1.0 + t*t) + 1.0)/2.0);
                }
                else
                {
                    double t = (*rp)/(*irp);
                    w = sqrt(fd)*sqrt((fabs(t) + sqrt(1.0 + t*t))/2.0);
                }

                if (!w)
                {
                    *rp = 0;
                    *irp = 0;
                }
                else if (*rp >= 0)
                {
                    *rp = w;
                    *irp = (*irp)/(2*w);
                }
                else 
                {
                    *rp = fd/(2*w);
                    if (*irp >= 0)
                        *irp = w;
                    else
                        *irp = -w;
                }
            }
        }
        break;
    case CALC_OP_LN:
        opLn(rp, irp);
        break;
    case CALC_OP_EXP:
        if (!*irp)
            *rp=exp(*rp);
        else
        {
            double t = exp(*rp);
            double sb, cb;
            sinandcos(*irp, &sb, &cb);
            *rp = t*cb;
            *irp = t*sb;
        }
        break;
    case CALC_OP_NROOT:
        opInv(rp, irp);
        
        // Fall through to POW
    case CALC_OP_NPOW:
        powC(rp + 1, irp + 1, *rp, *irp);
        *rp = rp[1];
        *irp = irp[1];
        Drop();
        break;
    case CALC_OP_LN10:
        opLn(rp, irp);
        *rp /= LN10;
        *irp /= LN10;
        break;
    case CALC_OP_10X:
        {
            double x = 10;
            double y = 0;
            powC(&x, &y, *rp, *irp);
            *rp = x;
            *irp = y;
        }
        break;
    case CALC_OP_SIN:
        if (DegreesMode) { *rp /= RAD;  *irp /= RAD; }
        if (!*irp)
            *rp=sin(*rp);
        else
        {
            // sin(a + ib) = sin(a)cosh(b) + i cos(a)sinh(b)
            double sa, ca;
            double shb, chb;
            sinandcos(*rp, &sa, &ca);
            sinhandcosh(*irp, &shb, &chb);
            *rp = sa*chb;
            *irp = ca*shb;
        }
        break;
    case CALC_OP_COS:
        if (DegreesMode) { *rp /= RAD;  *irp /= RAD; }
        if (!*irp)
            *rp=cos(*rp);
        else
        {
            // cos(a + ib) = cos(a)cosh(b) - i sin(a)sinh(b)
            double sa, ca;
            double shb, chb;
            sinandcos(*rp, &sa, &ca);
            sinhandcosh(*irp, &shb, &chb);
            *rp = ca*chb;
            *irp = -sa*shb;
        }
        break;
    case CALC_OP_TAN:
        if (DegreesMode) { *rp /= RAD;  *irp /= RAD; }
        if (!*irp)
            *rp=tan(*rp);
        else
        {
            // tan(a + ib) = (sin(2a) + i sinh(2b))/(cos(2a) + cosh(2b))

            double sa, ca;
            double shb, chb;
            double d;
            sinandcos((*rp)*2, &sa, &ca);
            sinhandcosh((*irp)*2, &shb, &chb);
            d = ca + chb;
            *rp = sa/d;
            *irp = shb/d;
        }
        break;
    case CALC_OP_ASIN:
        if (!*irp && *rp >= -1 && *rp <= 1)
            *rp=asin(*rp);
        else
        {
            // XX unstable?
            double t = *rp + 1;
            double u = *rp - 1;
            double v = (*irp)*(*irp);
            double w1 = sqrt(t*t + v);
            double w2 = sqrt(u*u + v);
            double al = (w1 + w2)/2;
            double be = (w1 - w2)/2;

            t = asin(be);
            u = log(al + sqrt(al*al - 1));
            
            // choose branch cut
            *rp = t;
            *irp = (*irp < 0) ? -u : u;
            
        }
        if (DegreesMode) { *rp *= RAD; *irp *= RAD; }
        break;
    case CALC_OP_ACOS:
        if (!*irp && *rp >= -1 && *rp <= 1)
            *rp=acos(*rp);
        else
        {
            // XX unstable?
            double t = *rp + 1;
            double u = *rp - 1;
            double v = (*irp)*(*irp);
            double w1 = sqrt(t*t + v);
            double w2 = sqrt(u*u + v);
            double al = (w1 + w2)/2;
            double be = (w1 - w2)/2;

            t = acos(be);
            u = log(al + sqrt(al*al - 1));
            
            // choose branch cut
            *rp = t;
            *irp = (*irp < 0) ? u : -u;
        }
        if (DegreesMode) { *rp *= RAD; *irp *= RAD; }
        break;
    case CALC_OP_ATAN:
        if (!*irp)
            *rp=atan(*rp);
        else
        {
            double t = (*rp)*(*rp);
            double u = (*irp)*(*irp);
            double w1 = (atan2(2*(*rp), 1-t-u))/2;
            double w2, v;

            u = (*irp)+1; 
            v = (*irp)-1; 

            w2 = log((t + u*u)/(t + v*v))/4;
            
            *rp = w1;
            *irp = w2;
        }
        if (DegreesMode) { *rp *= RAD; *irp *= RAD; }
        break;
    case CALC_OP_MODEDEG:
        DegreesMode = TRUE;
        break;
    case CALC_OP_MODERAD:
        DegreesMode = FALSE;
        break;
    case CALC_OP_PI:
        Push();
        *rp = PI;
        *irp = 0;
        break;
    case CALC_OP_HMS:
        *rp = hms(*rp);
        break;
    case CALC_OP_R2P:
        *irp = 0;
        irp[1] = 0;
        RtoP(rp, rp + 1);
        if (DegreesMode) rp[1] *= RAD;
        break;
    case CALC_OP_FACTORIAL:
        *rp = factorial(*rp);
        *irp = 0;
        break;
    case CALC_OP_DMY:
        {
            unsigned int y, m, d;
            caldati(*rp, &y, &m, &d);
            *rp = y/1.0e6 + m/100.0 + d;
        }
        break;
    case CALC_OP_HOURS:
        *rp = hr(*rp);
        break;
    case CALC_OP_P2R:
        *irp = 0;
        irp[1] = 0;
        if (DegreesMode) rp[1] /= RAD;
        PtoR(rp, rp + 1);
        break;
    case CALC_OP_SUNSET:
        {
            Push();
            Push();
            CalcRiseAndSet(rp + 1, rp);
            irp[1] = 0;
            *irp = 0;
        }
        break;
    case CALC_OP_DAYS:
        {
            // assume *rp is in format dd.mmyyyy
            int d = *rp;
            double t = (*rp - d)*100;
            int m = t;
            int y = (t - m) * 10000;
            *rp = mjd(y, m, d);
            *irp = 0;
        }
        break;
    case CALC_OP_RECORD:

        // when we go into record, reset the menu to the first page
        // so that we get consistent results on replay.
        CurrentMenu = 0;
        KeyRecord();
        break;
    case CALC_OP_PLAY:

        // for playback put the menu onto the same page as for record.
        CurrentMenu = 0;
        KeyReplay();
        break;
    case CALC_OP_CONV:
        Conversions();
        break;
    case CALC_OP_PARALLEL:
        *rp=(rp[1]*(*rp))/(rp[1]+(*rp));
        *irp = 0;
        Drop();
        break;
    case CALC_OP_PLUS:
        *rp += rp[1];        //perform PLUS operation
        *irp += irp[1];
        Drop();
        break;
    case CALC_OP_MINUS:
        *rp = rp[1] - *rp;            //perform MINUS operation
        *irp= irp[1] - *irp;
        Drop();
        break;
    case CALC_OP_MULT:        //perform MULTIPLY operation
        {
#if 0
            double drp[2], dirp[2];
            double t[2], u[2];

            drp[0] = 0; drp[1] = *rp;
            dirp[0] = 0; dirp[1] = *irp;

            MulDpSp(drp, rp[1], t);     // t = rp * rp1
            MulDpSp(dirp, irp[1], u);   // u = irp * irp1
            SubDpDp(t, u, t);           // t = t - u;

            *irp = (*rp)*irp[1] + (*irp)*rp[1];
            *rp = t[1]; 

#else
            MULC(*rp, *irp, rp[1], irp[1]);
#endif
        }
        Drop();
        break;
    case CALC_OP_DIVIDE:        //perform DIVIDE operation

        if (!*irp)
        {
            double x = rp[1]/(*rp);
            *irp = irp[1]/(*rp);
            *rp = x;
        }
        else if (!*rp)
        {
            double x = irp[1]/(*irp);
            *irp = -rp[1]/(*irp);
            *rp = x;
        }
        else
        {
            // (a + i b) / (c + i d) = 
            // (a*c + b*d) / (c*c + d*d) + i (b*c – a*d) / (c*c + d*d)

            // see Numerical recipes, 3rd p226
            double t, s;
            if (fabs(*rp) >= fabs(*irp))
            {
                t = (*irp)/(*rp);                       // d/c
                s = (*rp + (*irp)*t);                   // c + d(*d/c)
                *rp = (rp[1]+irp[1]*t)/s;               // (a + b*t)/s
                *irp = (irp[1]-rp[1]*t)/s;              // (b-a*t)/s
            }
            else
            {
                t = (*rp)/(*irp);                       // c/d
                s = (*rp)*t+(*irp);                     // c*t+d
                *rp = (rp[1]*t + irp[1])/s;             // (a*t + b)/s
                *irp = (irp[1]*t - rp[1])/s;            // (b*t - a)/s
            }
        }
        Drop();
        break;
    case CALC_OP_BASE:
        BaseMode();
        break;
    case CALC_OP_ABS:
        if (!*irp)
            *rp = fabs(*irp);
        else
        {
            *rp = fabsC(rp, irp);
            *irp = 0;
        }
        break;
    case CALC_OP_COMPLEX_SPLIT:
        Push();
        rp[1] = *irp;
        irp[1] = 0;
        *irp = 0;
        break;
    case CALC_OP_COMPLEX_JOIN:
        *irp = rp[1];
        Drop();
        break;
    case CALC_OP_CONJUGATE:
        *irp = -*irp;
        break;
    case CALC_OP_REAL_PART:
        *irp = 0;
        break;
    }
}

