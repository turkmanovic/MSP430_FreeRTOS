/**
 * @file    hal_led.c
 * @author  Haris Turkmanovic (haris@etf.rs), Strahinja Jankovic (jankovics@etf.bg.ac.rs)
 * @date    2021
 * @brief   LED API
 */

#include "hal_led.h"
#include "msp430.h"

void vHALInitLED( void )
{
    /* Switch port to output */
    LED_PORT_DIR |= LED_ALL;
    /* Switch LEDs off initially */
    LED_PORT_OUT &= ~LED_ALL;
}
