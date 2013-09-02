#ifndef ZDSECMGR_H
#define ZDSECMGR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ZComDef.h"
#include "ZDApp.h"

// Authentication options
typedef enum
{
  ZDSecMgr_Not_Authenticated = 0,   // The device has not been authenticated
  ZDSecMgr_Authenticated_CBCK,      // The devcie has been authenticated using CBKE
  ZDSecMgr_Authenticated_EA         // The device has been authenticated using EA
}ZDSecMgr_Authentication_Option;

extern void ZDSecMgrInit(uint8 state);
extern void ZDSecMgrConfig( void );
extern uint8 ZDSecMgrPermitJoining( uint8 duration );
extern void ZDSecMgrPermitJoiningTimeout( void );
extern uint8 ZDSecMgrNewDeviceEvent( void );
extern void ZDSecMgrEvent( void );
extern void ZDSecMgrEstablishKeyCfm( ZDO_EstablishKeyCfm_t* cfm );
extern void ZDSecMgrEstablishKeyInd( ZDO_EstablishKeyInd_t* ind );
extern void ZDSecMgrTransportKeyInd( ZDO_TransportKeyInd_t* ind );
extern void ZDSecMgrUpdateDeviceInd( ZDO_UpdateDeviceInd_t* ind );
extern void ZDSecMgrRemoveDeviceInd( ZDO_RemoveDeviceInd_t* ind );
extern void ZDSecMgrRequestKeyInd( ZDO_RequestKeyInd_t* ind );
extern void ZDSecMgrSwitchKeyInd( ZDO_SwitchKeyInd_t* ind );
extern void ZDSecMgrAuthenticateInd( ZDO_AuthenticateInd_t* ind );
extern void ZDSecMgrAuthenticateCfm( ZDO_AuthenticateCfm_t* cfm );
extern ZStatus_t ZDSecMgrUpdateNwkKey( uint8* key, uint8 keySeqNum, uint16 dstAddr );
extern ZStatus_t ZDSecMgrSwitchNwkKey( uint8 keySeqNum, uint16 dstAddr );
ZStatus_t ZDSecMgrRequestAppKey( uint16 partNwkAddr );
ZStatus_t ZDSecMgrSetupPartner( uint16 partNwkAddr, uint8* partExtAddr );
ZStatus_t ZDSecMgrAppKeyTypeSet( uint8 keyType );
extern ZStatus_t ZDSecMgrAddLinkKey( uint16 shortAddr, uint8 *extAddr, uint8 *key);
extern ZStatus_t ZDSecMgrDeviceRemoveByExtAddr( uint8 *pAddr );
extern uint8 ZDSecMgrInitNV( void );

ZStatus_t ZDSecMgrAPSRemove( uint16 nwkAddr, uint8 *extAddr, uint16 parentAddr );
uint8 ZDSecMgrAuthenticationCheck( uint16 shortAddr );
extern ZStatus_t APSME_TCLinkKeySync( uint16 srcAddr, SSP_Info_t* si );
extern ZStatus_t APSME_TCLinkKeyLoad( uint16 dstAddr, SSP_Info_t* si );

#ifdef __cplusplus
}
#endif

#endif /* ZDSECMGR_H */
