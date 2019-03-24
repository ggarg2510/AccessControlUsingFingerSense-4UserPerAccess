
#include <pic.h>
#include "lcd.h"
#include "keypd.h"
#include "FingerPrintSensor_R307.h"
#include "eeprom.h"

__CONFIG(0x3F3A);
#define RELAY	RB1
/*
 1234567890123456
  XX  XX  XX  XX 

 

*/	
const unsigned char UserNoArr[][3]={
	"A \0", "B \0", "C \0", 
	"D \0", "E \0", "F \0", 
	"G \0", "H \0", "I \0", 
	"J \0", "K \0", "L \0"
};
/*
struct UserNo
{
	unsigned char ShiftA[4][2];
	unsigned char ShiftB[4][2];
	unsigned char ShiftC[4][2];
};
struct UserNo		_UserNo={{"1A", "2A", "3A", "4A"},{"1B", "2B", "3B", "4B"},{"1C", "2C", "3C", "4C"},};
*///unsigned char UserNoIndex;
	
unsigned char EraseAllUsersAfterAdminConfirm()
{
	unsigned char isEraseAllUsers=1;
	if(ee_read(0x00) == 0x01)
	{
		LcdClear();
		LcdPrint("Admin Required\nPlace Finger",0,0);
		isEraseAllUsers = 0;
		//Match previous admin user
		unsigned char UserNo;
		unsigned char result =  HiSpeedScan0(&UserNo);
		if(result == 0xFF)	return 0;
		else
		{
			if(result == 1 && (UserNo == 0 || UserNo == 1))
			{
				LcdClear();
				LcdPrint("Admin found",0,0);
				Delay(2);
				isEraseAllUsers = 1;
			}	
			else
			{
				LcdClear();
				LcdPrint("Admin Not\n  found",0,0);
				Delay(2);
				return 0;
			}	
		}	
	}
	if(isEraseAllUsers == 1)
	{
		EraseAllUsers();
		ee_eraseNPos(MAX_R307_USERS);
		ee_write(TOT_USERS_COUNT_ADD,0x00);

		LcdClear();
		LcdPrint("All Users\n Deleted",0,0);
		Delay(2);
	}	
	return 0;
}	

unsigned char ControlAccess()
{
	unsigned char UserNo=0;
	unsigned char result=0;
	unsigned char SelectedUserArr[4];
	unsigned char SwitchRelay=1;

	RB5=0;
	LcdClear();
	LcdPrint("Place Finger", 0,0);
	memset1(SelectedUserArr, 0xFF, sizeof(SelectedUserArr));
	while(1)
	{
		result=HiSpeedScan0(&UserNo);
				
		if(result == 0xFF) break;
		else if(result == 0x01)
		{
			if(UserNo>=0 && UserNo<14)
			{
				if(UserNo>=2 && UserNo<14)
				{
					unsigned char c =0;
					UserNo = UserNo - 2;
					c = UserNo/3;
					SwitchRelay = 0;
					SelectedUserArr[c] = UserNo;
					
					if(SelectedUserArr[c]!=0xFF)	LcdPrint(&UserNoArr[SelectedUserArr[c]][0], 1,3+c*3);
					if(SelectedUserArr[0] != 0xFF && SelectedUserArr[1] != 0xFF && 
					   SelectedUserArr[2] != 0xFF && SelectedUserArr[3] != 0xFF)
					{
						SwitchRelay = 1;						
					}	
				}	
				if(SwitchRelay == 1)
				{
					RELAY = 1; 
					Delay(3);
					RELAY = 0; 
					return;					
				}
				Delay(3);	
			}	
		}	
		else if(result == 0x00)
		{
	//		LcdClear();
	//		LcdPrint("Invalid User", 0,0);
			RB5=1;
			Delay(1);
//			LcdDelay(200);
			RB5=0;
		//	break;
		}	
	}	
	return 0;	
}	


unsigned char AlterAdminUser(unsigned char UserIndex)
{
	unsigned char Create0thPositionUser=1;

	if(ee_read(UserIndex-'1') == 0x01)
	{
		LcdClear();
		LcdPrint("Admin Required\nPlace Finger",0,0);
		Create0thPositionUser = 0;
		//Match previous admin user
		unsigned char UserNo;
		unsigned char result =  HiSpeedScan0(&UserNo);
		if(result == 0xFF)	return 0;
		else
		{
			if(result == 1 && (UserNo == UserIndex - '1') )
			{
				LcdClear();
				LcdPrint("Admin found",0,0);
				Delay(2);
				if(DeleteUser(UserIndex-'1') == 1)
				{
					LcdClear();
					LcdPrint("Admin Deleted",0,0);
					Delay(1);
					Create0thPositionUser = 1;
				}
				else
				{
					LcdClear();
					LcdPrint("Can't Delete\n Admin",0,0);
					Delay(2);
					return 0;
				}
			}	
			else
			{
				LcdClear();
				LcdPrint("Admin Not\n  found",0,0);
				Delay(2);
				return 0;
			}	
		}	
	}
	if(Create0thPositionUser == 1)
	{
		LcdClear();
		LcdPrint("Plz Register\n new Admin",0,0);
		Delay(2);
		LcdClear();
		LcdPrint("Place Finger",0,0);
		Delay(2);
		unsigned char NewUserResult = AddNewUser(UserIndex-'1');
		if(NewUserResult == 1)
		{
			LcdClear();
			LcdPrint("Admin \n  Registered",0,0);
			Delay(2);
		}
		else if(NewUserResult == 0xFF)	return 0;
		else
		{
			LcdClear();
			LcdPrint("Error In\n  Registeration",0,0);
			Delay(2);
		}		
	//	addUser(0);
	}	
return 0;	
}	


unsigned char AddNewUserAfterAdminConfirm()
{
	unsigned char isAddNewUser=1,UserNo,result,key;
	unsigned char buff[5];
	signed char _availSpaceResult = -1;
	
	if(ee_read(0x00) == 0x01)
	{
		LcdClear();
		LcdPrint("Admin Required\nPlace Finger",0,0);
		isAddNewUser = 0;
		//Match previous admin user
		result =  HiSpeedScan0(&UserNo);
		if(result == 0xFF)	return 0;
		else
		{
			if(result == 1 && (UserNo == 0 || UserNo == 1))
			{
				LcdClear();
				LcdPrint("Admin found",0,0);
				Delay(2);
				isAddNewUser = 1;
			}	
			else
			{
				LcdClear();
				LcdPrint("Admin Not\n  found",0,0);
				Delay(2);
				return 0;
			}	
		}	
	}
	if(isAddNewUser == 1)
	{
		LcdClear();
		_availSpaceResult = -1;//getNextAvailLocation();
		LcdPrint("Select User", 0, 0);
		
		PutKey('1');
		while(1)
		{
			KeyScan();
			if(IsKeyPending())
			{	
				key = GetKey();
				if(key == '1'){ if(_availSpaceResult<MAX_R307_USERS-1) _availSpaceResult++; }
				else if(key == '2') {if(_availSpaceResult > 0) _availSpaceResult--; }
				else if(key == '3') return 0;
				else if(key == '4') break;
				
				buff[0]=UserNoArr[_availSpaceResult][0];
				buff[1]=UserNoArr[_availSpaceResult][1];
				buff[2]=0;
				LcdPrint(buff, 1, 2);
			}
		}
				
		if(_availSpaceResult == -1)	
		{
			LcdClear();
			LcdPrint("Invalid\n Selection",0,0);
			Delay(2);
		}	
		else if(_availSpaceResult >= 0 && _availSpaceResult < MAX_R307_USERS)
		{
			_availSpaceResult  += 2;
			LcdClear();
			LcdPrint("New User\nPlace Finger",0,0);
			unsigned char availSpaceResult = _availSpaceResult;
			unsigned char AddNewUserResult = AddNewUser(availSpaceResult);
			if(AddNewUserResult == 1)
			{
				unsigned char buff[10];
				LcdClear();
				LcdPrint("New User\n Created @:",0,0);
				DecToStr(availSpaceResult, buff, 2);
				LcdPrint(buff, 1, 12);
				Delay(2);
			}	
			else if(AddNewUserResult == 0xFF)	return 0;
			else
			{
				LcdClear();
				LcdPrint("Error In\n User Creation",0,0);
				Delay(2);
			}	
		}
		else
		{
			LcdClear();
			LcdPrint("Index Out\n of bound",0,0);
			Delay(2);
		}
	}	
	return 0;
}	


unsigned char DeleteUserAfterAdminConfirm()
{
	unsigned char isDeleteUser=1,UserNo,result;
	unsigned char buff[10];
	signed char DelUserNo ;

	if(ee_read(0x00) == 0x01)
	{
		LcdClear();
		LcdPrint("Admin Required\nPlace Finger",0,0);
		isDeleteUser = 0;
		//Match previous admin user
		result =  HiSpeedScan0(&UserNo);
		if(result == 0xFF)	return 0;
		else
		{
			if(result == 1 && (UserNo == 0 || UserNo == 1))
			{
				LcdClear();
				LcdPrint("Admin found",0,0);
				Delay(2);
		
				DelUserNo = -1;
				LcdClear();
				LcdPrint("Select User",0,0);
				PutKey('1');
				while(1)
				{
					KeyScan();
					if(IsKeyPending())
					{	
						unsigned char key = GetKey();
						if(key == '1'){ if(DelUserNo<MAX_R307_USERS-1) DelUserNo++; }
						else if(key == '2') {if(DelUserNo > 0) DelUserNo--; }
						else if(key == '3') return 0;
						else if(key == '4') break;
						
						buff[0]=UserNoArr[DelUserNo][0];
						buff[1]=UserNoArr[DelUserNo][1];
						buff[2]=0;
						LcdPrint(buff, 1, 5);
					}
				}
				if(DelUserNo == -1)
				{
					LcdClear();
					LcdPrint("Invalid\n Selection",0,0);
					Delay(2);				
				}
				else if(DelUserNo>=0 && DelUserNo<MAX_R307_USERS)
				{
					DelUserNo = DelUserNo + 2;
					if(DeleteUser(DelUserNo) == 1)
					{
						memset1(buff, 0, sizeof(buff));
						LcdClear();
						LcdPrint("User Deleted\nUser No:",0,0);
						DecToStr(DelUserNo, buff, 2);
						LcdPrint(buff, 1, 9);
						releaseEElocation(DelUserNo);
						Delay(2);
					}	
					else
					{
						LcdClear();
						LcdPrint("Cannot Delete",0,0);
						Delay(2);
					}	
				}
				else
				{
					LcdClear();
					LcdPrint("Index Out\n of bound",0,0);
					Delay(2);				
				}			
			}	
			else
			{
				LcdClear();
				LcdPrint("Admin Not\n  found",0,0);
				Delay(2);
				return 0;
			}	
		}	
	}
return 0;
}	
/*
unsigned char SelectUser()
{
	unsigned char key = 0;
	signed char i=-1;
	unsigned char buff[5]=0;
		
	while(1)
	{
		KeyScan();
		if(IsKeyPending())
		{	
			key = GetKey();
			if(key == '1'){ if(i<MAX_R307_USERS) i++; }
			else if(key == '2') {if(i > 0) i--; }
			else if(key == '3') return 0;
			else if(key == '4') break;
			
			memset1(buff, 0, sizeof(buff));
			_UserNo[][]
			DecToStr(DelUserNo, buff, 2);
			LcdPrint(buff, 1, 5);
		}
	}
}*/
	
unsigned char menu(const unsigned char *s)
{
	unsigned char keyHit=0xFF;
	LcdClear();
	LcdPrint(s, 0,0);
	while(1)
	{
		KeyScan();
		if(IsKeyPending())
		{
			keyHit = GetKey();
			if(keyHit == '1' || keyHit == '2' || keyHit == '3')
				return keyHit;
		}		
	}
	return 0;
}
void interrupt isr()
{
	if(RCIF == 1)
	{
		PutUartByte(RCREG);
		RCIF = 0;		
	}
}
void main()
{
	unsigned char menuResponse=0xFF;
	
	TRISC=0X8F;
	LcdInit();
	RB5=0;
	RELAY = 0; 
	Delay(1);
	LcdPrint("Welcome", 0,0);
	Delay(2);
	UartInit();
	
	VerifyPswd();
//	ChkFinger(0);
//	while(1)
//	ScanNthFinger(0);
	
	while(1)
	{
		menuResponse = menu("1 For Access\n2 For Alter User");
		if(menuResponse == '1')	{ ControlAccess();}
		else if(menuResponse == '2')
		{
			while(1)
			{
				menuResponse = menu("1 Change Admin\n2 Add/Del User");
				if(menuResponse == '1') 
				{
					while(1)
					{
						menuResponse = menu("1 Admin - 00\n2 Admin - 01");
						if(menuResponse == '1'){AlterAdminUser(menuResponse);}
						else if(menuResponse == '2'){AlterAdminUser(menuResponse);}
						else if(menuResponse == '3')break;
					}					
				}		
				else if(menuResponse == '2')
				{
					while(1)
					{
						menuResponse = menu("1 Add New User\n2 Delete Old User");
						if(menuResponse == '1'){AddNewUserAfterAdminConfirm();} 
						else if(menuResponse == '2')
						{
							while(1)
							{
								menuResponse = menu("1 Delete All\n2 Delete Single");
								if(menuResponse == '1'){EraseAllUsersAfterAdminConfirm();}
								else if(menuResponse == '2'){DeleteUserAfterAdminConfirm();}
								else if(menuResponse == '3')break;
							}	
						}	
						else if(menuResponse == '3')break;
					}
				}
				else if(menuResponse == '3')break;
			}	
		}
	}
}



