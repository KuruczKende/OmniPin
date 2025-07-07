#include "Data.h"

#include "FS.h"
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED true

// RTC RAM
  struct S_RTC_DATA{
    uint8_t valid; // 8 bit mask for valid(1) invalid(0)
    uint8_t usage; // 8 bit mask for usage(1) dont use/invalid(0)
    uint8_t current; // while not sure must be 255
    uint16_t sleepTime; // sleeptime between imagechanges. in minutes
  };
  static RTC_DATA_ATTR S_RTC_DATA RtcData;
// END RTC RAM

// NORMAL RAM
  static uint8_t imgName[8][32]; // name/id of image, empty string = no image.
// END NORMAL RAM

static bool readFile(fs::FS &fs, const char *path, uint8_t *buffer, size_t len);
static bool writeFile(fs::FS &fs, const char *path, const uint8_t *data, size_t len);
static uint8_t findNextIn(uint8_t list, uint8_t start);

void DATA_UpdateValidity(void){
  RtcData.valid=0;
  for(uint8_t i=0;i<8;i++){
    if(imgName[i][0]!=0){
      // valid image
      RtcData.valid |= (0x01<<i);
    }
  }
  Serial.printf("Valid: %u\n",RtcData.valid);
  if(RtcData.current >= 8){
    RtcData.current = 255;
    return;
  }
  if((RtcData.valid&(0x01<<RtcData.current))==0)
    RtcData.current = 255;
}

uint8_t DATA_GetImgUsage(){
  uint8_t ret=0;
  uint8_t idx=0;
  for(uint8_t i=0;i<8;i++){
    if((RtcData.valid&(0x01<<(i%8)))!=0){
      // valid image
      ret |= (((RtcData.usage>>i)&0x01)<<idx);
      idx++;
    }
  }
  return ret;
}
void DATA_SetImgUsage(uint8_t value){
  uint8_t idx=0;
  RtcData.usage=0;
  for(uint8_t i=0;i<8;i++){
    if((RtcData.valid&(0x01<<(i%8)))!=0){
      // valid image
      RtcData.usage |= (((value>>idx)&0x01)<<i);
      idx++;
    }
  }
  Serial.printf("Usage: %u\n",RtcData.usage);
}
void DATA_SetSleepTime(uint8_t high, uint8_t low){
  RtcData.sleepTime = (uint16_t)(high)*256+low;
}
bool DATA_GetSleepTime(uint8_t* high, uint8_t* low){
  if(low==nullptr||high==nullptr)return false;
  *high = (uint8_t)(RtcData.sleepTime >> 8);
  *low = (uint8_t)RtcData.sleepTime;
  return true;
}
bool DATA_SetImgName(uint8_t idx, uint8_t* data, size_t len){
  if(idx >= 8)return false;
  if((data == nullptr) && (len != 0)) return false; //invalid pointer
  if(data != nullptr){
    //valid data
    if(len == 0) return false; // invalid length
    if(data[0]==0) return false; //invalid string

    memcpy(imgName[idx],data,len);
  }
  memset(&imgName[idx][len],0,32-len);
  DATA_UpdateValidity();
  return true;
}
uint8_t* DATA_GetImgName(uint8_t idx){
  if(idx >= 8)return nullptr;
  return imgName[idx];
}

uint8_t DATA_FindImage(char* name){
  for(uint8_t i=0;i<8;i++){
    bool good=true;
    uint8_t j=0;
    while(j<32 && good && name[j]!=0 && imgName[i][j]!=0){
      if(name[j]!=imgName[i][j])
        good=false;
      j++;
    }
    if(good && name[j]==0 && imgName[i][j]==0 && j<32){
      return i;
    }
  }
  return 255;
}
static uint8_t findNextIn(uint8_t list, uint8_t start){
  uint8_t searchFrom=1;
  if(start >= 8){
    start = 0;
    searchFrom=0;
  }
  for(uint8_t i=searchFrom;i<8;i++){
    uint8_t mask = 0x01<<((i+start)%8);
    if((list&mask)!=0)
      return (i+start)%8;
  }
  return 255;
}
uint8_t DATA_FindNextUsage(void){
  return findNextIn(RtcData.usage,RtcData.current);
}
uint8_t DATA_FindNextValid(void){
  return findNextIn(RtcData.valid,RtcData.current);
}
uint8_t DATA_FindFree(){
  return findNextIn(~RtcData.valid,255);
}

bool DATA_CurrentImageInUsage(void){
  return (RtcData.usage&(0x01<<RtcData.current))!=0;
}

// PERSISTENT DATA
bool DATA_Init(bool cold){
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return false;
  }
  bool ret=true;
  if(cold)ret = DATA_LoadDataPersistent();
  return ret;
}
static bool readFile(fs::FS &fs, const char *path, uint8_t *buffer, size_t len) {
  if(buffer == nullptr) return false;

  Serial.printf("Reading binary file: %s\r\n", path);
  File file = fs.open(path, FILE_READ);
  if (!file) {
    Serial.println("- failed to open file for reading");
    return false;
  }

  size_t bytesRead = file.read(buffer, len);  // maxLen: buffer mérete
  Serial.printf("- read %u bytes\r\n", bytesRead);
  file.close();

  return (bytesRead == len);
}
static bool writeFile(fs::FS &fs, const char *path, const uint8_t *data, size_t len) {
  if(data == nullptr) return false;

  Serial.printf("Writing binary file: %s (%u bytes)\r\n", path, len);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return false;
  }

  size_t bytesWritten = file.write(data, len);
  if (bytesWritten == len) {
    Serial.println("- binary file written successfully");
  } else {
    Serial.printf("- write failed, wrote only %u bytes\n", bytesWritten);
  }

  file.close();
  return (bytesWritten == len);
}
bool DATA_SaveImagePersistent(uint8_t idx, uint8_t* data){
  if((data == nullptr)||(idx >= 8)) return false;

  char path[]="/imgX.dat";
  path[4]=(char)idx + '0';
  bool ret = writeFile(SPIFFS, path, data, 10000);
  RtcData.current = idx;
  ret &= DATA_SaveDataPersistent();
  return ret;
}
bool DATA_LoadImagePersistent(uint8_t imgIdx, uint8_t* data){
  if((data == nullptr)||(imgIdx >= 8)) return false;

  char path[]="/imgX.dat";
  path[4]=(char)imgIdx + '0';
  if(readFile(SPIFFS, path, data, 10000)){
    RtcData.current = imgIdx;
    return true;
  }
  return false;
}
bool DATA_SaveDataPersistent(){
  if(writeFile(SPIFFS, "/names.dat", (uint8_t*)imgName, sizeof(imgName))){
    DATA_UpdateValidity();
    return writeFile(SPIFFS, "/struct.dat", (uint8_t*)&RtcData, sizeof(RtcData));
  }
  return false;
}
bool DATA_LoadDataPersistent(){
  bool ret = readFile(SPIFFS, "/names.dat", (uint8_t*)imgName, sizeof(imgName));
  ret &= readFile(SPIFFS, "/struct.dat", (uint8_t*)&RtcData, sizeof(RtcData));
  RtcData.current = 255;
  return ret;
}
// END PERSISTENT DATA
