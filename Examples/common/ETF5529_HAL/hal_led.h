/**
 * @file    hal_led.h
 * @author  Haris Turkmanovic (haris@etf.rs), Strahinja Jankovic (jankovics@etf.bg.ac.rs)
 * @date    2021
 * @brief   LED API
 * 
 * Helper functions for LED management
 */

#ifndef HAL_LED_H
#define HAL_LED_H

/* LEDS are connected to Port D */
#define LED_PORT_DIR	( P2DIR )
#define LED_PORT_OUT	( P2OUT )

/* LED pins definitions */
#define LED3_BIT		( 4 )
#define LED4_BIT		( 5 )

/* macros to be used in code */
#define LED3			( 1 << LED3_BIT )
#define LED4			( 1 << LED4_BIT )
#define LED_ALL			( LED4 | LED3 )

/**
 * @brief Initialize ports supported by HAL
 * 
 * Initialize LED ports and pins
 */
extern void vHALInitLED( void );

/* LED management macros */
/* Turn on LED */
#define halSET_LED(led)			( LED_PORT_OUT |= led )
/* Turn off LED */
#define halCLR_LED(led)			( LED_PORT_OUT &= ~led )
/* Toggle LED */
#define halTOGGLE_LED(led)		( LED_PORT_OUT ^= led )


#endif /* HAL_BOARD_H */
