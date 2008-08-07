//********************************************************
// uWatch
// RPN Calculator Functions
// Version 1.3
// Last Update: 12th June 08
// Copyright(c) 2008 David L. Jones
// Written for the Microchip C30 Compiler
// Target Device: PIC24FJ64GA004 (44pin)
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




//***********************************
// The main RPN calculator routine
// Note that all variables are global
void RPNcalculator(void)
{
    unsigned int Key; //keypress variables
    double TEMPreg; //temp register for calculations
    int c;

    DisplayXreg[0] = 0;
    DisplayYreg[0] = 0;
    DecimalIncluded=FALSE;
    MinusIncluded=FALSE;	
    MinusIncludedInExponent=FALSE;
    ExponentIncluded=FALSE;
    CurrentMenu=0;
    ValueEntered=TRUE;
    UpdateXregDisplay();
    UpdateYregDisplay();
    UpdateLCDline1(DisplayYreg);
    UpdateLCDline2(DisplayXreg);

    //main keypress loop is infinite.
    for (;;)
    {
        //wait for a keypress
        while (!(Key = KeyScan())) ;
		
        ResetSleepTimer();		
        //start to process the keypress

        // mode key was pressed, exit calc mode
        if (Key==KeyMode)
        {
            DelayMs(KeyDelay);
            return;
        }

        //user pressed some key other than MODE, so ensure that when exit we go back to the time/date display
        NextMode=FALSE;		
        ResetSleepTimer();

        // common menu mode or not
        while (Key == KeyMenu)
        {
            int mi;
            CalcMenuInfo* mifo;

            mifo = MainMenus + CurrentMenu;
            mi = DriveMenu2(mifo->lines[0], mifo->lines[1],
                            mifo->lines[2], mifo->lines[3]);
            
            Key = 0;
            if (mi >= 0)
            {
                mi = mifo->ops[mi];
                if (mi > 0)
                {
                    Operate(mi);
                }
            }
            else
            {
                // retrieve key pressed
                Key = -mi;
                if (Key == KeyMenu)
                    if (++CurrentMenu>=DIM(MainMenus)) CurrentMenu=0;
            }
        }

        // handle numbers
        c = ReturnNumber(Key);
        if (c >= 0)
        {
            // key is 0 to 9
            ProcessNumberKey('0' + c);
            continue;
        }

        // handle specific keys
        switch (Key)
        {
        case KeyPoint: //user has pressed the DECIMAL POINT key
            {
                if (strlen(DisplayXreg)<MaxLCDdigits)
                    //only do if decimal point does not already exist AND there is no exponent
                    if ((DecimalIncluded==FALSE) && ExponentIncluded==FALSE)		
                    {
                        if (ValueEntered==TRUE) 	//user has pressed POINT as the first digit
                        {
                            strcpy(DisplayXreg,"0.");	//decimal point needs a 0 added to the start
                        }
                        else		
                            strcat(DisplayXreg,".");
                        DecimalIncluded=TRUE;
                        ValueEntered=FALSE;
                        UpdateLCDline1(DisplayYreg);
                        UpdateLCDline2(DisplayXreg);
                    }
            }
            break;
        case KeyEnter: //user has pressed the ENTER key
            {
                CompleteXreg();
                PushStackUp();
                ResetFlags();
                EnableXregOverwrite=TRUE;	//overwite the flag and force the Xreg to be overwritten on the next entry
                UpdateDisplayRegs();
            }
            break;
        case KeyPlus: //user has pressed the PLUS key
            {
                //CompleteXreg();
                //ResetFlags();
                //Xreg=Xreg+Yreg;		//perform PLUS operation
                //DropStack();
                //UpdateDisplayRegs();
                Operate(CALC_OP_PLUS);
            }
            break;
        case KeyMinus: //user has pressed the MINUS key
            {
                //CompleteXreg();
                //ResetFlags();
                //Xreg=Yreg-Xreg;			//perform MINUS operation
                //DropStack();
                //UpdateDisplayRegs();
                Operate(CALC_OP_MINUS);
            }
            break;
        case KeyMult: //user has pressed the MULTIPLY key
            {
                //CompleteXreg();
                //ResetFlags();
                //DropStack();
                //UpdateDisplayRegs();
                Operate(CALC_OP_MULT);
            }
            break;
        case KeyDiv: //user has pressed the DIVIDE key
            {
                //CompleteXreg();
                //ResetFlags();

                //DropStack();
                //UpdateDisplayRegs();
                Operate(CALC_OP_DIVIDE);
            }
            break;
        case KeyClear: //user has pressed the CLEAR key
            {
                InverseKey=FALSE;		//reset the function flags
                HYPkey=FALSE;
                
                //only clear if there is something in the display register to clear
                if (ValueEntered==FALSE)	
                {
                    Xreg = 0;
                    UpdateDisplayRegs();
                    ResetFlags();
                    EnableXregOverwrite = TRUE;
                }
                else //clear the X register instead and DROP the stack
                {
                    Xreg=Yreg; //drop X reg from the stack
                    DropStack();
                    Treg=0;
                    UpdateDisplayRegs();
                    ResetFlags();
                }


            }
            break;
        case KeyEXP: 
            {
                if (ExponentIncluded==FALSE)	//can't add exponent twice
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
        case KeyXY: 
            {
                CompleteXreg();		//enter value on stack if needed
                TEMPreg=Xreg;			
                Xreg=Yreg;			//swap and X and Y regs
                Yreg=TEMPreg;
                UpdateDisplayRegs();	//update display again
            }
            break;
        case KeyLP:  // ROLL
            {
                CompleteXreg();		//enter value on stack if needed
                TEMPreg=Xreg;			
                Xreg=Yreg;			// roll the stack DOWN
                Yreg=Zreg;
                Zreg=Treg;
                Treg=TEMPreg;
                UpdateDisplayRegs();	//update display again
            }
            break;
        case KeyRCL: 
            {
                //recalls the one of the user defined memory registers into the Xreg
                CompleteXreg();
                StoreRecall();
                UpdateDisplayRegs();	//update display again
            }
            break;
        case KeySign: 
            //changes the sign of the mantissa or exponent
            SignKey();
            break;
        }
    }
}

