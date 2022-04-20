/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs)
 * @date    2021
 * @brief   SRV Zadatak 17
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

/* Hardware includes. */
#include "msp430.h"

/* User's includes */
#include "../common/ETF5529_HAL/hal_ETF_5529.h"

typedef enum{
    DIODE_COMMAND_ON,
    DIODE_COMMAND_OFF,
    DIODE_COMMAND_UNDEF
}diode_command_t;


/* Task priorities */
/** "Button processing" priority */
#define mainBUTTON_PROCESSING_TASK_PRIO     ( 3 )
/** "Char processing" priority */
#define mainCHAR_PROCESSING_TASK_PRIO       ( 1 )
/** "LE Diode task" Priority */
#define mainDIODE_CONTROL_TASK_PRIO         ( 2 )

/* Char queue parameters value*/
#define mainCHAR_QUEUE_LENGTH               5
/* Diode command queue parameters value*/
#define mainDIODE_COMMAND_QUEUE_LENGTH      5

static void prvSetupHardware( void );

/* This semaphore will be used to signal "Button press" event*/
xSemaphoreHandle    xEvent_Button;
/* This queue will be used to buffer char received over UART interface*/
xQueueHandle        xCharQueue;
/* This queue will be used to buffer diode control messages*/
xQueueHandle        xCommandQueue;
/**
 * @brief "Char Processing" Function
 *
 * This task waits for character over UART received from xCharQueue. After character
 * is received it is processed and based on character value appropriate Diode Control
 * message is written to global variable
 */
static void prvCharProcessingTaskFunction( void *pvParameters )
{
    char        recChar =   0;
    diode_command_t commandToSend = DIODE_COMMAND_UNDEF;
    for ( ;; )
    {
        /*Read char from the queue*/
        xQueueReceive(xCharQueue, &recChar, portMAX_DELAY);
        /*Determine which character is received and based on character value
         * determine which command will be sent to "Diode Control" task*/
        switch(recChar){
            case 'e':
                commandToSend   = DIODE_COMMAND_ON;
                break;
            case 'd':
                commandToSend   = DIODE_COMMAND_OFF;
                break;
            default:
                commandToSend   = DIODE_COMMAND_UNDEF;
                break;
        }
        if(commandToSend != DIODE_COMMAND_UNDEF)
            xQueueSendToBack(xCommandQueue,&commandToSend,portMAX_DELAY);
    }
}
/**
 * @brief "Diode Control" task function
 *
 * This task set diode state based on prvDIODE_COMMAND variable
 */
static void prvDiodeControlTaskFunction( void *pvParameters )
{
    diode_command_t commandToProcess = DIODE_COMMAND_UNDEF;
    for ( ;; )
    {
        /* Wait on command*/
        xQueueReceive(xCommandQueue, &commandToProcess, portMAX_DELAY);
        /* Process command*/
        switch(commandToProcess){
            case DIODE_COMMAND_OFF:
                halCLR_LED(LED3);
                break;
            case DIODE_COMMAND_ON:
                halSET_LED(LED3);
                break;
            case DIODE_COMMAND_UNDEF:
                break;
        }
    }
}
/**
 * @brief "Button Task" Function
 *
 * This task waits for xEvent_Button semaphore to be given from
 * port ISR. After that, simple debauncing is performed in order
 * to verify that button is still pressed
 */
static void prvButtonTaskFunction( void *pvParameters )
{
    uint16_t i;
    /*Initial button states are 1 because of pull-up configuration*/
    uint8_t         currentButtonState  = 1;
    diode_command_t commandToSend       = DIODE_COMMAND_UNDEF;
    for ( ;; )
    {
        xSemaphoreTake(xEvent_Button,portMAX_DELAY);
        /*wait for a little to check that button is still pressed*/
        for(i = 0; i < 1000; i++);
        /*take button state*/
        /* check if button SW3 is pressed*/
        currentButtonState = ((P1IN & 0x10) >> 4);
        if(currentButtonState == 0){
            /* If SW3 is pressed send command to disable diode */
            commandToSend   =   DIODE_COMMAND_OFF;
            xQueueSendToBack(xCommandQueue, &commandToSend, portMAX_DELAY);
            continue;
        }
        /* check if button SW4 is pressed*/
        currentButtonState = ((P1IN & 0x20) >> 5);
        if(currentButtonState == 0){
            /* If SW4 is pressed send command to enable diode */
            commandToSend   =   DIODE_COMMAND_ON;
            xQueueSendToBack(xCommandQueue, &commandToSend, portMAX_DELAY);
            continue;
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
    xTaskCreate( prvCharProcessingTaskFunction,
                 "Char Processing Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainCHAR_PROCESSING_TASK_PRIO,
                 NULL
               );
    xTaskCreate( prvDiodeControlTaskFunction,
                 "Diode Control Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainDIODE_CONTROL_TASK_PRIO,
                 NULL
               );
    xTaskCreate( prvButtonTaskFunction,
                 "Button Processing Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainBUTTON_PROCESSING_TASK_PRIO,
                 NULL
               );
    /* Create FreeRTOS objects  */
    /* Create semaphores        */
    xEvent_Button           =   xSemaphoreCreateBinary();
    /* Create Queue*/
    xCharQueue              =   xQueueCreate(mainCHAR_QUEUE_LENGTH,sizeof(char));
    xCommandQueue           =   xQueueCreate(mainDIODE_COMMAND_QUEUE_LENGTH,sizeof(diode_command_t));
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

    /* Initialize UART */

     P4SEL       |= BIT4+BIT5;                    // P4.4,5 = USCI_AA TXD/RXD
     UCA1CTL1    |= UCSWRST;                      // **Put state machine in reset**
     UCA1CTL1    |= UCSSEL_2;                     // SMCLK
     UCA1BRW      = 1041;                         // 1MHz - Baudrate 9600
     UCA1MCTL    |= UCBRS_6 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
     UCA1CTL1    &= ~UCSWRST;                     // **Initialize USCI state machine**
     UCA1IE      |= UCRXIE;                       // Enable USCI_A1 RX interrupt

    /* initialize LEDs */
    vHALInitLED();
    /*enable global interrupts*/
    taskENABLE_INTERRUPTS();
}
// Echo back RXed character, confirm TX buffer is ready first
void __attribute__ ( ( interrupt( USCI_A1_VECTOR  ) ) ) vUARTISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    switch(UCA1IV)
    {
        case 0:break;                             // Vector 0 - no interrupt
        case 2:                                   // Vector 2 - RXIFG
            xQueueSendToBackFromISR(xCharQueue, &UCA1RXBUF, &xHigherPriorityTaskWoken);
        break;
        case 4:break;                             // Vector 4 - TXIFG
        default: break;
    }
    /* trigger scheduler if higher priority task is woken */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void __attribute__ ( ( interrupt( PORT1_VECTOR  ) ) ) vPORT1ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /* Give semaphore if button SW3 is pressed*/
    /* Note: This check is not truly necessary but it is good to
     * have it*/
    if(((P1IFG & 0x10) == 0x10) || ((P1IFG & 0x20) == 0x20)){
        xSemaphoreGiveFromISR(xEvent_Button, &xHigherPriorityTaskWoken);
    }
    /*Clear IFG register on exit. Read more about it in offical MSP430F5529 documentation*/
    P1IFG &=~0x30;
    /* trigger scheduler if higher priority task is woken */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
