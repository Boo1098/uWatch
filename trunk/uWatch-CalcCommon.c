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

#include "uWatch-astro.h"

extern void Conversions(void);
extern void HexEntry(void);
extern void UpdateXregDisplay(void);
extern void UpdateYregDisplay(void);
extern void UpdateDisplayRegs(void);
extern void CompleteXreg(void);
extern void ResetFlags(void);
extern double Factorial(double num);
extern void StoreRecall(void);
extern void SignKey(void);
extern void KeyRecord(void);
extern void KeyReplay(void);
extern long mjd(int y, int m, int d);
extern void caldati(long mjd,
                    unsigned int* y, unsigned int* m, unsigned int* d);
extern void BaseMode(void);
void Push(void)
{
    Treg=Zreg;
    Zreg=Yreg;
    Yreg=Xreg;
}

//***********************************
// pushes the stack up and leaves the Xreg intact
void PushStackUp(void)
{
    Push();
    UpdateYregDisplay(); //Yreg has changed, so keep display register up to date
}

//**********************************
// Drops the stack down and leaves Treg intact making it useful as a "constant" register
// Xreg is handled in the calling routine
void DropStack(void)
{
    Yreg=Zreg;
    Zreg=Treg;
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

void Drop(int level)
{
    if (!level) 
    {
        Yreg = Zreg;
    }
    Zreg = Treg;
    OperatorXY = OperatorYZ; // bring down any operator too
    OperatorYZ = 0;
}

// Raw operation
void Operation(int op, int level)
{
    double* rp = Regs + level;
    switch(op)
    {
    case CALC_OP_RECIPROCAL:
        *rp=1/(*rp);
        break;
    case CALC_OP_SQUARE:
        *rp=(*rp)*(*rp);
        break;
    case CALC_OP_SQRT:
        *rp=sqrt(*rp);
        break;
    case CALC_OP_LN:
        *rp=log(*rp);			
	break;
    case CALC_OP_EXP:
        *rp=exp(*rp);
        break;
    case CALC_OP_NPOW:
        *rp=pow(rp[1],*rp);			
        Drop(level);
        break;
    case CALC_OP_NROOT:
        *rp=pow(rp[1],1.0/(*rp));			        
        Drop(level);
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
        break;
    case CALC_OP_HMS:
        *rp = hms(*rp);
        break;
    case CALC_OP_R2P:
        RtoP();
        break;
    case CALC_OP_FACTORIAL:
        *rp = Factorial(*rp);
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
        Drop(level);
        break;
    case CALC_OP_PLUS:
        *rp=(*rp)+rp[1];		//perform PLUS operation
        Drop(level);
        break;
    case CALC_OP_MINUS:
        *rp=rp[1]-(*rp);			//perform MINUS operation
        Drop(level);
        break;
    case CALC_OP_MULT:
        *rp=(*rp)*rp[1];			//perform MULTIPLY operation
        Drop(level);
        break;
    case CALC_OP_DIVIDE:
        *rp=rp[1]/(*rp);		//perform DIVIDE operation
        Drop(level);
        break;
	case CALC_OP_BASE:
        BaseMode();
        break;
    }
}


// Perform operation and display
void Operate(int op)
{
    CompleteXreg();		//enter value on stack if needed
    Operation(op, 0);           // operate on X & Y
    UpdateDisplayRegs();	//update display again
}

void ProcessNumberKey(int digit)
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
        EnableXregOverwrite=FALSE;	//disable overwriting the Xreg for future key presses
    }

    if (l<MaxLCDdigits)
    {
        DisplayXreg[l] = digit;
        DisplayXreg[l+1] = 0;  // ensure termination

        ValueEntered=FALSE;
        UpdateLCDline2(DisplayXreg);
    }
}


// handle number input. return key if not handled (0 otherwise)
int EnterNumber(int Key)
{
    // handle numbers
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
        Key = 0; 
    }

    switch (Key)
    {
    case KeyPoint: //user has pressed the DECIMAL POINT key
        Key = 0;
        if (strlen(DisplayXreg)<MaxLCDdigits)
            //only do if decimal point does not already exist AND there is no exponent
            if (!DecimalIncluded && !ExponentIncluded)		
            {
                //user has pressed POINT as the first digit
                if (ValueEntered==TRUE) 
                {
                    //decimal point needs a 0 added to the start
                    strcpy(DisplayXreg,"0.");
                }
                else		
                    strcat(DisplayXreg,".");
                
                DecimalIncluded=TRUE;
                ValueEntered=FALSE;
                UpdateLCDline2(DisplayXreg);
            }
        break;
    case KeyEXP: 
        Key = 0;

		if ( CalcDisplayBase == 16 )
		{
			HexEntry();
		}
		else
		{
	        if (!ExponentIncluded)	//can't add exponent twice
	        {
	            ExponentIncluded=TRUE;
	            
	            // EXP is first key pressed, so add a 1 to the front
	            if (ValueEntered==TRUE) strcpy(DisplayXreg,"1e");
	            else strcat(DisplayXreg,"e");		
	            ValueEntered=FALSE;	
	            UpdateLCDline2(DisplayXreg);		//update the display
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
            Xreg = 0;
            UpdateDisplayRegs();
            ResetFlags();
            EnableXregOverwrite = TRUE;
        }
        break;
    }
    return Key;
}


void FormatValue( char* destination, double value, int precision )
{
    strcpy(destination,"                ");		//blank the string first

	char base = (WatchMode == WATCH_MODE_CALC) ? CalcDisplayBase : 10;

	int index=0;
    int p=0;
	unsigned int uval=0;
	unsigned long long ulVal=0;
	switch ( base )
	{
		case 2:
		{	
			char tmp[17];
			memset( tmp, 0, 17 );

			if ( (value > 32767) || (value < -32768) ) 
			{
				strcpy( destination, "  * OVERFLOW *" );
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
					sprintf( destination, "%ib", 0 );
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
					
					strcpy( destination, tmp+(p+1) );
	
					if ( index < 15 )
					{
						destination[index] = 'b';
						destination[index+1] = 0;
					}
				}
			}
		}
		break;

		case 10:
			    sprintf(destination,"%.10g", value);
			break;
		case 16:
		{
			double max = pow( 2, 64 );
			if ( fabs(value) > max )
			{
				strcpy( destination, "  * OVERFLOW *" );
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
		
					strcpy( destination, tmp+(p+1) );

					if ( index < 15 )
					{
						destination[index] = 'h';
						destination[index+1] = 0;
					}
				}
				else
				{
					strcpy( destination, "0h" );
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
        while(n>=1)		//perform the factorial algorithm
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


double ConvertDisplay( char* DisplayString )
{
	double result = 0.0;
	char base = (WatchMode == WATCH_MODE_CALC) ? CalcDisplayBase : 10;
	int i=0;
	double h=0.0;
	int length=strlen(DisplayString);
	switch ( base )
	{
		case 2:
           
			for( ; i < length; i++ )
			{
				if ( DisplayString[(length-1)-i] == '1' )
				{
					h += (int)pow(2, i);
				}	
			}
			result = h;	
			break;
		case 10:
			result = atof( DisplayString );	
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
		Xreg=ConvertDisplay(DisplayXreg);
        ResetFlags();
    }
}

//***********************************
// Converts the Xreg double value into a string for the DisplayXreg
void UpdateXregDisplay(void)
{
	FormatValue( DisplayXreg, Xreg, 10 );
}

//***********************************
// Converts the Yreg double value into a string for the DisplayYreg
void UpdateYregDisplay(void)
{
	FormatValue( DisplayYreg, Yreg, 9 );
}


//***********************************
//resets all the flags after a calculation, ready for the next number to be entered by the user
void ResetFlags(void)
{
    ValueEntered=TRUE;
    DecimalIncluded=FALSE;
    MinusIncluded=FALSE;
    MinusIncludedInExponent=FALSE;
    ExponentIncluded=FALSE;
    EnableXregOverwrite=FALSE;		//calling function must overwirte this if needed
}

//***********************************
//processes the store/recall function
void StoreRecall(void)
{
    char KeyPress2;
    int num;

    //this operation discards any value currently in the display register
    UpdateLCDline1("RECALL REG 0-9 ?");		//ask for a register number
    do KeyPress2=KeyScan(); while(KeyPress2==0);	//get the users response		
    if (KeyPress2==KeyRCL)						//check for 2nd press of RCl key to enter STO mode
    {										// do the STO function
        UpdateLCDline1(" STORE REG 0-9 ?");		//ask for a register number
        do KeyPress2=KeyScan(); while(KeyPress2==0);	//get the users response		

        num = ReturnNumber(KeyPress2);
        if(num >= 0)
        {
            Sreg[num]=Xreg;			//store the Xreg value in the appropriate Sreg
            UpdateLCDline1("  VALUE STORED  ");
            DelayMs(100);			//small visual delay
        }
    }
    else				//do the RCL function
    {
        num = ReturnNumber(KeyPress2);
        if (num >= 0)
        {
            Xreg=Sreg[num];			//restore the Xreg value from the Sreg
            ValueEntered=TRUE;
            UpdateLCDline1(" VALUE RECALLED ");
            DelayMs(100);			//small visual delay
        }
    }
}

//**********************************
// processes the sign key
void SignKey(void)
{
    int n = strlen(DisplayXreg);
    char* p = DisplayXreg;
    char* q;

    //do this if there is an exponent already
    if (ExponentIncluded)
    {
        //search Xreg for 'e'
        while (*p != 'e') ++p;
        ++p;

        //minus already exists so lets remove it
        if (MinusIncludedInExponent==TRUE)
        {
            q = p + 1;
            while ((*p++ = *q++) != 0) ;
            MinusIncludedInExponent=FALSE;
        }
        else //minus does not exist yet so lets add one
        {
            char* k;
            q = DisplayXreg + n + 1;
            k = q;
            while (q != p)
                *q-- = *--k;

            *p = '-';
            MinusIncludedInExponent=TRUE;
        }
        UpdateLCDline2(DisplayXreg);		//update the display
    }
    else //do this if there is only a mantissa
    {
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
            }
        }
	
        if (MinusIncluded==TRUE)
        {
            MinusIncluded=FALSE;
                
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

            MinusIncluded=TRUE;

            *p = '-';
        }
    }
    ValueEntered=FALSE;
    UpdateLCDline2(DisplayXreg);		//update the display
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
    ProgRec=TRUE;						//switch on keystroke programming mode
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
    "lb -> kg",
    "BASE N"
};

//**********************************
// display the conversions menu
void Conversions(void)
{
    char s[MaxLCDdigits+1];
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
            UpdateDisplayRegs();	//update display again
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
    case 6: //BASE-N mode
        {
            int Mode2=0;
            int KeyPress2;
            while(TRUE) 	//main keypress loop is infinite.
            {
                switch(Mode2)
                {
                case 0: strcpy(s,"Dec -> Hex"); break;
                case 1: strcpy(s,"Dec -> Bin"); break;
                case 2: strcpy(s,"Hex -> Dec"); break;
                case 3: strcpy(s,"Hex -> Bin"); break;
                case 4: strcpy(s,"Bin -> Dec"); break;
                case 5: strcpy(s,"Bin -> Hex"); break;
                }
					
                UpdateLCDline1(s);
                UpdateLCDline2("BASEN: +/- & ENT");
					
                do KeyPress2=KeyScan(); while(KeyPress2==0);
					
                ResetSleepTimer();		
                //start to process the keypress
					
                // clear key was pressed, exit setup mode
                if (KeyPress2==KeyClear) return;
			
                if (KeyPress2==KeyPlus)
                {
                    Mode2++;	//user has pressed the PLUS key
                    if (Mode2>5) Mode2=0;
                }
					
                if (KeyPress2==KeyMinus)
                {
                    Mode2--;		//user has pressed the MINUS key
                    if (Mode2<0) Mode2=5;
                }
					
                //user has pressed the ENTER key
                if (KeyPress2==KeyEnter) 
                {
#if 1
                    return;  // bail until implemented!!
#else
                    switch(Mode2)				
                    {
                    case 0:			//decimal to hex
                        {
                            //enter value on stack if needed
                            CompleteXreg(); 
                            value=0;
                            do
                            {
                                ;
                            }
                            while(1);
                            break;
                        }
                    }
#endif
                }
            }
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
