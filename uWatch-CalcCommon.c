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
extern void SwitchMenuOff(void);
extern double Factorial(double num);
extern void StoreRecall(void);
extern void SignKey(void);
extern void KeyRecord(void);
extern void KeyReplay(void);
extern long mjd(int y, int m, int d);
extern void caldati(long mjd,
                    unsigned int* y, unsigned int* m, unsigned int* d);

extern void DropStack(void);

void Push(void)
{
    Treg=Zreg;
    Zreg=Yreg;
    Yreg=Xreg;
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

void Operate(int op)
{
    CompleteXreg();		//enter value on stack if needed
            
    switch(op)
    {
    case CALC_OP_RECIPROCAL:
        Xreg=1/Xreg;			
        break;
    case CALC_OP_SQUARE:
        Xreg=Xreg*Xreg;			
        break;
    case CALC_OP_SQRT:
        Xreg=sqrt(Xreg);
        break;
    case CALC_OP_LN:
        Xreg=log(Xreg);			
	break;
    case CALC_OP_EXP:
        Xreg=exp(Xreg);
        break;
    case CALC_OP_NPOW:
        Xreg=pow(Yreg,Xreg);			
        break;
    case CALC_OP_NROOT:
        Xreg=pow(Yreg,1.0/Xreg);			        
        break;
    case CALC_OP_LN10:
        break;
    case CALC_OP_10X:
        break;
    case CALC_OP_SIN:
        if (DegreesMode) Xreg /= RAD;
        Xreg=sin(Xreg);
        break;
    case CALC_OP_COS:
        if (DegreesMode) Xreg /= RAD;
        Xreg=cos(Xreg);
        break;
    case CALC_OP_TAN:
        if (DegreesMode) Xreg /= RAD;
        Xreg=tan(Xreg);
        break;
    case CALC_OP_ASIN:
        Xreg=asin(Xreg);
        if (DegreesMode) Xreg *= RAD;
        break;
    case CALC_OP_ACOS:
        Xreg=acos(Xreg);
        if (DegreesMode) Xreg *= RAD;
        break;
    case CALC_OP_ATAN:
        Xreg=atan(Xreg);
        if (DegreesMode) Xreg *= RAD;
        break;
    case CALC_OP_MODEDEG:
        DegreesMode = TRUE;
        break;
    case CALC_OP_MODERAD:
        DegreesMode = FALSE;
        break;
    case CALC_OP_PI:
        Push();
        Xreg = PI;
        break;
    case CALC_OP_HMS:
        Xreg = hms(Xreg);
        break;
    case CALC_OP_R2P:
        RtoP();
        break;
    case CALC_OP_FACTORIAL:
        Xreg = Factorial(Xreg);
        break;
    case CALC_OP_DMY:
        {
            unsigned int y, m, d;
            caldati(Xreg, &y, &m, &d);
            Xreg = y/1.0e6 + m/100.0 + d;
        }
        break;
    case CALC_OP_HOURS:
        Xreg = hr(Xreg);
        break;
    case CALC_OP_P2R:
        PtoR();
        break;
    case CALC_OP_SUNSET:
        {
            Push();
            Push();
            CalcRiseAndSet(&Yreg, &Xreg);
        }
        break;
    case CALC_OP_DAYS:
        {
            // assume Xreg is in format dd.mmyyyy
            int d = Xreg;
            double t = (Xreg - d)*100;
            int m = t;
            int y = (t - m) * 10000;
            Xreg = mjd(y, m, d);
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
        Xreg=(Yreg*Xreg)/(Yreg+Xreg);
        DropStack();
        break;
    }
    UpdateDisplayRegs();	//update display again
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

    if (RPNmode==TRUE)
    {
        UpdateLCDline1(DisplayYreg);
        UpdateLCDline2(DisplayXreg);
        return;		//nothing more to do in RPN mode
    }

    //the rest of this code is for the ALG mode only
    switch (OperatorXY)
    {
    case OPRplus:  c='+'; break;
    case OPRminus: c='-'; break;
    case OPRmult:  c='x'; break;
    case OPRdiv:   c='/'; break;
    case OPRparra: c='|'; break;
    case OPRnPr:   c='P'; break;
    case OPRnCr:   c='C'; break;
    case OPRxpwry: c='^'; break;
    case OPRrtop:  c='>'; break;
    case OPRptor:  c='<'; break;
    default: c=' ';break;
    }
    for(i=0; i<=15; i++)
        if (DisplayYreg[i]==NULL) DisplayYreg[i]=' ';	//replace NULL with space
    DisplayYreg[15]=c;				//display the operator character
    DisplayYreg[16]=NULL;

    if (OperatorXY1!=0) 
    {
        DisplayYreg[13]='(';				//display the operator character	
        DisplayYreg[14]='1';
    }
    if (OperatorXY2!=0) DisplayYreg[14]='2';
    if (OperatorXY3!=0) DisplayYreg[14]='3';
    if (OperatorXY4!=0) DisplayYreg[14]='4';
    if (OperatorXY5!=0) DisplayYreg[14]='5';
    if (OperatorXY6!=0) DisplayYreg[14]='6'; 
	
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

//***********************************
// Checks to see if the Xreg has been ENTERed onto the stack
// If not then it it adds the value to the stack
// This routien is needed because the displayed Xreg value is not stored in the real Xreg variable yet
void CompleteXreg(void)
{
    //if ENTER has not been pressed then do it for them
    if (ValueEntered==FALSE)
    {
        Xreg=atof(DisplayXreg); //convert display string to number
        ResetFlags();
        ValueEntered=TRUE; //value is now entered on the stack, so we must flag this.
    }
}


//***********************************
// Converts the Xreg double value into a string for the DisplayXreg
void UpdateXregDisplay(void)
{
    strcpy(DisplayXreg,"                ");		//blank the string first
    sprintf(DisplayXreg,"%.10g",Xreg);
}

//***********************************
// Converts the Yreg double value into a string for the DisplayYreg
void UpdateYregDisplay(void)
{
    strcpy(DisplayYreg,"                ");		//blank the string first
    sprintf(DisplayYreg,"%.9g",Yreg);
}

//**********************************
//switches the menu off and restores the Xreg/Yreg display
void SwitchMenuOff(void)
{
    //MenuMode=FALSE;
    UpdateLCDline1(DisplayYreg);
    UpdateLCDline2(DisplayXreg);
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
    if (ExponentIncluded==TRUE)
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
        if (ValueEntered==TRUE) 	//if this is the first digit pressed
        {
            if (EnableXregOverwrite==FALSE)	//check to see if we don't have to overwrite the Xreg
                DisplayXreg[0] = 0; //clear (overwrite)what was in the Xreg
            EnableXregOverwrite=FALSE;	//disable overwriting the Xreg for future key presses
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
            q = DisplayXreg + n + 1;
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
        if (ValueEntered==TRUE) 	//if this is the first digit pressed
        {
            if (EnableXregOverwrite==FALSE)	//check to see if we don't have to overwrite the Xreg
                DisplayXreg[0] = 0;
            EnableXregOverwrite=FALSE;	//disable overwriting the Xreg for future key presses
        }
        digit[1] = 0;
        strcat(DisplayXreg,digit);

        ValueEntered=FALSE;
        //MenuMode=FALSE;					//flag that a number has been entered
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
            SwitchMenuOff();
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
