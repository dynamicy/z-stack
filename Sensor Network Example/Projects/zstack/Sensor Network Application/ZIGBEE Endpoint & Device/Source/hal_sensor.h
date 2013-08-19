/**************************************************************************************************
  Filename:       hal_sensor.h
  Revised:        $Date: 2010-07-14 (Wed, 14 July 2010) $
  Revision:       $Revision: 19453 $

  Description:    HAL Sensor - the device receive the sensor data application.
**************************************************************************************************/
/*********************************************************************
 * INCLUDES
 */
/*********************************************************************
 * FUNCTIONS
 */

/*
 *  The ZIGBEE Device to receive the M110 tempture data.
 */
 extern void M110_SensorFunction(void);

/*
 *  The ZIGBEE Device to receive the M140 tempture data.
 */
 extern void M140_SensorFunction(void);

/*
 *  The ZIGBEE Device to receive the M160 tempture data.
 */
 extern void M160_SensorFunction(void);

/*
 *  The ZIGBEE Device to receive the M170 optical data.
 */
 extern void M170_SensorFunction(void);
 
/*
 *  The ZIGBEE Device to receive the M190 optical data.
 */
 extern void M190_SensorFunction(void);
 
/*
 *  The ZIGBEE Device to receive the M200 Temp and Humi data.
 */
 extern void M200_SensorFunction(void);
 
 /*
 *  The ZIGBEE Device to control the M270 Relay.
 */
 extern void M270_SensorFunction(void);

extern void sleep(uint16 sec);

/*********************************************************************
*********************************************************************/
 
 
 
 
 