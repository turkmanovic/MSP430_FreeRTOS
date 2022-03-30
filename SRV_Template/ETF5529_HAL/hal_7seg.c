/**
 * @file    hal_7seg.c
 * @author  Haris Turkmanovic (haris@etf.rs)
 * @date    2021
 * @brief   7SEG DISPLAY API
 *
 * Helper functions for 7SEG Display management
 */

#include "hal_7seg.h"
#include "msp430.h"

void vHAL7SEGInit(){
    /*Init segment a*/
    HAL_7SEG_DISPLAY_1_DIR |=   HAL_7SEG_DISPLAY_1_MASK;
    HAL_7SEG_DISPLAY_2_DIR |=   HAL_7SEG_DISPLAY_2_MASK;
    HAL_7SEG_SEGMENT_A_DIR |=   HAL_7SEG_SEGMENT_A_MASK;
    HAL_7SEG_SEGMENT_B_DIR |=   HAL_7SEG_SEGMENT_B_MASK;
    HAL_7SEG_SEGMENT_C_DIR |=   HAL_7SEG_SEGMENT_C_MASK;
    HAL_7SEG_SEGMENT_D_DIR |=   HAL_7SEG_SEGMENT_D_MASK;
    HAL_7SEG_SEGMENT_E_DIR |=   HAL_7SEG_SEGMENT_E_MASK;
    HAL_7SEG_SEGMENT_F_DIR |=   HAL_7SEG_SEGMENT_F_MASK;
    HAL_7SEG_SEGMENT_G_DIR |=   HAL_7SEG_SEGMENT_G_MASK;
}

uint8_t vHAL7SEGWriteDigit(uint8_t digit){
    switch(digit){
        case 0:
            HAL_7SEG_SEGMENT_A_ON;
            HAL_7SEG_SEGMENT_B_ON;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_ON;
            HAL_7SEG_SEGMENT_E_ON;
            HAL_7SEG_SEGMENT_F_ON;
            HAL_7SEG_SEGMENT_G_OFF;
            break;
        case 1:
            HAL_7SEG_SEGMENT_A_OFF;
            HAL_7SEG_SEGMENT_B_ON;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_OFF;
            HAL_7SEG_SEGMENT_E_OFF;
            HAL_7SEG_SEGMENT_F_OFF;
            HAL_7SEG_SEGMENT_G_OFF;
            break;
        case 2:
            HAL_7SEG_SEGMENT_A_ON;
            HAL_7SEG_SEGMENT_B_ON;
            HAL_7SEG_SEGMENT_C_OFF;
            HAL_7SEG_SEGMENT_D_ON;
            HAL_7SEG_SEGMENT_E_ON;
            HAL_7SEG_SEGMENT_F_OFF;
            HAL_7SEG_SEGMENT_G_ON;
            break;
        case 3:
            HAL_7SEG_SEGMENT_A_ON;
            HAL_7SEG_SEGMENT_B_ON;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_ON;
            HAL_7SEG_SEGMENT_E_OFF;
            HAL_7SEG_SEGMENT_F_OFF;
            HAL_7SEG_SEGMENT_G_ON;
            break;
        case 4:
            HAL_7SEG_SEGMENT_A_OFF;
            HAL_7SEG_SEGMENT_B_ON;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_OFF;
            HAL_7SEG_SEGMENT_E_OFF;
            HAL_7SEG_SEGMENT_F_ON;
            HAL_7SEG_SEGMENT_G_ON;
            break;
        case 5:
            HAL_7SEG_SEGMENT_A_ON;
            HAL_7SEG_SEGMENT_B_OFF;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_ON;
            HAL_7SEG_SEGMENT_E_OFF;
            HAL_7SEG_SEGMENT_F_ON;
            HAL_7SEG_SEGMENT_G_ON;
            break;
        case 6:
            HAL_7SEG_SEGMENT_A_ON;
            HAL_7SEG_SEGMENT_B_OFF;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_ON;
            HAL_7SEG_SEGMENT_E_ON;
            HAL_7SEG_SEGMENT_F_ON;
            HAL_7SEG_SEGMENT_G_ON;
            break;
        case 7:
            HAL_7SEG_SEGMENT_A_ON;
            HAL_7SEG_SEGMENT_B_ON;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_OFF;
            HAL_7SEG_SEGMENT_E_OFF;
            HAL_7SEG_SEGMENT_F_OFF;
            HAL_7SEG_SEGMENT_G_OFF;
            break;
        case 8:
            HAL_7SEG_SEGMENT_A_ON;
            HAL_7SEG_SEGMENT_B_ON;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_ON;
            HAL_7SEG_SEGMENT_E_ON;
            HAL_7SEG_SEGMENT_F_ON;
            HAL_7SEG_SEGMENT_G_ON;
            break;
        case 9:
            HAL_7SEG_SEGMENT_A_ON;
            HAL_7SEG_SEGMENT_B_ON;
            HAL_7SEG_SEGMENT_C_ON;
            HAL_7SEG_SEGMENT_D_OFF;
            HAL_7SEG_SEGMENT_E_OFF;
            HAL_7SEG_SEGMENT_F_ON;
            HAL_7SEG_SEGMENT_G_ON;
            break;
        default:
            return 1;
    }
    return 0;
}




