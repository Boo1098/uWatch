//********************************************************
// uWatch
// Games Mode Functions
// Version 1.5
// Last Update: 16th March 09
// Copyright(c) 2008 David L. Jones
// Written for the Microchip C30 Compiler
// Target Device: PIC24FJ64GA004 (44pin)
// http://www.calcwatch.com
// EMAIL: david@alternatezone.com

// NOTE: This code is designed to be inserted inline into the uWatch-Main.C function
//       Most variables are passed as globals and are defined in uWatch-Main.C
//********************************************************

//global variables for Games Mode

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

static const char* GamesMenu[] = 
{
    "Lunar Lander",
    "Twenty One",
    "Chess",
};

static unsigned int wait()
{
    unsigned int KeyPress2;        //keypress variables
    do KeyPress2=KeyScan(); while(KeyPress2==0);
    return KeyPress2;
}

static int print_result()
{
    int i;
    int ok = 1;

    /* is there a legal move? */
    for (i = 0; i < first_move[1]; ++i)
        if (makemove(gen_dat[i].m.b)) 
        {
            takeback();
            break;
        }

    if (i == first_move[1]) 
    {
        ok = 0;
        if (in_check(side)) 
        {
            if (side == LIGHT)
                UpdateLCDline2("0-1 {Black}");
            else
                UpdateLCDline2("1-0 {White}");
        }
        else
            UpdateLCDline2("Stalemate");
    }
    else if (fifty >= 100)
    {
        UpdateLCDline2("Draw 50 moves");
        ok = 0;
    }

    if (!ok)
        wait();
    return ok;
}


//***********************************
// The main games mode routine
// Note that all variables are global
void GamesMode(void)
{
    unsigned int KeyPress2;        //keypress variables
    char s[MaxLCDdigits + 1];
    int Mode;

    Mode= DriveMenu("GAMES: +/- & ENT", GamesMenu, DIM(GamesMenu));

    switch(Mode)                
    {
        case 0: //lunar lander
        {
            UpdateLCDline1("--LUNAR LANDAR--");
            UpdateLCDline2(" By Shaun Chong ");
            KeyPress2=wait();
            if (KeyPress2==KeyMode) return;
            
            int elapsedTime   = 0;
            int height        = 1000;
            int velocity      = 50;
            int fuelRemaining = 150;
            int c;
            
            int burnAmount  = 0;
            int newVelocity = 0;
            int delta = 0;
            
            while (height > 0)
            {
                if (fuelRemaining > 0)
                {
                lunar:
                    UpdateLCDline1("ENT - burn fuel");
                    UpdateLCDline2("MENU - stats");
                    KeyPress2=wait();
                    if (KeyPress2==KeyMode) return;
                    if (KeyPress2==KeyMenu) //stats menu
                    {
                        UpdateLCDline1("1=Time  2=Height");
                        UpdateLCDline2("3=Speed 4=Fuel");
                        KeyPress2=wait();
                        if (KeyPress2==KeyMode) return;
                        if (KeyPress2==Key1)
                        {
                            UpdateLCDline1("Time (seconds):");
                            sprintf(s, "%i", elapsedTime);
                            UpdateLCDline2(s);
                            KeyPress2=wait();
                            if (KeyPress2==KeyMode) return;
                        }
                        if (KeyPress2==Key2)
                        {
                            UpdateLCDline1("Height (feet):");
                            sprintf(s, "%i", height);
                            UpdateLCDline2(s);
                            KeyPress2=wait();
                            if (KeyPress2==KeyMode) return;
                        }
                        if (KeyPress2==Key3)
                        {
                            UpdateLCDline1("Speed (feet/s):");
                            sprintf(s, "%i", velocity);
                            UpdateLCDline2(s);
                            KeyPress2=wait();
                            if (KeyPress2==KeyMode) return;
                        }
                        if (KeyPress2==Key4)
                        {
                            UpdateLCDline1("Fuel:");
                            sprintf(s, "%i", fuelRemaining);
                            UpdateLCDline2(s);
                            KeyPress2=wait();
                            if (KeyPress2==KeyMode) return;
                        }
                        goto lunar;
                    }
                    if (KeyPress2==KeyEnter)    //enter fuel to burn
                    {
                        UpdateLCDline1("Burn fuel(0-30):");
                        Xreg = burnAmount;
                        c = OneLineNumberEntry();
                        burnAmount = Xreg;
                    }
                }

                if (burnAmount < 0) burnAmount = 0;
                if (burnAmount > 30)    burnAmount = 30;
                if (burnAmount > fuelRemaining) burnAmount = fuelRemaining;

                newVelocity   = velocity - burnAmount + 5;
                fuelRemaining = fuelRemaining - burnAmount;
                height        = height - (velocity + newVelocity) * 0.5;
                elapsedTime   = elapsedTime + 1;
                velocity      = newVelocity;
                    
                sprintf(s, "%i", burnAmount);
                strcat(s, " fuel burnt");
                UpdateLCDline1(s);
                sprintf(s, "%i", fuelRemaining);
                strcat(s, " fuel left");
                UpdateLCDline2(s);
                do KeyPress2=KeyScan(); while(KeyPress2==0);
                if (KeyPress2==KeyMode) return;
            }

            /* Touchdown. Calculate landing parameters. */
            elapsedTime = elapsedTime - 1;
            velocity = velocity - 5 + burnAmount;
            height = height + (velocity + newVelocity) * 0.5;

            if (burnAmount == 5)
                delta = height / velocity;
            else
                delta = (sqrt (velocity * velocity + height * (10 - burnAmount * 2)) - velocity) / (5 - burnAmount);
            newVelocity = velocity + (5 - burnAmount) * delta;

            UpdateLCDline1("Touchdown!");
            UpdateLCDline2("ENT to see stats");
            KeyPress2=wait();
            if (KeyPress2==KeyMode) return;
            
            UpdateLCDline1("Time taken(sec):");
            sprintf(s, "%i", elapsedTime + delta);
            UpdateLCDline2(s);
            KeyPress2=wait();
            if (KeyPress2==KeyMode) return;
                
            UpdateLCDline1("Speed (feet/s):");
            sprintf(s, "%i", newVelocity);
            UpdateLCDline2(s);
            KeyPress2=wait();
            if (KeyPress2==KeyMode) return;
                
            UpdateLCDline1("Fuel left:");
            sprintf(s, "%i", fuelRemaining);
            UpdateLCDline2(s);
            KeyPress2=wait();
            if (KeyPress2==KeyMode) return;

            if (newVelocity <= 0)
                {
                    UpdateLCDline1("Perfect landing!");
                    UpdateLCDline2("ENT to cont.");
                    Xreg=0;
                    wait();
                    break;
                }
            else if (newVelocity < 2)
                {
                    UpdateLCDline1("Bumpy landing.");
                    UpdateLCDline2("ENT to cont.");
                    Xreg=0;
                    wait();
                    break;
                }
            else
                {
                    UpdateLCDline1("You crashed!");
                    UpdateLCDline2("ENT to cont.");
                    Xreg=0;
                    wait();
                    break;
                }
        }  break;
        case 1:   //twenty one
        {
            UpdateLCDline1("--TWENTY ONE--");
            UpdateLCDline2(" By Shaun Chong ");
            KeyPress2 = wait();
            if (KeyPress2==KeyMode) return;
            
            char s2[MaxLCDdigits+1];
            int player_total=0;
            int dealer_total=0;
            int done=0;
            
            void bust_player(void)
            {
               done=1;
               memset(s, '\0', sizeof(s)); //clear string
               sprintf(s2, "%i", player_total);
               strcat(s, "You:");
               strcat(s, s2);
               sprintf(s2, "%i", dealer_total);
               strcat(s, " Dealer:");
               strcat(s, s2);
               UpdateLCDline1(s);
               UpdateLCDline2("Player bust!");
               wait();
            }
            
            void bust_dealer(void)
            {
               done=1;
               memset(s, '\0', sizeof(s)); //clear string
               sprintf(s2, "%i", player_total);
               strcat(s, "You:");
               strcat(s, s2);
               sprintf(s2, "%i", dealer_total);
               strcat(s, " Dealer:");
               strcat(s, s2);
               UpdateLCDline1(s);
               UpdateLCDline2("Dealer bust!");
               wait();
            }
            
            void hit_player(void)
            {
               player_total+=rand()% 10+1;
               if (player_total>21) bust_player();
            }
            
            void hit_dealer(void)
            {
               dealer_total+=rand()% 10+1;
               if (dealer_total>21) bust_dealer();
            }
            
            void prepare(void)
            {
               player_total=0;
               dealer_total=0;   //reset card totals
               hit_player();
               hit_player();
               hit_dealer();
               hit_dealer();  //hit player and dealer twice for initial cards
               done=0;
            }
            
            void stand(void)
            {
               while (dealer_total<17) hit_dealer(); //The dealer's play, always hits when lower than 17 (vegas rules)
               done=1;
               memset(s, '\0', sizeof(s)); //clear string
               sprintf(s2, "%i", player_total);
               strcat(s, "You:");
               strcat(s, s2);
               sprintf(s2, "%i", dealer_total);
               strcat(s, " Dealer:");
               strcat(s, s2);
               UpdateLCDline1(s);
               if (player_total>dealer_total)
                  UpdateLCDline2("Player wins!");
               if (player_total==dealer_total)
                  UpdateLCDline2("Push!");
               if (player_total<dealer_total)
                  UpdateLCDline2("Dealer wins!");
               wait();
            }
            
            prepare();
            
            while (done==0)
            {
               memset(s, '\0', sizeof(s)); //clear string
               sprintf(s2, "%i", player_total);
               strcat(s, "You:");
               strcat(s, s2);
               strcat(s, " Dealer:?");
               UpdateLCDline1(s);
               UpdateLCDline2("Hit=1 Stand=2");
               KeyPress2 = wait();
               if (KeyPress2==KeyMode) return;
               if (KeyPress2==Key1) hit_player();
               if (KeyPress2==Key2) stand();
               if (done==1)
               {
                  UpdateLCDline1("Deal again?");
                  UpdateLCDline2("Yes=1 No=2");
                  KeyPress2 = wait();
                  if (KeyPress2==KeyMode) return;
                  if (KeyPress2==Key1) prepare();
                  if (KeyPress2==Key2) break;
               }
            }
        } break;
    case 2: // Chess
        {
            char s[16];
            int m;
            int from, to;
            int i;

            UpdateLCDline1("-- CHESS      --");
            UpdateLCDline2("enter move");
            KeyPress2=wait();
            if (KeyPress2==KeyMode) return;

            init_board();
            max_time = 1L<<25;
            max_depth = 3;

            // generate initial allowed moves
            gen();

            for (;;) 
            {
                m = -1;

                // get move
                if (OneLineNumberEntry() == KeyMode) return; // escape

                // parse move
                from = DisplayXreg[0] - '1';
                from += 8*(8 - (DisplayXreg[1] - '0'));

                to = DisplayXreg[2] - '1';
                to += 8*(8 - (DisplayXreg[3] - '0'));

                for (i = 0; i < first_move[1]; ++i)
                    if (gen_dat[i].m.b.from == from &&
                        gen_dat[i].m.b.to == to) 
                    {
                        if (gen_dat[i].m.b.bits & 32)
                        {
                          /* assume it's a queen */
                            i += 3;
                        }
                        m = i;
                        break;
                    }

                if (m == -1 || !makemove(gen_dat[m].m.b))  
                {
                    UpdateLCDline1("Illegal Move");
                    if (wait() == KeyMode) return;
                }
                else
                {
                    // is the game over?
                    ply = 0;
                    gen();
                    if (!print_result()) return;

                    /* computer's turn */
                    /* think about the move and make it */
                    UpdateLCDline1("thinking");

                    /* bump the CPU whilst we think... */
                    DisableSleepTimer();
                    Clock4MHz();
                    think();
                    /* and back again to slow.. */
                    Clock250KHz();
                    EnableSleepTimer();
                    ResetSleepTimer();

                    if (pv[0][0].u) 
                    {
                        UpdateLCDline1(move_str(pv[0][0].u));
                        makemove(pv[0][0].b);
                    }

                    ply = 0;
                    gen();
                    if (!print_result()) return;
                }
            }
        }
        break;
    }
}


