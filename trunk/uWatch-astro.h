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

#ifndef __uwatch_astro_h__
#define __uwatch_astro_h__

#include        <Generic.h>     // BOOL

#ifndef MaxLCDdigits
#define MaxLCDdigits    16
#endif

extern unsigned int MJD;
extern BOOL DST;

extern void UpdateLCDline1(const char*);
extern void UpdateLCDline2(const char*);

extern void CalcRiseAndSet(double* rise, double* set);

#endif // __uwatch_astro_h__
