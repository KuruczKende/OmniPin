#include "Display_EPD_W21.h"
#include <SPI.h>

void EPD_W21_Init(void){
  pinMode(BUSY_PIN, INPUT);  //BUSY
  pinMode(RES_PIN, OUTPUT); //RES 
  pinMode(DC_PIN, OUTPUT); //DC   
  pinMode(CS_PIN, OUTPUT); //CS   
  
  //SPI
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
  SPI.begin(SCLK_PIN,-1,SDI_PIN,CS_PIN);
}

void EPD_init(void)
{
 	delay(20);//At least 20ms delay 	
	EPD_W21_RST_0;		// Module reset
	delay(50);//At least 50ms delay 
	EPD_W21_RST_1;
	delay(50);//At least 50ms delay 

	lcd_chkstatus();
  EPD_W21_WriteCMD(0x4D);
  EPD_W21_WriteDATA(0x78);

  EPD_W21_WriteCMD(0x00);	//PSR
  EPD_W21_WriteDATA(0x0F);
  EPD_W21_WriteDATA(0x29);

  EPD_W21_WriteCMD(0X06); //BTST_P
  EPD_W21_WriteDATA(0x0D);  //47uH
  EPD_W21_WriteDATA(0x12);
  EPD_W21_WriteDATA(0x30);
  EPD_W21_WriteDATA(0x20);
  EPD_W21_WriteDATA(0x19);
  EPD_W21_WriteDATA(0x2A);
  EPD_W21_WriteDATA(0x22);

  EPD_W21_WriteCMD(0x50);	//CDI
  EPD_W21_WriteDATA(0x37);

  EPD_W21_WriteCMD(0x61); //TRES
  EPD_W21_WriteDATA(Source_BITS/256);		// Source_BITS_H
  EPD_W21_WriteDATA(Source_BITS%256);		// Source_BITS_L
  EPD_W21_WriteDATA(Gate_BITS/256);			// Gate_BITS_H
  EPD_W21_WriteDATA(Gate_BITS%256); 		// Gate_BITS_L	

  EPD_W21_WriteCMD(0xE9);
  EPD_W21_WriteDATA(0x01); 

  EPD_W21_WriteCMD(0x30);
  EPD_W21_WriteDATA(0x08);  
	
  EPD_W21_WriteCMD(0x04);
  lcd_chkstatus();  
}

//Fast full screen update initialization
void EPD_init_Fast(void)	
{
  delay(20);//At least 20ms delay 	
	EPD_W21_RST_0;		// Module reset
	delay(50);//At least 50ms delay 
	EPD_W21_RST_1;
	delay(50);//At least 50ms delay 
	
	lcd_chkstatus();
  EPD_W21_WriteCMD(0x4D);
  EPD_W21_WriteDATA(0x78);

  EPD_W21_WriteCMD(0x00);	//PSR
  EPD_W21_WriteDATA(0x0F);
  EPD_W21_WriteDATA(0x29);

  EPD_W21_WriteCMD(0X06); //BTST_P
  EPD_W21_WriteDATA(0x0D);  //47uH
  EPD_W21_WriteDATA(0x12);
  EPD_W21_WriteDATA(0x30);
  EPD_W21_WriteDATA(0x20);
  EPD_W21_WriteDATA(0x19);
  EPD_W21_WriteDATA(0x2A);
  EPD_W21_WriteDATA(0x22);

  EPD_W21_WriteCMD(0x50);	//CDI
  EPD_W21_WriteDATA(0x37);

  EPD_W21_WriteCMD(0x61); //TRES
  EPD_W21_WriteDATA(Source_BITS/256);		// Source_BITS_H
  EPD_W21_WriteDATA(Source_BITS%256);		// Source_BITS_L
  EPD_W21_WriteDATA(Gate_BITS/256);			// Gate_BITS_H
  EPD_W21_WriteDATA(Gate_BITS%256); 		// Gate_BITS_L	

  EPD_W21_WriteCMD(0xE9);
  EPD_W21_WriteDATA(0x01); 

  EPD_W21_WriteCMD(0x30);
  EPD_W21_WriteDATA(0x08);  
	
  EPD_W21_WriteCMD(0x04);
  lcd_chkstatus(); 
	
	EPD_W21_WriteCMD(0xE0);
	EPD_W21_WriteDATA(0x02);
		
	EPD_W21_WriteCMD(0xE6);
	EPD_W21_WriteDATA(0x5D);
	 
	EPD_W21_WriteCMD(0xA5);
	EPD_W21_WriteDATA(0x00);
	lcd_chkstatus(); 

	
}	
void EPD_sleep(void)
{   
	EPD_W21_WriteCMD(0X02);  	//power off
	EPD_W21_WriteDATA(0x00);
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal  
 
	EPD_W21_WriteCMD(0X07);  	//deep sleep
	EPD_W21_WriteDATA(0xA5);
}
void EPD_update(void)
{   
  EPD_W21_WriteCMD(0x12); //Display Update Control
  EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();   
}

void lcd_chkstatus(void)
{ 
  while(1)
  {  //=0 BUSY
     if(isEPD_W21_BUSY==1) break;
  }  
}


void Display_All_Black(void)
{
  unsigned long i; 

  EPD_W21_WriteCMD(0x10);
  {
    for(i=0;i<ALLSCREEN_BYTES;i++)
    {
      EPD_W21_WriteDATA(0x00);
    }
  } 
  EPD_update();  
  
}

void Display_All_White(void)
{
  unsigned long i;
 
  EPD_W21_WriteCMD(0x10);
  {
    for(i=0;i<ALLSCREEN_BYTES;i++)
    {
      EPD_W21_WriteDATA(0x55);
    }
  } 
   EPD_update(); 
}

void Display_All_Yellow(void)
{
  unsigned long i;
 
  EPD_W21_WriteCMD(0x10);
  {
    for(i=0;i<ALLSCREEN_BYTES;i++)
    {
      EPD_W21_WriteDATA(0xaa);
    }
  }
   EPD_update(); 
}


void Display_All_Red(void)
{
  unsigned long i;
 
  EPD_W21_WriteCMD(0x10);
  {
    for(i=0;i<ALLSCREEN_BYTES;i++)
    {
      EPD_W21_WriteDATA(0xff);
    }
  } 
   EPD_update(); 
}

void PIC_display(const unsigned char* picData)
{
  unsigned int i,j;
  unsigned char temp1;
  unsigned char data_H1,data_H2,data_L1,data_L2,data;
   
  EPD_W21_WriteCMD(0x10);        
  for(i=0;i<Gate_BITS;i++)  //Source_BITS*Gate_BITS/4
  { 
    for(j=0;j<Source_BITS/4;j++)
    {   
      temp1=picData[i*Source_BITS/4+j]; 

      data_H1=(temp1>>6&0x03)<<6;      
      data_H2=(temp1>>4&0x03)<<4;
      data_L1=(temp1>>2&0x03)<<2;
      data_L2=(temp1&0x03);
      
      data=data_H1|data_H2|data_L1|data_L2;
      EPD_W21_WriteDATA(data);
    }
  } 

   //update
    EPD_update();  
}

//SPI write byte
void SPI_Write(unsigned char value)
{				   			 
   SPI.transfer(value);
}

//SPI write command
void EPD_W21_WriteCMD(unsigned char command)
{
	EPD_W21_CS_0;
	EPD_W21_DC_0;  // D/C#   0:command  1:data  
	SPI_Write(command);
	EPD_W21_CS_1;
}
//SPI write data
void EPD_W21_WriteDATA(unsigned char datas)
{
	EPD_W21_CS_0;
	EPD_W21_DC_1;  // D/C#   0:command  1:data
	SPI_Write(datas);
	EPD_W21_CS_1;
}

/***********************************************************
            end file
***********************************************************/
