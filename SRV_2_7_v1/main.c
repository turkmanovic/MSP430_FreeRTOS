/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs)
 * @date    2021
 * @brief   SRV Zadatak 12 - Variant 1
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

/* Task priorities */
/** "Button task" priority */
#define mainBUTTON_TASK_PRIO            ( 3 )
/** "LE Diode task" Priority */
#define mainSTRING1_TASK_PRIO           ( 2 )
/** "LE Diode task" Priority */
#define mainPERIODIC_STRING_TASK_PRIO   ( 1 )

static void prvSetupHardware( void );


/*This semaphore will be used to signal "Button press" event*/
xSemaphoreHandle    xEvent_Button;
/*This semaphore will be used to signal printf of User string*/
xSemaphoreHandle    xEvent_PrintUserString;


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
    uint8_t     currentButtonState  = 1;
    for ( ;; )
    {
        xSemaphoreTake(xEvent_Button,portMAX_DELAY);
        /*wait for a little to check that button is still pressed*/
        for(i = 0; i < 1000; i++);
        /*take button state*/
        currentButtonState = ((P1IN & 0x10) >> 4);
        if(currentButtonState == 0){
            /* If button is still pressed send signal to "Diode" task */
            xSemaphoreGive(xEvent_PrintUserString);
        }
    }
}
/**
 * @brief "Button String" function
 *
 * This task print string when button SW3 is pressed
 */
static void prvString1TaskFunction( void *pvParameters )
{
    char    string[]="- Korisnik pritisnuo taster\r\n";
    for ( ;; )
    {
        /*Wait on event*/
        xSemaphoreTake(xEvent_PrintUserString, portMAX_DELAY);
        /*Wait on event*/
        char* tmp   =   string;
        while(*tmp != 0){
            while(!(UCA1IFG&UCTXIFG));
            UCA1TXBUF = *tmp;
            tmp += 1;
        }
    }
}
/**
 * @brief "Periodic String" function
 *
 * This task print string with period of 200 system ticks
 */
static void prvPeriodicStringTaskFunction( void *pvParameters )
{
    char    string[]="* Ovaj ispis se poziva periodicno iz \"Periodic Task\" taska\r\n";
    for ( ;; )
    {
        /*Wait on event*/
        char* tmp   =   string;
        while(*tmp != 0){
            while(!(UCA1IFG&UCTXIFG));
            UCA1TXBUF = *tmp;
            tmp += 1;
        }
        vTaskDelay(200);
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
    xTaskCreate( prvString1TaskFunction,
                 "Button String",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainSTRING1_TASK_PRIO,
                 NULL
               );
    xTaskCreate( prvPeriodicStringTaskFunction,
                 "Periodic String",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainPERIODIC_STRING_TASK_PRIO,
                 NULL
               );
    /*Create FreeRTOS objects*/
    /*Create semaphores*/
    xEvent_Button                       =   xSemaphoreCreateBinary();
    xEvent_PrintUserString              =   xSemaphoreCreateBinary();
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
     UCA1BRW      = 1041;                            // 1MHz 115200
     UCA1MCTL    |= UCBRS_6 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
     UCA1CTL1    &= ~UCSWRST;                     // **Initialize USCI state machine**
     UCA1IE      |= UCRXIE;                       // Enable USCI_A1 RX interrupt

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
        xSemaphoreGiveFromISR(xEvent_Button, &xHigherPriorityTaskWoken);
    }
    /*Clear IFG register on exit. Read more about it in offical MSP430F5529 documentation*/
    P1IFG &=~0x10;
    /* trigger scheduler if higher priority task is woken */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
