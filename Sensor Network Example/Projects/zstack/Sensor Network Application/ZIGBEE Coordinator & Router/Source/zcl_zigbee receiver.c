/**************************************************************************************************
  Filename:       zcl_zigbee receiver.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Zigbee Cluster Library - ZIGBEE Receiver device application.

  Copyright 2006-2009 Texas Instruments Incorporated. All rights reserved.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

/*************************************************************************************************
  This device will be like a Light device.  This application is not intended to be a Light device, 
  but will use the device description to implement this sample code.
*************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include <string.h>
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"

/* ZCL */
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_zigbee receiver.h"

/* HAL */
#include "onboard.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "hal_key.h"
#include "hal_keypad.h"

/* MAC_high-level */
#include "mac_high_level.h"

/* MT */
#include "MT_UART.h"
#include "MT.h"

/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * CONSTANTS
 */
 #define TRANSMITAPP_MAX_DATA_LEN    102

/*********************************************************************
 * TYPEDEFS
 */
/*********************************************************************
 * GLOBAL VARIABLES
 */

byte zclZigbeeReceiver_TaskID; // The zigbee task_ID

// The UART transmit variable and array
// the transmit data length
uint8 len; 
// Transmit message array
byte TransmitApp_Msg[ TRANSMITAPP_MAX_DATA_LEN ]; 
// The coordinator send to device message array
byte UartTransmit_Msg[ TRANSMITAPP_MAX_DATA_LEN ]; 
// Receive the UART command
uint8 uart_recv[TRANSMITAPP_MAX_DATA_LEN]; 

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
afAddrType_t zclZigbeeRecv_DstAddr;  // AF address type
aps_Group_t zclZigbeeRecv_Group;

#define ZCLZIGBEERECV_BINDINGLIST       2

static cId_t bindingInClusters[ZCLZIGBEERECV_BINDINGLIST] =
{
  ZCL_CLUSTER_ID_GEN_ON_OFF,
  ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL
};

// Test Endpoint to allow SYS_APP_MSGs
static endPointDesc_t zigbeeReceiver_TestEp =
{
  20,                                 // Test endpoint
  &zclZigbeeReceiver_TaskID,
  (SimpleDescriptionFormat_t *)NULL,  // No Simple description for this test endpoint
  (afNetworkLatencyReq_t)0            // No Network Latency req
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclZigbeeReceiver_HandleKeys( byte shift, byte keys );
static void zclZigbeeReceiver_BasicResetCB( void );
static void zclZigbeeReceiver_IdentifyCB( zclIdentify_t *pCmd );
static void zclZigbeeReceiver_IdentifyQueryRspCB( zclIdentifyQueryRsp_t *pRsp );
static void zclZigbeeReceiver_OnOffCB( uint8 cmd );
static void zclZigbeeReceiver_ProcessIdentifyTimeChange( void );

void zclUARTMsg_CallBack(uint8 port, uint8 event); // The UART Message callback
void zclUartReceiver( void ); // The coordinator receive command function

// Functions to process ZCL Foundation incoming Command/Response messages 
static void zclZigbeeReceiver_ProcessIncomingMsg( zclIncomingMsg_t *msg );
#ifdef ZCL_READ
  static uint8 zclZigbeeReceiver_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg );
#endif
#ifdef ZCL_WRITE
  static uint8 zclZigbeeReceiver_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg );
#endif
  static uint8 zclZigbeeReceiver_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg );
#ifdef ZCL_DISCOVER
  static uint8 zclZigbeeReceiver_ProcessInDiscRspCmd( zclIncomingMsg_t *pInMsg );
#endif
  
/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclZigbeeReceiver_CmdCallbacks =
{
  zclZigbeeReceiver_BasicResetCB,              // Basic Cluster Reset command
  zclZigbeeReceiver_IdentifyCB,                // Identify command  
  zclZigbeeReceiver_IdentifyQueryRspCB,        // Identify Query Response command
  zclZigbeeReceiver_OnOffCB,                   // On/Off cluster command
  NULL,                                        // Level Control Move to Level command
  NULL,                                        // Level Control Move command
  NULL,                                        // Level Control Step command
  NULL,                                        // Group Response commands
  NULL,                                        // Scene Store Request command
  NULL,                                        // Scene Recall Request command
  NULL,                                        // Scene Response command
  NULL,                                        // Alarm (Response) command
  NULL,                                        // RSSI Location commands
  NULL,                                        // RSSI Location Response commands
};

/*********************************************************************
 * @fn          zclZigbeeRecv_Init
 * @brief       Initialization function for the zclGeneral layer.
 */
void zclZigbeeRecv_Init( byte task_id )
{
  zclZigbeeReceiver_TaskID = task_id;
  
  // Setup for the flash command's destination address ( Group )
  zclZigbeeRecv_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;  // transmit mode - Group address
  zclZigbeeRecv_DstAddr.endPoint = ZIGBEERECV_ENDPOINT;        // end-point number:20
  zclZigbeeRecv_DstAddr.addr.shortAddr = RECV_GROUP;           // Group short address
  
  // This App is part of the Zigbee Device Profile
  zclZD_Init( &zclZigbeeRecv_SimpleDesc ); 
  
  // Register the ZCL General Cluster Library callback functions
  zclGeneral_RegisterCmdCallbacks( ZIGBEERECV_ENDPOINT, &zclZigbeeReceiver_CmdCallbacks ); 
  
  // Register the application's attribute list
  zcl_registerAttrList( ZIGBEERECV_ENDPOINT, ZIGBEERECV_MAX_ATTRIBUTES, zclZigbeeRecv_Attrs ); 
  
  // Register the Application to receive the unprocessed Foundation command/response messages
  zcl_registerForMsg( zclZigbeeReceiver_TaskID ); 
  
  // Register for all key events - This app will handle all key events
  RegisterForKeys( zclZigbeeReceiver_TaskID ); 
  
  // By default, all devices start out in Group 1 ( register grouop )
  zclZigbeeRecv_Group.ID = RECV_GROUP;
  osal_memcpy( zclZigbeeRecv_Group.name, "RECV_GROUP", 8  );
  aps_AddGroup( ZIGBEERECV_ENDPOINT, &zclZigbeeRecv_Group ); 
  
  // Register for a test endpoint
  afRegister( &zigbeeReceiver_TestEp ); 
}

/*********************************************************************
 * @fn          zclZigbeeRecv_event_loop
 * @brief       Event Loop Processor for zclGeneral.
 */
uint16 zclZigbeeRecv_event_loop( uint8 task_id, uint16 events )
{ 
    afIncomingMSGPacket_t *MSGpkt;
    (void)task_id;  // Intentionally unreferenced parameter
  
    if ( events & SYS_EVENT_MSG ) // SYSTEM Message event
    {
      while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclZigbeeReceiver_TaskID )) )
      { 
        char temp[32];
//        sprintf(temp, "Data:%s", MSGpkt->cmd.Data);
//        show(temp);
//        sprintf(temp, "Length:%d", MSGpkt->cmd.DataLength);  
//        show(temp);
//        sprintf(temp, "RSSI:%d", MSGpkt->rssi);    
//        show(temp);        
        
        switch ( MSGpkt->hdr.event )
        {
        case ZCL_INCOMING_MSG: //0x34 : Incoming ZCL foundation message
            show("ZCL_INCOMING_MSG");
            // Incoming ZCL Foundation command/response messages
            zclZigbeeReceiver_ProcessIncomingMsg( (zclIncomingMsg_t *)MSGpkt ); 
            break; 
        case KEY_CHANGE: //0xC0 : Key Events
            show("KEY_CHANGE");
            zclZigbeeReceiver_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
            break;
            
        case AF_DATA_CONFIRM_CMD:
            show("AF_DATA_CONFIRM_CMD");
            break;      
        case AF_INCOMING_MSG_CMD:
            show("AF_INCOMING_MSG_CMD");
            break;                
        case AF_INCOMING_KVP_CMD:       
            show("AF_INCOMING_KVP_CMD");
            break;
        case AF_INCOMING_GRP_KVP_CMD:       
            show("AF_INCOMING_GRP_KVP_CMD");
            break;   

        case ZDO_NEW_DSTADDR:       
            show("ZDO_NEW_DSTADDR");
            break;   
        case ZDO_STATE_CHANGE:       
            show("ZDO_STATE_CHANGE");
            break;         
        case ZDO_MATCH_DESC_RSP_SENT:       
            show("ZDO_MATCH_DESC_RSP_SENT");
            break;    
        case ZDO_CB_MSG:       
            show("ZDO_CB_MSG");
            break;    
        case ZDO_NETWORK_REPORT:       
            show("ZDO_NETWORK_REPORT");
            break;    
        case ZDO_NETWORK_UPDATE:       
            show("ZDO_NETWORK_UPDATE");
            break;     

        case NM_CHANNEL_INTERFERE:       
            show("NM_CHANNEL_INTERFERE");
            break;     
        case NM_ED_SCAN_CONFIRM:       
            show("NM_ED_SCAN_CONFIRM");
            break;     
        case SAPS_CHANNEL_CHANGE:       
            show("SAPS_CHANNEL_CHANGE");
            break;         
        case ZCL_KEY_ESTABLISH_IND:       
            show("ZCL_KEY_ESTABLISH_IND");
            break;   
           
          default:
            break;
        }
        osal_msg_deallocate( (uint8 *)MSGpkt ); // Release the memory
      }
      return (events ^ SYS_EVENT_MSG); // return unprocessed events
    }
  
    // The user define application events
    switch ( events )
    {
      case SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT: // ZIGBEE Receiver identify timeout event
        show("SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT");
        if ( zclZigbeeRecv_IdentifyTime > 0 )
             zclZigbeeRecv_IdentifyTime--;
             zclZigbeeReceiver_ProcessIdentifyTimeChange();
        break;
      
      case UART_MSG_EVT: // The UART Message event
        show("UART_MSG_EVT");
        zclUartReceiver();
        break;
      
      case KEYPAD_MSG_EVT: // The KEYPAD keyin event
        show("KEYPAD_MSG_EVT");
        #if defined( LCD_SUPPORTED )
          HalLcdWriteChar(HAL_LCD_LINE_3, 0, ch);
        #endif
        break;
    }
    
    return 0; // Discard unknown events
}

/*********************************************************************
 * @fn      zclZigbeeReceiver_HandleKeys
 * @brief   Handles all key events for this device.
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events.
 *          Valid entries: HAL_KEY_SW_1(MCU-K1)                        
 */
static void zclZigbeeReceiver_HandleKeys( byte shift, byte keys )
{
  zAddrType_t dstAddr;
  (void)shift; // Intentionally unreferenced parameter
  
#if defined(Coor_receiver) || defined (Router_receiver)

  if ( keys & HAL_KEY_SW1 ) // Init EndDevice bind reequest, and use cluster list important to binding.
  {
    dstAddr.addrMode = afAddr16Bit;
    dstAddr.addr.shortAddr = 0x0000;   // Coordinator makes the match
    ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(), ZIGBEERECV_ENDPOINT, // ZIGBEERECV_ENDPOINT = 13 is coordinator
                          ZCL_HA_PROFILE_ID, ZCLZIGBEERECV_BINDINGLIST, bindingInClusters, 0,
                          NULL, // No Outgoing clusters to bind
                          TRUE );
  }
  
  if ( keys & HAL_KEY_SW2 ) 
  {
    
  }
  
#endif
}

/*********************************************************************
 * @fn      zclZigbeeReceiver_ProcessIdentifyTimeChange
 * @brief   Called to process any change to the IdentifyTime attribute.
 */
static void zclZigbeeReceiver_ProcessIdentifyTimeChange( void )
{
  if ( zclZigbeeRecv_IdentifyTime > 0 )
  {
    osal_start_timerEx( zclZigbeeReceiver_TaskID, SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT, 1000 );
    HalLedBlink ( HAL_LED_2, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME );
  }
  else
    {
      if ( zclZigbeeRecv_OnOff )
      {
        HalLedSet ( HAL_LED_1, HAL_LED_MODE_ON );
      }
      else
        {
          HalLedSet ( HAL_LED_1, HAL_LED_MODE_OFF );
          osal_stop_timerEx( zclZigbeeReceiver_TaskID, SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT );
        }
    }
}

/*********************************************************************
 * @fn      zclZigbeeReceiver_BasicResetCB
 * @brief   Callback from the ZCL General Cluster Library
 *          to set all the Basic Cluster attributes to default values.
 */
static void zclZigbeeReceiver_BasicResetCB( void )
{
  // Reset all attributes to default values
}

/*********************************************************************
 * @fn      zclZigbeeReceiver_IdentifyCB
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an Identity Command for this application.
 * @param   srcAddr - source address and endpoint of the response message
 *          identifyTime - the number of seconds to identify yourself
 */
static void zclZigbeeReceiver_IdentifyCB( zclIdentify_t *pCmd )
{
  zclZigbeeRecv_IdentifyTime = pCmd->identifyTime;
  zclZigbeeReceiver_ProcessIdentifyTimeChange();
}

/*********************************************************************
 * @fn      zclZigbeeReceiver_IdentifyQueryRspCB
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an Identity Query Response Command for this application.
 * @param   srcAddr - requestor's address
 *          timeout - number of seconds to identify yourself (valid for query response)
 */
static void zclZigbeeReceiver_IdentifyQueryRspCB( zclIdentifyQueryRsp_t *pRsp )
{
  (void)pRsp; // Query Response (with timeout value)
}

/*********************************************************************
 * @fn      zclZigbeeReceiver_OnOffCB
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an On/Off Command for this application.
 * @param   cmd - COMMAND_ON, COMMAND_OFF or COMMAND_TOGGLE
 */
static void zclZigbeeReceiver_OnOffCB( uint8 cmd )
{ 
  if ( cmd == COMMAND_ON ) // Turn on the light
    zclZigbeeRecv_OnOff = LIGHT_ON;

  else if ( cmd == COMMAND_OFF ) // Turn off the light
    zclZigbeeRecv_OnOff = LIGHT_OFF;

  else // Toggle the light
  {
    if ( zclZigbeeRecv_OnOff == LIGHT_OFF )
          zclZigbeeRecv_OnOff = LIGHT_ON;
    else
          zclZigbeeRecv_OnOff = LIGHT_OFF;
  }

  // In this sample app, we use LED4 to simulate the Light  
  if ( zclZigbeeRecv_OnOff == LIGHT_ON )
  {
    HalLedSet( HAL_LED_2, HAL_LED_MODE_ON ); // setting the LED_1 on
  }
  else
    HalLedSet( HAL_LED_2, HAL_LED_MODE_OFF ); // setting the LED_1 off
}
/****************************************************************************** 
 * 
 *  Functions for processing ZCL Foundation incoming Command/Response messages
 *
 *****************************************************************************/

/*********************************************************************
 * @fn      zclZigbeeReceiver_ProcessIncomingMsg
 * @brief   Process ZCL Foundation incoming message
 * @param   pInMsg - pointer to the received message
 */
static void zclZigbeeReceiver_ProcessIncomingMsg( zclIncomingMsg_t *pInMsg)
{
  switch ( pInMsg->zclHdr.commandID )
  {
#ifdef ZCL_READ
    case ZCL_CMD_READ_RSP:
      zclZigbeeReceiver_ProcessInReadRspCmd( pInMsg );
      break;
#endif
      
#ifdef ZCL_WRITE    
    case ZCL_CMD_WRITE_RSP:
      zclZigbeeReceiver_ProcessInWriteRspCmd( pInMsg );
      break;
#endif
      
#ifdef ZCL_REPORT
    case ZCL_CMD_CONFIG_REPORT: // See ZCL Test Applicaiton (zcl_testapp.c) for sample code on Attribute Reporting
      //zclSampleLight_ProcessInConfigReportCmd( pInMsg );
      break;
    
    case ZCL_CMD_CONFIG_REPORT_RSP:
      //zclSampleLight_ProcessInConfigReportRspCmd( pInMsg );
      break;
    
    case ZCL_CMD_READ_REPORT_CFG:
      //zclSampleLight_ProcessInReadReportCfgCmd( pInMsg );
      break;
    
    case ZCL_CMD_READ_REPORT_CFG_RSP:
      //zclSampleLight_ProcessInReadReportCfgRspCmd( pInMsg );
      break;
    
    case ZCL_CMD_REPORT:
      //zclSampleLight_ProcessInReportCmd( pInMsg );
      break;
#endif   
      
    case ZCL_CMD_DEFAULT_RSP:
      zclZigbeeReceiver_ProcessInDefaultRspCmd( pInMsg );
      break;
      
#ifdef ZCL_DISCOVER     
    case ZCL_CMD_DISCOVER_RSP:
      zclZigbeeReceiver_ProcessInDiscRspCmd( pInMsg );
      break;
#endif
      
    default:
      break;
  }
  if ( pInMsg->attrCmd )
    osal_mem_free( pInMsg->attrCmd );
}

#ifdef ZCL_READ
/*********************************************************************
 * @fn      zclZigbeeReceiver_ProcessInReadRspCmd
 * @brief   Process the "Profile" Read Response Command
 * @param   pInMsg - incoming message to process
 */
static uint8 zclZigbeeReceiver_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg )
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
/*********************************************************************
 * @fn      zclZigbeeReceiver_ProcessInWriteRspCmd
 * @brief   Process the "Profile" Write Response Command
 * @param   pInMsg - incoming message to process
 */
static uint8 zclZigbeeReceiver_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg )
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

/*********************************************************************
 * @fn      zclZigbeeReceiver_ProcessInDefaultRspCmd
 * @brief   Process the "Profile" Default Response Command
 * @param   pInMsg - incoming message to process
 */
static uint8 zclZigbeeReceiver_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg )
{
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd; 
  // Device is notified of the Default Response command.
  (void)pInMsg;
  return TRUE; 
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      zclZigbeeReceiver_ProcessInDiscRspCmd
 * @brief   Process the "Profile" Discover Response Command
 * @param   pInMsg - incoming message to process
 */
static uint8 zclZigbeeReceiver_ProcessInDiscRspCmd( zclIncomingMsg_t *pInMsg )
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

/****************************************************************************
****************************************************************************/

/*********************************************************************
 * @fn      zclUARTMsg_CallBack
 * @brief   Send data OTA.
 * @param   port - UART port.
 * @param   event - the UART port event flag.
 */
void zclUARTMsg_CallBack(uint8 port, uint8 event)
{
  (void)port;
  
  if(Hal_UART_RxBufLen( MT_UART_DEFAULT_PORT) > 0 )
  {
      len = Hal_UART_RxBufLen( MT_UART_DEFAULT_PORT );
      HalUARTRead( MT_UART_DEFAULT_PORT, TransmitApp_Msg, len );
      osal_set_event( zclZigbeeReceiver_TaskID, UART_MSG_EVT );
  }
}

/*********************************************************************
 * @fn      zclUartReceiver
 * @brief   The coordinator receive command.
 */
void zclUartReceiver( void )
{ 
  show("zclUartReceiver");
  
  for(uint8 length = 0; length < len; length++)
  {
    uart_recv[length] = TransmitApp_Msg[length];
  #if defined( LCD_SUPPORTED )
    HalLcdWriteChar(HAL_LCD_LINE_4, length, uart_recv[length]);
  #endif
  }
  
  // Set the End Device state
  zclZigbeeRecv_DstAddr.endPoint = 12;
  zclZigbeeRecv_DstAddr.addr.shortAddr = 0xFFFF;
  zclZigbeeRecv_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
    
  // Transmit the UART command to End Device
  uint8 send = zcl_SendCommand( ZIGBEERECV_ENDPOINT, 
                                &zclZigbeeRecv_DstAddr,
                                ZCL_CLUSTER_ID_GEN_ON_OFF,
                                COMMAND_TOGGLE,
                                TRUE,
                                ZCL_FRAME_CLIENT_SERVER_DIR,
                                false,
                                0,
                                0,
                                10,
                                uart_recv );
}