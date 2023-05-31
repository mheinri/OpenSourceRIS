#ifndef INC_DWTDELAY_H_
#define INC_DWTDELAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

// Define init function
uint8_t initDWTDelay(void);

/*
 * Function for microsecond delay
 *
 * Input: number of microseconds to wait for
 */
__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds) {
	// Get current clock cycle
	uint32_t startClkCycle = DWT->CYCCNT;

	// Calculate number of clock cycles
	microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

	// Busy wait
	while ((DWT->CYCCNT - startClkCycle) < microseconds) {

	}
}

#ifdef __cplusplus
}
#endif

#endif
