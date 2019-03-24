
#ifndef LCD_H
#define LCD_H
	
#define LCD_PORT PORTD
#define LCD_RS	 RB2 //RB3   //2
#define LCD_EN	 RB3// RB2  //3
#define LCD_RW	 RD6
#define LCD_MAX_ROWS	2
#define LCD_MAX_COLS	16

enum ByteMode
{
	Cmd=0,
	Data=1
};

void LcdInit();
void LcdDelay(unsigned int d);
void SendByte(enum ByteMode Mode, unsigned char data);
void UploadLcdBuffer();
void LcdClear();
void LcdPrint(const unsigned char *s, unsigned char rowNo, unsigned char colNo);
void LcdByte(unsigned char b, unsigned char rowNo, unsigned char colNo);


#endif