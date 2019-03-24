
#ifndef EEPROM_H
#define EEPROM_H

#define MAX_R307_USERS	12
#define TOT_USERS_COUNT_ADD 30

void ee_write(unsigned char a,unsigned char d);
unsigned char ee_read(unsigned char a);
void Delay(unsigned int d);
void memcpy1(unsigned char *toStr, unsigned char *fromStr, unsigned char len);
void memset1(unsigned char *str, unsigned char data, unsigned int len);
unsigned char TotDigits(unsigned long val);
void DecToStr(unsigned long val, unsigned char *dest, unsigned char minDigit);
void ee_eraseNPos(unsigned char n);
signed char getNextAvailLocation();
void reserveEElocation(unsigned char loc);
void releaseEElocation(unsigned char loc);


#endif