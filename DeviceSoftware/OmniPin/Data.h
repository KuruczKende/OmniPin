#ifndef _DATA_H_
#define _DATA_H_
#include "Arduino.h"

void DATA_UpdateValidity(void);
uint8_t DATA_GetImgUsage();
void DATA_SetImgUsage(uint8_t value);
void DATA_SetSleepTime(uint8_t high, uint8_t low);
bool DATA_GetSleepTime(uint8_t* high, uint8_t* low);
bool DATA_SetImgName(uint8_t idx, uint8_t* data, size_t len);
uint8_t* DATA_GetImgName(uint8_t idx);
uint8_t DATA_FindImage(char* name);
uint8_t DATA_FindNextUsage(void);
uint8_t DATA_FindNextValid(void);
uint8_t DATA_FindFree();
bool DATA_CurrentImageInUsage(void);
// PERSISTENT DATA
bool DATA_Init(bool cold);
bool DATA_SaveImagePersistent(uint8_t idx, uint8_t* data);
bool DATA_LoadImagePersistent(uint8_t imgIdx, uint8_t* data);
bool DATA_SaveDataPersistent();
bool DATA_LoadDataPersistent();
// END PERSISTENT DATA

#endif