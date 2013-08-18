/**************************************************************************************************
  Filename:       hal_sensor.c
  Revised:        $Date: 2010-07-14 (Wed, 14 July 2010) $
  Revision:       $Revision: 19453 $

  Description:    HAL Sensor - the device receive the sensor data application.
**************************************************************************************************/
/*********************************************************************
 * INCLUDES
 */  
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

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte TransmitApp_Msg[ TRANSMITAPP_MAX_DATA_LEN ]; 
 
void M110_SensorFunction(void); // M140 Module function
void M140_SensorFunction(void); // M140 Module function
void M160_SensorFunction(void); // M160 Module function
void M170_SensorFunction(void); // M170 Module function
void M190_SensorFunction(void); // M190 Module function
void M200_SensorFunction(void); // M200 Module function
void M270_SensorFunction(void); // M270 Module function

#if defined(M110)
/*********************************************************************
 * @fn          M190_SensorFunction
 * @brief       The ZIGBEE Device to receive the M190 optical data.
 */
 void M110_SensorFunction(void)
 {
    // M110 sensor variable
    uint16 val;
    char buf[16];

    // ZCL transmit variable
    uint8 len;
      
    // initilization the device
    M110_Init();
    
    val = M110_GetValue();
   
    if(val >3000)
    {    
        sprintf(buf, "%s", "1");
    }
    else
    {
        sprintf(buf, "%s", "0");      
    }

    
#if defined ( LCD_SUPPORTED )
    HalLcdWriteString("* Z-Stack M110 *", HAL_LCD_LINE_1);
    HalLcdWriteString( buf, HAL_LCD_LINE_2 );
#endif
   
   len = 3;
   uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, len, TransmitApp_Msg );
 }
#endif
  
#if defined(M140)
/*********************************************************************
 * @fn          M140_SensorFunction
 * @brief       The ZIGBEE Device to receive the M140 tempture data.
 */
 void M140_SensorFunction(void)
 {
  // M140 sensor variable
  uint16 val;
  uint8 tmp1, tmp2, tmp3, tmp4;
  
  // ZCL transmit variable
  uint8 len;
  
  // initilization the device
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
  tmp1 = (val / 100) + '0';
  TransmitApp_Msg[0] = tmp1;
  tmp2 = ((val / 10) % 10) + '0';
  TransmitApp_Msg[1] = tmp2;
  tmp3 = '.';
  TransmitApp_Msg[2] = tmp3;
  tmp4 = (val % 10) + '0';
  TransmitApp_Msg[3] = tmp4;
  
  len = 4;
    
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString("* Z-Stack M140 *", HAL_LCD_LINE_1);
  HalLcdWriteString(" Temp. =       C", HAL_LCD_LINE_2);
 // HalLcdWriteString(TransmitApp_Msg, HAL_LCD_LINE_2);
  
  HalLcdWriteChar(HAL_LCD_LINE_2, 14, 0xDF);
  HalLcdWriteChar(HAL_LCD_LINE_2, 9, tmp1);
  HalLcdWriteChar(HAL_LCD_LINE_2, 10, tmp2);
  HalLcdWriteChar(HAL_LCD_LINE_2, 11, tmp3);
  HalLcdWriteChar(HAL_LCD_LINE_2, 12, tmp4);
 
#endif 

  len = 4;
  uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, ZCL_CLUSTER_ID_GEN_ON_OFF, 
                                ZCL_CLUSTER_ID_GEN_BASIC, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, len, TransmitApp_Msg );   
 }
#endif

#if defined(M160)
/*********************************************************************
 * @fn          M160_SensorFunction
 * @brief       The ZIGBEE Device to receive the M160 tempture data.
 */
 void M160_SensorFunction(void)
 {
  // M160 sensor variable
  static uint8 duty = 50;
  
  // initilization the device
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
    #if defined ( LCD_SUPPORTED )
      HalLcdWriteString("** M160 Test  **", HAL_LCD_LINE_1);
      HalLcdWriteString("   Duty =     % ", HAL_LCD_LINE_2);
      halLcdDisplayUint8(HAL_LCD_LINE_2, 10, HAL_LCD_RADIX_DEC, duty); 
    #endif
  }
//  M160_Off();
 }
#endif 

 
#if defined(M170)
/*********************************************************************
 * @fn          M170_SensorFunction
 * @brief       The ZIGBEE Device to receive the M170 optical data.
 */
 void M170_SensorFunction(void)
 {
    // M170 sensor variable
    uint16 val;
    uint8 tmp1, tmp2, tmp3;
    // ZCL transmit variable
    uint8 len;
      
    // initilization the device
    M170_Init();
  
   val = M170_GetValue();
   if (val > 2000)
   {
      val -= 2000;
   }
   else
    {
      val = 0;
    }val /= 120;
            
   if (val > 100)
   {
      val = 100;
   }
   
   tmp1 = (val / 100) + '0';
   TransmitApp_Msg[0] = tmp1;
   tmp2 = ((val / 10) % 10) + '0';
   TransmitApp_Msg[1] = tmp2;
   tmp3 = (val % 10) + '0';
   TransmitApp_Msg[2] = tmp3;
   
#if defined ( LCD_SUPPORTED )
   HalLcdWriteString("* Z-Stack M170 *", HAL_LCD_LINE_1);
   HalLcdWriteString(" Bright =     % ", HAL_LCD_LINE_2);
   HalLcdWriteChar(HAL_LCD_LINE_2, 10, tmp1);
   HalLcdWriteChar(HAL_LCD_LINE_2, 11, tmp2);
   HalLcdWriteChar(HAL_LCD_LINE_2, 12, tmp3);
#endif
   
   len = 3;
   uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, len, TransmitApp_Msg );
 }
#endif
 
#if defined(M190)
/*********************************************************************
 * @fn          M190_SensorFunction
 * @brief       The ZIGBEE Device to receive the M190 optical data.
 */
 void M190_SensorFunction(void)
 {
    // M190 sensor variable
    uint16 val;
    char buf[16];

    // ZCL transmit variable
    uint8 len;
      
    // initilization the device
    M190_Init();
    val = M190_GetValue();  
    sprintf(buf, "ADC = %u %", val);

#if defined ( LCD_SUPPORTED )
    HalLcdWriteString("* Z-Stack M190 *", HAL_LCD_LINE_1);
    HalLcdWriteString( buf, HAL_LCD_LINE_2 );
#endif
   
   len = 3;
   uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, len, TransmitApp_Msg );
 }
#endif
 
#if defined(M200)
/*********************************************************************
 * @fn          M200_SensorFunction
 * @brief       The ZIGBEE Device to receive the M200 Temp and Humi data.
 */
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