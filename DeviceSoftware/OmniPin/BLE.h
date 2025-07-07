#ifndef _BLE_H_
#define _BLE_H_
#include "Arduino.h"

void BLE_Init(void);
bool BLE_IsDeviceConnected(void);
void BLE_setResponse(uint8_t newResponse);
void BLE_SendResponse(void);
void BLE_DisConnecting(void);

#endif