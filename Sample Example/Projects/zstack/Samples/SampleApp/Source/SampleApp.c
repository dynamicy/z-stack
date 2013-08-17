/**************************************************************************************************
  Filename:       SampleApp.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Sample Application (no Profile).


  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends it's messages either as broadcast or
  broadcast filtered group messages.  The other (more normal)
  message addressing is unicast.  Most of the other sample
  applications are written to support the unicast message model.

  Key control:
    SW1:  Sends a flash command to all devices in Group 1.
    SW2:  Adds/Removes (toggles) this device in and out
          of Group 1.  This will enable and disable the
          reception of the flash command.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_keypad.h"

/* CALCULATE */
#include "math.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs. ¶Ç°e«Ê¥]ªºID,¥]§tPeriodic clusterID¤Îflash clusterID
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,   // periodic clusterID = 1
  SAMPLEAPP_FLASH_CLUSTERID       // flash clusterID = 2
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;          //SAMPLEAPP_ENDPOINT ~ SAMPLEAPP_FLAGS:À³¥Îµ{¦¡ªº¿ìÃÑID(¹ï©ó¤U¼h¨Ó»¡)
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;  //MAX_CLUSTERS ~ SampleApp_ClusterList:±µ¦¬&¶Ç°eªº«Ê¥]IDªí¡A¦¹À³¥Î¥u¶Ç°e¤W­zªº«Ê¥]ID
};                                                              
// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;   //§¹¾ã´y­z¦¹À³¥Îªº¸ê®Æµ²ºc

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr;
afAddrType_t SampleApp_Flash_DstAddr;

aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_SendPeriodicMessage( void );
void SampleApp_SendFlashMessage( uint16 flashTime ); 


/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SampleApp_Init( uint8 task_id )
{
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT; // Device state : Initialized - not connected to anything
  SampleApp_TransID = 0; // The unique message ID (counter)

  // Device hardware initialization can be added here or in main() (Zmain.c). If the hardware is application 
  // specific(¯S©wÀ³¥Î) - add it here. If the hardware is other parts of the device(¨ä¥L¸Ë¸mªº³¡¤À) add it in main().

//#if defined ( BUILD_ALL_DEVICES )
//  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START We are looking at a 
//  // jumper (defined in SampleAppHw.c) to be jumpered together - if they are - we will start up a 
//  // coordinator. Otherwise, the device will start as a router. -> ·íjumper¦³±µ¤W®É,¸Ë¸m¥HCoordinator±Ò°Ê,§_«h¥Hrouter±Ò°Ê.
//  if ( readCoordinatorJumper() )
//    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
//  else
//    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
//#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START ) // HOLD_AUTO_START½sÄ¶¿ï¶µ·|¥õ¨îZDAppªº±Ò°Ê¸Ë¸m¤ÎÀ³¥Îµ{§Ç
  // HOLD_AUTO_START is a compile option that will surpress ZDApp from starting the device 
  // and wait for the application to start the device.
  ZDOInitDevice(0);
#endif
  
  /* SampleApp_Periodic Message's */
  // Setup for the periodic message's destination address Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast; // ¶Ç°e¼Ò¦¡-¼s¼½
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;          // end-point½s¸¹:20
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;                // ºô¸ô¦ì§}(0xFFFF -> ¼s¼½¨ìºô¸ô¤¤©Ò¦³¸Ë¸m(§t¥ð¯v))

  /* SampleApp_Flash Command's */
  // Setup for the flash command's destination address - Group 1
  SampleApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;      // ¶Ç°e¼Ò¦¡-¸s²Õ¦ì§}
  SampleApp_Flash_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;             // end-point½s¸¹:20
  SampleApp_Flash_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;    // ¸s²Õ¦WºÙ¦ì§}
  
  /* SampleApp_epDesc description */
  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;                    // SampleApp´y­zªºendpoint½s¸¹
  SampleApp_epDesc.task_id = &SampleApp_TaskID;                      // SampleApp´y­zªºTaskID
  SampleApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;   // SampleApp´y­zªº²Å¸¹
  SampleApp_epDesc.latencyReq = noLatencyReqs;                       // ¦bAF¼hµù¥U§ï¥Îend-point

  // Register the endpoint description with the AF(¦V¤U¼hµù¥U¦¹À³¥Îµ{¦¡)
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events(¦V¤U¼hµù¥U«ö¶s¨Æ¥ó)
  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1 //µù¥U¸s²Õ
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );  

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG ) 
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID ); // ³z¹Losal_msg_receive function ±µ¦¬SampleApp_TaskID¼Ð»x
    while ( MSGpkt )
    {    
      switch ( MSGpkt->hdr.event )
      {
        case KEY_CHANGE: // Received when a key is pressed(·í«ö¤Ukey®É¶i¦æ±µ¦¬)
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case AF_INCOMING_MSG_CMD: // Received when a messages is received (OTA) for this endpoint(·íendpoint±µ¦¬¨ìmessage¶i¦æ±µ¦¬)
          SampleApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE: // Received whenever the device changes state in the network(¥[¤Jºô¸ô«áª¬ºA§ïÅÜ®É¶i¦æ±µ¦¬)
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ((SampleApp_NwkState == DEV_ZB_COORD) || (SampleApp_NwkState == DEV_ROUTER) || (SampleApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending the periodic message in a regular interval. ¶}©l¦b©w´Áªº®É¶¡¶¡¹j¶Ç°e©w´Á°T®§
            osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT, SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          }
          else
            { /* Device is no longer in the network(device¤£¦b¦¹ºô¸ô¤¤) */ }
            break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {
    // Send the periodic message
    SampleApp_SendPeriodicMessage();

    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, 
                        SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                       (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 )
  {
    /* This key sends the Flash Command is sent to Group 1. This device will not
       receive the Flash Command from this device (even if it belongs to group 1). */
    SampleApp_SendFlashMessage( SAMPLEAPP_FLASH_DURATION ); //SAMPLEAPP_FLASH_DURATION -> 1000 milliseconds
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    /* The Flashr Command is sent to Group 1.
     * This key toggles this device in and out of group 1.
     * If this device doesn't belong to group 1, this application
     * will not receive the Flash command sent to group 1. */
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  uint16 flashTime;
  
  switch ( pkt->clusterId )
  {
    // ¶g´Á©Ê°T®§
    case SAMPLEAPP_PERIODIC_CLUSTERID:  
      break;
    
    // LED Blink °T®§
    case SAMPLEAPP_FLASH_CLUSTERID: 
      flashTime = BUILD_UINT16(pkt->cmd.Data[1], pkt->cmd.Data[2] );
      HalLedBlink( HAL_LED_1, 4, 50, (flashTime / 4) ); //Blink the LED4
      break;
  }
}

/*********************************************************************
 * @fn      SampleApp_SendPeriodicMessage
 *
 * @brief   Send the periodic message.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_SendPeriodicMessage( void )
{
 
  if ( AF_DataRequest ( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       1,
                       (uint8*)&SampleAppPeriodicCounter,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS
     )
  { /* Success occurred in request to send*/ }
  else
  { /* Error occurred in request to send. */ }
}

/*********************************************************************
 * @fn      SampleApp_SendFlashMessage
 *
 * @brief   Send the flash message to group 1.
 *
 * @param   flashTime - in milliseconds
 *
 * @return  none
 */
void SampleApp_SendFlashMessage( uint16 flashTime )
{
  uint8 buffer[3];
  buffer[0] = (uint8)(SampleAppFlashCounter++);
  buffer[1] = LO_UINT16( flashTime );
  buffer[2] = HI_UINT16( flashTime );
  
  if ( AF_DataRequest               
       ( &SampleApp_Flash_DstAddr,     // ¥Øªº¦a¦ì§} 
         &SampleApp_epDesc,            // end-point´y­z²Å¸¹
         SAMPLEAPP_FLASH_CLUSTERID,    // Cluster ID
         3,                            // MessageData¼Æ¾Úªø«×
         buffer,                       // MessageData¼Æ¾Ú¥[¸ü
         &SampleApp_TransID,           // °T®§ID
         AF_DISCV_ROUTE,               
         AF_DEFAULT_RADIUS             // µo°e¥b®|
       ) == afStatus_SUCCESS
     )
  { /* occurred in request to send. */ }
  else
  { /* Error occurred in request to send. */ }
}

/*********************************************************************
*********************************************************************/