/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs)
 * @date    2021
 * @brief   SRV Zadatak 16
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
/** "Char processing" task priority */
#define mainCHAR_PROCESSING_TASK_PRIO   ( 1 )
/** "LE Diode task" Priority */
#define mainDIODE_CONTROL_TASK_PRIO     ( 2 )

/* Queue parameter value*/
#define mainCHAR_QUEUE_LENGTH             5

static void prvSetupHardware( void );


/* This semaphore will be used to signal "Diode Control" task to read prvDIODE_CONTROL*/
xSemaphoreHandle    xEvent_DiodeCommand;
/* This is a semaphore which will be used as mutex to prevent simultaneous
 * access to shared resource */
xSemaphoreHandle    xGuard_ControlMsg;
/* This queue will be used to buffer char received over UART interface*/
xQueueHandle        xCharQueue;
/**/
diode_command_t     prvDIODE_COMMAND;
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
        if(commandToSend != DIODE_COMMAND_UNDEF){
            xSemaphoreTake(xGuard_ControlMsg, portMAX_DELAY);
            prvDIODE_COMMAND = commandToSend;
            xSemaphoreGive(xGuard_ControlMsg);
            xSemaphoreGive(xEvent_DiodeCommand);
        }
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
        /* Wait on event*/
        xSemaphoreTake(xEvent_DiodeCommand, portMAX_DELAY);
        /* Read shared variable*/
        xSemaphoreTake(xGuard_ControlMsg, portMAX_DELAY);
        commandToProcess = prvDIODE_COMMAND;
        xSemaphoreGive(xGuard_ControlMsg);
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
    /* Create FreeRTOS objects  */
    /* Create semaphores        */
    xEvent_DiodeCommand     =   xSemaphoreCreateBinary();
    /* Create MUTEX             */
    xGuard_ControlMsg       =   xSemaphoreCreateMutex();
    /* Create Queue*/
    xCharQueue              =   xQueueCreate(mainCHAR_QUEUE_LENGTH,sizeof(char));
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
