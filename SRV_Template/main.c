/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs)
 * @date    2021
 * @brief   SRV Template
 *
 * Template to be used for lab work and projects
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware includes. */
#include "msp430.h"

/* User's includes */
#include "ETF5529_HAL/hal_ETF_5529.h"

/** delay used for task synchronization */
#define mainTASK_SYNC_DELAY     ( pdMS_TO_TICKS( 100 ) )		// ( ( ( TickType_t ) 100 ) / portTICK_PERIOD_MS )

/** lp task priority */
#define mainLP_TASK_PRIO        ( 1 )

static void prvSetupHardware( void );
static void prvTaskLED4( void *pvParameters );

/**
 * @brief Low priority task
 *
 * low priority task function which toggles LED D4
 */
static void prvTaskLED4( void *pvParameters )
{
    /* low priority task that toggles LED D4 */
    for ( ;; )
    {
        /* toggle LED D4 */
        halTOGGLE_LED( LED4);
        /* delay for synchronization */
        vTaskDelay( mainTASK_SYNC_DELAY );
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
    xTaskCreate( prvTaskLED4,                   // task function
                 "LP Task",                     // task name
                 configMINIMAL_STACK_SIZE,      // stack size
                 NULL,                          // no parameter is passed
                 mainLP_TASK_PRIO,              // priority
                 NULL                           // we don't need handle
               );

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
