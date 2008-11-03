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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "uWatch-dp.h"

static double _split = (double)((1<<27)+1);


void MulDpDp(double* a, double* b, double* c)
{
    TWO_TWO_PROD(a[0], a[1], b[0], b[1], c[0], c[1]);    
}

void MulDpSp(double* a, double b, double* c)
{
    TWO_ONE_PROD(a[0], a[1], b, c[0], c[1]);        
}

void SubDpDp(double* a, double* b, double* c)
{
    TWO_TWO_DIFF(a[0], a[1], b[0], b[1], c[0], c[1]);        
}





