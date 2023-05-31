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
