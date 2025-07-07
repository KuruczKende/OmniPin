#include "BLE.h"
#include "Modell.h"
#include "Data.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


static BLEServer* pServer = NULL;
static BLECharacteristic* pCharacteristic_Cmd = NULL;
static BLECharacteristic* pCharacteristic_Res = NULL;
static BLECharacteristic* pCharacteristic_Res_str = NULL;
static BLEDescriptor *pDescr;
static BLE2902 *pBLE2902;
static BLEDescriptor *pDescr_2;
static BLE2902 *pBLE2902_2;

static bool deviceConnected = false;
static bool oldDeviceConnected = false;
static uint8_t resIdx = 255;
static uint8_t response = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "a707b3e6-2629-4810-a2ce-a2c20e45bb5b"
#define CHAR_UUID_CMD       "404c76e5-eb91-48bf-9f50-b1d98fd1d586"
#define CHAR_UUID_RES       "2da21ff0-5598-4b99-a1aa-b82b9e66b3b6"
#define CHAR_UUID_RES_STR   "394f3ccd-1882-4af6-8cbe-31d3cd131b0f"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("disconnected");
    }
};

class CharacteristicCallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    MDL_MsgReceived(pChar->getData(),pChar->getLength());
  }
};

void BLE_Init(void){
  deviceConnected = false;
  oldDeviceConnected = false;
  resIdx = 255;
  response = 0;

  // Create the BLE Device
  BLEDevice::init("ESP32PinMaker");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic_Res = pService->createCharacteristic(
                      CHAR_UUID_RES,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );                   
  pCharacteristic_Res_str = pService->createCharacteristic(
                      CHAR_UUID_RES_STR,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );                   

  pCharacteristic_Cmd = pService->createCharacteristic(
                      CHAR_UUID_CMD,
                      BLECharacteristic::PROPERTY_READ | 
                      BLECharacteristic::PROPERTY_WRITE
                    );  

  // Create a BLE Descriptor
  
  pDescr = new BLEDescriptor((uint16_t)0x2901);
  pDescr->setValue("A very interesting variable");
  pCharacteristic_Res->addDescriptor(pDescr);
  
  pBLE2902 = new BLE2902();
  pBLE2902->setNotifications(true);
  pCharacteristic_Res->addDescriptor(pBLE2902);

  
  pDescr_2 = new BLEDescriptor((uint16_t)0x2902);
  pDescr_2->setValue("A very interesting variable");
  pCharacteristic_Res_str->addDescriptor(pDescr_2);
  
  pBLE2902_2 = new BLE2902();
  pBLE2902_2->setNotifications(true);
  pCharacteristic_Res_str->addDescriptor(pBLE2902_2);

  pCharacteristic_Cmd->addDescriptor(new BLE2902());
  
  // After defining the desriptors, set the callback functions
  pCharacteristic_Cmd->setCallbacks(new CharacteristicCallBack());
  
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x00);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

bool BLE_IsDeviceConnected(void){
  return deviceConnected;
}
void BLE_setResponse(uint8_t newResponse){
  response=newResponse;
  if(response == 'D') resIdx = 255U;
}

void BLE_SendResponse(void){
  if (response == 'D') {
    if (resIdx == 255U) {
      uint8_t resDataSS[4];
      resDataSS[0] = 68;  //'D'
      resDataSS[1] = DATA_GetImgUsage();
      if(!DATA_GetSleepTime(&resDataSS[2],&resDataSS[3]))MDL_HardFault(10);

      pCharacteristic_Res->setValue(resDataSS, 4);
      pCharacteristic_Res->notify();
      Serial.print("D...");
      resIdx = 0;
    } else if (resIdx < 8) {
      if (DATA_GetImgName(resIdx) != nullptr) {
        if (DATA_GetImgName(resIdx)[0] != 0) {
          pCharacteristic_Res_str->setValue(String((char*)DATA_GetImgName(resIdx)));
          pCharacteristic_Res_str->notify();
        }
      }
      resIdx++;
    } else {
      pCharacteristic_Res_str->setValue(String(" "));
      pCharacteristic_Res_str->notify();
      Serial.println("D");
      resIdx = 255;
      response = 0;
    }
  } else if (response == 'A' || response == 'N') {
    pCharacteristic_Res->setValue(&response, 1);
    pCharacteristic_Res->notify();
    if (MDL_printResponse()) {
      Serial.println((char)response);
    }
    response = 0;
  } else if (response == 0) {
    //do nothing
  } else {
    Serial.println("Undefined response");
    response = 0;
  }
}
void BLE_DisConnecting(void){
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}