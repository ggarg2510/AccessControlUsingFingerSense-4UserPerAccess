
#include <pic.h>
#include "eeprom.h"


void ee_eraseNPos(unsigned char n)
{
	unsigned char i=0;
	for(i=0;i<n;i++)
	{
		ee_write(i, 0xFF);
	}	
}	
signed char getNextAvailLocation()
{
	if(ee_read(TOT_USERS_COUNT_ADD) == MAX_R307_USERS)	return -1;
	
	unsigned char i=1;
	while(i<MAX_R307_USERS)
	{
		unsigned char result = ee_read(i);
		if(result == 0xFF)	return i;
		i++;
	}	
	return -1;
}
void reserveEElocation(unsigned char loc)
{
	ee_write(loc, 0x01);
	if(loc != 0 && loc != 1)	ee_write(TOT_USERS_COUNT_ADD, ee_read(TOT_USERS_COUNT_ADD)+1);
}	
void releaseEElocation(unsigned char loc)
{
	ee_write(loc, 0xFF);
	if(loc != 0 && loc != 1)	ee_write(TOT_USERS_COUNT_ADD, ee_read(TOT_USERS_COUNT_ADD)-1);
}	
	
void ee_write(unsigned char a,unsigned char d)
{
again:
EEPGD=0;
EEADR=a;
EEDATA=d;
WRERR=0;
WREN=1;
EECON2=0X55;
EECON2=0XAA;
WR=1;
while(WR==1)
WREN=0;
if(WRERR==1)
{
goto again;
}
}
    
     
unsigned char ee_read(unsigned char a)
{
unsigned char d;
EEPGD=0;
EEADR=a;
WREN=0;
RD=1;
while(RD==1);
d=EEDATA;
return d;
}

void Delay(unsigned int d)
{
	unsigned long dd=99999;
	dd=d*dd;
	while(dd--);
}
void memcpy1(unsigned char *toStr, unsigned char *fromStr, unsigned char len)
{
	while(len>0)
	{
		len--;
		*toStr = *fromStr;
		toStr++;
		fromStr++;
	}		
}	 
void memset1(unsigned char *str, unsigned char data, unsigned int len)
{
	while(len>0)
	{
		len--;
		*(str+len)=data;
	}	
}	
unsigned char TotDigits(unsigned long val)
{
	unsigned char result = 0;
	while(val!=0)
	{
		val=val/10;
		result++;
	}	
	return result;
}
	
void DecToStr(unsigned long val, unsigned char *dest, unsigned char minDigit)
{
	unsigned char len = TotDigits(val);
	if(len<minDigit)  len = minDigit;
	*(dest+len)=0;		
	len--;
	while(len!=0xFF)
	{
		//if(val != 0) 
		*(dest+len) = '0' + val%10;
		//else         *(dest+len) = ' ';
		
		len--;
		val=val/10;
	}	
}
	