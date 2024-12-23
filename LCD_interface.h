#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_

#define LCDPORTDIR 	DDRC
#define LCDPORT 	PORTC
#define rs 			0
#define rw			1
#define en 			2

enum
{
	CMD=0,
	DATA,
};

void lcdwrite(char ch,char r);
void lcdprint(const char *str);
void lcdgotoxy(int x, int y);
void lcdclear();
void lcdfirstrow();
void lcdsecondrow();
void lcdbegin();


#endif /* LCD_INTERFACE_H_ */
