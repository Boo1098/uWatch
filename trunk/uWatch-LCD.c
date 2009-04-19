//********************************************************
// uWatch
// LCD Functions
// Version 1.5
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
// strobes the LCD for data transmission
void LCD_STROBE(void)
{
	SetLCD_EN;
	ClearLCD_EN;
//	DelayMs(1);
}

// sets the LCD data bits
void SetLCDdata(char ch)
{
	PORTC=(PORTC&0xFF00)+ch;
}

// write a character to the LCD at current cursor position
void lcd_write(unsigned char ch)
{
	PORTC=(PORTC&0xFF00)+ch;
	LCD_STROBE();
}

// clear both lines of the LCD screen
void lcd_clear(void)
{
	ClearLCD_RS;
	lcd_write(0x1);
}

// write a string to the LCD
int lcd_puts(const char * s, int lmax)
{
    SetLCD_RS; // write characters

    int i = 0;
    while(s[i])
    {
        if (i == lmax) break;
        lcd_write(s[i++]);
    }
    return i;
}

const unsigned char *custom_char_memory[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

int custom_character( int charctr,const unsigned char* custom ) 
{
	// Note the absence of const in the parameter 'custom'
	// This is so we can have ram-based custom charactesr. This in turn
	// requires a manual cast to (non const) when calling with const data

    int i;
    ClearLCD_RS;
    lcd_write( 0x40 + charctr * 8 );
    SetLCD_RS;
    for ( i = 0; i < 8; i++ ) {
        lcd_write( custom[i] );
		custom_char_memory[charctr] = custom;	// save for power-up restoration
	}
    return charctr | 8;			// avoid '0' problem
}


// LCD GRAM is dynamic, so custom characters need to be restored on LCD power-up
void restoreCustomCharacters() {
	int i;
    for ( i = 0; i < 8; i++ )
		if ( custom_char_memory[i] )
			custom_character( i, &custom_char_memory[i][0] );
}


// move cursor to a specified position
// use position 40 for start of 2nd LCD line
void lcd_goto(unsigned char pos)
{
	ClearLCD_RS;
	lcd_write(0x80+pos);
}

/* initialise the LCD - put into 4 bit mode */ //BOO: seems to be 8-bit
void lcd_init(void)
{
	SetLCD_POWER;		//switch on the LCD power
	DelayMs(50);
	ClearLCD_RS;		// write control bytes
	DelayMs(20);		// power on delay
	lcd_write(0x0C );//F);
	DelayMs(20);
	lcd_write(0x01);		// CLEAR DISPLAY
	DelayMs(20);
	lcd_write(0x3C );// BOO: turn off cursor was 38);	//8 bit mode, 2 lines
}
