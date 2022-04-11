/**
 * @file    hal_7seg.h
 * @author  Haris Turkmanovic (haris@etf.rs)
 * @date    2021
 * @brief   7SEG DISPLAY API
 *
 * Helper functions for 7SEG Display management
 */


#include <stdint.h>
#ifndef HAL_7SEG_H_
#define HAL_7SEG_H_


/*Displays position inside of register*/
#define HAL_7SEG_DISPLAY_1_MASK          0x10
#define HAL_7SEG_DISPLAY_2_MASK          0x01
/*Segments position inside of registers*/
#define HAL_7SEG_SEGMENT_A_MASK          0x80
#define HAL_7SEG_SEGMENT_B_MASK          0x08
#define HAL_7SEG_SEGMENT_C_MASK          0x40
#define HAL_7SEG_SEGMENT_D_MASK          0x02
#define HAL_7SEG_SEGMENT_E_MASK          0x08
#define HAL_7SEG_SEGMENT_F_MASK          0x01
#define HAL_7SEG_SEGMENT_G_MASK          0x04

/*Displays  direction registers*/
#define HAL_7SEG_DISPLAY_1_DIR          P6DIR
#define HAL_7SEG_DISPLAY_2_DIR          P7DIR
/*Segments direction registers*/
#define HAL_7SEG_SEGMENT_A_DIR          P3DIR
#define HAL_7SEG_SEGMENT_B_DIR          P4DIR
#define HAL_7SEG_SEGMENT_C_DIR          P2DIR
#define HAL_7SEG_SEGMENT_D_DIR          P8DIR
#define HAL_7SEG_SEGMENT_E_DIR          P2DIR
#define HAL_7SEG_SEGMENT_F_DIR          P4DIR
#define HAL_7SEG_SEGMENT_G_DIR          P8DIR


/*Displays  direction registers*/
#define HAL_7SEG_DISPLAY_1_OUT          P6OUT
#define HAL_7SEG_DISPLAY_2_OUT          P7OUT
/*Segments output registers*/
#define HAL_7SEG_SEGMENT_A_OUT          P3OUT
#define HAL_7SEG_SEGMENT_B_OUT          P4OUT
#define HAL_7SEG_SEGMENT_C_OUT          P2OUT
#define HAL_7SEG_SEGMENT_D_OUT          P8OUT
#define HAL_7SEG_SEGMENT_E_OUT          P2OUT
#define HAL_7SEG_SEGMENT_F_OUT          P4OUT
#define HAL_7SEG_SEGMENT_G_OUT          P8OUT

#define HAL_7SEG_SEGMENT_A_ON  HAL_7SEG_SEGMENT_A_OUT &=~ HAL_7SEG_SEGMENT_A_MASK
#define HAL_7SEG_SEGMENT_B_ON  HAL_7SEG_SEGMENT_B_OUT &=~ HAL_7SEG_SEGMENT_B_MASK
#define HAL_7SEG_SEGMENT_C_ON  HAL_7SEG_SEGMENT_C_OUT &=~ HAL_7SEG_SEGMENT_C_MASK
#define HAL_7SEG_SEGMENT_D_ON  HAL_7SEG_SEGMENT_D_OUT &=~ HAL_7SEG_SEGMENT_D_MASK
#define HAL_7SEG_SEGMENT_E_ON  HAL_7SEG_SEGMENT_E_OUT &=~ HAL_7SEG_SEGMENT_E_MASK
#define HAL_7SEG_SEGMENT_F_ON  HAL_7SEG_SEGMENT_F_OUT &=~ HAL_7SEG_SEGMENT_F_MASK
#define HAL_7SEG_SEGMENT_G_ON  HAL_7SEG_SEGMENT_G_OUT &=~ HAL_7SEG_SEGMENT_G_MASK


#define HAL_7SEG_SEGMENT_A_OFF  HAL_7SEG_SEGMENT_A_OUT |= HAL_7SEG_SEGMENT_A_MASK
#define HAL_7SEG_SEGMENT_B_OFF  HAL_7SEG_SEGMENT_B_OUT |= HAL_7SEG_SEGMENT_B_MASK
#define HAL_7SEG_SEGMENT_C_OFF  HAL_7SEG_SEGMENT_C_OUT |= HAL_7SEG_SEGMENT_C_MASK
#define HAL_7SEG_SEGMENT_D_OFF  HAL_7SEG_SEGMENT_D_OUT |= HAL_7SEG_SEGMENT_D_MASK
#define HAL_7SEG_SEGMENT_E_OFF  HAL_7SEG_SEGMENT_E_OUT |= HAL_7SEG_SEGMENT_E_MASK
#define HAL_7SEG_SEGMENT_F_OFF  HAL_7SEG_SEGMENT_F_OUT |= HAL_7SEG_SEGMENT_F_MASK
#define HAL_7SEG_SEGMENT_G_OFF  HAL_7SEG_SEGMENT_G_OUT |= HAL_7SEG_SEGMENT_G_MASK

#define HAL_7SEG_DISPLAY_1_ON   HAL_7SEG_DISPLAY_1_OUT &=~  HAL_7SEG_DISPLAY_1_MASK
#define HAL_7SEG_DISPLAY_2_ON   HAL_7SEG_DISPLAY_2_OUT &=~  HAL_7SEG_DISPLAY_2_MASK

#define HAL_7SEG_DISPLAY_1_OFF  HAL_7SEG_DISPLAY_1_OUT |=  HAL_7SEG_DISPLAY_1_MASK
#define HAL_7SEG_DISPLAY_2_OFF  HAL_7SEG_DISPLAY_2_OUT |=  HAL_7SEG_DISPLAY_2_MASK



typedef enum{
    HAL_DISPLAY_1 = 0,
    HAL_DISPLAY_2 = 1
}hal_7seg_display_t;
/*Init 7seg displays and segments*/
void        vHAL7SEGInit();
/*Write digit to previously enabled display*/
uint8_t     vHAL7SEGWriteDigit(uint8_t digit);


#endif /* ETF5529_HAL_HAL_7SEG_H_ */
