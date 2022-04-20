/**
 * @file    main.c
 * @author  Haris Turkmanovic(haris@etf.rs)
 * @date    2021
 * @brief   SRV Zadatak 23
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

/* Hardware includes. */
#include "msp430.h"

/* User's includes */
#include "../common/ETF5529_HAL/hal_ETF_5529.h"

#define ULONG_MAX                       0xFFFFFFFF

/** "Display task" priority */
#define mainDISPLAY_TASK_PRIO           ( 1 )
/** "ADC task" priority */
#define mainADC_TASK_PRIO               ( 2 )
/** "Button task" priority */
#define mainBUTTON_TASK_PRIO            ( 3 )
/** "Diode task" priority */
#define mainDIODE_TASK_PRIO             ( 3 )


/** ADC Task bit masks */
#define mainADC_TAKE_SAMPLE             0x01    /* Start AD conversion bit mask */
#define mainADC_CHANGE_CHANEL           0x02    /* Change AD conversion channel bit mask */

/* Display queue parameters value*/
/* Queue with length 1 is mailbox*/
#define mainDISPLAY_QUEUE_LENGTH            1

static void prvSetupHardware( void );

/* This queue will be used to send data to display task*/
xQueueHandle        xDisplayMailbox;
/* This handle will be used as Button task instance*/
TaskHandle_t        xButtonTaskHandle;
/* This handle will be used as ADC task instance*/
TaskHandle_t        xADCTaskHandle;
/* This handle will be used as DIODE task instance*/
TaskHandle_t        xDIODETaskHandle;

/**
 * @brief "Diode Control" task function
 *
 * This task set diode state based on prvDIODE_COMMAND variable
 */
static void prvDiodeControlTaskFunction( void *pvParameters )
{
    uint8_t         diodeToTurnOn   =   LED3;
    uint8_t         diodeToTurnOff  =   LED4;
    halSET_LED(diodeToTurnOn);
    halCLR_LED(diodeToTurnOff);
    for ( ;; )
    {
        /* Wait for notification*/
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        diodeToTurnOn = diodeToTurnOn == LED3? LED4 : LED3;
        diodeToTurnOff = diodeToTurnOn == LED3? LED4 : LED3;
        halSET_LED(diodeToTurnOn);
        halCLR_LED(diodeToTurnOff);
    }
}
/**
 * @brief "Display Task" Function
 *
 * This task read data from xDisplayQueue but reading is not blocking.
 * xDisplayQueue is used to send data which will be printed on 7Seg
 * display. After data is received it is decomposed on high and low digit.
 */
static void prvDisplayTaskFunction( void *pvParameters )
{
    /* New received 8bit data */
    uint8_t         NewValueToShow      = 0;
    /* High and Low number digit*/
    uint8_t         digitLow, digitHigh;
    for ( ;; )
    {
        /* Check if new number is received*/
        if(xQueueReceive(xDisplayMailbox, &NewValueToShow, 0) == pdTRUE){
            /* If there is new number to show on display, split it on High and Low digit */
            /* Extract high digit*/
            digitHigh = NewValueToShow/10;
            /* Extract low digit*/
            digitLow = NewValueToShow - digitHigh*10;
        }
        HAL_7SEG_DISPLAY_1_ON;
        HAL_7SEG_DISPLAY_2_OFF;
        vHAL7SEGWriteDigit(digitLow);
        vTaskDelay(5);
        HAL_7SEG_DISPLAY_2_ON;
        HAL_7SEG_DISPLAY_1_OFF;
        vHAL7SEGWriteDigit(digitHigh);
        vTaskDelay(5);
    }
}

/**
 * @brief "ADC Task" Function
 *
 * This task start ADC conversion or change ADC channel wich depends on
 * notification value
 */
static void prvADCTaskFunction( void *pvParameters )
{
    uint32_t    notifyValue;
    uint8_t     channel      = 1;
    for ( ;; )
    {
       /* Waits for notification value */
       xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &notifyValue, portMAX_DELAY);
       /* Check if notification bit value, defined with mainADC_TAKE_SAMPLE mask, is set*/
       if((notifyValue & mainADC_TAKE_SAMPLE) != 0){
           ADC12CTL0 |= ADC12SC;
       }
       /* Check if notification bit value, defined with mainADC_CHANGE_CHANEL mask, is set*/
       if((notifyValue & mainADC_CHANGE_CHANEL) != 0){
           /* Determine what is next channel */
           channel        = channel == 1 ? 0 : 1;
           /* Change ADC12 periphery channel */
           ADC12CTL0      &=~ ADC12ENC;
           switch(channel){
           case 0:

               ADC12MCTL0     &=~ ADC12INCH_1;
               ADC12MCTL0     |= ADC12INCH_0;
               xTaskNotifyGive(xDIODETaskHandle);
               break;
           case 1:
               ADC12MCTL0     &=~ ADC12INCH_0;
               ADC12MCTL0     |= ADC12INCH_1;
               xTaskNotifyGive(xDIODETaskHandle);
               break;
           }
           ADC12CTL0      |= ADC12ENC;
       }
    }
}
/**
 * @brief "Button Task" Function
 *
 * This task waits for ISR notification
 */
static void prvButtonTaskFunction( void *pvParameters )
{
    uint16_t i;
    /*Initial button states are 1 because of pull-up configuration*/
    uint8_t         currentButtonState  = 1;
    for ( ;; )
    {
        /* Wait for notification from ISR*/
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        /*wait for a little to check that button is still pressed*/
        for(i = 0; i < 1000; i++);
        /* check if button SW3 is pressed*/
        currentButtonState = ((P1IN & 0x10) >> 4);
        if(currentButtonState == 0){
            /* If S3 is pressed set ADC task notification value bit defined
             * with mainADC_TAKE_SAMPLE mask */
            xTaskNotify(xADCTaskHandle, mainADC_TAKE_SAMPLE, eSetBits);
            continue;
        }
        /* check if button S4 is pressed*/
        currentButtonState = ((P1IN & 0x20) >> 5);
        if(currentButtonState == 0){
            /* If S4 is pressed set ADC task notification value bit defined
             * with mainADC_CHANGE_CHANEL mask */
            xTaskNotify(xADCTaskHandle, mainADC_CHANGE_CHANEL, eSetBits);
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
    xTaskCreate( prvDisplayTaskFunction,
                 "Display Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainDISPLAY_TASK_PRIO,
                 NULL
               );
    xTaskCreate( prvADCTaskFunction,
                 "ADC Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainADC_TASK_PRIO,
                 &xADCTaskHandle
               );
    xTaskCreate( prvButtonTaskFunction,
                 "Button Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainBUTTON_TASK_PRIO,
                 &xButtonTaskHandle
               );
    xTaskCreate( prvDiodeControlTaskFunction,
                 "Diode Task",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainBUTTON_TASK_PRIO,
                 &xDIODETaskHandle
               );
    /* Create FreeRTOS objects  */
    xDisplayMailbox       =   xQueueCreate(mainDISPLAY_QUEUE_LENGTH,sizeof(uint8_t));
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

    /*Initialize ADC */
    ADC12CTL0      = ADC12SHT02 + ADC12ON;       // Sampling time, ADC12 on
    ADC12CTL1      = ADC12SHP;                   // Use sampling timer
    ADC12IE        = 0x01;                       // Enable interrupt
    ADC12MCTL0     |= ADC12INCH_1;
    ADC12CTL0      |= ADC12ENC;
    P6SEL          |= 0x03;                      // P6.0 and P6.1 ADC option select

    /* initialize LEDs */
    vHALInitLED();
    /* initialize display*/
    vHAL7SEGInit();
    /*enable global interrupts*/
    taskENABLE_INTERRUPTS();
}
void __attribute__ ( ( interrupt( ADC12_VECTOR  ) ) ) vADC12ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint16_t    temp;
    switch(__even_in_range(ADC12IV,34))
    {
        case  0: break;                           // Vector  0:  No interrupt
        case  2: break;                           // Vector  2:  ADC overflow
        case  4: break;                           // Vector  4:  ADC timing overflow
        case  6:                                  // Vector  6:  ADC12IFG0
            /* Scaling ADC value to fit on two digits representation*/
            temp    = ADC12MEM0>>6;
            xQueueSendToBackFromISR(xDisplayMailbox,&temp,&xHigherPriorityTaskWoken);
            break;
        case  8:                                  // Vector  8:  ADC12IFG1
            break;
        case 10: break;                           // Vector 10:  ADC12IFG2
        case 12: break;                           // Vector 12:  ADC12IFG3
        case 14: break;                           // Vector 14:  ADC12IFG4
        case 16: break;                           // Vector 16:  ADC12IFG5
        case 18: break;                           // Vector 18:  ADC12IFG6
        case 20: break;                           // Vector 20:  ADC12IFG7
        case 22: break;                           // Vector 22:  ADC12IFG8
        case 24: break;                           // Vector 24:  ADC12IFG9
        case 26: break;                           // Vector 26:  ADC12IFG10
        case 28: break;                           // Vector 28:  ADC12IFG11
        case 30: break;                           // Vector 30:  ADC12IFG12
        case 32: break;                           // Vector 32:  ADC12IFG13
        case 34: break;                           // Vector 34:  ADC12IFG14
        default: break;
    }
    /* trigger scheduler if higher priority task is woken */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
void __attribute__ ( ( interrupt( PORT1_VECTOR  ) ) ) vPORT1ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /* Notify Button task that one of the button is pressed*/
    /* Note: This check is not truly necessary but it is good to
     * have it*/
    if(((P1IFG & 0x10) == 0x10) || ((P1IFG & 0x20) == 0x20)){
        vTaskNotifyGiveFromISR(xButtonTaskHandle, &xHigherPriorityTaskWoken);
    }
    /*Clear IFG register on exit. Read more about it in offical MSP430F5529 documentation*/
    P1IFG &=~0x30;
    /* trigger scheduler if higher priority task is woken */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
