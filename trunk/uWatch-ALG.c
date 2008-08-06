//********************************************************
// uWatch
// Algebraic Calculator Functions
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


#if 0


void ProcessXYoperator(void)
{
    if (OperatorXY==OPRplus)  {Xreg=Xreg+Yreg; Yreg=0;}
    if (OperatorXY==OPRminus) {Xreg=Yreg-Xreg; Yreg=0;}
    if (OperatorXY==OPRmult)  {Xreg=Xreg*Yreg; Yreg=0;}
    if (OperatorXY==OPRdiv)   {Xreg=Yreg/Xreg; Yreg=0;}
    if (OperatorXY==OPRparra) {Xreg=(Yreg*Xreg)/(Yreg+Xreg); Yreg=0;}
    if (OperatorXY==OPRnPr)   {Xreg=(Factorial(Xreg))/(Factorial(Xreg-Yreg)); Yreg=0;}
    if (OperatorXY==OPRnCr)   {Xreg=(Factorial(Xreg))/(Factorial(Yreg)*(Factorial(Xreg-Yreg))); Yreg=0;}
    if (OperatorXY==OPRxpwry) {Xreg=pow(Yreg,Xreg); Yreg=0;}
    if (OperatorXY==OPRrtop)  RtoP();
    if (OperatorXY==OPRptor)  PtoR();
}

void ProcessYZoperator(void)
{
    if (OperatorYZ==OPRplus)  Yreg=Zreg+Yreg;
    if (OperatorYZ==OPRminus) Yreg=Zreg-Yreg;
    if (OperatorYZ==OPRmult)  Yreg=Zreg*Yreg;
    if (OperatorYZ==OPRdiv)   Yreg=Zreg/Yreg;
    OperatorYZ=0;       //clear the operator we just used
    Zreg=0;             //clean up Zreg we just used
}

//**********************************
// Performs the function passed by the MenuItem number
// MenuItem numbers onm screen are as follows:
// " 1  2  3  "
// " 4  5  6  "
// CurrentRPNmenu holds the desired menu display number
void ProcessMenuItemALG(int MenuItem)
{   
    //  int c;
    //  double TEMPreg;
    //  unsigned int Key, num;
    //  unsigned char s[MaxLCDdigits+1];

    switch(CurrentMenu)
    {
    case 0:         //MENU#0
        {
            Menu0(MenuItem);
            break;
        }

    case 1:
        {
            Menu1(MenuItem);
            break;
        }
        
    case 2:
        {
            switch(MenuItem)
            {
            case 1:         //PI key
                {
                    strcpy(DisplayXreg,PIstring);
                    ValueEntered=FALSE;
                    SwitchMenuOff();                    
                    break;
                }
            case 2:         //X^Y key
                {
                    CompleteXreg();     //enter value on stack if needed
                    Yreg=Xreg;              //push X across and reset X ready for next value
                    Xreg=0;
                    OperatorXY=OPRxpwry;        //asign the new operator
                    UpdateDisplayRegs();    //update display again
                    SwitchMenuOff();                    
                    break;
                }
            case 3:         //e^X  key 
                {
                    CompleteXreg();     //enter value on stack if needed
                    Xreg=exp(Xreg);         
                    UpdateDisplayRegs();    //update display again
                    SwitchMenuOff();
                    break;
                }
            case 4:         // R-P key
                {
                    CompleteXreg();     //enter value on stack if needed
                    Yreg=Xreg;              //push X across and reset X ready for next value
                    Xreg=0;
                    OperatorXY=OPRrtop;     //asign the new operator
                    UpdateDisplayRegs();    //update display again
                    SwitchMenuOff();
                    break;
                }
            case 5:         // P-R key
                {
                    CompleteXreg();     //enter value on stack if needed
                    Yreg=Xreg;              //push X across and reset X ready for next value
                    Xreg=0;
                    OperatorXY=OPRptor;     //asign the new  operator
                    UpdateDisplayRegs();    //update display again
                    SwitchMenuOff();
                    break;
                }
            case 6:         // Parallel key
                {
                    CompleteXreg();     //enter value on stack if needed
                    Yreg=Xreg;              //push X across and reset X ready for next value
                    Xreg=0;
                    OperatorXY=OPRparra;        //asign the new operator
                    UpdateDisplayRegs();    //update display again
                    SwitchMenuOff();
                    break;
                }
            }
            break;
        }

    case 3:
        {
            switch(MenuItem)
            {
            case 1:         //Keystroke RECORD selected
                {
                    CompleteXreg();     //enter value on stack if needed
                    KeyRecord();
                    SwitchMenuOff();
                    break;
                }
            case 2:         //keystroke PLAYBACK
                {
                    CompleteXreg();     //enter value on stack if needed
                    KeyReplay();
                    SwitchMenuOff();
                    break;
                }
            case 3:         // Conversions
                {
                    Conversions();
                    break;
                }
            case 4:         //X! key 
                {
                    CompleteXreg();     //enter value on stack if needed
                    Xreg=Factorial(Xreg);
                    UpdateDisplayRegs();    //update display again
                    SwitchMenuOff();
                    break;
                }
            case 5:         //cube root key
                {
                    CompleteXreg();     //enter value on stack if needed
                    Xreg=pow(Xreg,(1.0/3.0));   
                    UpdateDisplayRegs();    //update display again
                    SwitchMenuOff();
                    break;
                }
            case 6:         //HEX entry key
                {
                    HexEntry();
                    break;
                }
            }
            break;
        }

    }
}

//**********************************
void ProcessNumberKeyALG(char digit[])
{
    if ((strlen(DisplayXreg)<MaxLCDdigits)&&(MenuMode==FALSE))
    {
        if (ValueEntered==TRUE)     //if this is the first digit pressed
        {
            //check to see if we don't have to overwrite the Xreg
            if (EnableXregOverwrite==FALSE)
                DisplayXreg[0] = 0;

            EnableXregOverwrite=FALSE;  //disable overwriting the Xreg for future key presses
        }
        strcat(DisplayXreg,digit);
        ValueEntered=FALSE;
        MenuMode=FALSE;                 //flag that a number has been entered
        UpdateLCDline2(DisplayXreg);
    }
}


//***********************************
// The main Algebraic calculator routine
// Note that all variables are global
void ALGcalculator(void)
{
    unsigned int Key;       //keypress variables
    double TEMPreg;             //temp register for calculations
    DisplayXreg[0] = 0;
    DisplayYreg[0] = 0;
    DecimalIncluded=FALSE;
    MinusIncluded=FALSE;    
    MinusIncludedInExponent=FALSE;
    ExponentIncluded=FALSE;
    InverseKey=FALSE;
    HYPkey=FALSE;
    CurrentMenu=0;
    ValueEntered=TRUE;
    UpdateXregDisplay();
    UpdateYregDisplay();
    UpdateLCDline1(DisplayYreg);
    UpdateLCDline2(DisplayXreg);

    while(TRUE)     //main keypress loop is infinite.
    {
        do {            //wait for a keypress
            Key=KeyScan();
        } while(Key==0);
        
        //start to process the keypress

        if (Key==KeyMode)       // mode key was pressed, exit calc mode
        {
            return;
        }

        NextMode=FALSE;             //user pressed some key other than MODE, so ensure that when exit we go back to the time/date display

        if (Key==Key1)              //user has pressed the 1 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("1");
            //key is ignored when menu is on.
        }

        if (Key==Key2)              //user has pressed the 2 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("2");
            //key is ignored when menu is on.
        }

        if (Key==Key3)              //user has pressed the 3 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("3");
            //key is ignored when menu is on.
        }

        if (Key==Key4)              //user has pressed the 4 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("4");
            else ProcessMenuItemALG(4); //user is in menu mode, so process as a menu key
        }

        if (Key==Key5)              //user has pressed the 5 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("5");
            else ProcessMenuItemALG(5); //user is in menu mode, so process as a menu key
        }

        if (Key==Key6)              //user has pressed the 6 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("6");
            else ProcessMenuItemALG(6); //user is in menu mode, so process as a menu key
        }

        if (Key==Key7)              //user has pressed the 7 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("7");
            else ProcessMenuItemALG(1); //user is in menu mode, so process as a menu key
        }
            
        if (Key==Key8)              //user has pressed the 8 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("8");
            else ProcessMenuItemALG(2); //user is in menu mode, so process as a menu key
        }

        if (Key==Key9)              //user has pressed the 9 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("9");
            else ProcessMenuItemALG(3); //user is in menu mode, so process as a menu key
        }
            
        if (Key==Key0)              //user has pressed the 0 key
        {
            if (MenuMode==FALSE) ProcessNumberKeyALG("0");
            //key is ignored when menu is on.
        }
            
        if (Key==KeyPoint)          //user has pressed the DECIMAL POINT key
        {
            if (strlen(DisplayXreg)<MaxLCDdigits)
                //only do if decimal point does not already exist AND there is no exponent
                if ((DecimalIncluded==FALSE) && ExponentIncluded==FALSE)        
                {
                    if (ValueEntered==TRUE)     //user has pressed POINT as the first digit
                    {
                        strcpy(DisplayXreg,"0.");   //decimal point needs a 0 added to the start
                    }
                    else        
                        strcat(DisplayXreg,".");
                    DecimalIncluded=TRUE;
                    ValueEntered=FALSE;
                    UpdateLCDline1(DisplayYreg);
                    UpdateLCDline2(DisplayXreg);
                }
        }

        if (Key==KeyEnter)          //user has pressed the ENTER (=) key
        {
            if (MenuMode==FALSE)    //this key is not used in MENU mode
            {
                CompleteXreg();
                ResetFlags();
                ProcessXYoperator();    //process the previous XY operator (if any)
                if (OperatorYZ!=0)
                {
                    Yreg=Xreg;              //push X across and reset X ready for next value                                        //result is returned in X
                    ProcessYZoperator();    //if there is a pending higher order operation in the Zreg, then process it.
                    Xreg=Yreg;              //push Yreg result back into Xreg,so it gets overwritten next time.
                }                       //result is stored in Yreg for us.
                OperatorXY=0;           //clear operator
                UpdateDisplayRegs();
            }
        }

        if (Key==KeyPlus)           //user has pressed the PLUS key
        {
            if (MenuMode==FALSE)    //this key is not used in MENU mode
            {
                CompleteXreg();
                ResetFlags();
                ProcessXYoperator();    //process the previous XY operator (if any)
                Yreg=Xreg;              //push X across and reset X ready for next value                                        //result is returned in X
                if (OperatorYZ!=0)
                    ProcessYZoperator();    //if there is a pending higher order operation in the Zreg, then process it.
                                            //result is stored in Yreg for us.
                Xreg=0;

                OperatorXY=OPRplus;     //asign the new PLUS operator
                UpdateDisplayRegs();
            }
        }

        if (Key==KeyMinus)          //user has pressed the MINUS key
        {
            if (MenuMode==FALSE)        //this key is not used in MENU mode
            {
                CompleteXreg();
                ResetFlags();
                ProcessXYoperator();    //process the previous XY operator (if any)
                                        //result is returned in X
                Yreg=Xreg;              //push X across and reset X ready for next value
                if (OperatorYZ!=0)
                    ProcessYZoperator();    //if there is a pending higher order operation in the Zreg, then process it.
                                            //result is stored in Yreg for us.
                Xreg=0;
                OperatorXY=OPRminus;        //asign the new MINUS operator
                UpdateDisplayRegs();
            }
        }

        if (Key==KeyMult)               //user has pressed the MULTIPLY key
        {
            if (MenuMode==FALSE)        //this key is not used in MENU mode
            {
                CompleteXreg();
                ResetFlags();
                if ((OperatorXY==OPRplus)||(OperatorXY==OPRminus))  //check for operator precedence
                {
                    OperatorYZ=OperatorXY;
                    Zreg=Yreg;
                }
                else ProcessXYoperator();   //process the previous XY operator (if any)
                //result is returned in X
                Yreg=Xreg;              //push X across and reset X ready for next value
                Xreg=0;
                OperatorXY=OPRmult;     //asign the new MULT operator

                UpdateDisplayRegs();
            }
        }

        if (Key==KeyDiv)                //user has pressed the DIVIDE key
        {
            if (MenuMode==FALSE)            //this key is not used in MENU mode
            {
                CompleteXreg();
                ResetFlags();
                if ((OperatorXY==OPRplus)||(OperatorXY==OPRminus))  //check for operator precedence
                {
                    OperatorYZ=OperatorXY;
                    Zreg=Yreg;
                }
                else ProcessXYoperator();   //process the previous XY operator (if any)
                //result is returned in X
                Yreg=Xreg;              //push X across and reset X ready for next value
                Xreg=0;
                OperatorXY=OPRdiv;      //asign the new DIVIDE operator

                UpdateDisplayRegs();
            }
        }
        if (Key==KeyClear)          //user has pressed the CLEAR key
        {
            InverseKey=FALSE;       //reset the function flags
            HYPkey=FALSE;

            if (MenuMode==TRUE)     //menu is being displayed
            {
                MenuMode=FALSE;     //switch off menu mode
                UpdateLCDline1(DisplayYreg);    //display the X & Y regs again
                UpdateLCDline2(DisplayXreg);
            }
            else
            {
                if (ValueEntered==FALSE)        //only clear if there is something in the display register to clear
                {
                    strcpy(DisplayXreg,"");     //ONLY clear the display register
                    UpdateLCDline1(DisplayYreg);
                    UpdateLCDline2(DisplayXreg);
                    ResetFlags();
                }
                else                        //clear all the registers and operators
                {
                    ClearAllRegs();
                    UpdateDisplayRegs();
                    ResetFlags();
                }
            }
        }

        if (Key==KeyMenu)           //user has pressed the MENU key
        {
            CalcMenu();
        }

        if (Key==KeyEXP)            //user has pressed the EXP key
        {
            if (MenuMode==FALSE)            //this key is not used in MENU mode         
                if (ExponentIncluded==FALSE)    //can't add exponent twice
                {
                    ExponentIncluded=TRUE;
                    if (ValueEntered==TRUE) strcpy(DisplayXreg,"1e");   // EXP is first key pressed, so add a 1 to the front
                    else strcat(DisplayXreg,"e");   
                    ValueEntered=FALSE; 
                    UpdateLCDline2(DisplayXreg);        //update the display
                }
        }

        if (Key==KeyXY)             //user has pressed the X-Y key
        {
            if (MenuMode==FALSE)            //this key is not used in MENU mode         
            {
                CompleteXreg();     //enter value on stack if needed
                TEMPreg=Xreg;           
                Xreg=Yreg;          //swap and X and Y regs
                Yreg=TEMPreg;
                UpdateDisplayRegs();    //update display again
            }
        }

        if (Key==KeyLP)             ////user has pressed the left parentheses key
        {
            if (ValueEntered==FALSE)
            {
                UpdateLCDline1("ERR: No Operator");
                DelayMs(1000);
                UpdateLCDline1(DisplayYreg);        //restore the display
            }
            if ((MenuMode==FALSE) && (ValueEntered==TRUE))      //this key is not used in MENU mode         
                //and can only execute parentheses if the key is pressed directly after an operator
            {
                //all we do now is store the operators and Y/Zreg values in the stack
                OperatorXY6=OperatorXY5;    //shift all the operators
                OperatorXY5=OperatorXY4;
                OperatorXY4=OperatorXY3;
                OperatorXY3=OperatorXY2;
                OperatorXY2=OperatorXY1;
                OperatorXY1=OperatorXY;
                OperatorYZ6=OperatorYZ5;    //shift all the operators
                OperatorYZ5=OperatorYZ4;
                OperatorYZ4=OperatorYZ3;
                OperatorYZ3=OperatorYZ2;
                OperatorYZ2=OperatorYZ1;
                OperatorYZ1=OperatorYZ;
                Yreg6=Yreg5;                //shift all the regiaters
                Yreg5=Yreg4;
                Yreg4=Yreg3;
                Yreg3=Yreg2;
                Yreg2=Yreg1;
                Yreg1=Yreg;
                Zreg6=Zreg5;
                Zreg5=Zreg4;
                Zreg4=Zreg3;
                Zreg3=Zreg2;
                Zreg2=Zreg1;
                Zreg1=Zreg;
                
                Yreg=0; Zreg=0; OperatorYZ=0; OperatorXY=0;

                UpdateDisplayRegs();    //update display again
            }
        }

        if (Key==KeyRP)             //right parentheses
        {
            if ((MenuMode==FALSE))      //this key is not used in MENU mode         
                //and can only execute close parentheses if there is a matching opening one 
            {
                //closing a parenthese is the same as pressing =
                //so lets do the same as what we do with =
                CompleteXreg();
                ResetFlags();
                ProcessXYoperator();    //process the previous XY operator (if any)
                if (OperatorYZ!=0)
                {
                    Yreg=Xreg;              //push X across and reset X ready for next value                                        //result is returned in X
                    ProcessYZoperator();    //if there is a pending higher order operation in the Zreg, then process it.
                    Xreg=Yreg;              //push Yreg result back into Xreg,so it gets overwritten next time.
                }                       //result is stored in Yreg for us.
                Yreg=0;
                OperatorXY=0;           //clear operator

                //now we want to retrieve all the operators and Y/Zreg values from the stack
                OperatorXY =OperatorXY1;    //shift all the operators
                OperatorXY1=OperatorXY2;
                OperatorXY2=OperatorXY3;
                OperatorXY3=OperatorXY4;
                OperatorXY4=OperatorXY5;
                OperatorXY5=OperatorXY6;
                OperatorXY6=0;
                OperatorYZ =OperatorYZ1;    //shift all the operators
                OperatorYZ1=OperatorYZ2;
                OperatorYZ2=OperatorYZ3;
                OperatorYZ3=OperatorYZ4;
                OperatorYZ4=OperatorYZ5;
                OperatorYZ5=OperatorYZ6;
                OperatorYZ6=0;
                Yreg =Yreg1;                //shift all the regiaters
                Yreg1=Yreg2;
                Yreg2=Yreg3;
                Yreg3=Yreg4;
                Yreg4=Yreg5;
                Yreg5=Yreg6;
                Yreg6=0;
                Zreg =Zreg1;
                Zreg1=Zreg2;
                Zreg2=Zreg3;
                Zreg3=Zreg4;
                Zreg4=Zreg5;
                Zreg5=Zreg6;
                Zreg6=0;

                UpdateDisplayRegs();    //update display again
            }
        }

        if (Key==KeyRCL)                //user has pressed the RCL/STO key
        {
            //recalls one of the user defined memory registers into the Xreg
            if (MenuMode==FALSE)            //this key is not used in MENU mode         
            {
                CompleteXreg();
                StoreRecall();
                UpdateDisplayRegs();    //update display again
            }
        }
        
        if (Key==KeySign)           //user has pressed the +/- key
        {
            //changes the sign of the mantissa or exponent
            SignKey();
        }

    }   //end of entire loop
}

#endif
