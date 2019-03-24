
#include <pic.h>
#include "keypd.h"
#include "lcd.h"

#define KEY_1	RC0
#define KEY_2	RC1
#define KEY_3	RC2
#define KEY_4	RC3


#define MAX_BUFFER_SIZE	20
unsigned char KeypdBuffer[MAX_BUFFER_SIZE];
unsigned char IxPtr;
unsigned char OxPtr;
unsigned int buzzOffCounter;

unsigned char IsKeyPending()
{
	return IxPtr != OxPtr;
}

void PutKey(unsigned char key)
{
	KeypdBuffer[IxPtr] = key;
	if(++IxPtr == MAX_BUFFER_SIZE)
		IxPtr = 0;
	RB5=1;
	LcdDelay(800);
	RB5=0;
//	buzzOffCounter = 300;
		
}

unsigned char GetKey()
{
	unsigned char key = KeypdBuffer[OxPtr];
	if(++OxPtr == MAX_BUFFER_SIZE)
		OxPtr = 0;
	
	return key;
}

void KeyScan()
{
	unsigned char keyNo=0;
//	if(buzzOffCounter)
//	{
//		if(--buzzOffCounter == 0)	RB5=0;
//	}	

	if(KEY_1 == 1 && KEY_2 == 0 && KEY_3 == 0 && KEY_4 == 0)
	{
		while(KEY_1 ==1);
		keyNo = '3';
	}
	else if(KEY_1 == 0 && KEY_2 == 1 && KEY_3 == 0 && KEY_4 == 0)
	{
		while(KEY_2 ==1);
		keyNo = '4';
	}
	else if(KEY_1 == 0 && KEY_2 == 0 && KEY_3 == 1 && KEY_4 == 0)
	{
		while(KEY_3 ==1);
		keyNo = '2';
	}
	else if(KEY_1 == 0 && KEY_2 == 0 && KEY_3 == 0 && KEY_4 == 1)
	{
		while(KEY_4 ==1);
		keyNo = '1';
	}
	if(keyNo != 0)	PutKey(keyNo);
}	