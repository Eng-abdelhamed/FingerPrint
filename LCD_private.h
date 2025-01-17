#include <xc.h>

#ifndef LCD_PRIVATE_H_
#define LCD_PRIVATE_H_

#define uchar		unsigned char
#define uint 		unsigned int

#define RSLow		(LCDPORT&=~(1<<rs))
#define RSHigh	 	(LCDPORT|= (1<<rs))
#define RWLow 		(LCDPORT&=~(1<<rw))
#define ENLow		(LCDPORT&=~(1<<en))
#define ENHigh		(LCDPORT|= (1<<en))


#endif
