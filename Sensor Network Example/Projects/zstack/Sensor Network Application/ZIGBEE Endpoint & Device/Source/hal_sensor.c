#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include "mac_radio_defs.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_zigbee device.h"

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
    
    TransmitApp_Msg[0] = 1 + '0';  
    TransmitApp_Msg[1] = 1 + '0';   
    TransmitApp_Msg[2] = 0 + '0';        
    TransmitApp_Msg[3] = ',';
    TransmitApp_Msg[4] = 'N';     
    TransmitApp_Msg[5] = ',';
    if(val > 3000)
    {    
        TransmitApp_Msg[6] = 1 + '0';
    }
    else
    {
        TransmitApp_Msg[6] = 0 + '0';  
    }       

    uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                 ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 7, 
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

  TransmitApp_Msg[0] = 1 + '0';  
  TransmitApp_Msg[1] = 4 + '0';   
  TransmitApp_Msg[2] = 0 + '0';        
  TransmitApp_Msg[3] = ',';
  TransmitApp_Msg[4] = 'B';   
  TransmitApp_Msg[5] = ',';  
  TransmitApp_Msg[6] = (val / 100) + '0';
  TransmitApp_Msg[7] = ((val / 10) % 10) + '0';
  TransmitApp_Msg[8] = '.';
  TransmitApp_Msg[9] = (val % 10) + '0';
     
  uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC, 
                                TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 10, 
                                TransmitApp_Msg );   
}
#endif

#if defined(M160)
uint8 duty_M160 = 50;
void M160_SensorFunction(void)
{

  //M160_Init();
  //M160_On(duty_M160);
  /*
  if (ch > 0)
  {           
    if (ch == 'U')
    {
      if (duty_M160 < 100)
      {
        duty_M160 = duty_M160 + 5;
      }
    }
    
    if (ch == 'N')
    {
      if (duty_M160 > 0)
      {
        duty_M160 = duty_M160 - 5;
      }
    }
      
    M160_On(duty_M160);
  }
  */
  TransmitApp_Msg[0] = 1 + '0';  
  TransmitApp_Msg[1] = 6 + '0';   
  TransmitApp_Msg[2] = 0 + '0';        
  TransmitApp_Msg[3] = ',';
  TransmitApp_Msg[4] = 'A';   
  TransmitApp_Msg[5] = ',';  
  TransmitApp_Msg[6] = (duty_M160 / 100) + '0';
  TransmitApp_Msg[7] = ((duty_M160 / 10) % 10) + '0';
  TransmitApp_Msg[8] = (duty_M160 % 10) + '0';
 
  uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                 ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC, 
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 9, 
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
   
    TransmitApp_Msg[0] = 1 + '0';  
    TransmitApp_Msg[1] = 7 + '0';   
    TransmitApp_Msg[2] = 0 + '0';        
    TransmitApp_Msg[3] = ',';
    TransmitApp_Msg[4] = 'A';   
    TransmitApp_Msg[5] = ',';  
    TransmitApp_Msg[6] = (val / 100) + '0';
    TransmitApp_Msg[7] = ((val / 10) % 10) + '0';
    TransmitApp_Msg[8] = (val % 10) + '0';

    uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                 ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 9, 
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
    
    TransmitApp_Msg[0] = 1 + '0';  
    TransmitApp_Msg[1] = 9 + '0';   
    TransmitApp_Msg[2] = 0 + '0';        
    TransmitApp_Msg[3] = ',';
    TransmitApp_Msg[4] = 'A';   
    TransmitApp_Msg[5] = ',';  
    TransmitApp_Msg[6] = (val / 100) + '0';
    TransmitApp_Msg[7] = ((val / 10) % 10) + '0';
    TransmitApp_Msg[8] = (val % 10) + '0';
   
    uint8 temp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, 
                                 ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                 TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 9, 
                                 TransmitApp_Msg );
}
#endif
 
#if defined(M200)
void M200_SensorFunction(void)
 {
    // M200 module variable
    uint16 temp, humi;
      
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
    
    TransmitApp_Msg[0] = (humi / 100) + '0';
    TransmitApp_Msg[1] = ((humi / 10) % 10) + '0';
    TransmitApp_Msg[2] = '.';
    TransmitApp_Msg[3] = (humi % 10) + '0';
    TransmitApp_Msg[4] = ' ';
    TransmitApp_Msg[5] = (temp / 100) + '0';
    TransmitApp_Msg[6] = ((temp / 10) % 10) + '0';
    TransmitApp_Msg[7] = '.';
    TransmitApp_Msg[8] = (temp % 10) + '0';
    
    uint8 tmp = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_DstAddr, ZCL_CLUSTER_ID_GEN_ON_OFF, 
                                 ZCL_CLUSTER_ID_GEN_BASIC, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, 9, TransmitApp_Msg );
 }
#endif
 
#if defined(M270)

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
}
#endif