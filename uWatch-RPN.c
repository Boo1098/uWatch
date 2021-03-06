
//********************************************************
// uWatch
// RPN Calculator Functions
// Version 1.6
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

#include "def.h"
#include "calculator.h"
#include "menu.h"


static void pressEnter() {
    CompleteXreg();
    PushStackUp();
    ResetFlags();

    //overwite the flag and force the Xreg to be overwritten on the next entry
    EnableXregOverwrite=TRUE;	
    UpdateDisplayRegs();
}

//***********************************
// The main RPN calculator routine
// Note that all variables are global
void RPNcalculator(void)
{
    unsigned int Key; //keypress variables

    //CurrentMenu=0;
    UpdateDisplayRegs();
    ResetFlags();

    //main keypress loop is infinite.
    for (;;)
    {
		Key = GetDebouncedKey();

        //start to process the keypress
        // mode key was pressed, exit calc mode
        if (Key==KeyMode)
            return;

        //user pressed some key other than MODE, so ensure that when exit we go back to the time/date display
        NextMode=FALSE;		

        if ( Key == KeyMenu ) {
            WatchMode = WATCH_MODE_CALC_MENU;
            CompleteXreg();
            ResetFlags();
            calculatorMenu( calcMenus, CALC_MENU_SIZE );
            UpdateDisplayRegs();
            WatchMode = WATCH_MODE_CALC;
        }

        // handle numbers
        Key = EnterNumber(Key);
        
        // handle specific keys
        switch (Key)
        {
        case KeyEnter: //user has pressed the ENTER key
            pressEnter();
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
            break;
        case KeyXY: 
            CompleteXreg();		//enter value on stack if needed
            SwapXY();
            UpdateDisplayRegs();	//update display again
            break;
        case KeyLP:  // ROLL
            {
                double TEMPreg, iTEMPreg;
                CompleteXreg();		//enter value on stack if needed
                TEMPreg=Xreg; iTEMPreg = iXreg;
                PopStack();
                Treg=TEMPreg; iTreg = iTEMPreg;
                UpdateDisplayRegs();	//update display again
            }
            break;
        case KeyRCL: 
            //recalls the one of the user defined memory registers into the Xreg
            CompleteXreg();
            StoreRecall();
            UpdateDisplayRegs();	//update display again
            break;
        }


    }
}

