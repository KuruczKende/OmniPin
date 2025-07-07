#include "Modell.h"
#include "Data.h"
#include "BLE.h"
#include "Display_EPD_W21.h"

static uint8_t idxToDownload=255;
static uint16_t dataIdx=10000;
static uint8_t imgName[32]; // name to save in case of sucessfull download
static uint8_t imgNameLen;

static uint8_t bufferedImg[10000]; // buffer to hold image data.
static uint8_t bufferedIdx; // buffered image idx, allways set after fill.

static void resetDownload(void);

void MDL_HardFault(uint8_t errCode){
  Serial.printf("HardFault!%u!\n",errCode);
  /*while(true){
    #ifdef USER_LED
      delay(100);
      digitalWrite(USER_LED,HIGH);
      delay(100);
      digitalWrite(USER_LED,LOW);
    #endif
  }*/
}

void MDL_Init(void){
  resetDownload();
  bufferedIdx=255;
  #ifdef USER_LED
    pinMode(USER_LED,OUTPUT);
    digitalWrite(USER_LED,LOW);
  #endif
}

static void resetDownload(void){
  idxToDownload=255;
  dataIdx=10000;
  memset(imgName,0,32);
  imgNameLen=0;
}

void DownloadStart(uint8_t* data, size_t len){
  idxToDownload = DATA_FindImage((char*)data);
  if(idxToDownload==255){
    // new image
    idxToDownload=DATA_FindFree();
    if(idxToDownload==255){
      BLE_setResponse('N');
    }
    else{
      memcpy(imgName,data,len);
      imgNameLen = len;
      dataIdx=0;
      BLE_setResponse('A');
    }
  }
  else{
    dataIdx=0;
    BLE_setResponse('A');
  }
}
void DownloadData(uint8_t* data, size_t len){
  memcpy(&bufferedImg[dataIdx],data,len);
  dataIdx+=len;
  if(dataIdx%1000==0)
    Serial.println(dataIdx/100);
  if(dataIdx<10000){
    BLE_setResponse('A');
  }
  else{
    bufferedIdx=idxToDownload;
    if(!DATA_SetImgName(bufferedIdx,imgName,imgNameLen))MDL_HardFault(20);
    if(!DATA_SaveImagePersistent(bufferedIdx, bufferedImg))MDL_HardFault(21);
    MDL_ShowImage(bufferedIdx);
    BLE_setResponse('D');
  }
}
void Read(){
  BLE_setResponse('D');
}
void Delete(uint8_t* data, size_t len){
  uint8_t idx = DATA_FindImage((char*)data);
  if(idx==255){
    BLE_setResponse('N');
  }
  else{
    if(!DATA_SetImgName(idx,nullptr,0))MDL_HardFault(30);
    if(!DATA_SaveDataPersistent())MDL_HardFault(31);
    BLE_setResponse('D');
  }
}
void SetSlideShow(uint8_t* data, size_t len){
  if(len!=4)MDL_HardFault(40);

  DATA_SetImgUsage(data[1]);
  DATA_SetSleepTime(data[2],data[3]);
  if(DATA_CurrentImageInUsage()){
    //current image is in usage, no need to select other image
  }
  else{
    MDL_ShowImage(DATA_FindNextUsage());// show image from usage
  }
  BLE_setResponse('D');
  DATA_SaveDataPersistent();
}
void FactoryResest(){
  for(uint8_t i=0;i<8;i++){
	if(!DATA_SetImgName(i,nullptr,0))MDL_HardFault(60+i);
  }
  DATA_SetSleepTime(0,60);
  BLE_setResponse('D');
}

bool MDL_printResponse(void){
  return ((idxToDownload == 255) || (dataIdx == 0));
}
void MDL_MsgReceived(uint8_t* data, size_t len) {
  if (data == nullptr) {
    Serial.println("No CMD");
    BLE_setResponse('N');
    return;
  }
  if (idxToDownload != 255 && dataIdx != 10000) {
    DownloadData(data, len);
  } else {
    uint8_t Cmd = data[0];
    resetDownload();
    Serial.printf("CMD: %c\n", Cmd);
    switch (Cmd) {
      case 'U':
        DownloadStart(&data[1], len - 1);
        break;
      case 'D':
        Delete(&data[1], len - 1);
        break;
      case 'R':
        Read();
        break;
      case 'S':
        SetSlideShow(&data[1], len - 1);
        break;
      case 'F':
        FactoryResest();
        break;
      default:
        Serial.println("Invalid CMD");
        BLE_setResponse('N');
        break;
    }
  }
}

void MDL_ShowImage(uint8_t imgToShow){
    if(bufferedIdx!=imgToShow){
      if(DATA_LoadImagePersistent(imgToShow, bufferedImg)){
        bufferedIdx = imgToShow;
      }
      else{
        return;
      }
    }
    EPD_init(); //Full screen update initialization.
    PIC_display(bufferedImg);//To Display one image using full screen update.
    EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
}

/*
TODO:
- Factory reset code.
- Set current image code.
*/