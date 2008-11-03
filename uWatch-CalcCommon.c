//********************************************************
// uWatch
// Common Calculator Functions
// Version 1.3
// Last Update: 12th June 08
// Written for the Microchip C30 Compiler
// Target Device: PIC24FJ64GA004 (44pin)
// Copyright(c) 2008 David L. Jones
// http://www.calcwatch.com
// EMAIL: david@alternatezone.com

// NOTE: This code is designed to be inserted inline into the uWatch-Main.C function
//       Most variables are passed as globals and are defined in uWatch-Main.C
//********************************************************

/*********************************************************
This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************************************/

#include <ctype.h>
#include "uWatch-astro.h"

extern void Conversions(void);
extern void HexEntry(void);
extern void UpdateXregDisplay(void);
extern void UpdateYregDisplay(void);
extern void UpdateDisplayRegs(void);
extern void CompleteXreg(void);
extern void ResetFlags(void);
extern double Factorial(double num);
extern BOOL StoreRecall(void);
extern void SignKey(void);
extern void KeyRecord(void);
extern void KeyReplay(void);
extern long mjd(int y, int m, int d);
extern void caldati(long mjd,
                    unsigned int* y, unsigned int* m, unsigned int* d);
extern void BaseMode(void);

void Push(void)
{
    Treg=Zreg; iTreg=iZreg;
    Zreg=Yreg; iZreg=iYreg;
    Yreg=Xreg; iYreg=iXreg;
}

//**********************************
// Drops the stack down and leaves Treg intact making it useful as a "constant" register
void DropStack(void)
{
    Yreg=Zreg; iYreg=iZreg;
    Zreg=Treg; iZreg=iTreg;
}

void PopStack()
{
    Xreg = Yreg; iXreg = iYreg;
    DropStack();
    Treg=0; iTreg=0;
}

//***********************************
// pushes the stack up and leaves the Xreg intact
void PushStackUp(void)
{
    Push();
    UpdateYregDisplay(); //Yreg has changed, so keep display register up to date
}


void RtoP(void)
{
    double temp;
    temp=sqrt((Yreg*Yreg)+(Xreg*Xreg));
    Xreg=atan(Xreg/Yreg);
    if (DegreesMode) Xreg *= RAD;
    Yreg=temp;
}

void PtoR(void)
{
    double temp;
    if (DegreesMode) Xreg /= RAD;
    temp=Yreg*cos(Xreg);
    Xreg=Yreg*sin(Xreg);
    Yreg=temp;
}

double hms(double h)
{
    return (90*h+100*(int)(h)+(int)(60*h))/250;
}

double hr(double x)
{
    return (250*x-60*(int)(x)-(int)(100*x))/90;
}

void Drop()
{
    DropStack();
    OperatorXY = OperatorYZ; // bring down any operator too
    OperatorYZ = 0;
}

// complex fabs
static double fabsC(double* a, double* b)
{
    // use numerically stable method, Numerical Recipies 3rd p226
    double fa, fb;

    fa = fabs(*a);
    fb = fabs(*b);

    if (!*a) return fb;
    if (!*b) return fa;

    if (fa >= fb)
        return fa*sqrt((*b)/(*a)+1.0);
    else
        return fb*sqrt((*a)/(*b)+1.0);
}

static void sinandcos(double v, double* sa, double* ca)
{
    // need to find a more efficient way to calculate both sin and cos
    *sa = sin(v);
    *ca = cos(v);
}

// Raw operation
void Operation(int op)
{
    double* rp = Regs;
    double* irp = iRegs;
    switch(op)
    {
    case CALC_OP_RECIPROCAL:
        if (!*irp)
            *rp = 1.0/(*rp);
        else
        {
            // 1 / (c + i d) = c / (c*c + d*d) - i d / (c*c + d*d)
            double d = (*rp)*(*rp) + (*irp)*(*irp);
            *rp = (*rp)/d;
            *irp = -(*irp)/d;
        }
        break;
    case CALC_OP_SQUARE:
        if (!*irp)
            *rp *= (*rp);
        else
        {
            double x = (*rp)*(*rp) - (*irp)*(*irp);
            *irp = 2*(*rp)*(*irp);
            *rp=x;
        }
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
        if (*rp >= 0 && !*irp)
        {
            *rp=log(*rp);           
        }
        else
        {
            double r = fabsC(rp, irp);
            *irp = atan2(*irp, *rp);
            *rp = log(r);
        }
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
    case CALC_OP_NPOW:
        *rp=pow(rp[1],*rp);         
        Drop();
        break;
    case CALC_OP_NROOT:
        *rp=pow(rp[1],1.0/(*rp));                   
        Drop();
        break;
    case CALC_OP_LN10:
        *rp=log10(*rp);
        break;
    case CALC_OP_10X:
        *rp = pow(10, *rp);
        break;
    case CALC_OP_SIN:
        if (DegreesMode) *rp /= RAD;
        *rp=sin(*rp);
        break;
    case CALC_OP_COS:
        if (DegreesMode) *rp /= RAD;
        *rp=cos(*rp);
        break;
    case CALC_OP_TAN:
        if (DegreesMode) *rp /= RAD;
        *rp=tan(*rp);
        break;
    case CALC_OP_ASIN:
        *rp=asin(*rp);
        if (DegreesMode) *rp *= RAD;
        break;
    case CALC_OP_ACOS:
        *rp=acos(*rp);
        if (DegreesMode) *rp *= RAD;
        break;
    case CALC_OP_ATAN:
        *rp=atan(*rp);
        if (DegreesMode) *rp *= RAD;
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
        RtoP();
        break;
    case CALC_OP_FACTORIAL:
        // XX needs complex factorial
        *rp = Factorial(*rp);
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
        PtoR();
        break;
    case CALC_OP_SUNSET:
        {
            Push();
            Push();
            CalcRiseAndSet(rp + 1, &*rp);
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
        Drop();
        break;
    case CALC_OP_PLUS:
        *rp= *rp + rp[1];        //perform PLUS operation
        *irp= *irp + irp[1];
        Drop();
        break;
    case CALC_OP_MINUS:
        *rp= rp[1] - *rp;            //perform MINUS operation
        *irp= irp[1] - *irp;
        Drop();
        break;
    case CALC_OP_MULT:        //perform MULTIPLY operation
        {
            // XX single precision version only
            double x = (*rp)*rp[1] - (*irp)*irp[1];
            *irp = (*rp)*irp[1] + (*irp)*rp[1];
            *rp = x;
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
    }
}


// Perform operation and display
void Operate(int op)
{
    CompleteXreg();             //enter value on stack if needed
    Operation(op);              // operate on X & Y
    UpdateDisplayRegs();        //update display again
}

static void ProcessNumberKey(char digit)
{
    unsigned int l = strlen(DisplayXreg);
 
    //if this is the first digit pressed
    if (ValueEntered==TRUE)
    {
        //check to see if we don't have to overwrite the Xreg
        if (EnableXregOverwrite==FALSE)
        {
            PushStackUp(); //push the stck up for the first key entry, i.e. *don't* overwrite the Xreg
            UpdateLCDline1(DisplayYreg);
        }
            
        l = 0;
        EnableXregOverwrite=FALSE;  //disable overwriting the Xreg for future key presses
    }

    if (l<XBufSize)
    {
        DisplayXreg[l] = digit;
        DisplayXreg[l+1] = 0;  // ensure termination

        ValueEntered=FALSE;
    }
}

void EnterComplex()
{
    if (!ComplexIncluded)
    {
        strcat(DisplayXreg, "+i");

        // we are entering the i part, clear other entry flags
        ComplexIncluded = TRUE;
        ExponentIncluded = FALSE;
        DecimalIncluded = FALSE;
    }
}

// handle number input. return key if not handled (0 otherwise)
int EnterNumber(int Key)
{
    // handle numbers
    int l;
    int c = ReturnNumber(Key);
    if (c >= 0)
    {
        // ignore anything but 0 & 1 in binary mode
        if (CalcDisplayBase == 2)
        {
            if (c > 1)
                return 0;
        }
        
        // key is 0 to 9
        ProcessNumberKey('0' + c);
        UpdateLCDline2(DisplayXreg);
        Key = 0; 
    }

    // how many chars in input buffer so far?
    l = strlen(DisplayXreg);

    switch (Key)
    {
    case KeyPoint: //user has pressed the DECIMAL POINT key
        Key = 0;
        if (l<XBufSize-1) // allow for 2 
        {
            //only do if decimal point does not already exist AND there is no exponent
            if (ExponentIncluded)
            {
                // interpret '.' after 'e' as '+i'
                EnterComplex();
            }
            else
            {
                if (DecimalIncluded)
                {
                    if (DisplayXreg[l-1] == '.')
                    {
                        // last was '.', re-interpret as +i
                        DisplayXreg[l-1] = 0;
                    }
                    EnterComplex();
                }
                else
                {
                    // insert a 0 if not following a digit
                    if (l == 0 || !isdigit(DisplayXreg[l-1])
                        || ValueEntered == TRUE)
                    {
                        //decimal point needs a 0 added to the start
                        ProcessNumberKey('0');
                    }
                    
                    ProcessNumberKey('.');
                    UpdateLCDline2(DisplayXreg);
                    DecimalIncluded=TRUE;
                }
            }
            UpdateLCDline2(DisplayXreg);
        }
        break;
    case KeyRP:
        if (RPNmode)
        {
            // overload the right bracket to be single backspace in RPN 
            // mode ONLY.
            if (l > 0 && !ValueEntered)
            {
                if (DisplayXreg[l-1] == 'e')
                {
                    ExponentIncluded = FALSE;
                }
                else if (DisplayXreg[l-1] == '.')
                {
                    DecimalIncluded = FALSE;
                }
                DisplayXreg[l-1] = 0;
                UpdateLCDline2(DisplayXreg); //update the display
            }
        }
        break;
    case KeyEXP: 

        Key = 0;
        if (l < XBufSize-1) // space for 2
        {
            if ( CalcDisplayBase == 16 )
                HexEntry();
            else
            {
                if (!ExponentIncluded)  //can't add exponent twice
                {
                    ExponentIncluded=TRUE;
                
                    // EXP is first key pressed, so add a 1 to the front
                    if (l == 0 || !isdigit(DisplayXreg[l-1]) ||
                        ValueEntered == TRUE)
                        ProcessNumberKey('1');
                    
                    ProcessNumberKey('e');
                    UpdateLCDline2(DisplayXreg);
                }
            }
        }
        break;
    case KeySign: 
        //changes the sign of the mantissa or exponent
        Key = 0;
        SignKey();
        break;
    case KeyClear: 
        // only handle one level of clear. ie Clear Entry
        if (ValueEntered==FALSE)    
        {
            Key = 0;
            Xreg = 0; iXreg = 0;
            UpdateDisplayRegs();
            ResetFlags();
            EnableXregOverwrite = TRUE;
        }
        break;
    }
    return Key;
}

// tidy as many unnecessary chars as possible from a sci number
static void tidyNumber(char* p)
{
    p = strchr(p, 'e'); // exponent?
    if (p)
    {
        char* q;
        char* p0;
        ++p;
        p0 = p;
        if (*p == '+')
        {
            q = p++;
            while ((*q++ = *p++) != 0);
        }
        if (*p0 == '-') ++p0; // '-' allowed

        if (*p0 == '0') // leading zero in exponent
        {
            q = p0++;
            while ((*q++ = *p0++) != 0);
        }
    }
}

void FormatValue(char* dest,
                 double value, double ivalue,
                 int precision )
{
    // strcpy(dest,"                ");     //blank the string first

    char base = (WatchMode == WATCH_MODE_CALC) ? CalcDisplayBase : 10;

    int index;
    int p=0;
    switch ( base )
    {
    case 2:
        {   
            unsigned int uval=0;
            char tmp[MaxLCDdigits+1];
            memset( tmp, 0, sizeof(tmp));

            if ( (value > 32767) || (value < -32768) ) 
            {
                strcpy( dest, "  * OVERFLOW *" );
            }
            else
            {
                if ( value < 0 )
                {
                    uval  = (unsigned int)(-1 * value);
                    uval  = ~uval + 1;
                }
                else 
                    uval = value;

                if ( uval == 0 )
                {
                    sprintf( dest, "%ib", 0 );
                }
                else 
                {
                    p = 15;
    
                    for ( index=0; (index < 16) && uval; index++ )
                    {
                        if ( uval & 1  )
                            tmp[p--] = '1';
                        else
                            tmp[p--] = '0';
    
                        uval = (uval >> 1);             
                    }
                    
                    strcpy( dest, tmp+(p+1) );
    
                    if ( index < 15 )
                    {
                        dest[index] = 'b';
                        dest[index+1] = 0;
                    }
                }
            }
        }
        break;
        case 10:
            {
                if (ivalue == 0)
                {
                    sprintf(dest,"%.10g", value);
                }
                else
                {
                    int l;
                    char c = '+';
                    int id = 6;
                    
                    if (ivalue < 0)
                    {
                        ivalue = -ivalue;
                        c = '-';
                    }

                    // textify the real part
                    sprintf(dest,"%.6g", value);

                    // tidy to save precious chars
                    tidyNumber(dest);
                    
                    l = strlen(dest);                        
                    dest[l++] = c;
                    dest[l++] = 'i';
                    dest[l] = 0;
                    
                    if (ivalue != 1)
                    {
                        // now fit as much of the ipart as we can
                        do
                        {
                            sprintf(dest + l,"%.*g", id--, ivalue);
                            tidyNumber(dest+l);
                            if (!id) break; // fail safe!
                        } while (strlen(dest) > MaxLCDdigits);
                    }
                }
            }
            break;
        case 16:
        {
            unsigned long long ulVal=0;
            double max = pow( 2, 64 );
            if ( fabs(value) > max )
            {
                strcpy( dest, "  * OVERFLOW *" );
            }
            else 
            {
                if ( value < 0 )
                {
                    ulVal  = (unsigned long long)(-1 * value);
                    ulVal  = ~ulVal + 1;
                }
                else 
                    ulVal = value;
    
    
                if ( ulVal )
                {           
                    char tmp[17];
                    memset( tmp, 0, 17 );
                    p=15;
                    for( index=0; (index < 16) && ulVal; index++ )
                    {
                        int d = ulVal % 16;
                
                        char c=0;
                
                        if ( d < 10 )
                            c = '0' + d;
                        else
                            c = 'A' + (d-10);
                        
                        tmp[p--] = c;
                
                        ulVal = ulVal >> 4;
                    }
        
                    strcpy( dest, tmp+(p+1) );

                    if ( index < 15 )
                    {
                        dest[index] = 'h';
                        dest[index+1] = 0;
                    }
                }
                else
                {
                    strcpy( dest, "0h" );
                }
            }

        }
        break;
    }
} 
//***********************************
//Converts both X&Y regs from real numbers into display strings and then displays them
//Also displays the current algebraic operator
void UpdateDisplayRegs(void)
{
    char c;
    unsigned int i;
    UpdateXregDisplay();
    UpdateYregDisplay();

    if (!RPNmode)
    {
        //the rest of this code is for the ALG mode only
        c = ' ';
        if (OperatorXY)
        {
            switch (OperatorXY)
            {
            case CALC_OP_PLUS:  c='+'; break;
            case CALC_OP_MINUS: c='-'; break;
            case CALC_OP_MULT:  c='x'; break;
            case CALC_OP_DIVIDE:  c='/'; break;
            case CALC_OP_NPOW:
            case CALC_OP_NROOT:
                c = '^'; 
                break;
            case CALC_OP_P2R:
                c = 'p';
                break;
            case CALC_OP_R2P:
                c = 'r';
                break;
            case CALC_OP_PARALLEL:
                c = 'l';
                break;
            default:
                c = 'o'; 
            }
        }

    //replace NULL with space
        for(i=0; i<MaxLCDdigits; i++)
            if (!DisplayYreg[i]) DisplayYreg[i]=' ';

        //display the operator character
        DisplayYreg[15]=c;      
        DisplayYreg[16]=0;

        for (i = 0; i < 7; ++i)
        {
            if (!OperatorsXY[i]) break;
        }
        
        if (i > 1)
        {
            //display the operator character    
            DisplayYreg[13]='(';
            DisplayYreg[14]='0' + i - 1;
        }
    }

    UpdateLCDline1(DisplayYreg);
    UpdateLCDline2(DisplayXreg);
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

//***********************************
//perform the factorial operation and return the result
double Factorial(double num)
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

int xtio( char c )
{
    if ( (c >= '0') && (c <= '9') )
    {
        char digit[2];
        digit[1] = 0;
        digit[0] = c;

        return atoi( digit );
    }
    else
    {
        if ( (c >= 'A') && (c <= 'F') )
        {
            return ( c - 'A') + 10;
        }
    }
    return 0;
}


double ConvertDisplay(char* DisplayString, double* ip)
{
    double result;
    int base = (WatchMode == WATCH_MODE_CALC) ? CalcDisplayBase : 10;
    *ip = 0;
    switch ( base )
    {
        case 2:
            {
                int i=0;
                int length=strlen(DisplayString);
                double h=0.0;
                
                for( ; i < length; i++ )
                {
                    if ( DisplayString[(length-1)-i] == '1' )
                    {
                        h += (int)pow(2, i);
                    }   
                }
                result = h; 
            }
            break;
        case 10:
            {
                char* p = strchr(DisplayString, 'i');
                if (p)
                {
                    // contains imaginary part
                    char c = *--p; // +/- preceeding 
                    *p = 0;
                    result = atof( DisplayString );
                    if (!p[2])
                        *ip = 1; // +i is 1
                    else
                        *ip = atof(p + 2);

                    if (c == '-')
                        *ip = -*ip;

                    // put back
                    *p = c;
                }
                result = atof( DisplayString ); 
            }
            break;
        case 16:
        {
            int len = strlen( DisplayString );
            double val=0;
            int i;
            for( i=0; i < len; i++ )
            {
                int d = xtio( DisplayString[(len-1)-i] );
                
                double power = pow( 16, i); 
                val += (power * d);
            }
            result = val;
        }
        break;
    }
    return result;
}

//***********************************
// Checks to see if the Xreg has been ENTERed onto the stack
// If not then it it adds the value to the stack
// This routien is needed because the displayed Xreg value is not stored in the real Xreg variable yet
void CompleteXreg(void)
{
    //if ENTER has not been pressed then do it for them
    if (ValueEntered==FALSE)
    {
        Xreg=ConvertDisplay(DisplayXreg, &iXreg);
        ResetFlags();
    }
    EnableXregOverwrite=FALSE;
}

//***********************************
// Converts the Xreg double value into a string for the DisplayXreg
void UpdateXregDisplay(void)
{
    FormatValue( DisplayXreg, Xreg, iXreg, 10 );
}

//***********************************
// Converts the Yreg double value into a string for the DisplayYreg
void UpdateYregDisplay(void)
{
    FormatValue( DisplayYreg, Yreg, iYreg, 9 );
}


//***********************************
//resets all the flags after a calculation, ready for the next number to be entered by the user
void ResetFlags(void)
{
    ValueEntered=TRUE;
    DecimalIncluded=FALSE;
    ExponentIncluded=FALSE;
    ComplexIncluded = FALSE;
    EnableXregOverwrite=FALSE;      //calling function must overwirte this if needed
}

//***********************************
//processes the store/recall function
BOOL StoreRecall(void)
{
    char KeyPress2;
    int num;
    BOOL rcl = FALSE;

    // return TRUE if RCL

    //this operation discards any value currently in the display register
    UpdateLCDline1("RECALL REG 0-9 ?");     //ask for a register number
    do KeyPress2=KeyScan(); while(KeyPress2==0);    //get the users response        
    if (KeyPress2==KeyRCL) //check for 2nd press of RCl key to enter STO mode
    {                                       // do the STO function
        UpdateLCDline1(" STORE REG 0-9 ?");     //ask for a register number
        do KeyPress2=KeyScan(); while(KeyPress2==0);    //get the users response        
        num = ReturnNumber(KeyPress2);
        if(num >= 0)
        {
            //store the Xreg value in the appropriate Sreg
            Sreg[num]=Xreg; iSreg[num] = iXreg;
            //UpdateLCDline1("  VALUE STORED  ");
            //DelayMs(100);           //small visual delay
        }
    }
    else                //do the RCL function
    {
        num = ReturnNumber(KeyPress2);
        if (num >= 0)
        {
            Push();

            //restore the Xreg value from the Sreg
            Xreg=Sreg[num]; iXreg = Sreg[num];
            ValueEntered=TRUE;
            //UpdateLCDline1(" VALUE RECALLED ");
            //DelayMs(100);           //small visual delay
            rcl = TRUE;
        }
    }
    return rcl;
}

//**********************************
// processes the sign key
void SignKey(void)
{
    int n = strlen(DisplayXreg);
    char* p;
    char* q;

    // adjust any exponent 
    p = strrchr(DisplayXreg, 'e');
    q = strchr(DisplayXreg, 'i');
    
    //do this if there is an exponent already
    if (!ValueEntered && (p && q < p))
    {
        ++p; // to sign

        //minus already exists so lets remove it
        if (*p == '-')
        {
            q = p + 1;
            while ((*p++ = *q++) != 0) ;
        }
        else //minus does not exist yet so lets add one
        {
            char* k;
            q = DisplayXreg + n + 1;
            k = q;
            while (q != p)
                *q-- = *--k;

            *p = '-';
        }
    }
    else 
    {
        // complex?
        if (!ValueEntered && q)
        {
            --q; // to sign
            if (*q == '+') *q = '-';
            else *q = '+';
        }
        else  // adjust sign of mantissa
        {
            p = DisplayXreg;

            //if this is the first digit pressed
            if (ValueEntered)
            {
                //check to see if we don't have to overwrite the Xreg
                if (EnableXregOverwrite)
                {
                    *p = 0; //clear (overwrite)what was in the Xreg
                    n = 0;

                    //disable overwriting the Xreg for future key presses
                    EnableXregOverwrite=FALSE;
                    ValueEntered = FALSE;
                }
            }
    
            if (*p == '-')
            {
                //shift all characters one space left to remove existing minus sign
                q = p + 1;
                while ((*p++ = *q++) != 0) ;
            }
            else
            {
                char* k;
                q = p + n + 1;
                k = q;
                while (q != p)
                    *q-- = *--k;
            
                *p = '-';
            }
        }
    }

    if (ValueEntered)
    {
        // perform a sign changed on entered value and leave
        // as entered.
        ValueEntered = FALSE;
        CompleteXreg();
    }
    
    UpdateLCDline2(DisplayXreg);        //update the display
}

void KeyRecord(void)
{
    unsigned int num,c;
    char s[MaxLCDdigits+1];

    UpdateLCDline1("Keystroke Rec");
    UpdateLCDline2("Program#(00-59)");

    //now get a two digit number from the user
    num=GetNumBCD();
    if (num < 0) return; // escape

    num = BCDtoDEC(num);
    if (num>59) num=59;
    
    //set the memory pointer to after the header info
    MemPointer=(num*1024)+16;

    sprintf(s,"Program %2i",num);
    for(c=0;c<15;c++) I2CmemoryWRITE((MemPointer-16)+c,s[c]);
    ProgRec=TRUE;                       //switch on keystroke programming mode
    UpdateLCDline1(s);
    UpdateLCDline2("Rec Keys Now...");
    DelayMs(1000);
}

void KeyReplay(void)
{
    unsigned int num,c;
    char s[MaxLCDdigits+1];

    UpdateLCDline1("Keystroke Play");
    UpdateLCDline2("Program#(00-59)");

    //now get a two digit number from the user
    num = GetNumBCD();
    if (num < 0) return; // escape
    
    num = BCDtoDEC(num);
    if (num>59) num=59;

    //set the memory pointer to after the header info
    MemPointer=(num*1024)+16;
    for(c=0;c<15;c++) s[c]=I2CmemoryREAD((MemPointer-16)+c);
    s[16]=NULL;
    ProgPlay=TRUE; //switch on keystroke programming mode
    UpdateLCDline1(s);
    UpdateLCDline2("Running...");
    DelayMs(1000);
}



//**************************************
// enter a HEX digit A-F onto the current Xreg string
void HexEntry(void)
{
    int KeyPress2;
    char digit[2];
    UpdateLCDline1(" F1=A F2=B F3=C ");
    UpdateLCDline2(" F4=D F5=E F6=F ");
    do KeyPress2=KeyScan(); while(KeyPress2==0);
    switch(KeyPress2)
    {
    case Key7: {digit[0]='A'; break;}
    case Key8: {digit[0]='B'; break;}
    case Key9: {digit[0]='C'; break;}
    case Key4: {digit[0]='D'; break;}
    case Key5: {digit[0]='E'; break;}
    case Key6: {digit[0]='F'; break;}
    default:
        {
            UpdateLCDline1(DisplayYreg);
            UpdateLCDline2(DisplayXreg);
            return;         
        }
    }
        
    if (strlen(DisplayXreg)<MaxLCDdigits)
    {
        ProcessNumberKey( digit[0] );
        UpdateLCDline1(DisplayYreg);
        UpdateLCDline2(DisplayXreg);
    }
}   

static const char* ConversionsMenu[] = 
{
    "mils -> mm",
    "mm -> mils",
    "Deg F -> Deg C",
    "Deg C -> Deg F",
    "kg -> lb",
    "lb -> kg"
};

//**********************************
// display the conversions menu
void Conversions(void)
{
    int Mode;
    Mode= DriveMenu("CONV: +/- & ENT", ConversionsMenu, DIM(ConversionsMenu));
    switch(Mode)                
    {
    case 0: 
        {
            Xreg=Xreg*0.0254;
            break;
        }
    case 1:
        {
            Xreg=Xreg*(1.0/0.0254);
            break;
        }
    case 2: 
        {
            Xreg=(Xreg-32.0)*5.0/9.0;
            break;
        }
    case 3:
        {
            Xreg=Xreg*9.0/5.0+32.0;
            UpdateDisplayRegs();    //update display again
            break;
        }
    case 4:
        {
            Xreg=Xreg*2.2046226218487757;
            break;
        }
    case 5:
        {
            Xreg=Xreg*0.45359237;
            break;
        }
    }
} 

void BaseMode(void)
{
    int KeyPress2;
    UpdateLCDline1(" Bin  Dec  Hex  ");
    UpdateLCDline2("                ");
    do KeyPress2=KeyScan(); while(KeyPress2==0);
    switch(KeyPress2)
    {
    case Key7: {CalcDisplayBase = 2; break;}
    case Key8: {CalcDisplayBase =10; break;}
    case Key9: {CalcDisplayBase =16; break;}
    default:
        {
            UpdateLCDline1(DisplayYreg);
            UpdateLCDline2(DisplayXreg);
            return;         
        }
    }
        
    CompleteXreg();
    return;
}
