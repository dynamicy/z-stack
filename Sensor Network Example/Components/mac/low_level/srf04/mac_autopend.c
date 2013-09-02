#include "mac_api.h"
#include "mac_radio_defs.h"
#include "OSAL.h"
#include "saddr.h"
#include "ZComDef.h"
#include "mac_autopend.h"
#include "mac_high_level.h"
#include "mac_spec.h"
#include "hal_lcd.h"

#define MAC_SRCMATCH_INVALID_INDEX           0xFF
#define MAC_SRCMATCH_SHORT_ENTRY_SIZE        4
#define MAC_SRCMATCH_EXT_ENTRY_SIZE          Z_EXTADDR_LEN
#define MAC_SRCMATCH_SHORT_MAX_NUM_ENTRIES   24
#define MAC_SRCMATCH_EXT_MAX_NUM_ENTRIES     12
#define MAC_SRCMATCH_ENABLE_BITMAP_LEN       3
/* bit of proprietary FCS format that indicates if the CRC is OK */
#define PROPRIETARY_FCS_CRC_OK_BIT  0x80

bool macSrcMatchIsEnabled = FALSE; 
uint8 macSrcMatchMaxNumEntries = 0;   
uint8 macSrcMatchAddrMode = SADDR_MODE_SHORT;  
bool macSrcMatchIsAckAllPending = FALSE;

static uint8 macSrcMatchFindEmptyEntry( void );
static uint8 macSrcMatchCheckSrcAddr ( sAddr_t *addr, uint16 panID  );
static void macSrcMatchSetPendEnBit( uint8 index );
static void macSrcMatchSetEnableBit( uint8 index, bool option );
static bool macSrcMatchCheckEnableBit( uint8 index );
static uint24 macSrcMatchGetEnableBit( void );
static uint24 macSrcMatchGetPendEnBit( void );

uint8 MAC_SrcMatchEnable ( uint8 addrType, uint8 num  )
{
  uint8 rtn;
  uint8 maxNum;
    
  /* Verify the address type */
  if( addrType != SADDR_MODE_SHORT && addrType != SADDR_MODE_EXT )
  {
    return MAC_INVALID_PARAMETER;
  }
  
  maxNum = ( addrType == SADDR_MODE_SHORT ) ? 
           MAC_SRCMATCH_SHORT_MAX_NUM_ENTRIES : MAC_SRCMATCH_EXT_MAX_NUM_ENTRIES;
           
  if( num > maxNum )
  {
    rtn = MAC_INVALID_PARAMETER;
    num = maxNum;
  }
  else
  {
    rtn = MAC_SUCCESS;
  }
    
  /* Turn on Frame Filter (TIMAC enables frame filter by default), TBD */
  MAC_RADIO_TURN_ON_RX_FRAME_FILTERING();
  
  /* Turn on Auto ACK (TIMAC turn on Auto ACK by default), TBD */
  MAC_RADIO_TURN_ON_AUTO_ACK();
  
  /* Turn on Autopend: set SRCMATCH.AUTOPEND and SRCMATCH.SRC_MATCH_EN */
  MAC_RADIO_TURN_ON_SRC_MATCH();
 
  /* Set SRCMATCH.AUTOPEND */
  MAC_RADIO_TURN_ON_AUTOPEND();
  
  /* Configure all the globals */
  macSrcMatchIsEnabled = TRUE;
  macSrcMatchMaxNumEntries = num;
  macSrcMatchAddrMode = addrType;           

  return rtn;
}

uint8 MAC_SrcMatchAddEntry ( sAddr_t *addr, uint16 panID )
{
  uint8 index;
  uint8 entry[MAC_SRCMATCH_SHORT_ENTRY_SIZE];
  
  /* Check if the input parameters are valid */
  if ( addr == NULL || addr->addrMode != macSrcMatchAddrMode )
  {
    return MAC_INVALID_PARAMETER;  
  }
  
  /* Check if the entry already exists. Do not add duplicated entry */
  if ( macSrcMatchCheckSrcAddr( addr, panID ) != MAC_SRCMATCH_INVALID_INDEX )
  {
    return MAC_DUPLICATED_ENTRY; 
  }
  
  /* If not duplicated, write to the radio RAM and enable the control bit */
  
  /* Find the first empty entry */
  index = macSrcMatchFindEmptyEntry();
  if ( index == macSrcMatchMaxNumEntries )
  {
    return MAC_NO_RESOURCES;   /* Table is full */
  }
  
  if ( macSrcMatchAddrMode == SADDR_MODE_SHORT )
  {
    /* Write the PanID and short address */
    entry[0] = LO_UINT16( panID );  /* Little Endian for the radio RAM */
    entry[1] = HI_UINT16( panID );
    entry[2] = LO_UINT16( addr->addr.shortAddr );
    entry[3] = HI_UINT16( addr->addr.shortAddr );
    MAC_RADIO_SRC_MATCH_TABLE_WRITE( ( index * MAC_SRCMATCH_SHORT_ENTRY_SIZE ), 
                   entry, MAC_SRCMATCH_SHORT_ENTRY_SIZE );
  }
  else
  {
    /* Write the extended address */
    MAC_RADIO_SRC_MATCH_TABLE_WRITE( ( index * MAC_SRCMATCH_EXT_ENTRY_SIZE ), 
                   addr->addr.extAddr, MAC_SRCMATCH_EXT_ENTRY_SIZE ); 
  }
  
  /* Set the Autopend enable bits */
  macSrcMatchSetPendEnBit( index );
  
  /* Set the Src Match enable bits */
  macSrcMatchSetEnableBit( index, TRUE );
  
  return MAC_SUCCESS;
}

uint8 MAC_SrcMatchDeleteEntry ( sAddr_t *addr, uint16 panID  )
{
  uint8 index;
  
  if ( addr == NULL || addr->addrMode != macSrcMatchAddrMode )
  {
    return MAC_INVALID_PARAMETER;  
  }
  
  /* Look up the source address table and find the entry. */
  index = macSrcMatchCheckSrcAddr( addr, panID );

  if( index == MAC_SRCMATCH_INVALID_INDEX )
  {
    return MAC_INVALID_PARAMETER; 
  }
  
  /* Clear Src Match enable bits */
  macSrcMatchSetEnableBit( index, FALSE );

  return MAC_SUCCESS;
}
                  
void MAC_SrcMatchAckAllPending ( uint8 option  ) 
{
  if( option == TRUE )
  {
    macSrcMatchIsAckAllPending = TRUE;
    
    /* Set the PENDING_OR register */
    MAC_RADIO_TURN_ON_PENDING_OR();
  }
  else
  {
    macSrcMatchIsAckAllPending = FALSE;
    
    /* Clear the PENDING_OR register */
    MAC_RADIO_TURN_OFF_PENDING_OR();
  }
}

/*********************************************************************
 * @fn          MAC_SrcMatchCheckAllPending
 *
 * @brief       Check if acknowledging all packets with pending bit set
 *              is enabled. 
 *
 * @param       none 
 *
 * @return      MAC_AUTOACK_PENDING_ALL_ON or MAC_AUTOACK_PENDING_ALL_OFF
 */
uint8 MAC_SrcMatchCheckAllPending ( void )
{
  if( macSrcMatchIsAckAllPending == TRUE )
  {
    return MAC_AUTOACK_PENDING_ALL_ON; 
  }
  
  return MAC_AUTOACK_PENDING_ALL_OFF;
}

/*********************************************************************
 * @fn          MAC_SrcMatchCheckResult
 *
 * @brief       Check the result of source matching
 *
 * @param       index - index of the entry in the source address table
 *
 * @return      TRUE or FALSE
 */
MAC_INTERNAL_API bool MAC_SrcMatchCheckResult( void )
{
  uint8 resIndex;
  
  if ( macSrcMatchIsAckAllPending )
  {
    return (TRUE);
  }
  
  MAC_RADIO_SRC_MATCH_RESINDEX( resIndex );
  
  return ( resIndex & AUTOPEND_RES );
}

/*********************************************************************
 * @fn          macSrcMatchFindEmptyEntry
 *
 * @brief       return index of the first empty entry found
 *
 * @param       none
 *
 * @return      uint8 - return index of the first empty entry found
 */
static uint8 macSrcMatchFindEmptyEntry( void )
{
  uint8  index;
  uint24 enable;
     
  enable = MAC_RADIO_SRC_MATCH_GET_EN();
        
  if( macSrcMatchAddrMode == SADDR_MODE_SHORT )
  {
    for( index = 0; index < macSrcMatchMaxNumEntries; index++ )
    {  
      if( ( enable & ( (uint24)0x01 << index ) ) == 0 )
      {
        return index;
      }
    }
  }
  else
  {
    for( index = 0; index < macSrcMatchMaxNumEntries; index++ )
    {  
      if( ( enable & ( (uint24)0x01 << ( index * 2 ) ) ) == 0 )
      {
        return index;
      }
    }
  }
  
  /* 
   The value of index shall be macSrcMatchMaxNumEntries when it executes
   here. The table is full.
  */
  return index;
}

static uint8 macSrcMatchCheckSrcAddr ( sAddr_t *addr, uint16 panID  )
{
  uint8 index;     
  uint8 *pAddr;
  uint8 entrySize;
  uint8 entry[MAC_SRCMATCH_SHORT_ENTRY_SIZE];  
  uint8 ramEntry[MAC_SRCMATCH_EXT_ENTRY_SIZE];

  if( macSrcMatchAddrMode == SADDR_MODE_SHORT )
  {
    entry[0] = LO_UINT16( panID );  /* Little Endian for the radio RAM */
    entry[1] = HI_UINT16( panID );
    entry[2] = LO_UINT16( addr->addr.shortAddr );
    entry[3] = HI_UINT16( addr->addr.shortAddr );
    pAddr = entry;
    entrySize = MAC_SRCMATCH_SHORT_ENTRY_SIZE;
  }
  else
  {
    pAddr = addr->addr.extAddr;
    entrySize = MAC_SRCMATCH_EXT_ENTRY_SIZE;
  }

  for( index = 0; index < macSrcMatchMaxNumEntries; index++ )
  {
    /* Check if the entry is enabled */
    if( macSrcMatchCheckEnableBit( index ) == FALSE )
    {
      continue; 
    }
      
    /* Compare the short address and pan ID */
    MAC_RADIO_SRC_MATCH_TABLE_READ( ( index * entrySize ), ramEntry, entrySize );
     
    if( osal_memcmp( pAddr, ramEntry, entrySize ) == TRUE )
    {
      /* Match found */
      return index;
    }
  }
  return MAC_SRCMATCH_INVALID_INDEX;
}

static void macSrcMatchSetPendEnBit( uint8 index )
{
  uint24 enable;
  uint8 buf[MAC_SRCMATCH_ENABLE_BITMAP_LEN];
       
  enable = MAC_RADIO_SRC_MATCH_GET_PENDEN(); 
      
  if( macSrcMatchAddrMode == SADDR_MODE_SHORT )
  {
    enable |= ( (uint24)0x01 << index );
    osal_buffer_uint24( buf, enable );
    MAC_RADIO_SRC_MATCH_SET_SHORTPENDEN( buf );
  }
  else
  {
    enable |= ( (uint24)0x01 << ( index * 2 ) );
    osal_buffer_uint24( buf, enable );
    MAC_RADIO_SRC_MATCH_SET_EXTPENDEN( buf );
  }
}

static void macSrcMatchSetEnableBit( uint8 index, bool option )
{
  uint24 enable;  
  
  enable = MAC_RADIO_SRC_MATCH_GET_EN(); 
      
  if( option == TRUE )
  {
    if( macSrcMatchAddrMode == SADDR_MODE_SHORT )
    {
      enable |= ( (uint24)0x01 << index );
      MAC_RADIO_SRC_MATCH_SET_SHORTEN( enable );
    }
    else
    {
      enable |= ( (uint24)0x01 << ( index * 2 ) );
      MAC_RADIO_SRC_MATCH_SET_EXTEN( enable );
    }
  }
  else
  {
    if( macSrcMatchAddrMode == SADDR_MODE_SHORT )
    {
      enable &= ~( (uint24)0x01 << index );
      MAC_RADIO_SRC_MATCH_SET_SHORTEN( enable );
    }
    else
    {
      enable &= ~( (uint24)0x01 << ( index * 2 ) );
      MAC_RADIO_SRC_MATCH_SET_EXTEN( enable );
    }
  }
}

static bool macSrcMatchCheckEnableBit( uint8 index )
{
  uint24 enable;
  
  if( macSrcMatchAddrMode == SADDR_MODE_EXT )
  {
    index *= 2;
  }
  
  enable = MAC_RADIO_SRC_MATCH_GET_EN();
     
  if( enable & ( (uint24)0x01 << index ) )
  {
    return TRUE;
  }

  return FALSE; 
}

static uint24 macSrcMatchGetEnableBit( void )
{ 
  uint8 buf[MAC_SRCMATCH_ENABLE_BITMAP_LEN];
  
  if( macSrcMatchAddrMode == SADDR_MODE_SHORT )
  {
    MAC_RADIO_GET_SRC_SHORTEN( buf );
  }
  else
  {
    MAC_RADIO_GET_SRC_EXTEN( buf );
  }
  
  return osal_build_uint32( buf, MAC_SRCMATCH_ENABLE_BITMAP_LEN );
}

static uint24 macSrcMatchGetPendEnBit( void )
{
  uint8 buf[MAC_SRCMATCH_ENABLE_BITMAP_LEN];

  if( macSrcMatchAddrMode == SADDR_MODE_SHORT )
  {
    MAC_RADIO_GET_SRC_SHORTPENDEN( buf );
  }
  else
  {
    MAC_RADIO_GET_SRC_EXTENPEND( buf );
  }
  
  return osal_build_uint32( buf, MAC_SRCMATCH_ENABLE_BITMAP_LEN );
}