/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs)
 * @date    2021
 * @brief   SRV Zadatak 7
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Hardware includes. */
#include "msp430.h"

/* User's includes */
#include "../common/ETF5529_HAL/hal_ETF_5529.h"

/* User configuration*/
#define mainMAX_COUNTING_VALUE      9

/* Task priorities */
/** "Button task" priority */
#define mainBUTTON_TASK_PRIO        ( 1 )
/** "Counting task" Priority */
#define mainCOUNTING_TASK_PRIO      ( 2 )
/** "LE Diode task" Priority */
#define mainLED_TASK_PRIO           ( 3 )

static void prvSetupHardware( void );

/*This semaphore whill be used to signal "Button press" event*/
xSemaphoreHandle    xEvent_ButtonPressed;
xSemaphoreHandle    xEvent_Counting;

/**
 * @brief "Button Task" Function
 *
 * This task detect button press event and signal it to the other task
 * Thought binary semaphore
 */
static void prvButtonTaskFunction( void *pvParameters )
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
                xSemaphoreGive(xEvent_ButtonPressed);
            }
        }
    }
}
/**
 * @brief "Button Task" Function
 *
 * This task detect button press event and signal it to the other task
 * Thought binary semaphore
 */
static void prvCountingTaskFunction( void *pvParameters )
{
    uint16_t i;
    /*Init counter value to 0*/
    uint8_t     counter = 0;
    for ( ;; )
    {
        /*Wait on "Counting" event*/
        xSemaphoreTake(xEvent_ButtonPressed, portMAX_DELAY);
        if(counter == mainMAX_COUNTING_VALUE){
            /* if counting value reached, */
            counter = 0;
            xSemaphoreGive(xEvent_Counting);
        }
        else{
            counter +=1;
        }
        vHAL7SEGWriteDigit(counter);
    }
}
/**
 * @brief "LE Diode Task" function
 *
 * This task wait on "Counting" event and change LD3 diode state
 */
static void prvLEDTaskFunction( void *pvParameters )
{
    uint16_t i;
    for ( ;; )
    {
        /*Wait on "Counting" event*/
        xSemaphoreTake(xEvent_Counting, portMAX_DELAY);
        halTOGGLE_LED( LED3 );
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
    xTaskCreate( prvButtonTaskFunction,
                 "Button Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainBUTTON_TASK_PRIO,
                 NULL
               );
    xTaskCreate( prvCountingTaskFunction,
                 "Button Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainBUTTON_TASK_PRIO,
                 NULL
               );
    xTaskCreate( prvLEDTaskFunction,
                 "LED Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainLED_TASK_PRIO,
                 NULL
               );
    /*Create FreeRTOS objects*/
    /*Create semaphores*/
    xEvent_ButtonPressed    =   xSemaphoreCreateBinary();
    xEvent_Counting         =   xSemaphoreCreateBinary();
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
    /* init 7seg*/
    vHAL7SEGInit();
    /*left only one display*/
    HAL_7SEG_DISPLAY_2_OFF;
}

