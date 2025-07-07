#ifndef _SLEEP_H_
#define _SLEEP_H_
#include "Arduino.h"

void Sleep_Init(void);
void Sleep_PrintWakeupReason(void);
bool Sleep_WakeUpBtn(void);
bool Sleep_WakeUpTimer(void);
bool Sleep_WakeUpRst(void);
bool Sleep_ButtonPressed(void);
void Sleep_GoSleep(void);

#endif