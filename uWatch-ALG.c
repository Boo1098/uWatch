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

void PushOp(int op)
{
    OperatorZT = OperatorYZ;
    OperatorYZ = OperatorXY;
    OperatorXY = op;
}

void reduce(int lev)
{
    while (OperatorXY && opPrec(OperatorXY) <= lev)
    {
        Operation(OperatorXY);
    }
}

//***********************************
// The main Algebraic calculator routine
// Note that all variables are global
void ALGcalculator(void)
{
    unsigned int Key;       //keypress variables
    int c;
    DisplayXreg[0] = 0;
    DisplayYreg[0] = 0;
    ResetFlags();
    CurrentMenu=0;
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

            // pressing menu completes.
            CompleteXreg();

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
                        reduce(p);

                        // push OP
                        ResetFlags();
                        PushOp(mi);

                        // and values
                        Push();
                        Clx();

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
        Key = EnterNumber(Key);

        if (Key==KeyEnter)          //user has pressed the ENTER (=) key
        {
            CompleteXreg();
            ResetFlags();
            reduce(9); // ALL
            UpdateDisplayRegs();
        }

        if (Key==KeyPlus)           //user has pressed the PLUS key
        {
            CompleteXreg();
            ResetFlags();
            reduce(3);
            Push();
            PushOp(CALC_OP_PLUS);
            Clx();
            EnableXregOverwrite = TRUE;
            UpdateDisplayRegs();
        }

        if (Key==KeyMinus)          //user has pressed the MINUS key
        {
            CompleteXreg();
            ResetFlags();
            reduce(3);

            PushOp(CALC_OP_MINUS);
            Push();
            Clx();

            EnableXregOverwrite = TRUE;
            UpdateDisplayRegs();
        }

        if (Key==KeyMult)               //user has pressed the MULTIPLY key
        {
            CompleteXreg();
            ResetFlags();

            reduce(2);
            PushOp(CALC_OP_MULT);
            Push();
            Clx();
            EnableXregOverwrite = TRUE;
            UpdateDisplayRegs();
        }

        if (Key==KeyDiv)                //user has pressed the DIVIDE key
        {
            CompleteXreg();
            ResetFlags();
            reduce(2);
            PushOp(CALC_OP_DIVIDE);
            Push();
            Clx();
            EnableXregOverwrite = TRUE;
            UpdateDisplayRegs();
        }

        //user has pressed the CLEAR key
        if (Key==KeyClear)  
        {
            //clear all the registers and operators
            ClearAllRegs();
            ResetFlags();
            UpdateDisplayRegs();
        }

        if (Key==KeyXY)             //user has pressed the X-Y key
        {
            CompleteXreg();     //enter value on stack if needed
            SwapXY();
            UpdateDisplayRegs();    //update display again
        }

        ////user has pressed the left parentheses key
        if (Key==KeyLP)           
        {
            int i;

            if (!ValueEntered)
            {
                // perform implied multiply
                CompleteXreg();    
                ResetFlags();
                reduce(2);
                PushOp(CALC_OP_MULT);
                Push();
            }

            // restore values

            //shift all the operators
            for (i = PAREN_LEVELS; i > 0; --i)
            {
                OperatorsXY[i] = OperatorsXY[i-1];
                OperatorsYZ[i] = OperatorsYZ[i-1];
                OperatorsZT[i] = OperatorsZT[i-1];
            }

            for (i = PAREN_LEVELS-1; i > 0; --i)
            {
                Yregs[i] = Yregs[i-1]; iYregs[i] = iYregs[i-1];
                Zregs[i] = Zregs[i-1]; iZregs[i] = iZregs[i-1];
                Tregs[i] = Tregs[i-1]; iTregs[i] = iTregs[i-1];
            }

            Yregs[0] = Yreg;  iYregs[0] = iYreg;
            Zregs[0] = Zreg;  iZregs[0] = iZreg;
            Tregs[0] = Treg;  iTregs[0] = iTreg;

            ClearCurrentRegs();
            UpdateDisplayRegs();    //update display again
        }

        if (Key==KeyRP)             //right parentheses
        {
            int i;

            CompleteXreg();
            ResetFlags();
            reduce(9); // all
            
            if (OperatorsXY[1])
            {
                for (i = 0; i < PAREN_LEVELS; ++i)
                {
                    OperatorsXY[i] = OperatorsXY[i+1];
                    OperatorsYZ[i] = OperatorsYZ[i+1];
                    OperatorsZT[i] = OperatorsZT[i+1];
                }

                // clear top
                OperatorsXY[i] = 0;
                OperatorsYZ[i] = 0;
                OperatorsZT[i] = 0;
            
                // bring regs back
                Yreg = Yregs[0]; iYreg = iYregs[0];
                Zreg = Zregs[0]; iZreg = iZregs[0];
                Treg = Tregs[0]; iTreg = iTregs[0];

                for (i = 0; i < PAREN_LEVELS-1; ++i)
                {
                    Yregs[i] = Yregs[i+1]; iYregs[i] = iYregs[i+1];
                    Zregs[i] = Zregs[i+1]; iZregs[i] = iZregs[i+1];
                    Tregs[i] = Tregs[i+1]; iTregs[i] = iTregs[i+1];
                }
            }
            
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

