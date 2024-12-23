
#include <xc.h>
#include"STD_TYPEs.h"
#include"DIO_register.h"
#include "keys_interface.h"


void keys_init()
{
	DDRA	= 0xF0;
	PORTA	= 0x0F;
}

char keys_Feedback()
{
	if((PINA & (1<<0)) == 0)
	{
		return keys_up;
	}
	else if((PINA & (1<<1)) == 0)
	{
		return keys_down;
	}
	else if((PINA & (1<<2)) == 0)
	{
		return keys_ok;
	}
	else if((PINA & (1<<3)) == 0)
	{
		return keys_cancel;
	}
	else
	return keys_non;
}
