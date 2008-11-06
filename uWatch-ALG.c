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




void ProcessXYoperator(void)
{
    Operation(OperatorXY);
}

//***********************************
// The main Algebraic calculator routine
// Note that all variables are global
void ALGcalculator(void)
{
    unsigned int Key;       //keypress variables
    double TEMPreg;             //temp register for calculations
    int c;
    DisplayXreg[0] = 0;
    DisplayYreg[0] = 0;
    DecimalIncluded=FALSE;
    ExponentIncluded=FALSE;
    CurrentMenu=0;
    ValueEntered=TRUE;
    UpdateXregDisplay();
    UpdateYregDisplay();
    UpdateLCDline1(DisplayYreg);
    UpdateLCDline2(DisplayXreg);

    for (;;)
    {
        //wait for a keypress
        while (!(Key = KeyScan())) ;

        ResetSleepTimer();		

        //start to process the keypress
        if (Key==KeyMode)       // mode key was pressed, exit calc mode
        {
            return;
        }

        NextMode=FALSE;             //user pressed some key other than MODE, so ensure that when exit we go back to the time/date display

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
                    int p = opPrec(mi);

                    if (!p)
                    {
                        // operator now!
                        Operate(mi);
                    }
                    else
                    {
                        // push OP
                        CompleteXreg();
                        ResetFlags();

                        OperatorYZ=OperatorXY;
                        OperatorXY = mi;

                        // and values
                        Push();
                        Xreg = 0;

                        EnableXregOverwrite = TRUE;
                        UpdateDisplayRegs();
                    }
                }
            }
            else
            {
                // retrieve key pressed
                Key = -mi;
                if (Key == KeyMenu)
                {
                    if (++CurrentMenu>=DIM(MainMenus)) CurrentMenu=0;
                }
                else
                {
                    // escape from menu
                    Key = 0;
                    UpdateDisplayRegs();
                }

            }
        }

        // handle numbers
        c = EnterNumber(Key);

        if (Key==KeyEnter)          //user has pressed the ENTER (=) key
        {
            CompleteXreg();
            ResetFlags();
            ProcessXYoperator();    //process the previous XY operator (if any)
            ProcessXYoperator();    
            UpdateDisplayRegs();
        }

        if (Key==KeyPlus)           //user has pressed the PLUS key
        {
            CompleteXreg();
            ResetFlags();
            ProcessXYoperator();    //process the previous XY operator (if any)
            ProcessXYoperator();    //process the previous XY operator (if any)
            Yreg = Xreg;
            Xreg=0;
            EnableXregOverwrite = TRUE;

            OperatorXY=CALC_OP_PLUS;
            UpdateDisplayRegs();
        }

        if (Key==KeyMinus)          //user has pressed the MINUS key
        {
            CompleteXreg();
            ResetFlags();
            ProcessXYoperator();    //process the previous XY operator (if any)
            ProcessXYoperator();    //process the previous XY operator (if any)
            Yreg=Xreg;              //push X across and reset X ready for next value
            Xreg=0;
            EnableXregOverwrite = TRUE;
            OperatorXY=CALC_OP_MINUS;
            UpdateDisplayRegs();
        }

        if (Key==KeyMult)               //user has pressed the MULTIPLY key
        {
            CompleteXreg();
            ResetFlags();
            if (OperatorXY==CALC_OP_PLUS||OperatorXY==CALC_OP_MINUS)  //check for operator precedence
            {
                OperatorYZ=OperatorXY;
                Zreg=Yreg;
            }
            else ProcessXYoperator();   //process the previous XY operator (if any)
            //result is returned in X
            Yreg=Xreg;              //push X across and reset X ready for next value
            Xreg=0;
            EnableXregOverwrite = TRUE;
            OperatorXY=CALC_OP_MULT;
            UpdateDisplayRegs();
        }

        if (Key==KeyDiv)                //user has pressed the DIVIDE key
        {
            CompleteXreg();
            ResetFlags();
            if (OperatorXY==CALC_OP_PLUS||OperatorXY==CALC_OP_MINUS)  //check for operator precedence
            {
                OperatorYZ=OperatorXY;
                Zreg=Yreg;
            }
            else ProcessXYoperator();   //process the previous XY operator (if any)
                //result is returned in X
            Yreg=Xreg;              //push X across and reset X ready for next value
            Xreg=0;
            EnableXregOverwrite = TRUE;
            OperatorXY=CALC_OP_DIVIDE;
            UpdateDisplayRegs();
        }

        if (Key==KeyClear)          //user has pressed the CLEAR key
        {
            //clear all the registers and operators
            ClearAllRegs();
            ResetFlags();
            UpdateDisplayRegs();
        }

        if (Key==KeyXY)             //user has pressed the X-Y key
        {
            CompleteXreg();     //enter value on stack if needed
            TEMPreg=Xreg;           
            Xreg=Yreg;          //swap and X and Y regs
            Yreg=TEMPreg;
            UpdateDisplayRegs();    //update display again
        }

        if (Key==KeyLP)             ////user has pressed the left parentheses key
        {
            if (ValueEntered==FALSE)
            {
                UpdateLCDline1("ERR: No Operator");
                DelayMs(1000);
                UpdateLCDline1(DisplayYreg);        //restore the display
            }
            else
                //and can only execute parentheses if the key is pressed directly after an operator
            {
                //all we do now is store the operators and Y/Zreg values in the stack

                //shift all the operators
                memmove(OperatorsXY + 1, OperatorsXY,  6*sizeof(OperatorXY));
                memmove(OperatorsYZ + 1, OperatorsYZ,  6*sizeof(OperatorYZ));

                memmove(Yregs + 1, Yregs, 5*sizeof(Yreg));
                Yregs[0] = Yreg;

                memmove(Zregs + 1, Zregs, 5*sizeof(Zreg));
                Zregs[0] = Zreg;

                Yreg=0; Zreg=0; OperatorYZ=0; OperatorXY=0;
                UpdateDisplayRegs();    //update display again
            }
        }

        if (Key==KeyRP)             //right parentheses
        {
            //and can only execute close parentheses if there is a matching opening one 
            //closing a parenthese is the same as pressing =
            //so lets do the same as what we do with =
            CompleteXreg();
            ResetFlags();
            ProcessXYoperator();    //process the previous XY operator (if any)
            ProcessXYoperator();    //process the previous YZ operator (if any)

            //now we want to retrieve all the operators and Y/Zreg values from the stack
            memmove(OperatorsXY, OperatorsXY + 1,  6*sizeof(OperatorXY));
            memmove(OperatorsYZ, OperatorsYZ + 1,  6*sizeof(OperatorYZ));

            Yreg = Yregs[0];
            memmove(Yregs, Yregs + 1, 5*sizeof(Yreg));

            Zreg = Zregs[0];
            memmove(Zregs, Zregs + 1, 5*sizeof(Zreg));
            
            UpdateDisplayRegs();    //update display again
        }

        if (Key==KeyRCL)                //user has pressed the RCL/STO key
        {
            //recalls one of the user defined memory registers into the Xreg
            CompleteXreg();
            StoreRecall();
            UpdateDisplayRegs();    //update display again
        }
    }
}

