#ifndef _MDL_H_
#define _MDL_H_
#include "Arduino.h"

//bool MDL_isDataUpload(){return ((idxToUpload!=255) && (dataIdx!=10000));}
void MDL_HardFault(uint8_t errCode);
void MDL_Init(void);
bool MDL_printResponse(void);
void MDL_MsgReceived(uint8_t* data, size_t len);
void MDL_ShowImage(uint8_t imgToShow);
#endif