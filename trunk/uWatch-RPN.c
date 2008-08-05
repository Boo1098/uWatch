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
// pushes the stack up and leaves the Xreg intact
void PushStackUp(void)
{
    Treg=Zreg;
    Zreg=Yreg;
    Yreg=Xreg;
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


//**********************************
// Performs the function passed by the MenuItem number
// MenuItem numbers on screen are as follows:
// " 1  2  3  4 "
// " 5  6  7  8 "
// CurrentMenu holds the desired menu display number
void ProcessMenuItemRPN(int MenuItem)
{	
    switch(CurrentMenu)
    {
    case 0:			//MENU #1
        Menu0(MenuItem);
        break;
    case 1:
        Menu1(MenuItem);
        break;
    case 2:
        switch(MenuItem)
        {
        case 1:		//PI key
            {
                if (ValueEntered==TRUE) PushStackUp();
                strcpy(DisplayXreg,PIstring);
                ValueEntered=FALSE;
                break;
            }
        case 2:		//X^Y key
            {
                CompleteXreg();		//enter value on stack if needed
                Xreg=pow(Yreg,Xreg); 
                DropStack();
                UpdateDisplayRegs();	//update display again
                break;
            }
        case 3:		//e^X key
            {
                CompleteXreg();		//enter value on stack if needed
                Xreg=exp(Xreg);				
                UpdateDisplayRegs();	//update display again
                break;
            }
        case 4:		// R-P key
            {
                CompleteXreg();			//enter value on stack if needed
                RtoP();
                UpdateDisplayRegs();	//update display again
                break;
            }
        case 5:		// P-R key
            {
                CompleteXreg();		//enter value on stack if needed					
                PtoR();
                UpdateDisplayRegs();	//update display again
                break;
            }
        case 6:		//Parallel key 
            {
                CompleteXreg();		//enter value on stack if needed
                Xreg=(Yreg*Xreg)/(Yreg+Xreg);
                DropStack();
                UpdateDisplayRegs();	//update display again
                break;
            }
        }

        // all cases
        SwitchMenuOff();
        break;

    case 3:

        switch(MenuItem)
        {
        case 1:			//Keystroke RECORD selected
            {
                CompleteXreg();		//enter value on stack if needed
                KeyRecord();
                SwitchMenuOff();
                break;
            }
        case 2:			//keystroke PLAYBACK
            {
                CompleteXreg();		//enter value on stack if needed
                KeyReplay();
                SwitchMenuOff();
                break;
            }
        case 3:			// conversions
            {
                Conversions();
                break;
            }
        case 4:			//X! key 
            {
                CompleteXreg();		//enter value on stack if needed
                Xreg=Factorial(Xreg);
                UpdateDisplayRegs();	//update display again
                SwitchMenuOff();
                break;
            }
        case 5:			
            {
#if 1
                // continue with time functions until escape
                TimeFunctions();
                UpdateDisplayRegs();	//update display again
                SwitchMenuOff();
#else
                //cube root key
                CompleteXreg();		//enter value on stack if needed
                Xreg=pow(Xreg,(1.0/3.0));	
                UpdateDisplayRegs();	//update display again
                SwitchMenuOff();
#endif
            }
            break;
        case 6:			// HEX entry key
            {
                HexEntry();
                break;
            }
        }
        break;
    }
}

//**********************************
void ProcessNumberKey(char* digit)
{
    if (strlen(DisplayXreg)<MaxLCDdigits)
    {
 	//if this is the first digit pressed
        if (ValueEntered==TRUE)
        {
            //check to see if we don't have to overwrite the Xreg
            if (EnableXregOverwrite==FALSE)
                PushStackUp(); //push the stck up for the first key entry, i.e. *don't* overwrite the Xreg
            
            //clear (overwrite)what was in the Xreg
            DisplayXreg[0] = 0;
            EnableXregOverwrite=FALSE;	//disable overwriting the Xreg for future key presses
        }
        strcat(DisplayXreg,digit);
        ValueEntered=FALSE;
        MenuMode=FALSE;
        UpdateLCDline1(DisplayYreg);
        UpdateLCDline2(DisplayXreg);
    }
}


//***********************************
// The main RPN calculator routine
// Note that all variables are global
void RPNcalculator(void)
{
    unsigned int Key; //keypress variables
    double TEMPreg; //temp register for calculations
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
        if (Key == KeyMenu)
        {
            CalcMenu();
        }
        else if (MenuMode)
        {
            switch (Key)
            {
            case Key4: //user has pressed the 4 key
                ProcessMenuItemRPN(4);
                break;
            case Key5:
                ProcessMenuItemRPN(5);
                break;
            case Key6:
                ProcessMenuItemRPN(6);
                break;
            case Key7:
                ProcessMenuItemRPN(1);
                break;
            case Key8:
                ProcessMenuItemRPN(2);
                break;
            case Key9:
                ProcessMenuItemRPN(3);
                break;
            case KeyClear: //user has pressed the CLEAR key
                {
                    InverseKey=FALSE;		//reset the function flags
                    HYPkey=FALSE;
                    MenuMode=FALSE;		//switch off menu mode

                    //display the X & Y regs again
                    UpdateLCDline1(DisplayYreg);
                    UpdateLCDline2(DisplayXreg);
                }
                break;
            }
        }
        else 
        {
            switch (Key)
            {
            case Key1: //user has pressed the 1 key
                ProcessNumberKey("1");
                break;
            case Key2: //user has pressed the 2 key
                ProcessNumberKey("2");
                break;
            case Key3: //user has pressed the 3 key
                ProcessNumberKey("3");
                break;
            case Key4: //user has pressed the 4 key
                ProcessNumberKey("4");
                break;
            case Key5: //user has pressed the 5 key
                ProcessNumberKey("5");
                break;
            case Key6: //user has pressed the 6 key
                ProcessNumberKey("6");
                break;
            case Key7: //user has pressed the 7 key
                ProcessNumberKey("7");
                break;
            case Key8: //user has pressed the 8 key
                ProcessNumberKey("8");
                break;
            case Key9: //user has pressed the 9 key
                ProcessNumberKey("9");
                break;
            case Key0: //user has pressed the 0 key
                ProcessNumberKey("0");
                break;
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
                    CompleteXreg();
                    ResetFlags();
                    Xreg=Xreg+Yreg;		//perform PLUS operation
                    DropStack();
                    UpdateDisplayRegs();
                }
                break;
            case KeyMinus: //user has pressed the MINUS key
                {
                    CompleteXreg();
                    ResetFlags();
                    Xreg=Yreg-Xreg;			//perform MINUS operation
                    DropStack();
                    UpdateDisplayRegs();
                }
                break;
            case KeyMult: //user has pressed the MULTIPLY key
                {
                    CompleteXreg();
                    ResetFlags();
                    Xreg=Xreg*Yreg;			//perform MULTIPLY operation
                    DropStack();
                    UpdateDisplayRegs();
                }
                break;
            case KeyDiv: //user has pressed the DIVIDE key
                {
                    CompleteXreg();
                    ResetFlags();
                    Xreg=Yreg/Xreg;		//perform DIVIDE operation
                    DropStack();
                    UpdateDisplayRegs();
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
            case KeyLP: 
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
    }	//end of entire loop
}

