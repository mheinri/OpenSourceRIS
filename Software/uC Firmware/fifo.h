/*
 * fifo.h
 *
 *  Created on: May 30, 2023
 *      Author: Heinrichs
 */

#ifndef INC_FIFO_H_
#define INC_FIFO_H_

#include <stdlib.h>
#include "stm32f4xx.h"

#define FIFO8_DEFAULT_MAXLENGTH		64
#define FIFO16_DEFAULT_MAXLENGTH	64

// FIFO 8 bit
typedef struct fifo8element_struct{

	uint8_t data;
	struct fifo8element_struct *next;

} fifo8element;

typedef struct fifo_8_struct{

	uint32_t maxLength;
	uint32_t length ;

	fifo8element *first;
	fifo8element *last;

} fifo8;

// FIFO 16 bit
typedef struct fifo16element_struct{

	uint16_t data;
	struct fifo16element_struct *next;

} fifo16element;

typedef struct fifo_16_struct{

	uint32_t maxLength;
	uint32_t length ;

	fifo16element *first;
	fifo16element *last;

} fifo16;

//FIFO 8 bit
extern fifo8 *newFifo8();
extern fifo8element *newFifo8Element();
extern void deleteFifo8(fifo8 *fifo);
extern uint8_t enqueueFifo8(fifo8 *fifo, fifo8element *newElement);
extern uint8_t enqueueFifo8Data(fifo8 *fifo, uint8_t data);
extern fifo8element *dequeueFifo8(fifo8 *fifo);
extern uint8_t dequeueFifo8Data(fifo8 *fifo);

// FIFO 16 bit
extern fifo16 *newFifo16();
extern fifo16element *newFifo16Element();
extern void deleteFifo16(fifo16 *fifo);
extern uint16_t enqueueFifo16(fifo16 *fifo, fifo16element *newElement);
extern uint16_t enqueueFifo16Data(fifo16 *fifo, uint16_t data);
extern fifo16element *dequeueFifo16(fifo16 *fifo);
extern uint16_t dequeueFifo16Data(fifo16 *fifo);

#endif /* INC_FIFO_H_ */
