/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs), Strahinja Jankovic (jankovics@etf.bg.ac.rs)
 * @date    2021
 * @brief   SRV Zadatak 13
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

/* Hardware includes. */
#include "msp430.h"

/* User's includes */
#include "../common/ETF5529_HAL/hal_ETF_5529.h"

/* Task priorities */
/** "Button task" priority */
#define mainBUTTON_TASK_PRIO                ( 1 )

/* Diode Timer Period */
#define mainDIODE_CHANGE_STATE_PERIOD_MS    500

/* This handle will be used as Button task instance*/
TaskHandle_t        xButtonTaskHandle;

/* Software Timer handler*/
TimerHandle_t       xDiodeTimer;

/* Currently active diode */
uint8_t             xActiveDiode;

static void prvSetupHardware( void );

/**
 * @brief "Button Task" Function
 *
 * This task waits for ISR notification after which it realizes a debugging and
 * change active diode
 */
static void prvButtonTaskFunction( void *pvParameters )
{
    uint16_t i;
    /*Initial button states are 1 because of pull-up configuration*/
    uint8_t     currentButtonState  = 1;
    for ( ;; )
    {
        /* Before we blocked this task, start time */
        xTimerStart(xDiodeTimer,portMAX_DELAY);
        /* waits for notification from ISR*/
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        /*wait for a little to check that button is still pressed*/
        for(i = 0; i < 1000; i++);
        /*take button state*/
        currentButtonState = ((P1IN & 0x10) >> 4);
        if(currentButtonState == 0){
            /* If button S3 is still pressed first stop time */
            /* it is important to stop timer before change diode state
             * because prvDiodeTimerCallback function also access global
             * variable named xActiveDiode. In order to prevent mutual exclusion
             * we stop timer */
            xTimerStop(xDiodeTimer, portMAX_DELAY);
            /* Change active diode */
            xActiveDiode = xActiveDiode == LED3? LED4 : LED3;
        }
    }
}
/**
 * @brief "Diode timer" Callback function
 *
 * This function change diode state
 */
void    prvDiodeTimerCallback(TimerHandle_t xTimer){
    halTOGGLE_LED(xActiveDiode);
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
                 &xButtonTaskHandle
               );
    /* Create timer */
    xDiodeTimer         = xTimerCreate("Diode timer",
                                       pdMS_TO_TICKS(mainDIODE_CHANGE_STATE_PERIOD_MS),
                                       pdTRUE,
                                       NULL,
                                       prvDiodeTimerCallback);
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

    /* - Init buttons - */
    /*Set direction to input*/
    P1DIR &= ~0x30;
    /*Enable pull-up resistor*/
    P1REN |= 0x30;
    P1OUT |= 0x30;
    /*Enable interrupt for pin connected to SW3*/
    P1IE  |= 0x10;
    P1IFG &=~0x10;
    /*Interrupt is generated during high to low transition*/
    P1IES |= 0x10;

    xActiveDiode    = LED3;
    /* initialize LEDs */
    vHALInitLED();
    /*enable global interrupts*/
    taskENABLE_INTERRUPTS();
}
void __attribute__ ( ( interrupt( PORT1_VECTOR  ) ) ) vPORT1ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Give semaphore if button SW3 is pressed*/
    /* Note: This check is not truly necessary but it is good to
     * have it*/
    if((P1IFG & 0x10) == 0x10){
        vTaskNotifyGiveFromISR(xButtonTaskHandle, &xHigherPriorityTaskWoken);
    }
    /*Clear IFG register on exit. Read more about it in offical MSP430F5529 documentation*/
    P1IFG &=~0x10;
    /* trigger scheduler if higher priority task is woken */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
