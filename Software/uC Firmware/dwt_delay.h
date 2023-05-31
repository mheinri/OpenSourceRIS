/*
 * dwt_delay.h
 *
 *  Created on: May 30, 2023
 *      Author: Heinrichs
 */

#ifndef INC_DWT_DELAY_H_
#define INC_DWT_DELAY_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx_hal.h"
uint32_t DWT_Delay_Init(void);


/**
 * @brief  This function provides a delay (in microseconds)
 * @param  microseconds: delay in microseconds
 */
__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;

  /* Go to number of cycles for system */
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}


#ifdef __cplusplus
}
#endif

#endif /* INC_DWT_DELAY_H_ */
