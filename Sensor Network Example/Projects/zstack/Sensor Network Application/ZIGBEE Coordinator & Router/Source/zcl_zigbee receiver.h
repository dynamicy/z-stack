/**************************************************************************************************
  Filename:       zcl_zigbee receiver.h
  Revised:        $Date: 2009-12-29 18:31:22 -0800 (Tue, 29 Dec 2009) $
  Revision:       $Revision: 21416 $

  Description:    This file contains the Zigbee Cluster Library Home
                  Automation Sample Application.

  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/
#ifndef ZCL_SAMPLELIGHT_H
#define ZCL_SAMPLELIGHT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "zcl.h"

/*********************************************************************
 * CONSTANTS
 */
#define ZIGBEERECV_ENDPOINT            13
#define ZIGBEERECV_MAX_ATTRIBUTES      12

#define LIGHT_OFF                      0x00
#define LIGHT_ON                       0x01

// Application Events
#define SAMPLELIGHT_IDENTIFY_TIMEOUT_EVT     0x0001
#define UART_MSG_EVT                         0x0002
#define KEYPAD_MSG_EVT                       0x0003
  
// Group ID for Flash Command
#define RECV_GROUP                           0x2010
  
/*********************************************************************
 * VARIABLES
 */
extern SimpleDescriptionFormat_t zclZigbeeRecv_SimpleDesc;
extern CONST zclAttrRec_t zclZigbeeRecv_Attrs[];
extern uint8  zclZigbeeRecv_OnOff;
extern uint16 zclZigbeeRecv_IdentifyTime;

extern byte zclZigbeeReceiver_TaskID;

/*
 * Initialization for the task
 */
extern void zclZigbeeRecv_Init( byte task_id );
extern void ZSendMsgProcess(char *temp);

/*
 *  Event Process for the task
 */
extern UINT16 zclZigbeeRecv_event_loop( byte task_id, UINT16 events );
extern uint16 zclZigbeeSendMsg_event_loop(uint8 task_id, uint16 events);
extern void zclUARTMsg_CallBack(uint8 port, uint8 event); // The UART Message callback

#ifdef __cplusplus
}
#endif

#endif /* ZCL_SAMPLELIGHT_H */
