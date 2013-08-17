/**************************************************************************************************
  Filename:       zcl_zigbee devcie_data.c
  Revised:        $Date: 2008-03-11 11:01:35 -0700 (Tue, 11 Mar 2008) $
  Revision:       $Revision: 16570 $

  Description:    Zigbee Cluster Library - sample device application.

  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"

#include "zcl_zigbee device.h"

/*********************************************************************
 * CONSTANTS
 */

#define SAMPLESW_DEVICE_VERSION     0
#define SAMPLESW_FLAGS              0

#define SAMPLESW_HWVERSION          0
#define SAMPLESW_ZCLVERSION         0

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Basic Cluster
const uint8 zclSampleSw_HWRevision = SAMPLESW_HWVERSION;
const uint8 zclSampleSw_ZCLVersion = SAMPLESW_ZCLVERSION;
const uint8 zclSampleSw_ManufacturerName[] = { 16, 'T','e','x','a','s','I','n','s','t','r','u','m','e','n','t','s' };
const uint8 zclSampleSw_ModelId[] = { 16, 'T','I','0','0','0','1',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
const uint8 zclSampleSw_DateCode[] = { 16, '2','0','0','6','0','8','3','1',' ',' ',' ',' ',' ',' ',' ',' ' };
const uint8 zclSampleSw_PowerSource = POWER_SOURCE_MAINS_1_PHASE;

uint8 zclSampleSw_LocationDescription[17] = { 16, ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
uint8 zclSampleSw_PhysicalEnvironment = 0;
uint8 zclSampleSw_DeviceEnable = DEVICE_ENABLED;

// Identify Cluster
uint16 zclSampleSw_IdentifyTime = 0;

// On/Off Cluster
uint8  zclSampleSw_OnOff = LIGHT_OFF;

/*********************************************************************
 * ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
 */
CONST zclAttrRec_t zclZigbeeDevice_Attrs[ZIGBEEDEVICE_MAX_ATTRIBUTES] =
{
  // *** General Basic Cluster Attributes ***
  {
    ZCL_CLUSTER_ID_GEN_BASIC,             // Cluster IDs - defined in the foundation (ie. zcl.h)
    { // Attribute record
      ATTRID_BASIC_HW_VERSION,            // Attribute ID - Found in Cluster Library header (ie. zcl_general.h)
      ZCL_DATATYPE_UINT8,                 // Data Type - found in zcl.h
      ACCESS_CONTROL_READ,                // Variable access control - found in zcl.h
      (void *)&zclSampleSw_HWRevision     // Pointer to attribute variable
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_ZCL_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclSampleSw_ZCLVersion
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_MANUFACTURER_NAME,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSampleSw_ManufacturerName
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_MODEL_ID,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSampleSw_ModelId
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_DATE_CODE,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSampleSw_DateCode
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_POWER_SOURCE,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclSampleSw_PowerSource
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_LOCATION_DESC,
      ZCL_DATATYPE_CHAR_STR,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)zclSampleSw_LocationDescription
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_PHYSICAL_ENV,
      ZCL_DATATYPE_UINT8,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSampleSw_PhysicalEnvironment
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_DEVICE_ENABLED,
      ZCL_DATATYPE_UINT8,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSampleSw_DeviceEnable
    }
  },
  
  // *** Identify Cluster Attribute ***
  {
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    { // Attribute record
      ATTRID_IDENTIFY_TIME,
      ZCL_DATATYPE_UINT16,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSampleSw_IdentifyTime
    }
  },
  
  // *** On / Off Cluster Attributes ***
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    { // Attribute record
      ATTRID_ON_OFF,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclSampleSw_OnOff
    }
  },
};

/*********************************************************************
 * SIMPLE DESCRIPTOR
 */
// This is the Cluster ID List and should be filled with Application specific cluster IDs.

#define ZCLSAMPLESW_MAX_INCLUSTERS       1
const cId_t zclSampleSw_InClusterList[ZCLSAMPLESW_MAX_INCLUSTERS] =
{
  ZCL_CLUSTER_ID_GEN_BASIC
};

#define ZCLSAMPLESW_MAX_OUTCLUSTERS       1
const cId_t zclSampleSw_OutClusterList[ZCLSAMPLESW_MAX_OUTCLUSTERS] =
{
  ZCL_CLUSTER_ID_GEN_ON_OFF
};

SimpleDescriptionFormat_t zclZigbeeDevice_SimpleDesc =
{
  ZIGBEEDEVICE_ENDPOINT,              //  int Endpoint;
  ZCL_HA_PROFILE_ID,                  //  uint16 AppProfId[2];
  ZCL_HA_DEVICEID_ON_OFF_SWITCH,      //  uint16 AppDeviceId[2];
  SAMPLESW_DEVICE_VERSION,            //  int   AppDevVer:4;
  SAMPLESW_FLAGS,                     //  int   AppFlags:4;
  ZCLSAMPLESW_MAX_INCLUSTERS,         //  byte  AppNumInClusters;
  (cId_t *)zclSampleSw_InClusterList, //  byte *pAppInClusterList;
  ZCLSAMPLESW_MAX_OUTCLUSTERS,        //  byte  AppNumInClusters;
  (cId_t *)zclSampleSw_OutClusterList //  byte *pAppInClusterList;
};

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/****************************************************************************
****************************************************************************/


