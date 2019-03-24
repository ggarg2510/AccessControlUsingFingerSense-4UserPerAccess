
#ifndef KEYPD_H
#define KEYPD_H

extern unsigned int buzzOffCounter;
unsigned char IsKeyPending();
void PutKey(unsigned char key);
unsigned char GetKey();
void KeyScan();

#endif

