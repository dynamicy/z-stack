/**************************************************************************************************
  Filename:       zcl_zigbee device.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Zigbee Cluster Library - sample device application.
**************************************************************************************************/

/**************************************************************************************************
  This device will be like an On/Off Switch device. This application is not intended to be a On/Off 
  Switch device, but will use the device description to implement this sample code.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ZComDef.h"

/* OSAL */
#include "OSAL.h"
#include "OSAL_Nv.h"

/* Profile */
#include "AF.h"

/* ZDO */
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

/* MT */
#include "MT_UART.h"

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
#include "hal_keypad.h"
#include "hal_buzzer.h"

/* SENSOR */
#if defined(M110)
  #include "M110.h"
#endif
#if defined(M140)
  #include "M140.h"
#endif
#if defined(M160)
  #include "M160.h"
#endif
#if defined(M200)
  #include "M200.h"
#endif
#if defined(M220)
  #include "M220.h"
#endif
#if defined(M250)
  #include "M250.h"
#endif
#if defined(M320)
  #include "M320.h"
#endif

/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * CONSTANTS
 */
afAddrType_t zclZigbeeDevice_DstAddr;

/*********************************************************************
 * TYPEDEFS
 */
/*********************************************************************
 * GLOBAL VARIABLES
 */
byte zclZigbeeDevice_TaskID; // Task ID variable
endPointDesc_t TransmitApp_epDesc;
uint8 len; // the transmit data length

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
#define ZCLZIGBEEDEV_BINDINGLIST       1
static cId_t bindingOutClusters[ZCLZIGBEEDEV_BINDINGLIST] = {
                                                              ZCL_CLUSTER_ID_GEN_ON_OFF // The binding cluster ID
                                                            };

// Test Endpoint to allow SYS_APP_MSGs
static endPointDesc_t zigbeeDevice_TestEp = {
                                              20,                                 // Test endpoint
                                              &zclZigbeeDevice_TaskID,
                                              (SimpleDescriptionFormat_t *)NULL,  // No Simple description for this test endpoint
                                              (afNetworkLatencyReq_t)0            // No Network Latency req
                                            };

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclZigbeeDevice_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void zclZigbeeDevice_HandleKeys( byte shift, byte keys );
static void zclZigbeeDevice_BasicResetCB( void );
static void zclZigbeeDevice_IdentifyCB( zclIdentify_t *pCmd );
static void zclZigbeeDevice_IdentifyQueryRspCB( zclIdentifyQueryRsp_t *pRsp );
static void zclZigbeeDevice_ProcessIdentifyTimeChange( void );

// Functions to process ZCL Foundation incoming Command/Response messages
static void zclZigbeeDevice_ProcessIncomingMsg( zclIncomingMsg_t *msg );
#ifdef ZCL_READ
  static uint8 zclZigbeeDevice_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg );
#endif
#ifdef ZCL_WRITE
  static uint8 zclZigbeeDevice_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg );
#endif
  static uint8 zclZigbeeDevice_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg );
#ifdef ZCL_DISCOVER
  static uint8 zclZigbeeDevice_ProcessInDiscRspCmd( zclIncomingMsg_t *pInMsg );
#endif

void zclZIGBEEDevice_SendMsg(void); // The Send ZIGBEE Message

void zclRS485_CallBack(uint8 port, uint8 event); // The RS485 Message callback
void zclRS485_SendMsg( void ); // The device receive RS485 transmit data

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclZigbeeDevice_CmdCallbacks =
{
  zclZigbeeDevice_BasicResetCB,       // Basic Cluster Reset command
  zclZigbeeDevice_IdentifyCB,         // Identify command
  zclZigbeeDevice_IdentifyQueryRspCB, // Identify Query Response command
  NULL,                               // On / Off cluster command - not needed.
  NULL,                               // Level Control Move to Level command
  NULL,                               // Level Control Move command
  NULL,                               // Level Control Step command
  NULL,                               // Group Response commands
  NULL,                               // Scene Store Request command
  NULL,                               // Scene Recall Request command
  NULL,                               // Scene Response commands
  NULL,                               // Alarm (Response) commands
  NULL,                               // RSSI Location commands
  NULL,                               // RSSI Location Response commands
};


/*********************************************************************
 * @fn          zcl_ZigbeeDevice_Init
 * @brief       Initialization function for the zclGeneral layer.
 */
void zcl_ZigbeeDevice_Init( byte task_id )
{
  zclZigbeeDevice_TaskID = task_id;

  // Set destination address to indirect
  zclZigbeeDevice_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent; //傳送模式-AddrNotPresent=0; AddrBroadcast=15(廣播)
  zclZigbeeDevice_DstAddr.endPoint = 0;                            //對象應用程式ID
  zclZigbeeDevice_DstAddr.addr.shortAddr = 0;                      //廣播內定地址
  
  zclZD_Init( &zclZigbeeDevice_SimpleDesc ); // This App is part of the ZIGBEE Device Profile                      
  zclGeneral_RegisterCmdCallbacks( ZIGBEEDEVICE_ENDPOINT, &zclZigbeeDevice_CmdCallbacks ); // Register the ZCL General Cluster Library callback functions
  zcl_registerAttrList( ZIGBEEDEVICE_ENDPOINT, ZIGBEEDEVICE_MAX_ATTRIBUTES, zclZigbeeDevice_Attrs ); // Register the application's attribute list 
  zcl_registerForMsg( zclZigbeeDevice_TaskID ); // Register the Application to receive the unprocessed Foundation command/response messages
  RegisterForKeys( zclZigbeeDevice_TaskID ); // Register for all key events - This app will handle all key events 
  afRegister( &zigbeeDevice_TestEp ); // Register for a test endpoint
  
  ZDO_RegisterForZDOMsg( zclZigbeeDevice_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( zclZigbeeDevice_TaskID, Match_Desc_rsp );
}

/*********************************************************************
 * @fn          zclZigbeeDevice_event_loop
 * @brief       Event Loop Processor for zclGeneral.
 */
uint16 zclZigbeeDevice_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter
  
  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclZigbeeDevice_TaskID )) )
    { 
      switch ( MSGpkt->hdr.event )
      {
        case ZCL_INCOMING_MSG:
          zclZigbeeDevice_ProcessIncomingMsg( (zclIncomingMsg_t *)MSGpkt ); // Incoming ZCL Foundation command/response messages         
          break;

        case ZDO_CB_MSG:
          zclZigbeeDevice_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          zclZigbeeDevice_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        default:
          break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt ); // Release the memory
    }
    return (events ^ SYS_EVENT_MSG); // return unprocessed events
  }
  
  // Called to process any change to the IdentifyTime attribute.
  if ( events & SAMPLESW_IDENTIFY_TIMEOUT_EVT )
  {
    zclSampleSw_IdentifyTime = 10;
    zclZigbeeDevice_ProcessIdentifyTimeChange();
    return ( events ^ SAMPLESW_IDENTIFY_TIMEOUT_EVT );
  }
  
  // Send the sensor data to Coordinator
  if ( events & SENSOR_SEND_MSG_EVT )
  {
  #if !defined(M270)
    osal_start_timerEx( zclZigbeeDevice_TaskID, SENSOR_SEND_MSG_EVT, SEND_DISPLAY_TIMER);
  #endif
    zclZIGBEEDevice_SendMsg();
    return ( events ^ SENSOR_SEND_MSG_EVT );
  }
  
  // The KEYPAD keyin event
  if ( events & KEYPAD_MSG_EVT )
  {
    HalLcdWriteChar(HAL_LCD_LINE_3, 0, ch);
    return ( events ^ KEYPAD_MSG_EVT );
  }
  
  // The RS485 transmit data event
  if ( events & RS485_MSG_EVT )
  {
    zclRS485_SendMsg();
    return ( events ^ RS485_MSG_EVT );
  }

  return 0; // Discard unknown events
}

void zclZigbeeDevice_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        HalLedSet( HAL_LED_1, HAL_LED_MODE_ON ); // Light LED
      }
      #if defined(BLINK_LEDS)
      else
      {      
        HalLedSet ( HAL_LED_1, HAL_LED_MODE_FLASH ); // Flash LED to show failure
      }
      #endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            zclZigbeeDevice_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            zclZigbeeDevice_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            
            // Take the first endpoint, Can be changed to search through endpoints
            zclZigbeeDevice_DstAddr.endPoint = pRsp->epList[0];
            
            HalLedSet( HAL_LED_1, HAL_LED_MODE_ON ); // Light LED
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

static void zclZigbeeDevice_HandleKeys( byte shift, byte keys )
{
  zAddrType_t dstAddr;
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW1 ) // Using timer to control send sensor data event
  { 
      osal_start_timerEx( zclZigbeeDevice_TaskID, SENSOR_SEND_MSG_EVT, SEND_DISPLAY_TIMER); // Start the OSAL timer
      osal_set_event( zclZigbeeDevice_TaskID, SENSOR_SEND_MSG_EVT ); // Action the SENSOR_SEND_MSG_EVT
  }
  
  if ( keys & HAL_KEY_SW2 ) // Atuo binding the device 
  {
    HalLedSet ( HAL_LED_2, HAL_LED_MODE_BLINK );

    // Initiate a Match Description Request (Service Discovery)
    dstAddr.addrMode = AddrBroadcast;
    dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
    ZDP_MatchDescReq( &dstAddr,  NWK_BROADCAST_SHORTADDR, ZCL_HA_PROFILE_ID, ZCLZIGBEEDEV_BINDINGLIST, bindingOutClusters,
                      0,  NULL, FALSE );
  }
}

static void zclZigbeeDevice_ProcessIdentifyTimeChange( void )
{
  if ( zclSampleSw_IdentifyTime > 0 )
  {
    osal_start_timerEx( zclZigbeeDevice_TaskID, SAMPLESW_IDENTIFY_TIMEOUT_EVT, 1000 );
    HalLedBlink ( HAL_LED_1, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME );
  }
  else
  {
    if ( zclSampleSw_OnOff )
      HalLedSet ( HAL_LED_1, HAL_LED_MODE_ON );
    else
      HalLedSet ( HAL_LED_1, HAL_LED_MODE_OFF );
      osal_stop_timerEx( zclZigbeeDevice_TaskID, SAMPLESW_IDENTIFY_TIMEOUT_EVT );
  }
}

static void zclZigbeeDevice_BasicResetCB( void )
{
}

static void zclZigbeeDevice_IdentifyCB( zclIdentify_t *pCmd )
{
  zclSampleSw_IdentifyTime = pCmd->identifyTime;
  zclZigbeeDevice_ProcessIdentifyTimeChange();
}

static void zclZigbeeDevice_IdentifyQueryRspCB( zclIdentifyQueryRsp_t *pRsp )
{
  (void)pRsp; // Query Response (with timeout value)
}

static void zclZigbeeDevice_ProcessIncomingMsg( zclIncomingMsg_t *pInMsg )
{
  switch ( pInMsg->zclHdr.commandID )
  {
    #ifdef ZCL_READ
      case ZCL_CMD_READ_RSP:
        zclZigbeeDevice_ProcessInReadRspCmd( pInMsg );
        break;
    #endif
    #ifdef ZCL_WRITE
      case ZCL_CMD_WRITE_RSP:
        zclZigbeeDevice_ProcessInWriteRspCmd( pInMsg );
        break;
    #endif
    #ifdef ZCL_REPORT // See ZCL Test Applicaiton (zcl_testapp.c) for sample code on Attribute Reporting     
      case ZCL_CMD_CONFIG_REPORT:
        //zclSampleSw_ProcessInConfigReportCmd( pInMsg );
        break;

      case ZCL_CMD_CONFIG_REPORT_RSP:
        //zclSampleSw_ProcessInConfigReportRspCmd( pInMsg );
        break;

      case ZCL_CMD_READ_REPORT_CFG:
        //zclSampleSw_ProcessInReadReportCfgCmd( pInMsg );
        break;

      case ZCL_CMD_READ_REPORT_CFG_RSP:
        //zclSampleSw_ProcessInReadReportCfgRspCmd( pInMsg );
        break;

      case ZCL_CMD_REPORT:
        //zclSampleSw_ProcessInReportCmd( pInMsg );
        break;
    #endif
      case ZCL_CMD_DEFAULT_RSP:
        zclZigbeeDevice_ProcessInDefaultRspCmd( pInMsg );
        break;
    #ifdef ZCL_DISCOVER
      case ZCL_CMD_DISCOVER_RSP:
        zclZigbeeDevice_ProcessInDiscRspCmd( pInMsg );
        break;
    #endif
      default:
        break;
  }
  if ( pInMsg->attrCmd )
    osal_mem_free( pInMsg->attrCmd );
}

#ifdef ZCL_READ
static uint8 zclZigbeeDevice_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclReadRspCmd_t *readRspCmd;
  uint8 i;

  readRspCmd = (zclReadRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < readRspCmd->numAttr; i++)
  {
    // Notify the originator of the results of the original read attributes attempt and, 
    // for each successfull request, the value of the requested attribute
  }
  return TRUE;
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
static uint8 zclZigbeeDevice_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclWriteRspCmd_t *writeRspCmd;
  uint8 i;

  writeRspCmd = (zclWriteRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < writeRspCmd->numAttr; i++)
  {
    // Notify the device of the results of the its original write attributes command.
  }
  return TRUE;
}
#endif // ZCL_WRITE

static uint8 zclZigbeeDevice_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg )
{
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;
  
  // Device is notified of the Default Response command.
  (void)pInMsg;
  return TRUE;
}

#ifdef ZCL_DISCOVER
static uint8 zclZigbeeDevice_ProcessInDiscRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclDiscoverRspCmd_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverRspCmd_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numAttr; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }
  return TRUE;
}
#endif // ZCL_DISCOVER

void zclRS485_CallBack(uint8 port, uint8 event)
{
  (void)port;
  
  if(Hal_UART_RxBufLen( MT_UART_DEFAULT_PORT) > 0 )
  {
      len = Hal_UART_RxBufLen( MT_UART_DEFAULT_PORT );
      HalUARTRead( MT_UART_DEFAULT_PORT, TransmitApp_Msg, len );
      osal_set_event( zclZigbeeDevice_TaskID, RS485_MSG_EVT );
  }
}
#if defined( MMN_UART )
uint8 uart_recv_str[20];
int str_loc = 0;
#endif
void zclRS485_SendMsg(void)
{
  uint8 len;
  uint8 uart_recv[20]; // Receive the UART command
  char temp[20];
  int null_loc;
  
  for(len = 0; len < 20; len++)
  {
    uart_recv[len]     = TransmitApp_Msg[len];
  #if defined( LCD_SUPPORTED )
    HalLcdWriteChar(HAL_LCD_LINE_4, len, uart_recv[len]);
  #endif
  #if defined( MMN_UART )
    uart_recv_str[len+str_loc] = TransmitApp_Msg[len];
    if(TransmitApp_Msg[len]=='\0'){
      null_loc = len;
      str_loc += null_loc;
      break;
    }
  #endif
  }
  
  //sprintf(temp, ",%d,", (int)TransmitApp_Msg[null_loc-1]);
  //HalUARTWrite( MT_UART_DEFAULT_PORT, temp, 20 );
  #if defined( MMN_UART )
  if(TransmitApp_Msg[null_loc-1]=='\r'){
  // Transmit the UART command to End Device
    uint8 send = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT,  &zclZigbeeDevice_DstAddr, 
                               ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, str_loc-1, uart_recv_str );
    str_loc = 0;
    memset(uart_recv_str, 0x0, 20);
  }
  #else
  uint8 send = zcl_SendCommand( ZIGBEEDEVICE_ENDPOINT,  &zclZigbeeDevice_DstAddr, 
                               ZCL_CLUSTER_ID_GEN_ON_OFF, ZCL_CLUSTER_ID_GEN_BASIC,
                                TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, false, 0, 0, len, uart_recv );
  #endif
    
}

void zclZIGBEEDevice_SendMsg(void)
{
  #if defined(M110)
    M110_SensorFunction();
  #endif
  #if defined(M140)
    M140_SensorFunction();
  #endif
  #if defined(M160)
    M160_SensorFunction();
  #endif
  #if defined(M170)
    M170_SensorFunction();
  #endif
  #if defined(M190)
    M190_SensorFunction();
  #endif
  #if defined(M200)
    M200_SensorFunction();
  #endif
  #if defined(M270)
    M270_SensorFunction();
  #endif
}

