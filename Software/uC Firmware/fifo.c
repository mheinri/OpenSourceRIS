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

#include "fifo.h"

fifo8 *newFifo8() {
	fifo8 *newFifo8 = malloc(sizeof(fifo8));

	newFifo8->maxLength = FIFO8_DEFAULT_MAXLENGTH;
	newFifo8->length = 0;
	newFifo8->first = 0;
	newFifo8->last = 0;

	return newFifo8;
}

fifo8element *newFifo8Element() {
	fifo8element *newFifo8element = malloc(sizeof(fifo8element));

	newFifo8element->data = 0;
	newFifo8element->next = 0;

	return newFifo8element;
}

void deleteFifo8(fifo8 *fifo){
	while(fifo->length > 0){
		dequeueFifo8Data(fifo);
	}

	free(fifo);
}

uint8_t enqueueFifo8(fifo8 *fifo, fifo8element *newElement) {
	uint8_t result = 1;

	if (fifo->length == 0) {
		fifo->first = newElement;
		fifo->last = newElement;
		fifo->length = 1;

		result = 0;
	} else if (fifo->length < fifo->maxLength) {
		fifo8element *lastElement = fifo->last;
		lastElement->next = newElement;
		fifo->last = newElement;
		fifo->length++;

		result = 0;
	}

	return result;
}

uint8_t enqueueFifo8Data(fifo8 *fifo, uint8_t data) {
	uint8_t result = 1;

	if (fifo->length == 0) {
		fifo8element *newElement = newFifo8Element();
		newElement->data = data;

		fifo->first = newElement;
		fifo->last = newElement;
		fifo->length = 1;

		result = 0;
	} else if (fifo->length < fifo->maxLength) {
		fifo8element *newElement = newFifo8Element();
		newElement->data = data;

		fifo8element *lastElement = fifo->last;
		lastElement->next = newElement;
		fifo->last = newElement;
		fifo->length++;

		result = 0;
	}

	return result;
}

fifo8element *dequeueFifo8(fifo8 *fifo) {
	fifo8element *result = 0;

	if (fifo->length > 0) {
		result = fifo->first;
		fifo->first = result->next;
		fifo->length--;
	}

	return result;
}

uint8_t dequeueFifo8Data(fifo8 *fifo) {
	uint8_t result = 0;

	if (fifo->length > 0) {
		fifo8element *firstElement = fifo->first;

		result = firstElement->data;
		fifo->first = firstElement->next;
		fifo->length--;

		free(firstElement);
	}

	return result;
}

/*
 * FIFO 16 bit
 */

fifo16 *newFifo16() {
	fifo16 *newFifo16 = malloc(sizeof(fifo16));

	newFifo16->maxLength = FIFO16_DEFAULT_MAXLENGTH;
	newFifo16->length = 0;
	newFifo16->first = 0;
	newFifo16->last = 0;

	return newFifo16;
}

fifo16element *newFifo16Element() {
	fifo16element *newFifo16element = malloc(sizeof(fifo16element));

	newFifo16element->data = 0;
	newFifo16element->next = 0;

	return newFifo16element;
}

void deleteFifo16(fifo16 *fifo){
	while(fifo->length > 0){
		dequeueFifo16Data(fifo);
	}

	free(fifo);
}

uint16_t enqueueFifo16(fifo16 *fifo, fifo16element *newElement) {
	uint16_t result = 1;

	if (fifo->length == 0) {
		fifo->first = newElement;
		fifo->last = newElement;
		fifo->length = 1;

		result = 0;
	} else if (fifo->length < fifo->maxLength) {
		fifo16element *lastElement = fifo->last;
		lastElement->next = newElement;
		fifo->last = newElement;
		fifo->length++;

		result = 0;
	}

	return result;
}

uint16_t enqueueFifo16Data(fifo16 *fifo, uint16_t data) {
	uint16_t result = 1;

	if (fifo->length == 0) {
		fifo16element *newElement = newFifo16Element();
		newElement->data = data;

		fifo->first = newElement;
		fifo->last = newElement;
		fifo->length = 1;

		result = 0;
	} else if (fifo->length < fifo->maxLength) {
		fifo16element *newElement = newFifo16Element();
		newElement->data = data;

		fifo16element *lastElement = fifo->last;
		lastElement->next = newElement;
		fifo->last = newElement;
		fifo->length++;

		result = 0;
	}

	return result;
}

fifo16element *dequeueFifo16(fifo16 *fifo) {
	fifo16element *result = 0;

	if (fifo->length > 0) {
		result = fifo->first;
		fifo->first = result->next;
		fifo->length--;
	}

	return result;
}

uint16_t dequeueFifo16Data(fifo16 *fifo) {
	uint16_t result = 0;

	if (fifo->length > 0) {
		fifo16element *firstElement = fifo->first;

		result = firstElement->data;
		fifo->first = firstElement->next;
		fifo->length--;

		free(firstElement);
	}

	return result;
}

