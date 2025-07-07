#include <SPI.h>
#include "Sleep.h"
#include "BLE.h"
#include "Modell.h"
#include "Data.h"
#include "Display_EPD_W21.h"

uint32_t disconnectedTimer;

void coldReset(){
  BLE_Init();
  disconnectedTimer = 0;
}

void nextImage(){
  uint8_t nextImg=255;
  if(Sleep_WakeUpBtn()){
    nextImg = DATA_FindNextValid();
  }
  else if(Sleep_WakeUpTimer()){
    nextImg = DATA_FindNextUsage();
  }
  MDL_ShowImage(nextImg);
  Sleep_GoSleep();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Sleep_Init();
  Sleep_PrintWakeupReason();
  DATA_Init(Sleep_WakeUpRst());
  MDL_Init();
  EPD_W21_Init();

  if(Sleep_WakeUpRst()){
    coldReset();
  }
  else{
    nextImage();
  }
}

void loop() {
    if (BLE_IsDeviceConnected()) {
        BLE_SendResponse();
        disconnectedTimer=0;
    }
    else{
      disconnectedTimer++;
      delay(10);
      if(disconnectedTimer > 6000){
        Sleep_GoSleep();
      }
    }
    if (Sleep_ButtonPressed()){
      MDL_ShowImage(DATA_FindNextValid());
    }
    BLE_DisConnecting();
}