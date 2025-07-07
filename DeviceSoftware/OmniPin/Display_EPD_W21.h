#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

#include "Arduino.h"

#define BUSY_PIN 6
#define RES_PIN 5
#define DC_PIN 4
#define CS_PIN 10
#define SDI_PIN 0
#define SCLK_PIN 1

//IO settings
//SCK--GPIO23(SCLK)
//SDIN---GPIO18(MOSI)
#define isEPD_W21_BUSY digitalRead(BUSY_PIN)  //BUSY
#define EPD_W21_RST_0 digitalWrite(RES_PIN,LOW)  //RES
#define EPD_W21_RST_1 digitalWrite(RES_PIN,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC_PIN,LOW) //DC
#define EPD_W21_DC_1  digitalWrite(DC_PIN,HIGH)
#define EPD_W21_CS_0 digitalWrite(CS_PIN,LOW) //CS
#define EPD_W21_CS_1 digitalWrite(CS_PIN,HIGH)

//2bit
#define black   0x00  /// 00
#define white   0x01  /// 01
#define yellow  0x02  /// 10
#define red     0x03  /// 11


#define Source_BITS     200
#define Gate_BITS   200
#define ALLSCREEN_BYTES   Source_BITS*Gate_BITS/4


//EPD
void EPD_W21_Init(void);
void EPD_init(void);
void PIC_display (const unsigned char* picData);
void EPD_sleep(void);
void EPD_update(void);
void lcd_chkstatus(void);

void Display_All_Black(void);
void Display_All_White(void);
void Display_All_Yellow(void);
void Display_All_Red(void);

void Acep_color(unsigned char color);
void EPD_init_Fast(void);



void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);


#endif
/***********************************************************
            end file
***********************************************************/
