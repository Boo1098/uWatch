//********************************************************
// uWatch
// LCD Functions
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
void lcd_puts(const char * s)
 {
	SetLCD_RS;			// write characters
	while(*s)
		lcd_write(*s++);
}

// move cursor to a specified position
// use position 40 for start of 2nd LCD line
void lcd_goto(unsigned char pos)
{
	ClearLCD_RS;
	lcd_write(0x80+pos);
}

/* initialise the LCD - put into 4 bit mode */
void lcd_init(void)
{
	SetLCD_POWER;		//switch on the LCD power
	DelayMs(50);
	ClearLCD_RS;		// write control bytes
	DelayMs(20);		// power on delay
	lcd_write(0x0F);
	DelayMs(20);
	lcd_write(0x01);
	DelayMs(20);
	lcd_write(0x38);	//8 bit mode, 2 lines
}
