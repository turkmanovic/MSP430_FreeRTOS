/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs)
 * @date    2021
 * @brief   SRV Zadatak 3
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware includes. */
#include "msp430.h"

/* User include */
#include "../common/ETF5529_HAL/hal_ETF_5529.h"

/** Task 1 Priority */
#define mainTAKS_1_PRIO        ( 2 )
/** Task 2 Priority */
#define mainTAKS_2_PRIO        ( 1 )

static void prvSetupHardware( void );
static void prvTask1Function( void *pvParameters );
static void prvTask2Function( void *pvParameters );

/**
 * @brief Task 1 function
 *
 * This task check if button is pressed and change state of diode
 * LD3. At the end, task is blocked for 200 Ticks
 */
static void prvTask1Function( void *pvParameters )
{
    uint16_t i;
    /*Initial button states are 1 because of pull-up configuration*/
    uint8_t     previousButtonState = 1;
    uint8_t     currentButtonState  = 1;
    for ( ;; )
    {
        /*Read button state*/
        currentButtonState = ((P1IN & 0x10) >> 4);
        /*Detected button signal edge*/
        if( previousButtonState != currentButtonState){
            for(i = 0; i < 1000; i++);
            /*read button state again to verify rising edge of button signal*/
            currentButtonState = ((P1IN & 0x10) >> 4);
            previousButtonState = currentButtonState;
            if(currentButtonState == 0){
                /* If button is still pressed toggle diode LD3 */
                halTOGGLE_LED( LED3 );
            }
        }
        /* delay for synchronization */
        vTaskDelay( 200 );
    }
}
/**
 * @brief Task 2 function
 *
 * This task change state of diode LD 4. At the end, task is blocked
 * for 100 Ticks
 */
static void prvTask2Function( void *pvParameters )
{
    /* low priority task that toggles LED D4 */
    for ( ;; )
    {
        /* toggle LED D4 */
        halTOGGLE_LED( LED4 );
        /* delay for synchronization */
        vTaskDelay( 100 );
    }
}
/**
 * @brief main function
 */
void main( void )
{
    /* Configure peripherals */
    prvSetupHardware();

    /* Create tasks */
    if(xTaskCreate( prvTask1Function,
                 "Task 1",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainTAKS_1_PRIO,
                 NULL
               ) != pdPASS) while(1);
    if(xTaskCreate( prvTask2Function,
                 "Task 2",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainTAKS_2_PRIO,
                 NULL
               ) != pdPASS) while(1);

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well then this line will never be reached.  If it is reached
    then it is likely that there was insufficient (FreeRTOS) heap memory space
    to create the idle task.  This may have been trapped by the malloc() failed
    hook function, if one is configured. */	
    for( ;; );
}

/**
 * @brief Configure hardware upon boot
 */
static void prvSetupHardware( void )
{
    taskDISABLE_INTERRUPTS();

    /* Disable the watchdog. */
    WDTCTL = WDTPW + WDTHOLD;

    hal430SetSystemClock( configCPU_CLOCK_HZ, configLFXT_CLOCK_HZ );

    /* Init buttons */
    P1DIR &= ~0x30;
    P1REN |= 0x30;
    P1OUT |= 0x30;

    /* initialize LEDs */
    vHALInitLED();
}
