/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs)
 * @date    2021
 * @brief   SRV Zadatak 11
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

typedef enum{
    BUTTON_SW3,
    BUTTON_SW4,
    BUTTON_UNDEF
}button_t;

/* Task priorities */
/** "Button task" priority */
#define mainBUTTON_TASK_PRIO        ( 2 )
/** "LE Diode task" Priority */
#define mainLED_TASK_PRIO           ( 1 )

static void prvSetupHardware( void );

/*Used to signal "Button press" event*/
xSemaphoreHandle    xEvent_Button;
/*Used to signal "Change diode state" event*/
xSemaphoreHandle    xEvent_PrintUserString;
/*This semaphore will be used to protect shared resource*/
xSemaphoreHandle    xGuard_PressedButton;

/*This button is used to remember which button is pressed*/
button_t            prvPressedButton;

/**
 * @brief "Button Task" Function
 *
 * This task waits for xEvent_Button semaphore to be given from
 * port ISR. After that, simple debauncing is performed in order
 * to verify which button is pressed and also to verify that button
 * is still pressed
 */
static void prvButtonTaskFunction( void *pvParameters )
{
    uint16_t i;
    /*Initial button states are 1 because of pull-up configuration*/
    uint8_t     currentButtonState  = 1;
    for ( ;; )
    {
        xSemaphoreTake(xEvent_Button,portMAX_DELAY);
        /*wait for a little to check that button is still pressed*/
        for(i = 0; i < 1000; i++);
        /*Check is SW3 still pressed*/
        currentButtonState = ((P1IN & 0x10) >> 4);
        if(currentButtonState == 0){
            /* If button is still pressed write that info to global variable*/
            /* Global variable is shared resource. To access it, first try to
             * take mutex */
            xSemaphoreTake(xGuard_PressedButton, portMAX_DELAY);
            /* Write which button press event is detect */
            prvPressedButton    = BUTTON_SW3;
            /* Give mutex */
            xSemaphoreGive(xGuard_PressedButton);
            /* Signal to "Diode task" to change state */
            xSemaphoreGive(xEvent_PrintUserString);
            continue;
        }
        /*Check is SW4 still pressed*/
        currentButtonState = ((P1IN & 0x20) >> 5);
        if(currentButtonState == 0){
            /* If button is still pressed write that info to global variable*/
            /* Global variable is shared resource. To access it, first try to
             * take mutex */
            xSemaphoreTake(xGuard_PressedButton, portMAX_DELAY);
            /* Write which button press event is detect */
            prvPressedButton    = BUTTON_SW4;
            /* Give mutex */
            xSemaphoreGive(xGuard_PressedButton);
            /* Signal to "Diode task" to change state */
            xSemaphoreGive(xEvent_PrintUserString);
            continue;
        }
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
    button_t    pressedButton; /*This variable will be used to temporally store global variable value */
    for ( ;; )
    {
        /*Wait on event*/
        xSemaphoreTake(xEvent_PrintUserString, portMAX_DELAY);
        /* Global variable is shared resource. To access it, first try to
         * take mutex */
        xSemaphoreTake(xGuard_PressedButton, portMAX_DELAY);
        /* Read which button press event is detect */
        pressedButton    = prvPressedButton;
        /* Give mutex */
        xSemaphoreGive(xGuard_PressedButton);
        switch(pressedButton){
        case BUTTON_SW3:
            halTOGGLE_LED(LED3);
            break;
        case BUTTON_SW4:
            halTOGGLE_LED(LED4);
            break;
        case BUTTON_UNDEF:
            //TODO: Implement error detection
            break;
        }
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
    xTaskCreate( prvLEDTaskFunction,
                 "LED Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainLED_TASK_PRIO,
                 NULL
               );
    /*Create FreeRTOS objects*/
    /*Create semaphores*/
    xEvent_Button           =   xSemaphoreCreateBinary();
    xEvent_PrintUserString            =   xSemaphoreCreateBinary();
    xGuard_PressedButton    =   xSemaphoreCreateMutex();

    prvPressedButton        =   BUTTON_UNDEF;
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
    /*Enable interrupt for pins connected to SW3 and SW4*/
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
    /* Give semaphore if button SW3 or button SW4 is pressed*/
    /* Note: This check is not truly necessary but it is good to
     * have it*/
    if(((P1IFG & 0x10) == 0x10) || ((P1IFG & 0x20) == 0x20)){
        xSemaphoreGiveFromISR(xEvent_Button, &xHigherPriorityTaskWoken);
    }
    /*Clear IFG register on exit. Read more about it in official MSP430F5529 documentation*/
    P1IFG &= ~ (((P1IFG & 0x10) == 0x10) ? 0x10 : 0x20);
    /* trigger scheduler if higher priority task is woken */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
