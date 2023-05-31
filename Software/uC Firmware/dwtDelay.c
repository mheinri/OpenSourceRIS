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

#include "dwtDelay.h"

/*
 * Function for initialization of DWT delay
 *
 * Result:
 * 			0: initialization successful
 * 			1: error
 */
uint8_t initDWTDelay(void) {
	// Disable trace
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
	// Enable trace
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	// Disable clock cycle counter
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
	// Enable clock cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	// Reset clock cycle counter
	DWT->CYCCNT = 0;

	// Wait three clock cycles
	__ASM
	volatile ("NOP");
	__ASM
	volatile ("NOP");
	__ASM
	volatile ("NOP");

	// Check if clock cycle counter started successfully
	if (DWT->CYCCNT) {
		return 0;
	} else {
		return 1;
	}
}
