/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs), Strahinja Jankovic (jankovics@etf.bg.ac.rs)
 * @date    2021
 * @brief   SRV Zadatak 14
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

/**
 * @brief Button enumeration
 */
typedef enum{
    BUTTON_NONE,
    BUTTON_S3,
    BUTTON_S4
}button_t;

/* Task priorities */
/** "Button task" priority */
#define mainBUTTON_TASK_PRIO                ( 1 )

/* Diode Timer Period */
#define mainDIODE_CHANGE_STATE_PERIOD_100MS    10

/*This Semaphore will be used to signal potential "Button press" event*/
xSemaphoreHandle    xEvent_Button;

/* Software Timer handler*/
TimerHandle_t       xDiodeTimer;

/* Diode change state priod */
uint8_t             xPeriod;

/* Remember which diode is pressed. Its used as communication between ISR and
 * Button task*/
volatile button_t   xPressedButton;

static void prvSetupHardware( void );

/**
 * @brief "Button Task" Function
 *
 * This task waits for xEvent_Button semaphore to be given from
 * port ISR. After that, simple debauncing is performed in order
 * to verify that button is still pressed. If button is still pressed
 * start timer
 */
static void prvButtonTaskFunction( void *pvParameters )
{
    uint16_t i;
    /*Initial button states are 1 because of pull-up configuration*/
    uint8_t     currentButtonState  = 1;
    for ( ;; )
    {
        /* waits for semaphore to be released from ISR*/
        xSemaphoreTake(xEvent_Button,portMAX_DELAY);
        /*wait for a little to check that button is still pressed*/
        for(i = 0; i < 1000; i++);
        switch(xPressedButton){
        case BUTTON_NONE:
            //
            break;
        case BUTTON_S3:
            /*read button state after debauncing*/
            currentButtonState = ((P1IN & 0x10) >> 4);
            if(currentButtonState == 0){
                if(xPeriod < 30){
                    xPeriod += 2;
                }
                xTimerChangePeriod(xDiodeTimer, pdMS_TO_TICKS(xPeriod*100), 0);
            }
            break;
        case BUTTON_S4:
            /*read button state after debauncing*/
            currentButtonState = ((P1IN & 0x20) >> 5);
            if(currentButtonState == 0){
                if(xPeriod > 2){
                    xPeriod -= 2;
                }
                xTimerChangePeriod(xDiodeTimer, pdMS_TO_TICKS(xPeriod*100), 0);
            }
            break;
        }
    }
}
/**
 * @brief "Diode timer" Callback function
 *
 * This function change diode state
 */
void    prvDiodeTimerCallback(TimerHandle_t xTimer){
    halTOGGLE_LED(LED3);
}
/**
 * @brief main function
 */
void main( void )
{
    /* Configure peripherals */
    prvSetupHardware();

    /* Init user variables*/
    xPeriod = mainDIODE_CHANGE_STATE_PERIOD_100MS;
    xPressedButton = BUTTON_NONE;

    /* Create tasks */
    xTaskCreate( prvButtonTaskFunction,
                 "Button Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainBUTTON_TASK_PRIO,
                 NULL
               );
    /* Create timer */
    xDiodeTimer         = xTimerCreate("Diode timer",
                                       pdMS_TO_TICKS(xPeriod*100),
                                       pdTRUE,
                                       NULL,
                                       prvDiodeTimerCallback);
    /*Create semaphores*/
    xEvent_Button           =   xSemaphoreCreateBinary();

    taskDISABLE_INTERRUPTS();

    taskENABLE_INTERRUPTS();

    /* Start timer with initial period */
    xTimerStart(xDiodeTimer,portMAX_DELAY);

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
    P1IE  |= 0x30;
    P1IFG &=~0x30;
    /*Interrupt is generated during high to low transition*/
    P1IES |= 0x30;

    /* initialize LEDs */
    vHALInitLED();
    /*enable global interrupts*/
    taskENABLE_INTERRUPTS();
}
void __attribute__ ( ( interrupt( PORT1_VECTOR  ) ) ) vPORT1ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if((P1IFG & 0x10) == 0x10){
        /* Enter here if button SW3 is pressed*/
        xPressedButton = BUTTON_S3;
        xSemaphoreGiveFromISR(xEvent_Button, &xHigherPriorityTaskWoken);
        /*Clear IFG register on exit. Read more about it in official MSP430F5529 documentation*/
        P1IFG &=~ 0x10;
    }
    else if((P1IFG & 0x20) == 0x20){
        /* Enter here if button SW4 is pressed*/
        xPressedButton = BUTTON_S4;
        xSemaphoreGiveFromISR(xEvent_Button, &xHigherPriorityTaskWoken);
        /*Clear IFG register on exit. Read more about it in official MSP430F5529 documentation*/
        P1IFG &=~ 0x20;
    }
    else{
        /* If there is other Interrupt source, that is fault in our case and enter here*/
        xPressedButton = BUTTON_NONE;
    }
    /* trigger scheduler if higher priority task is woken */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
