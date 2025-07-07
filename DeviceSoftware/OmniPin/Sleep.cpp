#include "Sleep.h"
#include "Data.h"
#include "Modell.h"

#include "driver/rtc_io.h"

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  // 2 ^ GPIO_NUMBER in hex
#define WAKEUP_GPIO GPIO_NUM_3
#define WAKEUP_PIN 3

#define MIN_TO_US 60000000

static esp_sleep_wakeup_cause_t wakeup_reason;

void Sleep_Init(void){
  wakeup_reason = esp_sleep_get_wakeup_cause();
  pinMode(WAKEUP_PIN, INPUT);
}

void Sleep_PrintWakeupReason(void){
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_GPIO:     Serial.println("Wakeup caused by gpio"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:Serial.println("Wakeup caused not by sleep exit"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}
bool Sleep_WakeUpBtn(void){
  return ESP_SLEEP_WAKEUP_GPIO == wakeup_reason;
}
bool Sleep_WakeUpTimer(void){
  return ESP_SLEEP_WAKEUP_TIMER == wakeup_reason;
}
bool Sleep_WakeUpRst(void){
  return ESP_SLEEP_WAKEUP_UNDEFINED == wakeup_reason;
}

bool Sleep_ButtonPressed(void){
  return digitalRead(WAKEUP_PIN)==HIGH;
}

void Sleep_GoSleep(void){
  esp_deep_sleep_enable_gpio_wakeup(BUTTON_PIN_BITMASK(WAKEUP_PIN), ESP_GPIO_WAKEUP_GPIO_HIGH);
  gpio_pulldown_dis(WAKEUP_GPIO);
  gpio_pullup_dis(WAKEUP_GPIO);

  while(Sleep_ButtonPressed());
  Serial.println("going to sleep");
  uint8_t high,low;
  if(!DATA_GetSleepTime(&high,&low))MDL_HardFault(50);
  uint16_t time=high*256+low;
  if(time != 0) esp_deep_sleep(time * MIN_TO_US);
  else esp_deep_sleep_start();
}