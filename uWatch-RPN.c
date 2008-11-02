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
    double TEMPreg, iTEMPreg; //temp register for calculations
    int c;

    DisplayXreg[0] = 0;
    DisplayYreg[0] = 0;
    ResetFlags();
    CurrentMenu=0;
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
        c = EnterNumber(Key);
        
        // handle specific keys
        switch (Key)
        {
        case KeyEnter: //user has pressed the ENTER key
            {
                CompleteXreg();
                PushStackUp();
                ResetFlags();

                //overwite the flag and force the Xreg to be overwritten on the next entry
                EnableXregOverwrite=TRUE;	
                UpdateDisplayRegs();
            }
            break;
        case KeyPlus: //user has pressed the PLUS key
            Operate(CALC_OP_PLUS);
            break;
        case KeyMinus: //user has pressed the MINUS key
            Operate(CALC_OP_MINUS);
            break;
        case KeyMult: //user has pressed the MULTIPLY key
            Operate(CALC_OP_MULT);
            break;
        case KeyDiv: //user has pressed the DIVIDE key
            Operate(CALC_OP_DIVIDE);
            break;
        case KeyClear: //user has pressed the CLEAR key
            PopStack();
            UpdateDisplayRegs();
            ResetFlags();
            EnableXregOverwrite=TRUE;
            break;
        case KeyXY: 
            {
                CompleteXreg();		//enter value on stack if needed

		//swap and X and Y regs
                TEMPreg=Xreg; iTEMPreg = iXreg;			
                Xreg=Yreg; iXreg = iYreg;
                Yreg=TEMPreg; iYreg = iTEMPreg;

                UpdateDisplayRegs();	//update display again
            }
            break;
        case KeyLP:  // ROLL
            {
                CompleteXreg();		//enter value on stack if needed
                TEMPreg=Xreg; iTEMPreg = iXreg;
                PopStack();
                Treg=TEMPreg; iTreg = iTEMPreg;

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
        }
    }
}

