/*
 * MIT License
 *
 * Copyright (c) 2023 Markus Heinrichs
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
