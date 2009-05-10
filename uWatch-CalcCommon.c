
/********************************************************/
// uWatch
// Common Calculator Functions
// Version 1.6
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
#include "uWatch-op.h"
#include "def.h"
#include "menu.h"
#include "characterset.h"
#include "calculator.h"
#include "uWatch-LCD.h"

void FormatValue(char* dest,
                 double value, double ivalue,
                 int space, BOOL tidy, BOOL truncate );


#define NUMBER_BASE 2
#define NUMBER_EXTEND 0


int HexEntry();

extern void UpdateXregDisplay(void);
extern void UpdateYregDisplay(void);
//extern void UpdateDisplayRegs(void);
extern void CompleteXreg(void);
extern void ResetFlags(void);
extern void StoreRecall(void);
extern void SignKey(void);
extern void UpdateDisplayRegX();


void Push()
{
    Treg=Zreg; iTreg=iZreg;
    Zreg=Yreg; iZreg=iYreg;
    Yreg=Xreg; iYreg=iXreg;
}

void PushOp(int op)
{
    OperatorZT = OperatorYZ;
    OperatorYZ = OperatorXY;
    OperatorXY = op;
}

void PopOp()
{
    OperatorXY = OperatorYZ; // bring down any operator too
    OperatorYZ = OperatorZT;
    OperatorZT = 0;
}

//**********************************
// Drops the stack down and leaves Treg intact making it useful as a "constant" register
void DropStack(void)
{
    Yreg=Zreg; iYreg=iZreg;
    Zreg=Treg; iZreg=iTreg;
}

void Clt()
{
    Treg=0; iTreg=0;
}

void PopStack()
{
    Xreg = Yreg; iXreg = iYreg;
    DropStack();
    Clt();
}

//***********************************
// pushes the stack up and leaves the Xreg intact
void PushStackUp(void)
{
    Push();
    UpdateYregDisplay(); //Yreg has changed, so keep display register up to date
}

void Drop()
{
    DropStack();
    if (!RPNmode)
        Clt();        // dont propagate T in ALG.
}


// clear X reg.
void Clx()
{
    Xreg = 0; iXreg = 0;
}

void SwapXY()
{
    //swap and X and Y regs
    double TEMPreg=Xreg; 
    double iTEMPreg = iXreg;			

    Xreg=Yreg; iXreg = iYreg;
    Yreg=TEMPreg; iYreg = iTEMPreg;
}

// Perform operation and display
void Operate(int op)
{
    CompleteXreg();             //enter value on stack if needed
    Operation(op);              // operate on X & Y
    UpdateDisplayRegs();        //update display again
}

extern void reduce(int lev);

int OperatePrecedence( int op ) {

    int p = opPrec( op );
    if ( !p || RPNmode )
        Operate( op );

    else {

        // Algebraic calculator has to deal with operator precedence

        reduce(p);
    
        // push OP
        ResetFlags();
        PushOp( op );
    
        // and values
        Push();
        Clx();
    
        EnableXregOverwrite = TRUE;
        UpdateDisplayRegs();
    }

    return MODE_EXIT;
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

void ChangeSign()
{
    // if not entered, enter it!
    CompleteXreg();

    Xreg = -Xreg;
    iXreg = -iXreg;
}

// handle number input. return key if not handled (0 otherwise)
int EnterNumber(int Key)
{
    // handle numbers
    int l;
    int c = ReturnNumber(Key);
    if (c >= 0 && c <= 9)
    {
        if (( CalcDisplayBase == 2 && c > 1 )        // limit binary entry
            || ( CalcDisplayBase == 8 && c > 7 ))        // limit octal entry
            return 0;
        
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
            if ( CalcDisplayBase == 16 )
                HexEntry();
            else if ( CalcDisplayBase == 10 ) {


                //only do if decimal point does not already exist AND there is no exponent
                char* p = DisplayXreg + l - 1;
                if (ExponentIncluded)
                {
                    // interpret '.' after 'e' as '+i'
                    EnterComplex();
                }
                else
                {
                    if (DecimalIncluded)
                    {
                        if (!ComplexIncluded)
                        {
                            // last was '.', re-interpret as +i
                            if (*p == '.') *p = 0;
                            EnterComplex();
                        }
                    }
                    else
                    {
                        // insert a 0 if not following a digit
                        if (l == 0 || !isdigit(*p) || ValueEntered == TRUE)
                        {
                            //decimal point needs a 0 added to the start
                            ProcessNumberKey('0');
                        }
                        
                        ProcessNumberKey('.');
                        UpdateLCDline2(DisplayXreg);
                        DecimalIncluded=TRUE;
                    }
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
                char* p = DisplayXreg + l - 1;
                if (*p == 'e')
                {
                    ExponentIncluded = FALSE;
                }
                else if (*p == '.')
                {
                    DecimalIncluded = FALSE;
                }
                else if (*p == 'i')
                {
                    if (strchr(DisplayXreg, '.'))
                        DecimalIncluded = TRUE;
                    if (strchr(DisplayXreg, 'e'))
                        ExponentIncluded = TRUE;

                    ComplexIncluded = FALSE;
                    
                    // clear +/- before i as well
                    p[-1] = 0;
                }

                *p = 0;
                UpdateLCDline2(DisplayXreg); //update the display
            }
        }
        break;

    case KeyEXP: 

        // IF the number displayed is too big (indicated by a < indicator at left side)
        // then the EXP key is a trigger to display the extended number with scrolling viewer
        // Note, this is only operational in base 2,8,16 at the moment

        Key = 0;

        if ( *DisplayXreg == '<' ) {

            char *longstr = displayBuffer+200;
            FormatValue( longstr, Xreg, iXreg, MaxLCDdigits, squash, FALSE );
            int sel = strlen( longstr ) - 16;
            viewString( 0, longstr, &sel, 2 );
            UpdateLCDline2( DisplayXreg );
            break;
        }

        if (l < XBufSize-1) // space for 2
        {
//            if ( CalcDisplayBase == 16 )
//                HexEntry();
//            else
//            {
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
//            }
        }
        break;
    case KeySign: 
        //changes the sign of the mantissa or exponent
        Key = 0;
        if (ValueEntered)
        {
            ChangeSign();
            UpdateDisplayRegX();
        }
        else
            SignKey();
        break;
    case KeyClear: 
        // only handle one level of clear. ie Clear Entry
        if (!ValueEntered)
        {
            Key = 0;
            Clx();
            UpdateDisplayRegX();
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
    if (*p == '-') ++p; // skip sign
    if (*p == '0' && p[1] == '.')
    {
        // 0.xxx
        strcpy(p, p+1);
    }
    
    p = strchr(p, 'e'); // exponent?
    if (p)
    {
        ++p;
        if (*p == '+')
            strcpy(p, p+1);

        if (*p == '-') ++p; // '-' allowed

        if (*p == '0') // leading zero in exponent
            strcpy(p, p+1);
    }
}

BOOL squash = FALSE;
void FormatValue(char* dest,
                 double value, double ivalue,
                 int space, BOOL tidy, BOOL truncate )
{
    char base = (WatchMode == WATCH_MODE_CALC || WatchMode == WATCH_MODE_CALC_MENU ) ? CalcDisplayBase : 10;


    int shift = 1;
    switch ( base )
    {

        case 16:
            shift++;
        case 8:
            shift += 2;
        case 2:

         {
        
            const char *digit = "0123456789ABCDEFGH";
            
            double max = pow( 2, 64 );
            if ( fabs(value) > max )
                strcpy( dest, "  * OVERFLOW *" );
            else {
            
                unsigned long long uval;
                if ( value < 0 )
                {
                    uval  = (unsigned long long)(-1 * value);
                    uval  = ~uval + 1;
                }
                else 
                    uval = value;
        
                // Kind of clever -- builds the number 'backwards' into a string buffer
            
                char *p = displayBuffer + 70;       //arbitrary, just long enough is all
                *p-- = 0;
                *p-- = NUMBER_BASE;           // 'base' character
                
                do {
                    *p-- = digit[ uval & (base-1) ];
                    uval >>= shift;
                } while ( uval );
                
                p++;
                
                if ( truncate && strlen( p ) > 16 ) {
                    p = p + strlen( p ) - 16;
                    *p = '<';
                }    
                


                strcpy( dest, p );
            }
        
        }
        break;


        case 10:
            if (ivalue == 0)
            {
                // fit into `space's on screen
                int p = space-1;
                int l;
                if (value < 0) --p; // adjust for sign
                    
                for (;;)
                {
                    sprintf(dest,"%.*g", p, value);
                    l = strlen(dest);

                    if (l <= space) 
                        break;

                    if (tidy)
                    {
                        // try tidying
                        tidyNumber(dest);
                        if (strlen(dest) <= space) 
                            break;
                    }

                    p -= (l - space);
                    if (p <= 0) break; // fail safe
                }
            }
            else
            {
                int l;
                char c = '+';
                int id = 6;
                int d = 7;

//                if ( space > 16 ) {
//                    id = space/2;
//                    d = space/2;
//                }

                    
                if (ivalue < 0)
                {
                    ivalue = -ivalue;
                    c = '-';
                }

            again:

                // textify the real part
                sprintf(dest,"%.*g", d, value);

                // tidy to save precious chars
                tidyNumber(dest);
                    
                l = strlen(dest);                        
                dest[l++] = c;
                dest[l++] = 'i';
                dest[l] = 0;
                    
                if (ivalue != 1)
                {
                    int li;
                    // now fit as much of the ipart as we can
                    for (;;)
                    {
                        sprintf(dest + l,"%.*g", id, ivalue);
                        tidyNumber(dest+l);

                        li = strlen(dest);
                        if (li <= space) break; // done

                        id -= (li - space);
                        if (id <= 0) 
                        {
                            // very rarely we cant fit ANY of the ipart
                            // on display. shorten the real part and
                            // try again.
                            d -= 2;
                            id = d-1;
                            if (d > 2) goto again;
                        }
                    }
                }
            }

            // ensure we dont overrun whatever.
            dest[space] = 0;

            break;
    }
} 

// format and display just X
void UpdateDisplayRegX()
{
    UpdateXregDisplay();
    UpdateLCDline2(DisplayXreg);
}

void UpdateDisplayRegs(void)
{
    unsigned int i;
    UpdateXregDisplay();
    UpdateYregDisplay();

    if (!RPNmode)
    {
        char c;
        BOOL fill;

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
                c = 'r';
                break;
            case CALC_OP_R2P:
                c = 'p';
                break;
            case CALC_OP_PARALLEL:
                c = '|';
                break;
            case CALC_OP_COMPLEX_JOIN:
                c = 'i';
                break;
            default:
                c = 'o'; 
            }
        }

        
        fill = FALSE;
        for(i=0; i<MaxLCDdigits; i++)
        {
            if (!DisplayYreg[i]) fill = TRUE;
            if (fill) DisplayYreg[i] = ' ';
        }

        //display the operator character
        DisplayYreg[MaxLCDdigits-1]=c;      
        DisplayYreg[MaxLCDdigits]=0;

        for (i = 0; i <= PAREN_LEVELS; ++i)
        {
            if (!OperatorsXY[i]) break;
        }
        
        if (i > 1)
        {
            //display the operator character    
            DisplayYreg[MaxLCDdigits-3]='(';
            DisplayYreg[MaxLCDdigits-2]='0' + i - 1;
        }
    }

    UpdateLCDline1(DisplayYreg);
    UpdateLCDline2(DisplayXreg);
}

int xtio( char c )
{
    if ( (c >= '0') && (c <= '9') )
        return c-'0';

    if ( (c >= 'A') && (c <= 'F') )
        return ( c - 'A' ) + 10;

    return 0;
}


double ConvertDisplay(char* DisplayString, double* ip)
{
    double result = 0;
    int base = (WatchMode == WATCH_MODE_CALC) ? CalcDisplayBase : 10;
    *ip = 0;
    switch ( base )
    {
        case 2:
        case 8:
        case 16:
        {
            int sign = 1;
            char *p = DisplayString;

            // Handle sign
            if ( *p == '-' ) {
                sign = -1;
                p++;
            }    

            do {
                result = result * base + xtio(*p);
                p++;
            } while ( *p );

            result *= sign;
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

void UpdateMANTDisplay()
{
    char* p = DisplayYreg;
    
    // leave a space on the left so it looks a bit different
    *p = ' ';
    FormatValue( p + 1, Xreg, 0, MaxLCDdigits-1, FALSE, FALSE );

    p = DisplayXreg;
    *p++ = ' ';
    FormatValue( p+1, iXreg, 0, MaxLCDdigits-2, FALSE, FALSE );
    if (p[1] == '-')
        *p++ = '-';

    *p = 'i';

    UpdateLCDline1(DisplayYreg);
    UpdateLCDline2(DisplayXreg);
}

extern BOOL squash;
//***********************************
// Converts the Xreg double value into a string for the DisplayXreg
void UpdateXregDisplay(void) {
    FormatValue( DisplayXreg, Xreg, iXreg, MaxLCDdigits, squash, TRUE );
//    strncpy( DisplayXreg, displayBuffer, 16 );
//    if ( strlen( displayBuffer ) > 16 )
//        *DisplayXreg = '<';
}

//***********************************
// Converts the Yreg double value into a string for the DisplayYreg
void UpdateYregDisplay(void)
{
    int l = MaxLCDdigits;
    if (!RPNmode) l -= 4; // leave space for indicator

    FormatValue( DisplayYreg, Yreg, iYreg, l, TRUE, TRUE );
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
void StoreRecall(void)
{
    char KeyPress2;
    int num;

    //this operation discards any value currently in the display register
    UpdateLCDline1("RECALL REG 0-9 ?");     //ask for a register number

	KeyPress2 = GetDebouncedKey();

    if (KeyPress2==KeyRCL) //check for 2nd press of RCl key to enter STO mode
    {                                       // do the STO function
        UpdateLCDline1(" STORE REG 0-9 ?");     //ask for a register number

		KeyPress2 = GetDebouncedKey();
        num = ReturnNumber(KeyPress2);
        if(num >= 0 && num <= 9)
        {
            //store the Xreg value in the appropriate Sreg
            Sreg[num]=Xreg; iSreg[num] = iXreg;
        }
    }
    else //do the RCL function
    {
        num = ReturnNumber(KeyPress2);
        if (num >= 0 && num <= 9 )
        {
            Push();

            //restore the Xreg value from the Sreg
            Xreg=Sreg[num]; iXreg = iSreg[num];
            ValueEntered=TRUE;
        }
    }
}

// processes the sign key during input
void SignKey(void)
{
    // ASSUME value not entered.

    int n = strlen(DisplayXreg);
    char* p;
    char* q;

    // adjust any exponent 
    p = strrchr(DisplayXreg, 'e');
    q = strchr(DisplayXreg, 'i');
    
    //do this if there is an exponent already
    if (p && q < p)
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
        if (q)
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

int hexSelect( int sel ) {
    char hex[] = { 'D', 'E', 'F', 'A', 'B', 'C' };
    ProcessNumberKey( hex[sel-4] );
    UpdateLCDline1(DisplayYreg);
    UpdateLCDline2(DisplayXreg);
    return MODE_EXIT;
}

//**************************************
// enter a HEX digit A-F onto the current Xreg string
int HexEntry(void)
{
    const menuItem hexMenuMenu[] = {
        { "   A ", hexSelect, 7 },
        { "  B  ", hexSelect, 8 },
        { " C", hexSelect, 9 },
        { "   D ", hexSelect, 4 },
        { "  E  ", hexSelect, 5 },
        { " F", hexSelect, 6 },
    };

    const packedMenu2 hexMenu = {
        0,
        printMenu,
        0, 0, 6, hexMenuMenu
    };

    return genericMenu2( &hexMenu, 0 );
}   

void setBase( int base ) {

    // LONG numbers use left menu (char 0) as indicator.
    // Compatible with viewString -- don't change char usage 

    custom_character( NUMBER_EXTEND, character_left_menu );

    CalcDisplayBase = base;
    switch ( base ) {
        case 2:
            custom_character( NUMBER_BASE, character_base2 );
            break;
        case 8:
            custom_character( NUMBER_BASE, character_base8 );
            break;
        case 16:
            custom_character( NUMBER_BASE, character_base16 );
            break;
    }

    UpdateXregDisplay();
    UpdateYregDisplay();
}
