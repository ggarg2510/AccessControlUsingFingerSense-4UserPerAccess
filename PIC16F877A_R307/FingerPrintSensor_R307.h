
#ifndef FINGER_PRINT_SENSOR_R307_H
#define FINGER_PRINT_SENSOR_R307_H

#define MAX_UART_BUFFER_SIZE	56



enum InstructionCode
{
	GenImg   	= 0x01,
	Img2Tz   	= 0x02,
	Match	 	= 0x03,
	Search		= 0x04,
	RegModel	= 0x05,
	Store		= 0x06,
	LoadChar	= 0x07,
	UpChar		= 0x08,
	DownChar	= 0x09,
	UpImage		= 0x0A,
	DownImage	= 0x0B,
	DeleteChar	= 0x0C,
	Empty		= 0x0D,
	SetSysParam	= 0x0E,
	ReadSysParam = 0x0F,
	SetPwd		= 0x12,
	VfyPwd		= 0x13,
	GetRandomCode = 0x14,
	SetAddr		= 0x15,
	Control		= 0x17,
	WriteNotepad = 0x18,
	ReadNotepad	= 0x19,
	HiSpeedSearch = 0x1B,
	TemplateNum	= 0x1D
		
};
enum PackageIdentifier
{
	pCmd	 = 0x01,
	pData = 0x02,
	pAck  = 0x07,
	pEOP  = 0x08
};

struct Package
{
	unsigned int Header;
	unsigned long Address;
	enum PackageIdentifier Identifier;
	unsigned int Length;
	enum InstructionCode Instruction;
	unsigned char *Content;
	unsigned int CheckSum;		//sum of each byte of Identifier, Length, Content; taking only lower 2 bytes.
};

struct Response0
{
	unsigned int Header;
	unsigned long Address;
	enum PackageIdentifier Identifier;
	unsigned int Length;
	unsigned char ConfirmationCode;
}	
struct Response
{
	struct Response0  _Response0;
//	unsigned char *Data;
	unsigned int CheckSum0;		//sum of each byte of Identifier, Length, Content; taking only lower 2 bytes.
};
	
void UartInit();
void SendUartByte(unsigned char val);
void SendUartValue(unsigned long value, unsigned char size);
void SendUartString(unsigned char *s);
void ClearUartBuffer();
unsigned char IsUartBytePending();
void PutUartByte(unsigned char val);
unsigned char GetUartByte();
void sendPackage(enum PackageIdentifier identifier, unsigned char length, enum InstructionCode instruction, unsigned char *content);
unsigned long ToggleEndianFormat(unsigned long val, unsigned char size);
unsigned char FillUpResponse(unsigned char *data, unsigned char *confirmationCode);
unsigned char DeleteUser(unsigned char UserNo);
unsigned char VerifyPswd();
unsigned char ScanForImage();
unsigned char GenCharacterFileFromImage();
unsigned char LoadFingerTemplate(unsigned int position);
unsigned char MatchTemplate();
unsigned char HiSpeedScan0(unsigned char *UserNo);
unsigned char HiSpeedScan(unsigned char *UserNo);
unsigned char EraseAllUsers();
unsigned char CheckUser(unsigned int position);
unsigned char AddUserA();
unsigned char AddUserB();
unsigned char AddUserC(unsigned char position);
unsigned char AddNewUser(unsigned char position);
/*
EF 01
FF FF FF FF 
01 
00 08
1B 01 00 00 03 E9
01 11

res:
EF 01
FF FF FF FF 
07
00 07
00 00 04 00 6A
00 7C
*/ 
/*
******** Confirmation Codes ******** 
00h: commad execution complete;
01h: error when receiving data package;
02h: no finger on the sensor;
03h: fail to enroll the finger;
04h: fail to generate character file due to the over-disorderly fingerprint image;
05h: fail to generate character file due to the over-wet fingerprint image;
06h: ail to generate character file due to the over-disorderly fingerprint image;
07h: fail to generate character file due to lackness of character point or over-smallness of fingerprint image
08h: finger doesn’t match;
09h: fail to find the matching finger;
0Ah: fail to combine the character files;
0Bh: addressing PageID is beyond the finger library
0Ch: error when reading template from library or the template is invalid;
0Dh: error when uploading template;
0Eh: Module can’t receive the following data packages.
0Fh: error when uploading image;
10h: fail to delete the template;
11h: fail to clear finger library;
13h: wrong password!
15h: fail to generate the image for the lackness of valid primary image;
18h: error when writing flash;
19h: No definition error;
1Ah: invalid register number;
1Bh: incorrect configuration of register;
1Ch: wrong notepad page number;
1Dh: fail to operate the communication port;

******** others: system reserved; ******** 
41h: No finger on sensor when add fingerprint on second time.
42h: fail to enroll the finger for second fingerprint add.
43h: fail to generate character file due to lackness of character point or over-smallness of fingerprint image for second fingerprint add
44h: fail to generate character file due to the over-disorderly fingerprint image for second fingerprint add;
45h: Duplicate fingerprint
*/
//const unsigned char VfyPwd[]={0xEF, 0x01, 
//0xFF, 0xFF, 0xFF, 0xFF, 
//0x01, 
//0x00, 0x07, 
//0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B};
//const unsigned char VfyPwdAck[]={
//0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x03, 0x00, 0x00, 0x0A};
//
//EF 01 FF FF FF FF 07 00 03 00 00 0A
/*
 -- MACTHING WITH 0th location
EF 01 FF FF FF FF 01 00 06 07 02 00 00 00 10
EF 01 FF FF FF FF 01 00 03 01 00 05

 -- MACTHING WITH 1st location
EF 01 FF FF FF FF 01 00 06 07 02 00 01 00 11
      EF 01 FF FF FF FF 07 00 03 00 00 0A

EF 01 FF FF FF FF 01 00 03 01 00 05
      EF 01 FF FF FF FF 07 00 03 02 00 0C  (No Finger found)
	  EF 01 FF FF FF FF 07 00 03 00 00 0A  (Finger found)
	  
EF 01 FF FF FF FF 01 00 04 02 01 00 08
      EF 01 FF FF FF FF 07 00 03 00 00 0A
  
EF 01 FF FF FF FF 01 00 03 03 00 07
	  EF 01 FF FF FF FF 07 00 05 00 00 B7 00 C3  (Finger matched)
	  EF 01 FF FF FF FF 07 00 05 08 00 06 00 1A  (Finger not matched)
	  
	  
	  
EF 01 FF FF FF FF 07 00 05 08 00 02 00 16
*/

/*

*/

/*
	Erase 12th
EF 01 
FF FF FF FF 
01 
00 07 
0C 
00 0B 00 01 
00 20
	  EF 01 FF FF FF FF 07 00 03 00 00 0A	
*/
/*
ERASE ALL
EF 01 FF FF FF FF 01 03 0D 00 11
      EF 01 FF FF FF FF 07 03 00 00 0A {SUCESS)
      EF 01 FF FF FF FF 07 03 00 01 0B {Fail)
 */

#endif