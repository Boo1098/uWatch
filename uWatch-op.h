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

/* Calculator Operation function implementation */

#ifndef __uwatch_op_h__
#define __uwatch_op_h__

// 0=sunday, 1=monday etc.
#define DAYOFWEEK(_x)    (((_x)+6)%7)

#define PI              3.141592653589793238462643
#define RAD             (180.0/PI)

extern int DegreesMode;
extern double Regs[4], iRegs[4];
extern int CurrentMenu;
extern int RPNmode;

extern void Operation(int op);
extern void caldati(long mjd,
                    unsigned int* y, unsigned int* m, unsigned int* d);
extern int moonPhase(int year, int month, int day);
extern long mjd(int y, int m, int d);
extern double hms(double h);
extern double hr(double x);

// should be in calc-common.h
extern void Push();
extern void Drop();
extern void KeyRecord(void);
extern void KeyReplay(void);
//extern int Conversions(void);
//extern void BaseMode(void);
extern void SwapXY();

/* Operation Codes */

#define CALC_OP_NULL            0
#define CALC_OP_RECIPROCAL      1
#define CALC_OP_SQUARE          2
#define CALC_OP_SQRT            3
#define CALC_OP_LN              4
#define CALC_OP_EXP             5
#define CALC_OP_NPOW            6
#define CALC_OP_NROOT           7
#define CALC_OP_LN10            8
#define CALC_OP_10X             9
#define CALC_OP_SIN             10
#define CALC_OP_COS             11
#define CALC_OP_TAN             12
#define CALC_OP_ASIN            13
#define CALC_OP_ACOS            14
#define CALC_OP_ATAN            15
#define CALC_OP_MODEDEG         16
#define CALC_OP_MODERAD         17
#define CALC_OP_PI              18
#define CALC_OP_HMS             19
#define CALC_OP_R2P             20
#define CALC_OP_FACTORIAL       21
#define CALC_OP_DMY             22
#define CALC_OP_HOURS           23
#define CALC_OP_P2R             24
#define CALC_OP_SUNSET          25
#define CALC_OP_DAYS            26
#define CALC_OP_RECORD          27
#define CALC_OP_PLAY            28
#define CALC_OP_CONV            29
#define CALC_OP_PARALLEL        30
#define CALC_OP_PLUS            31
#define CALC_OP_MINUS           32
#define CALC_OP_MULT            33
#define CALC_OP_DIVIDE          34
#define CALC_OP_BASE		35
#define CALC_OP_ABS             36
#define CALC_OP_COMPLEX_SPLIT   37
#define CALC_OP_COMPLEX_JOIN    38
#define CALC_OP_CONJUGATE       39
#define CALC_OP_REAL_PART       40
#define CALC_OP_HYP_MENU        41
#define CALC_OP_HYP_SIN         42
#define CALC_OP_HYP_COS         43
#define CALC_OP_HYP_TAN         44
#define CALC_OP_HYP_ASIN        45
#define CALC_OP_HYP_ACOS        46
#define CALC_OP_HYP_ATAN        47
#define CALC_OP_BASE2           48
#define CALC_OP_BASE10          49
#define CALC_OP_BASE16          50

#define CALC_OP_CONV_MILS_TO_MM 51
#define CALC_OP_CONV_MM_TO_MILS 52
#define CALC_OP_CONV_FC         53
#define CALC_OP_CONV_CF         54
#define CALC_OP_CONV_KG_LB      55
#define CALC_OP_CONV_LB_KG      56

#define CALC_OP_LOGIC_AND       57
#define CALC_OP_LOGIC_OR        58
#define CALC_OP_LOGIC_XOR       59
#define CALC_OP_LOGIC_NOT       60
#define CALC_OP_LOGIC_NOR       61
#define CALC_OP_LOGIC_NAND      62

#define CALC_OP_LIGHT           63
#define CALC_OP_AVOGADRO        64
#define CALC_OP_E               65

#define CALC_OP_BASE8           66

#endif // __uwatch_op_h__
