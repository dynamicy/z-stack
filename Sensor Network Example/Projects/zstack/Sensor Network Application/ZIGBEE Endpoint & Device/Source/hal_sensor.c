#include <string.h>
#include <stdio.h>
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include "mac_radio_defs.h"

/* ZCL */
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_zigbee device.h"

/* HAL */
#include "onboard.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_keypad.h"
#include "hal_buzzer.h"
  
/* HAL Sensor */
#include "hal_sensor.h"

#if defined(M110)
  #include "M110.h"
#endif
#if defined(M140)
  #include "M140.h"
#endif
#if defined(M160)
  #include "M160.h"
#endif
#if defined(M170)
  #include "M170.h"
#endif
#if defined(M190)
  #include "M190.h"
#endif
#if defined(M200)
  #include "M200.h"
#endif
#if defined(M270)
  #include "M270.h"
#endif

byte TransmitApp_Msg[ TRANSMITAPP_MAX_DATA_LEN ]; 
 
void M110_SensorFunction(void); // M140 Module function
void M140_SensorFunction(void); // M140 Module function
void M160_SensorFunction(void); // M160 Module function
void M170_SensorFunction(void); // M170 Module function
void M190_SensorFunction(void); // M190 Module function
void M200_SensorFunction(void); // M200 Module function
void M270_SensorFunction(void); // M270 Module function

void sleep(uint16 sec)
{
    uint16 i,j,k;
    
    k = sec * 1000;
    
    for(i=0;i<1600;i++)
      for(j=0;j<k;j++)
        asm("nop");
}

#if defined(M110)

void M110_SensorFunction(void)
{
    uint16 val;
      
    M110_Init();
    
    val = M110_GetValue();
    
    TransmitApp_Msg[0] = 3 + '0';   
    TransmitApp_Msg[1] = ',';
    TransmitApp_Msg[2] = 1 + '0';  
    TransmitApp_Msg[3] = 1 + '0';   
    TransmitApp_Msg[4] = 0 + '0';        
    TransmitApp_Msg[5] = ',';
    TransmitApp_Msg[6] = 'N';     
    TransmitApp_Msg[7] = ',';
    if(val > 3000)
    {    
        TransmitApp_Msg[8] = 1 + '0';
    }
    else
    {
        TransmitApp_Msg[8] = 0 + '0';  
    }    

    TransmitApp_Msg[9] = '$';     
    TransmitApp_Msg[10] = '\n';     
  
    sleep(1);
   
    uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                 ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 11, 
                                 TransmitApp_Msg );
 }
#endif
  
#if defined(M140)

void M140_SensorFunction(void)
{
  uint16 val;

  M140_Init();
      
  val = M140_GetValue();  
  if((0x2000 & val) == 0x2000) // check sign bit for negative value
  {
    val = ((val - 16384) * 10) / 32;
  }
  else
  {
    val = (val * 10) / 32;
  }

  TransmitApp_Msg[0] = 3 + '0';   
  TransmitApp_Msg[1] = ',';
  TransmitApp_Msg[2] = 1 + '0';  
  TransmitApp_Msg[3] = 4 + '0';   
  TransmitApp_Msg[4] = 0 + '0';        
  TransmitApp_Msg[5] = ',';
  TransmitApp_Msg[6] = 'B';   
  TransmitApp_Msg[7] = ',';  
  TransmitApp_Msg[8] = (val / 100) + '0';
  TransmitApp_Msg[9] = ((val / 10) % 10) + '0';
  TransmitApp_Msg[10] = '.';
  TransmitApp_Msg[11] = (val % 10) + '0';
  TransmitApp_Msg[12] = '$'; 
  TransmitApp_Msg[13] = '\n'; 
  
  sleep(1);
    
  uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC, 
                                TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 14, 
                                TransmitApp_Msg );   
 }
#endif

#if defined(M160)
void M160_SensorFunction(void)
{
  static uint8 duty = 50;

  M160_Init();
  
  if (ch > 0)
  {           
    if (ch == 'U')
    {
      if (duty < 100)
      {
        duty = duty + 5;
      }
    }
    
    if (ch == 'N')
    {
      if (duty > 0)
      {
        duty = duty - 5;
      }
    }
      
    M160_On(duty);
  }
  
  TransmitApp_Msg[0] = 3 + '0';   
  TransmitApp_Msg[1] = ',';
  TransmitApp_Msg[2] = 2 + '0';  
  TransmitApp_Msg[3] = 8 + '0';   
  TransmitApp_Msg[4] = 0 + '0';        
  TransmitApp_Msg[5] = ',';
  TransmitApp_Msg[6] = 'A';   
  TransmitApp_Msg[7] = ',';  
  TransmitApp_Msg[8] = (duty / 100) + '0';
  TransmitApp_Msg[9] = ((duty / 10) % 10) + '0';
  TransmitApp_Msg[10] = (duty % 10) + '0';
  TransmitApp_Msg[11] = '$'; 
  TransmitApp_Msg[12] = '\n';
    
  uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                 ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC, 
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 13, 
                                 TransmitApp_Msg ); 
}
#endif 

#if defined(M170)
void M170_SensorFunction(void)
{
    uint16 val;
     
    M170_Init();
  
    val = M170_GetValue();
    if (val > 2000)
    {
        val -= 2000;
    }
    else
    {
        val = 0;
    }
    val /= 120;
            
    if (val > 100)
    {
        val = 100;
    }
   
    TransmitApp_Msg[0] = 3 + '0';   
    TransmitApp_Msg[1] = ',';
    TransmitApp_Msg[2] = 1 + '0';  
    TransmitApp_Msg[3] = 7 + '0';   
    TransmitApp_Msg[4] = 0 + '0';        
    TransmitApp_Msg[5] = ',';
    TransmitApp_Msg[6] = 'A';   
    TransmitApp_Msg[7] = ',';  
    TransmitApp_Msg[8] = (val / 100) + '0';
    TransmitApp_Msg[9] = ((val / 10) % 10) + '0';
    TransmitApp_Msg[10] = (val % 10) + '0';
    TransmitApp_Msg[11] = '$'; 
    TransmitApp_Msg[12] = '\n'; 

    sleep(1);
    uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                 ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 13, 
                                 TransmitApp_Msg );
 }
#endif
 
#if defined(M190)
void M190_SensorFunction(void)
{
    uint16 val;

    M190_Init();
    val = M190_GetValue();  

    if (val > 2000)
    {
        val -= 2000;
    }
    else
    {
        val = 0;
    }
    val /= 120;
            
    if (val > 100)
    {
        val = 100;
    }
    
    TransmitApp_Msg[0] = 3 + '0';   
    TransmitApp_Msg[1] = ',';
    TransmitApp_Msg[2] = 1 + '0';  
    TransmitApp_Msg[3] = 9 + '0';   
    TransmitApp_Msg[4] = 0 + '0';        
    TransmitApp_Msg[5] = ',';
    TransmitApp_Msg[6] = 'A';   
    TransmitApp_Msg[7] = ',';  
    TransmitApp_Msg[8] = (val / 100) + '0';
    TransmitApp_Msg[9] = ((val / 10) % 10) + '0';
    TransmitApp_Msg[10] = (val % 10) + '0';
    TransmitApp_Msg[11] = '$'; 
    TransmitApp_Msg[12] = '\n';

    sleep(1);    
    uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                 ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 13, 
                                 TransmitApp_Msg );
 }
#endif
 
#if defined(M200)
void M200_SensorFunction(void)
 {
    // M200 module variable
    uint16 temp, humi;
    uint8 hum_1,hum_2,hum_3,hum_4;
    uint8 temp_1,temp_2,temp_3,temp_4;
    uint8 len;
      
    // initilization the device
    M200_Init();
  
    M200_GetValue();
    temp = (uint16)(((((float)M200_Temp) *0.01) - 40.0) *10.0);
    humi = (uint16)(((((float)M200_Humi) *0.0405) - (((float)M200_Humi)*((float)M200_Humi) *0.0000028) - 4.0) *10.0);
    if (humi > 999)
    {
      humi = 999;
    }
    if (temp > 999)
    {
      temp = 999;
    }
    hum_1 = (humi / 100) + '0';
    TransmitApp_Msg[0] = hum_1;
    hum_2 = ((humi / 10) % 10) + '0';
    TransmitApp_Msg[1] = hum_2;
    hum_3 = '.';
    TransmitApp_Msg[2] = hum_3;
    hum_4 = (humi % 10) + '0';
    TransmitApp_Msg[3] = hum_4;
    TransmitApp_Msg[4] = ' ';
    temp_1 = (temp / 100) + '0';
    TransmitApp_Msg[5] = temp_1;
    temp_2 = ((temp / 10) % 10) + '0';
    TransmitApp_Msg[6] = temp_2;
    temp_3 = '.';
    TransmitApp_Msg[7] = temp_3;
    temp_4 = (temp % 10) + '0';
    TransmitApp_Msg[8] = temp_4;
    
    #if defined ( LCD_SUPPORTED )
      HalLcdWriteString("Humidity=      %", HAL_LCD_LINE_1);
      HalLcdWriteString(" Temp. =       C", HAL_LCD_LINE_2);
      // Humi
      HalLcdWriteChar(HAL_LCD_LINE_1, 10, hum_1);
      HalLcdWriteChar(HAL_LCD_LINE_1, 11, hum_2);
      HalLcdWriteChar(HAL_LCD_LINE_1, 12, hum_3);
      HalLcdWriteChar(HAL_LCD_LINE_1, 13, hum_4);
      // Temp
      HalLcdWriteChar(HAL_LCD_LINE_2, 9, temp_1);
      HalLcdWriteChar(HAL_LCD_LINE_2, 10, temp_2);
      HalLcdWriteChar(HAL_LCD_LINE_2, 11, temp_3);
      HalLcdWriteChar(HAL_LCD_LINE_2, 12, temp_4);
    #endif

    len = 9;
    uint8 tmp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, ZCL_CLUSTER_ID_GEN_ON_OFF, 
                                 ZCL_CLUSTER_ID_GEN_BASIC, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, len, TransmitApp_Msg );
 }
#endif
 
#if defined(M270)
/*********************************************************************
 * @fn          M270_SensorFunction
 * @brief       The ZIGBEE Device to receive the M200 Temp and Humi data.
 */
 void M270_SensorFunction(void)
 {
    static uint8 udo;

    M270_Init();
    
     if( ch == 'A' )
     {
        udo = 1;
        M270_SetDO(udo);
     }
     
     if( ch == 'D' )
     {
        udo = 0;
        M270_SetDO(udo);
     }
    
     #if defined ( LCD_SUPPORTED )
        HalLcdWriteString("** M270 Test  **", HAL_LCD_LINE_1);
        HalLcdWriteString("* RELAY DO=[0] *", HAL_LCD_LINE_2);
        // Show the control value
        HalLcdWriteChar(HAL_LCD_LINE_2, 12, udo + '0');
     #endif
 }
#endif