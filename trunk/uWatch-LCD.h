
#ifndef __UWATCH_LCD_H
#define __UWATCH_LCD_H

int custom_character( int charctr,const unsigned char* custom );
void lcd_goto(unsigned char pos);
int lcd_puts(const char * s, int lmax);
void lcd_write(unsigned char ch);
void lcd_clear(void);
void lcd_init(void);
void restoreCustomCharacters();


#endif
