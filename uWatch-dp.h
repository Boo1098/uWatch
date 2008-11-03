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

/* Precision Doubling */

#ifndef __uwatch_dp_h__
#define __uwatch_dp_h__

#define D double

#define QUICK_TWO_SUM(a, b, s, err)     \
    s = a + b;                          \
    err = b - (s - a)

#define QUICK_TWO_DIFF(a, b, s, err)    \
    s = a - b;                          \
    err = (a - s) - b

/* Computes fl(a+b) and err(a+b).  */
#define TWO_SUM(a, b, s, err)                           \
{                                                       \
    D bb;                                               \
    s = a + b;                                          \
    bb = s - a;                                         \
    err = (a - (s - bb)) + (b - bb);                    \
}

#define TWO_DIFF(a, b, s, err)                          \
{                                                       \
    D bb;                                               \
    s = a - b;                                          \
    bb = s - a;                                         \
    err = (a - (s - bb)) - (b + bb);                    \
}

#define SPLIT_(a, hi, lo)                                \
{                                                       \
    D t = _split * a;                                    \
    hi = t - (t - a);                                   \
    lo = a - hi;                                        \
}

#define TWO_PROD(a, b, p, err)                                  \
{                                                               \
    D a_hi, a_lo, b_hi, b_lo;                                   \
    p = a * b;                                                  \
    SPLIT_(a, a_hi, a_lo);                                       \
    SPLIT_(b, b_hi, b_lo);                                       \
    err = a_hi*b_hi - p + a_hi*b_lo + a_lo*b_hi + a_lo*b_lo;    \
}


#define TWO_TWO_SUM(a0, a1, b0, b1, x0, x1)                     \
{                                                               \
    D _s1, _s2, _t1, _t2, _u1, _u2;                             \
    TWO_SUM(a1, b1, _s1, _s2);                                  \
    TWO_SUM(a0, b0, _t1, _t2);                                  \
    _s2 += _t1;                                                 \
    QUICK_TWO_SUM(_s1, _s2, _u1, _u2);                          \
    _s2 = _u2 + _t2;                                            \
    QUICK_TWO_SUM(_u1, _s2, x1, x0);                            \
}

#define TWO_TWO_DIFF(a0, a1, b0, b1, x0, x1)                    \
{                                                               \
    D _s1, _s2, _t1, _t2, _u1, _u2;                             \
    TWO_DIFF(a1, b1, _s1, _s2);                                 \
    TWO_DIFF(a0, b0, _t1, _t2);                                 \
    _s2 += _t1;                                                 \
    QUICK_TWO_SUM(_s1, _s2, _u1, _u2);                          \
    _s1 = _u1;                                                  \
    _s2 = _u2 + _t2;                                            \
    QUICK_TWO_SUM(_s1, _s2, x1, x0);                            \
}

#define TWO_TWO_PROD(a0, a1, b0, b1, x0, x1)            \
{                                                       \
    D _p1, _p2;                                         \
    TWO_PROD(a1, b1, _p1, _p2);                         \
    _p2 += a1*b0;                                       \
    _p2 += a0*b1;                                       \
    QUICK_TWO_SUM(_p1, _p2, x1, x0);                    \
}

#define TWO_ONE_SUM(a0, a1, b, x0, x1)                  \
{                                                       \
    D _s1, _s2;                                         \
    TWO_SUM(a1, b, _s1, _s2);                           \
    _s2 += a0;                                          \
    QUICK_TWO_SUM(_s1, _s2, x1, x0);                    \
}

#define TWO_ONE_PROD(a0, a1, b, x0, x1)                 \
{                                                       \
    D _p1, _p2;                                         \
    TWO_PROD(a1, b, _p1, _p2);                          \
    _p2 += a0*b;                                        \
    QUICK_TWO_SUM(_p1, _p2, x1, x0);                    \
}

#define TWO_TWO_DIV(a0, a1, b0, b1, x0, x1)             \
{                                                       \
    D q1, q2;                                           \
    D _r0, _r1;                                         \
    D v0, v1, v2, v3;                                   \
    D q3;                                               \
    q1 = a1/b1;                                         \
    TWO_ONE_PROD(b0, b1, q1, v2, v3);                   \
    TWO_TWO_DIFF(a0, a1, v2, v3, v0, v1);               \
    q2 = v1/b1;                                         \
    TWO_ONE_PROD(b0, b1, q2, v2, v3);                   \
    TWO_TWO_DIFF(v0, v1, v2, v3, _r0, _r1);             \
    q3 = _r1/b1;                                        \
    QUICK_TWO_SUM(q1, q2, v0, v1);                      \
    TWO_ONE_SUM(q2, q1, q3, x0, x1);                    \
}

#define TWO_ONE_DIV(a0, a1, b, x0, x1)                  \
{                                                       \
    D q1, q2;                                           \
    D _p1, _p2;                                         \
    D s, e;                                             \
    q1 = a1/b;                                          \
    TWO_PROD(q1, b, _p1, _p2);                          \
    TWO_DIFF(a1, _p1, s, e);                            \
    e = e + a0 - _p2;                                   \
    q2 = DIVOP(ADDOP(s, e), b);                         \
    QUICK_TWO_SUM(q1, q2, x1, x0);                      \
}

#define TWO_ONE_DIFF(a0, a1, b, x0, x1)                 \
{                                                       \
    D _s1, _s2;                                         \
    TWO_DIFF(a1, b, _s1, _s2);                          \
    _s2 += a0;                                          \
    QUICK_TWO_SUM(_s1, _s2, x1, x0);                    \
}

#define TWO_SQR(a, x0, x1)                              \
{                                                       \
    D hi, lo, t;                                        \
    x1 = a*a;                                           \
    SPLIT_(a, hi, lo);                                   \
    t = hi*lo;                                          \
    x0 = hi*hi - x1 + t + t + lo*lo;                    \
}

#define TWO_TWO_SQR(a0, a1, x0, x1)                     \
{                                                       \
    D _p1, _p2;                                         \
    D _s1, _s2;                                         \
    D t;                                                \
    TWO_SQR(a1, _p2, _p1);                              \
    t = a1*a0;                                          \
    _p2 += t + t - a0*a0;                               \
    QUICK_TWO_SUM(_p1, _p2, x1, x0);                    \
}

extern void MulDpDp(double* a, double* b, double* c);
extern void MulDpSp(double* a, double b, double* c);
extern void SubDpDp(double* a, double* b, double* c);

#endif // __uwatch_dp_h__
