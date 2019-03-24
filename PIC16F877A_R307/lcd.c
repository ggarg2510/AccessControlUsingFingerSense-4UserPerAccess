

#include <pic.h>
#include "lcd.h"
#include "eeprom.h"

unsigned char LcdBuffer[2][16];

	
void LcdInit()
{
	TRISD = 0x00;
	TRISB = 0X00;      // set portb as output;
//	TRISB3 = 0;
//	TRISB2 = 0;
//	TRISB3 = 0;
    LCD_RW=0;
	LCD_PORT=0;
	LcdDelay(1000);
	SendByte(Cmd, 0x38);
	SendByte(Cmd, 0x0C);
	LcdClear();	
}

void UploadLcdBuffer()
{
	unsigned char i,j;
	
	SendByte(Cmd, 0x01);
	SendByte(Cmd, 0x80);
	for(i=0; i<LCD_MAX_ROWS; i++)
	{
		for(j=0; j<LCD_MAX_COLS; j++)
		{
			SendByte(Data, LcdBuffer[i][j]);
		}	
		SendByte(Cmd, 0xC0);
	}	
}
void LcdClear()
{
	memset1((unsigned char *)&LcdBuffer, 0, sizeof(LcdBuffer));
	SendByte(Cmd, 0x01);
}
			
void LcdByte(unsigned char b, unsigned char rowNo, unsigned char colNo)
{
	LcdBuffer[rowNo][colNo] = b;
	UploadLcdBuffer();
}

void LcdPrint(const unsigned char *s, unsigned char rowNo, unsigned char colNo)
{
	while(*s!=0)
	{
		if(*s == '\n')
		{
			if(++rowNo > LCD_MAX_COLS)	break;
			else colNo=0;
		}
		else if(rowNo<LCD_MAX_COLS && colNo<LCD_MAX_COLS)
		{
			LcdBuffer[rowNo][colNo] = *s;
			colNo++;
		}
		s++;
	}
	UploadLcdBuffer();
}
	
void LcdDelay(unsigned int d)
{
	while(d>0)	d--;
}	
	
void SendByte(enum ByteMode Mode, unsigned char val)
{
	if(Mode == Data && val == 0)	val = ' ';
	LCD_PORT = val;
	
	LCD_RS = Mode;
	LCD_EN=0;
	LcdDelay(500);
	LCD_EN=1;
	LcdDelay(500);
};
	