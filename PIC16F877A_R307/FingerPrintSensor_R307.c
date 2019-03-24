
#include <pic.h>
#include "FingerPrintSensor_R307.h"
#include "eeprom.h"
#include "lcd.h"
#include "keypd.h"

struct Package	_Package;

unsigned int PackageHeader = 0xEF01;
unsigned long DefModuleAddress = 0xFFFFFFFF;
unsigned long DefModulePassword=0x00000000;

unsigned char UartBuffer[MAX_UART_BUFFER_SIZE];
unsigned char UartIxPtr;
unsigned char UartOxPtr;


void UartInit()
{
	//Enables Recieve Interrupt
	GIE = 1;
	PEIE = 1;
	RCIE = 1;
	
	//Configuring Baud Rate 57600,N,8,1
    TXSTA=0x24;
    RCSTA=0x90;
    SPBRG=20;
}
//unsigned char UartTBuff[50];
//unsigned char i=0;
//unsigned char UFlag=0;
void SendUartByte(unsigned char val)
{
//	if(UFlag == 1)
//	{
//		UartTBuff[i]=val;
//		i++;
//	}
//	else
	{
		TXIF=0;
		TXREG=val;
	    while(TXIF==0);
	}
}		
void SendUartValue(unsigned long value, unsigned char size)
{	
	for(;size>0;size--)
	{
		if(size!=1)			SendUartByte((unsigned char)(value >> 8*(size-1)));
		else if(size==1)	SendUartByte((unsigned char)(value & 0xFF));
	}
}	
void SendUartString(unsigned char *s)
{
	while(*s!=0)
	{
		SendUartByte(*s);
		s++;
	}
}		
void ClearUartBuffer()
{
	unsigned char i;
	for(i=0; i<MAX_UART_BUFFER_SIZE; i++)	UartBuffer[i]=0;
	UartIxPtr = UartOxPtr = 0;
}
unsigned char IsUartBytePending()
{
	return UartIxPtr != UartOxPtr;
}
void PutUartByte(unsigned char val)
{
	UartBuffer[UartIxPtr] = val;
	if(++UartIxPtr == MAX_UART_BUFFER_SIZE)
		UartIxPtr = 0;
}
unsigned char GetUartByte()
{
	unsigned char val = UartBuffer[UartOxPtr];
	if(++UartOxPtr == MAX_UART_BUFFER_SIZE)
		UartOxPtr = 0;
	
	return val;
}
void sendPackage(enum PackageIdentifier identifier, unsigned char length, enum InstructionCode instruction, unsigned char *content)
{
	unsigned int i=0;
	_Package.CheckSum = 0;

	_Package.Header = PackageHeader;
	SendUartValue(_Package.Header, sizeof(_Package.Header));
	
	_Package.Address = DefModuleAddress;
	SendUartValue(_Package.Address, sizeof(_Package.Address));
	
	_Package.Identifier = identifier;
	SendUartValue(_Package.Identifier, sizeof(_Package.Identifier));
	
	_Package.Length = length;
	SendUartValue(_Package.Length, sizeof(_Package.Length));
	
	_Package.Instruction = instruction;
	SendUartValue(_Package.Instruction, sizeof(_Package.Instruction));
	
	_Package.Content = content;
	for(i=0; i<_Package.Length-3; i++)
	{
		SendUartByte(*(_Package.Content+i));
		_Package.CheckSum += *(_Package.Content+i);
	}	
	_Package.CheckSum += _Package.Identifier + (unsigned char)(_Package.Length >> 8) + 
						(unsigned char)(_Package.Length & 0xFF) + _Package.Instruction; 
	_Package.CheckSum = (unsigned int)_Package.CheckSum;
	SendUartValue(_Package.CheckSum, sizeof(_Package.CheckSum));
}


unsigned long ToggleEndianFormat(unsigned long val, unsigned char size)
{
	if(size == 1){}
	else if(size == 2)
	{
		unsigned int result=(unsigned int)val;
		result = (result<<8 | result >> 8);
		return result;
	}	
	else if(size == 4)
	{	
		return (val<<24 | ((val>>8)<<16) | ((val>>16)<<8) | val>>24);
	}	
	return val;
}

unsigned char FillUpResponse(unsigned char *data, unsigned char *confirmationCode)
{
	struct Response		_Response;
	unsigned long wait=0xFFFFF;
	unsigned char i=0,c=0,l=0,chkSumDigitCount=0,chkLength=0;
	unsigned char *respPtr = (unsigned char*)&_Response;
	unsigned int checkSumCalculated=0;
	
//	_Response.Data = data;
//	Delay(2);
	memset1(respPtr, 0, sizeof(struct Response));
	while(wait--)
	{
		if(IsUartBytePending())
		{
			c = GetUartByte();
			wait=0xFFFFF;
			if(chkLength == 0)
			{
				*(respPtr+i) = c;
				i++;
			}		
			else if(chkLength == 1 && l < (_Response._Response0.Length-3))
			{
				*(data+l) = c;
//				*(_Response.Data+l) = c;
				l++;
				checkSumCalculated += c;
			}
			else
			{
				*(respPtr + i) = c;
				i++;
				if(++chkSumDigitCount == 2)
				{
					break;
				}
			}		
			if(chkLength == 0 && i == sizeof(struct Response0))
			{
				_Response._Response0.Header = (unsigned int)ToggleEndianFormat(_Response._Response0.Header, sizeof(_Response._Response0.Header));
				_Response._Response0.Address = (unsigned long)ToggleEndianFormat(_Response._Response0.Address, sizeof(_Response._Response0.Address));
				_Response._Response0.Identifier = (unsigned char)ToggleEndianFormat(_Response._Response0.Identifier, sizeof(_Response._Response0.Identifier));
				_Response._Response0.Length = (unsigned int)ToggleEndianFormat(_Response._Response0.Length, sizeof(_Response._Response0.Length));
				_Response._Response0.ConfirmationCode = (unsigned char)ToggleEndianFormat(_Response._Response0.ConfirmationCode, sizeof(_Response._Response0.ConfirmationCode));
				//i++;
//				ToggleEndianFormat(_Response._Response0.Header,(unsigned char*)_Response._Response0.Header, sizeof(_Response._Response0.Header));
//				ToggleEndianFormat(_Response._Response0.Address,_Response._Response0.Address, sizeof(_Response._Response0.Address));
//				ToggleEndianFormat(_Response._Response0.Identifier,_Response._Response0.Identifier, sizeof(_Response._Response0.Identifier));
//				ToggleEndianFormat(_Response._Response0.Length,_Response._Response0.Length, sizeof(_Response._Response0.Length));
//				ToggleEndianFormat(_Response._Response0.ConfirmationCode,(unsigned char*)_Response._Response0.ConfirmationCode, sizeof(_Response._Response0.ConfirmationCode));
				chkLength = 1;	
				if(_Response._Response0.Header != 0xEF01 || _Response._Response0.Address != DefModuleAddress || _Response._Response0.Identifier != pAck)	
					return 0;
				checkSumCalculated += (unsigned char)_Response._Response0.Identifier +
									  (unsigned char)(_Response._Response0.Length >> 8) + 
									  (unsigned char)(_Response._Response0.Length & 0xFF) + 
									  (unsigned char)(_Response._Response0.ConfirmationCode);	
			}	
		}	
	}
	_Response.CheckSum0= (unsigned int)ToggleEndianFormat(_Response.CheckSum0, sizeof(_Response.CheckSum0));
	if(_Response.CheckSum0 == checkSumCalculated)
	{
		*confirmationCode = _Response._Response0.ConfirmationCode;			
		return 1;
	}		
	return 0;
}
unsigned char DeleteUser(unsigned char UserNo)
{
/*
	Erase 12th
EF 01 FF FF FF FF 01 00 07 0C 00 0B 00 01 00 20
	  EF 01 FF FF FF FF 07 00 03 00 00 0A	
*/
	unsigned char contentData[]={0x00, 0x00, 0x00, 0x01};
	unsigned char confirmationCode=0xFF;
	unsigned char dataOut[1]={0xFF};
	
	contentData[1]=UserNo;
	ClearUartBuffer();
	sendPackage(pCmd, 0x07, 0x0C, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}	
	return 0;
}		

unsigned char VerifyPswd()
{	
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	unsigned char confirmationCode=0xFF;
//UFlag=0;
	ClearUartBuffer();
	sendPackage(pCmd, 0x07, 0x13, (unsigned char *)&DefModulePassword);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}
	return 0;
}
unsigned char ScanForImage()
{
	unsigned char contentData[]={0};
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	unsigned char confirmationCode=0xFF;

	ClearUartBuffer();
	sendPackage(pCmd, 0x03, 0x01, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}
	return 0;
}
unsigned char GenCharacterFileFromImage()
{
	unsigned char contentData[]={0x01};
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	unsigned char confirmationCode=0xFF;
//UFlag=0;
	ClearUartBuffer();
	sendPackage(pCmd, 0x04, 0x02, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}
	return 0;
}
unsigned char LoadFingerTemplate(unsigned int position)
{
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	unsigned char confirmationCode=0xFF;
	unsigned char contentData[3]={0x02, 0x00};

	contentData[2]=position;
	ClearUartBuffer();
	sendPackage(pCmd, 0x06, 0x07, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}
	return 0;
}
unsigned char MatchTemplate()
{
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	unsigned char confirmationCode=0xFF;
	unsigned char contentData[]={0};

	ClearUartBuffer();
	sendPackage(pCmd, 0x03, 0x03, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}
	return 0;
}
unsigned char HiSpeedScan0(unsigned char *UserNo)
{
	unsigned char scanResult=0xFF;
//	unsigned char UserNo=0;
	while(1)
	{
		KeyScan();
		if(IsKeyPending())
		{
			if(GetKey() == '3')	return 0xFF;
		}	
		scanResult = ScanForImage();
		if(scanResult == 1)
		{
			if(GenCharacterFileFromImage())
			{
				return HiSpeedScan(UserNo);
/*
				if(HiSpeedScan(UserNo) == 1)
				{
					return HiSpeedScan(UserNo);
					if(*UserNo == 0)
					{
						
						asm("nop");
						asm("nop");
					}
				}	
			*/
			}
		}
	}
	return 0;
}			
	  
	  
unsigned char HiSpeedScan(unsigned char *UserNo)
{
/*
EF 01 FF FF FF FF 01 00 08 1B 01 00 00 03 E9 01 11
	  EF 01 FF FF FF FF 07 00 07 00 00 00 00 65 00 73 (oth finger)
	  EF 01 FF FF FF FF 07 00 07 00 00 01 00 3C 00 4B (1th finger)
	  EF 01 FF FF FF FF 07 00 07 09 00 00 00 00 00 17 (Not Matched)
*/
	unsigned char dataOut[]={0xFF, 0xFF, 0xFF, 0xFF, 0x00}; //dummy value assigned
	unsigned char confirmationCode=0xFF;
	unsigned char contentData[]={0x01, 0x00, 0x00, 0x03, 0xE9};
//UFlag=0;
	ClearUartBuffer();
	sendPackage(pCmd, 0x08, 0x1B, contentData);
//	Delay(2);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00){*UserNo=dataOut[1];	return 1; }
	}
	*UserNo = 0;
	return 0;
}
unsigned char EraseAllUsers()
{
/*
ERASE ALL
EF 01 FF FF FF FF 01 00 03 0D 00 11
      EF 01 FF FF FF FF 07 00 03 00 00 0A {SUCCESS)
      EF 01 FF FF FF FF 07 00 03 01 00 0B {Fail)
 */	
	unsigned char contentData[]={0x00};//, 0x0B, 0x00, 0x01};
	unsigned char confirmationCode=0xFF;
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	
	ClearUartBuffer();
	sendPackage(pCmd, 0x03, 0x0D, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}	
	return 0;
}


//
	
unsigned char CheckUser(unsigned int position)
{
	unsigned char scanResult=0xFF;
	
	LoadFingerTemplate(position);
	while(1)
	{
		scanResult = ScanForImage();
		if(scanResult == 0)
		{
			if(!GenCharacterFileFromImage())
			{
				unsigned char MatchResult = MatchTemplate();
				if(MatchResult == 0x08)
				{
					return 0;
				}
				else if(MatchResult == 0x00)
				{
					return 1;
				}
			}
		}	
	}
	return 0;	
}
//
unsigned char AddUserA()
{
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	unsigned char confirmationCode=0xFF;
	unsigned char contentData[]={0x02};

	ClearUartBuffer();
	sendPackage(pCmd, 0x04, 0x02, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}
	return 0;
}
unsigned char AddUserB()
{
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	unsigned char confirmationCode=0xFF;
	unsigned char contentData[]={0x05};

	ClearUartBuffer();
	sendPackage(pCmd, 0x03, 0x05, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}
	return 0;
}
unsigned char AddUserC(unsigned char position)
{
	unsigned char dataOut[]={0xFF}; //dummy value assigned
	unsigned char confirmationCode=0xFF;
	unsigned char contentData[]={0x02, 0x00, 0x00, 0x00};
	contentData[2] = position;
	
	ClearUartBuffer();
	sendPackage(pCmd, 0x06, 0x06, contentData);
	if(FillUpResponse(dataOut, &confirmationCode) )
	{
		if(confirmationCode == 0x00)	return 1;
	}
	return 0;
}
unsigned char AddNewUser(unsigned char position)
{
/*	ADD NEW USER

EF 01 FF FF FF FF 01 00 03 01 00 05
      EF 01 FF FF FF FF 07 00 03 02 00 0C (No Finger Found)
      EF 01 FF FF FF FF 07 00 03 00 00 0A (Finger Detected)

EF 01 FF FF FF FF 01 00 04 02 01 00 08
      EF 01 FF FF FF FF 07 00 03 00 00 0A

EF 01 FF FF FF FF 01 00 03 01 00 05
      EF 01 FF FF FF FF 07 00 03 02 00 0C (No Finger Found)
      EF 01 FF FF FF FF 07 00 03 00 00 0A (Finger Detected)

EF 01 FF FF FF FF 01 00 04 02 02 00 09
      EF 01 FF FF FF FF 07 00 03 00 00 0A

EF 01 FF FF FF FF 01 00 03 05 00 09
      EF 01 FF FF FF FF 07 00 03 00 00 0A

EF 01 FF FF FF FF 01 00 06 06 02 00 0B 00 1A
      EF 01 FF FF FF FF 07 00 03 00 00 0A (Success)
      EF 01 FF FF FF FF 07 00 03 0A 00 14 (Combination Error)	 
*/
	unsigned char scanResult=0xFF;
	
	while(1)
	{
		KeyScan();
		if(IsKeyPending())
		{
			if(GetKey() == '3')	return 0xFF;
		}
		scanResult = ScanForImage();
		if(scanResult == 1)
		{
			GenCharacterFileFromImage();
			while(1)
			{
				KeyScan();
				if(IsKeyPending())
				{
					if(GetKey() == '3')	return 0xFF;
				}
				scanResult = ScanForImage();
				if(scanResult == 1)
				{			
					AddUserA();
					AddUserB();
					if(AddUserC(position) == 1) 
					{
						reserveEElocation(position);
						return 1;
					}	
					else	return 0;
				}				
			}
		}	
	}	
	return 0;
}
	

