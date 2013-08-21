#include "hal_defs.h"
//#include "ioCC2530.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "M160.h"

uint8 M160_DutyCount;
uint8 M160_DutyValue;

HAL_ISR_FUNCTION(T4_ISR, T4_VECTOR)
{
    M160_DutyCount++;
    if (M160_DutyCount >= 100)
    {
        M160_DutyCount = 0;
    }
    if (M160_DutyCount >= M160_DutyValue)
    {
        HAL_DO_OFF();
    }
    else
    {
        HAL_DO_ON();
    }
}

void M160_Init(void)
{
    HAL_DO_OUTPUT();

    // Set prescaler divider value to 128 (8KHz)
    X_T4CTL |= 0x80;

    X_T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt
}

void M160_On(uint8 duty)
{
    M160_DutyCount = 0;
    M160_DutyValue = duty;

    X_T4CTL |= 0x10; // Start timer
    T4IE = 1; // Enable interrupt
}

void M160_Off(void)
{
    X_T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt
    HAL_DO_OFF();
}