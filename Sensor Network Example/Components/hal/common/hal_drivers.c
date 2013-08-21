#include "hal_types.h"
#include "OSAL.h"
#include "hal_drivers.h"
#include "hal_adc.h"
#if (defined HAL_DMA) && (HAL_DMA == TRUE)
  #include "hal_dma.h"
#endif

/* HAL */ 
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_sleep.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_flash.h"
#include "hal_buzzer.h"
#include "hal_keypad.h" 
#if (defined HAL_AES) && (HAL_AES == TRUE)
  #include "hal_aes.h"
#endif
#if (defined HAL_SPI) && (HAL_SPI == TRUE)
  #include "hal_spi.h"
#endif
#if (defined HAL_HID) && (HAL_HID == TRUE)
  #include "usb_hid.h"
#endif
#if defined(M170)
  #include "M170.h"
#endif
#if defined(M190)
  #include "M190.h"
#endif
#if defined(M140) || (M200) // SensorApp example include header file
  #include "M140.h"
  #include "M200.h"
#endif

uint8 Hal_TaskID;

extern void HalLedUpdate( void ); /* Notes: This for internal only so it shouldn't be in hal_led.h */

void Hal_Init( uint8 task_id )
{
  /* Register task ID */
  Hal_TaskID = task_id;
}

void HalDriverInit (void)
{
  /* TIMER */
#if (defined HAL_TIMER) && (HAL_TIMER == TRUE)
  HalTimerInit();
#endif

  /* ADC */
#if (defined HAL_ADC) && (HAL_ADC == TRUE)
  HalAdcInit();
  #if defined(M170)
    M170_Init();
  #endif
  #if defined(M190)
    M190_Init();
  #endif
#endif

  /* DMA */
#if (defined HAL_DMA) && (HAL_DMA == TRUE)
  // Must be called before the init call to any module that uses DMA.
  HalDmaInit();
#endif

  /* Flash */
#if (defined HAL_FLASH) && (HAL_FLASH == TRUE)
  // Must be called before the init call to any module that uses Flash access or NV.
  HalFlashInit();
#endif

  /* AES */
#if (defined HAL_AES) && (HAL_AES == TRUE)
  HalAesInit();
#endif

  /* LCD */
#if (defined HAL_LCD) && (HAL_LCD == TRUE)
  HalLcdInit();
#endif

  /* UART */
#if (defined HAL_UART) && (HAL_UART == TRUE)
  HalUARTInit();
#endif

  /* KEY */
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HalKeyInit();
#endif
  
  /* SPI */
#if (defined HAL_SPI) && (HAL_SPI == TRUE)
  HalSpiInit();
#endif

  /* HID */
#if (defined HAL_HID) && (HAL_HID == TRUE)
  usbHidInit();
#endif
    
  /* KEYPAD */
#if (defined HAL_KEYPAD) && (HAL_KEYPAD == TRUE)
  halKeypadInit();
#endif
  
  /* BUZZER */
#if (defined HAL_BUZZER) && (HAL_BUZZER == TRUE)
  halBuzzerInit();
#endif
  
  /* I2C (M200 module) */
#if (defined HAL_I2C) && (HAL_I2C == TRUE)
  #if defined(M200)
    M200_Init();
  #endif
#endif
    
  /* LED */
#if (defined HAL_LED) && (HAL_LED == TRUE)
  HalLedInit();
#endif
  
}

uint16 Hal_ProcessEvent( uint8 task_id, uint16 events )
{
  uint8 *msgPtr;
  
  (void)task_id; // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    msgPtr = osal_msg_receive(Hal_TaskID);
    
    while (msgPtr)
    {
      /* Do something here - for now, just deallocate the msg and move on */
      
      /* De-allocate(分配) */ 
      osal_msg_deallocate( msgPtr );
      /* Next */
      msgPtr = osal_msg_receive( Hal_TaskID );
    }
    return events ^ SYS_EVENT_MSG;
  }

  if ( events & HAL_LED_BLINK_EVENT )
  {
    #if (defined (BLINK_LEDS)) && (HAL_LED == TRUE)
      HalLedUpdate();
    #endif /* BLINK_LEDS && HAL_LED */
    return events ^ HAL_LED_BLINK_EVENT;
  }

  if (events & HAL_KEY_EVENT)
  {
    #if (defined HAL_KEY) && (HAL_KEY == TRUE)
    
    HalKeyPoll();/* Check for keys */
    
    if (!Hal_KeyIntEnable)/* if interrupt disabled, do next polling */
    {
      osal_start_timerEx( Hal_TaskID, HAL_KEY_EVENT, 100);
    }
    #endif // HAL_KEY
    return events ^ HAL_KEY_EVENT;
  }

#ifdef POWER_SAVING
  if ( events & HAL_SLEEP_TIMER_EVENT )
  {
    halRestoreSleepLevel();
    return events ^ HAL_SLEEP_TIMER_EVENT;
  }
#endif
  
  /* Nothing interested, discard the message */
  return 0;

}

void Hal_ProcessPoll ()
{
  /* Timer Poll */
#if (defined HAL_TIMER) && (HAL_TIMER == TRUE)
  HalTimerTick();  // 檢查終止的counter
#endif

  /* UART Poll */
#if (defined HAL_UART) && (HAL_UART == TRUE)
  HalUARTPoll();  // Poll the UART
#endif  

  /* SPI Poll */
#if (defined HAL_SPI) && (HAL_SPI == TRUE)
  HalSpiPoll();  
#endif

  /* HID poll */
#if (defined HAL_HID) && (HAL_HID == TRUE)
  usbHidProcessEvents();
#endif

}