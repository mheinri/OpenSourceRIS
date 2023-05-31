/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "dwt_delay.h"
#include "fifo.h"
#include "string.h"
#include "math.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

// UART transmit timeout in ms
#define UART_TX_TIMEOUT_MS 1000
// Number of rows
#define ROW_COUNT 16
// Number of columns
#define COL_COUNT 16
// Number of bytes per setPattern command (rowCount*columnCount/8 rounded up to the next integer if required)
#define DATA_BYTE_COUNT 32

// Static pass key of BT module
#define STATIC_PASS_KEY_BT 524953

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// Variable for calibration data of internal reference voltage
uint16_t vrefint_cal = 0;

// Initialization strings send via UART on startup
char initString0[] = "\nOpen Source RIS\n\0";
char initString1[] = "Firmware version: 1.0\n\0";
char initString2[] = "Serial no.: 5\n\0";
char initString3[] = "Row count: 16\n\0";
char initString4[] = "Column count: 16\n\0";
char initString5[] = "\n#READY!\n\0";

// Variable for one character from UART (USB interface)
uint8_t rxChar_USB = 0;
// Variable for one character from UART (BT module)
uint8_t rxChar_BT = 0;

// Flag for command-has-started indication (USB interface)
volatile uint8_t commandStartFlag_USB = 0;
// Flag for command-is-completed indication (USB interface)
volatile uint8_t commandCompleteFlag_USB = 0;
// Flag for request-has-started indication (USB interface)
volatile uint8_t requestStartFlag_USB = 0;
// Flag for request-is-completed indication (USB interface)
volatile uint8_t requestCompleteFlag_USB = 0;
// Flag for command-has-started indication (BT module)
volatile uint8_t commandStartFlag_BT = 0;
// Flag for command-is-completed indication (BT module)
volatile uint8_t commandCompleteFlag_BT = 0;
// Flag for request-has-started indication (BT module)
volatile uint8_t requestStartFlag_BT = 0;
// Flag for request-is-completed indication (BT module)
volatile uint8_t requestCompleteFlag_BT = 0;

// Pointer to a FIFO for characters received via UART (USB interface)
fifo8 *uartRxFifo_USB = 0;
// Pointer to a FIFO for characters received via UART (BT module)
fifo8 *uartRxFifo_BT = 0;

// Number of data bytes per pattern
static uint8_t dataByteCount = DATA_BYTE_COUNT;

// Pointer to a character array containing received data
uint8_t *data = NULL;
// Pointer to a uint8_t array containing column data
uint32_t *columnData = NULL;
// String containing the current pattern configuration
char *currentPattern = NULL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

static inline void transmitUart_USB(char string[]);
static inline void transmitUart_BT(char string[]);
uint8_t hexCharToU8(char c);

void resetCommandsAndFIFO_USB(void);
void resetCommandsAndFIFO_BT(void);
uint8_t areAllFlagsCleared_USB(void);
uint8_t areAllFlagsCleared_BT(void);

void shiftData256(void);
static inline void latchData(void);

static inline float readADC(void);
static inline uint8_t transmitSupplyVoltage(UART_HandleTypeDef *huart,
		float voltage);

static inline void resetBT(void);
static inline void clearUartRxBufferBT(uint16_t timeout_ms);
static inline uint32_t readStaticPassKeyBT(void);
static inline uint8_t setStaticPasskeyBT(uint32_t passKey);
static inline void resetBT_commandMode();
static inline void resetBT_peripheralOnyMode();
static inline void initBT(void);

static inline void enableLED_YE(void);
static inline void disableLED_YE(void);
static inline void enableLED_BL(void);
static inline void disableLED_BL(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	vrefint_cal = (uint16_t) *VREFINT_CAL_ADDR;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	// Enable blue LED on startup
	// Blue LED remains enabled until a fatal error occurs
	enableLED_BL();

	// Initialize delay library
	DWT_Delay_Init();

	// Initialize BT module
	initBT();
	HAL_Delay(100);

	// Allocate memory for data array
	data = (uint8_t*) malloc(dataByteCount * sizeof(uint8_t));

	// Allocate and initialize memory for column data array
	columnData = (uint32_t*) malloc(COL_COUNT * sizeof(uint32_t));
	for (uint8_t i = 0; i < COL_COUNT; i++) {
		columnData[i] = 0;
	}

	// Allocate memory for current pattern array
	currentPattern = (char*) malloc(2 * dataByteCount * sizeof(char) + 5);
	// Initialize current pattern array
	currentPattern[0] = '#';
	currentPattern[1] = '0';
	currentPattern[2] = 'X';
	uint8_t k = 0;
	for (k = 0; k < 2 * dataByteCount; k++) {
		currentPattern[k + 3] = '0';
	}
	currentPattern[2 * dataByteCount + 3] = '\n';
	currentPattern[2 * dataByteCount + 4] = '\0';

	// Reset shift registers
	shiftData256();
	// Latch data in shift registers
	latchData();

	// Allocate memory for new-pattern string
	char *newPattern = (char*) malloc(2 * dataByteCount * sizeof(char));

	// Create empty FIFO
	uartRxFifo_USB = newFifo8();
	// Set maximum length to 128
	uartRxFifo_USB->maxLength = 128;

	// Create empty FIFO
	uartRxFifo_BT = newFifo8();
	// Set maximum length to 128
	uartRxFifo_BT->maxLength = 128;

	// Wait for 100 ms
	HAL_Delay(100);

	// Enable yellow LED
	enableLED_YE();

	// Transmit initialization strings via UART
	transmitUart_USB(initString0);
	transmitUart_USB(initString1);
	transmitUart_USB(initString2);
	transmitUart_USB(initString3);
	transmitUart_USB(initString4);
	transmitUart_USB(initString5);

	// Enable UART1 (USB interface) receive interrupt
	// This will save the received character into "rxChar" variable and
	// will call the RX-complete callback function afterwards
	HAL_UART_Receive_IT(&huart1, &rxChar_USB, 1);

	// Enable UART2 (BT module) receive interrupt
	// This will save the received character into "rxCharBT" variable and
	// will call the RX-complete callback function afterwards
	HAL_UART_Receive_IT(&huart2, &rxChar_BT, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		// If a complete command has been received via UART (USB interface)
		if (commandCompleteFlag_USB) {
			// If the number of received characters is correct (pattern command)
			if (uartRxFifo_USB->length >= dataByteCount + 3) {
				// Dequeue the first two characters
				volatile uint8_t char1 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char2 = dequeueFifo8Data(uartRxFifo_USB);

				// If first two characters are correct
				if (char1 == '0' && char2 == 'X') {
					// Initialize index variable
					uint8_t i = 0;
					// Error flag
					uint8_t error = 0;
					// Initialize temporary character variables
					char c1 = 0;
					char c2 = 0;

					// Convert data bytes, save them into data array and update new-pattern string
					for (i = 0; i < dataByteCount && !error; i++) {
						// Dequeue two hex-characters (one byte)
						c1 = dequeueFifo8Data(uartRxFifo_USB);
						c2 = dequeueFifo8Data(uartRxFifo_USB);

						// If c1 and c2 are valid hey digits
						if (((c1 >= '0' && c1 <= '9')
								|| (c1 >= 'A' && c1 <= 'F'))
								&& ((c2 >= '0' && c2 <= '9')
										|| (c2 >= 'A' && c2 <= 'F'))) {
							// Calculate data bytes
							data[i] = (16 * hexCharToU8(c1) + hexCharToU8(c2));
							// Reverse bit order
							data[i] = (uint8_t) (__RBIT((uint32_t) data[i])
									>> 24);

							// Save characters into new-pattern string
							newPattern[2 * i] = c1;
							newPattern[2 * i + 1] = c2;
						} else {
							error = 1;
#ifdef DEBUG_META
	  							// Send debug message via UART
	  							char string[] = "ERROR in main (USB):\n\tInvalid hex character in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  							transmitUart_USB(string);
	  							transmitUart_BT(string);
	  #endif
						}
					}

					// If command is ended with a NL character
					if (dequeueFifo8Data(uartRxFifo_USB) == '\n' && !error) {

						uint8_t dataArrayIndex = 0;
						uint8_t dataBitIndex = 0;

						// Reset column data
						for (k = 0; k < COL_COUNT; k++) {
							columnData[k] = 0x00000000;
						}

						// For all rows
						for (k = 0; k < ROW_COUNT; k++) {
							// For all columns
							for (i = 0; i < COL_COUNT; i++) {

								columnData[i] |= ((data[dataArrayIndex]
										& (1 << dataBitIndex)) >> dataBitIndex);

								if (k < 11) {
									columnData[i] = columnData[i] << 1;
								}

								dataBitIndex++;
								if (dataBitIndex > 7) {
									dataBitIndex = 0;
									dataArrayIndex++;
								}

							}
						}

						// Copy new pattern into current pattern array
						for (i = 0; i < 2 * dataByteCount; i++) {
							currentPattern[i + 3] = newPattern[i];
						}

						// Shift data into shift registers
						shiftData256();
						// Latch shift register outputs
						latchData();

						// Send "#OK" message via UART
						char stringOK[] = "#OK\n\0";
						transmitUart_USB(stringOK);
#ifdef DEBUG_META
	  						// Send debug message via UART
	  						char string[] = "Data shifted, output latched, flags are cleared\n\0";
	  						transmitUart_USB(string);
	  						transmitUart_BT(string);
	  #endif
						// Reset flag for command-is-complete indication
						commandCompleteFlag_USB = 0;
						// Reset flag for command-has-started indication
						commandStartFlag_USB = 0;
					}
					// If command has not been ended with a NL character
					else {
						// Reset flags and empty FIFO
						resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
	  						// Send debug message via UART
	  						char string[] = "ERROR in main (USB):\n\tInvalid command in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  						transmitUart_USB(string);
	  						transmitUart_BT(string);
	  #endif
					}
				}
				// If first two characters are incorrect (do not match '0x')
				else {
					// Reset flags and empty FIFO
					resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] = "ERROR in main (USB):\n\tInvalid command in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}

			}
			// If the number of received characters is correct (BT-Key command)
			else if (uartRxFifo_USB->length == 14) {
				// Dequeue the complete command
				volatile uint8_t command[14] = { 0 };
				for (uint8_t i = 0; i < 14; i++) {
					command[i] = dequeueFifo8Data(uartRxFifo_USB);
				}

				// If the command header is correct
				if (command[0] == 'B' && command[1] == 'T' && command[2] == '-'
						&& command[3] == 'K' && command[4] == 'E'
						&& command[5] == 'Y' && command[6] == '='
						&& command[13] == '\n') {
					// If new static pass key consists of numbers
					if (command[7] >= '0' && command[7] <= '9'
							&& command[8] >= '0' && command[8] <= '9'
							&& command[9] >= '0' && command[9] <= '9'
							&& command[10] >= '0' && command[10] <= '9'
							&& command[11] >= '0' && command[11] <= '9'
							&& command[12] >= '0' && command[12] <= '9') {

						uint32_t newPassKey = 0;
						newPassKey = command[12] - '0';
						newPassKey += (command[11] - '0') * 10;
						newPassKey += (command[10] - '0') * 100;
						newPassKey += (command[9] - '0') * 1000;
						newPassKey += (command[8] - '0') * 10000;
						newPassKey += (command[7] - '0') * 100000;

						// Set new static pass key
						uint8_t result = setStaticPasskeyBT(newPassKey);

						if (result == 0) {
							// Send "#OK" message via UART
							char string[] = "#OK\n\0";
							transmitUart_USB(string);
#ifdef DEBUG_META
	  							// Send debug message via UART
	  							char string2[] = "Static pass key set successfully\n\0";
	  							transmitUart_USB(string2);
	  							transmitUart_BT(string2);
	  #endif
						} else {
							// Send "#ERROR" message via UART
							char string[] = "#ERROR\n\0";
							transmitUart_USB(string);
#ifdef DEBUG_META
	  							// Send debug message via UART
	  							char string2[] =
	  									"Static pass key not set due to error\n\tReturn code: 0x__\n\0";
	  							if ((result & 0xf0) >> 4 > 9) {
	  								string2[53] = ((result & 0xf0) >> 4) - 10 + 'a';
	  							} else {
	  								string2[53] = ((result & 0xf0) >> 4) + '0';
	  							}
	  							if ((result & 0x0f) > 9){
	  								string2[54] = (result & 0x0f) - 10 + 'a';
	  							}else{
	  								string2[54] = (result & 0x0f) + '0';
	  							}
	  							transmitUart_USB(string2);
	  							transmitUart_BT(string2);
	  #endif
						}

						// Reset flag for command-is-complete indication
						commandCompleteFlag_USB = 0;
						// Reset flag for command-has-started indication
						commandStartFlag_USB = 0;

					} else {
						// Reset flags and empty FIFO
						resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
	  						// Send debug message via UART
	  						char string[] =
	  								"ERROR in main (USB):\n\tInvalid static pass key - only numbers are allowed\n\tFIFO is now empty, flags are cleared\n\0";
	  						transmitUart_USB(string);
	  						transmitUart_BT(string);
	  #endif
					}

				} else {
					// Reset flags and empty FIFO
					resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] = "ERROR in main (USB):\n\tInvalid command in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}
			}
			// If number of received characters is incorrect
			else {
				// Reset flags and empty FIFO
				resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
	  				// Send debug message via UART
	  				char string[] =
	  						"ERROR in main (USB):\n\tIncomplete command in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  				transmitUart_USB(string);
	  				transmitUart_BT(string);
	  #endif
			}
		} // END if (commandCompleteFlag)
		else if (requestCompleteFlag_USB) {
			// If the number of received characters is correct (5 for "Vext"+NL)
			if (uartRxFifo_USB->length == 5) {
				// Dequeue the first two characters
				volatile uint8_t char1 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char2 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char3 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char4 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char5 = dequeueFifo8Data(uartRxFifo_USB);

				// If request is correct
				if (char1 == 'V' && char2 == 'E' && char3 == 'X' && char4 == 'T'
						&& char5 == '\n') {
					// Sample external supply voltage
					float batteryVoltage = readADC();
					// Transmit result over UART (USB)
					transmitSupplyVoltage(&huart1, batteryVoltage);

					// Reset flag for request-is-complete indication
					requestCompleteFlag_USB = 0;
					// Reset flag for request-has-started indication
					requestStartFlag_USB = 0;
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string2[] =
	  							"Supply voltage sampled, flags are cleared\n\0";
	  					transmitUart_USB(string2);
	  					transmitUart_BT(string2);
	  #endif
				}
				// If request is incorrect
				else {
					// Reset flags and empty FIFO
					resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] =
	  							"ERROR in main (USB):\n\tInvalid request in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}

			}
			// If the number of received characters is correct (8 for "Pattern"+NL)
			else if (uartRxFifo_USB->length == 8) {
				// Dequeue the first two characters
				volatile uint8_t char1 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char2 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char3 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char4 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char5 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char6 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char7 = dequeueFifo8Data(uartRxFifo_USB);
				volatile uint8_t char8 = dequeueFifo8Data(uartRxFifo_USB);

				// If request is correct
				if (char1 == 'P' && char2 == 'A' && char3 == 'T' && char4 == 'T'
						&& char5 == 'E' && char6 == 'R' && char7 == 'N'
						&& char8 == '\n') {
					// Send current pattern configuration via UART
					transmitUart_USB(currentPattern);

					// Reset flag for request-is-complete indication
					requestCompleteFlag_USB = 0;
					// Reset flag for request-has-started indication
					requestStartFlag_USB = 0;
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] =
	  							"Current pattern transmitted, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}
				// If request is incorrect
				else {
					// Reset flags and empty FIFO
					resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] =
	  							"ERROR in main (USB):\n\tInvalid request in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}
			}
			// If number of received characters is incorrect
			else {
				// Reset flags and empty FIFO
				resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
	  				// Send debug message via UART
	  				char string[] =
	  						"ERROR in main (USB):\n\tInvalid request in FIFO - request is of incorrect length\n\tFIFO is now empty, flags are cleared\n\0";
	  				transmitUart_USB(string);
	  				transmitUart_BT(string);
	  #endif
			}
		} // END if (requestCompleteFlag_USB)

		// If a complete command has been received via UART (BT module)
		if (commandCompleteFlag_BT) {
			// If the number of received characters is correct
			if (uartRxFifo_BT->length >= dataByteCount + 3) {
				// Dequeue the first two characters
				volatile uint8_t char1 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char2 = dequeueFifo8Data(uartRxFifo_BT);

				// If first two characters are correct
				if (char1 == '0' && (char2 == 'X' || char2 == 'x')) {
					// Initialize index variable
					uint8_t i = 0;
					// Initialize temporary character variables
					char c1 = 0;
					char c2 = 0;

					// Convert data bytes, save them into data array and update new-pattern string
					for (i = 0; i < dataByteCount; i++) {
						// Dequeue two hex-characters (one byte)
						c1 = dequeueFifo8Data(uartRxFifo_BT);
						c2 = dequeueFifo8Data(uartRxFifo_BT);

						// Calculate data bytes
						data[i] = (16 * hexCharToU8(c1) + hexCharToU8(c2));
						// Reverse bit order
						data[i] = (uint8_t) (__RBIT((uint32_t) data[i]) >> 24);

						// Save characters into new-pattern string
						newPattern[2 * i] = c1;
						newPattern[2 * i + 1] = c2;
					}

					// If command is ended with a NL character
					if (dequeueFifo8Data(uartRxFifo_BT) == '\n') {

						uint8_t dataArrayIndex = 0;
						uint8_t dataBitIndex = 0;

						// Reset column data
						for (k = 0; k < COL_COUNT; k++) {
							columnData[k] = 0x00000000;
						}

						// For all rows
						for (k = 0; k < ROW_COUNT; k++) {
							// For all columns
							for (i = 0; i < COL_COUNT; i++) {

								columnData[i] |= ((data[dataArrayIndex]
										& (1 << dataBitIndex)) >> dataBitIndex);

								if (k < 11) {
									columnData[i] = columnData[i] << 1;
								}

								dataBitIndex++;
								if (dataBitIndex > 7) {
									dataBitIndex = 0;
									dataArrayIndex++;
								}

							}
						}

						// Copy new pattern into current pattern array
						for (i = 0; i < 2 * dataByteCount; i++) {
							currentPattern[i + 3] = newPattern[i];
						}

						// Shift data into shift registers
						shiftData256();
						// Latch shift register outputs
						latchData();

						// Send "#OK" message via UART
						char stringOK[] = "#OK\n\0";
						transmitUart_BT(stringOK);
#ifdef DEBUG_META
	  						// Send debug message via UART
	  						char string[] =
	  								"Data shifted, output latched, flags are cleared\n\0";
	  						transmitUart_USB(string);
	  						transmitUart_BT(string);
	  #endif
						// Reset flag for command-is-complete indication
						commandCompleteFlag_BT = 0;
						// Reset flag for command-has-started indication
						commandStartFlag_BT = 0;
					}
					// If command has not been ended with a NL character
					else {
						// Reset flags and empty FIFO
						resetCommandsAndFIFO_BT();
#ifdef DEBUG_META
	  						// Send debug message via UART
	  						char string[] =
	  								"ERROR in main (BT):\n\tInvalid command in FIFO - command is too long\n\tFIFO is now empty, flags are cleared\n\0";
	  						transmitUart_USB(string);
	  						transmitUart_BT(string);
	  #endif
					}
				}
				// If first two characters are incorrect (do not match '0x')
				else {
					// Reset flags and empty FIFO
					resetCommandsAndFIFO_BT();
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] =
	  							"ERROR in main (BT):\n\tInvalid command in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}

			}
			// If number of received characters is incorrect
			else {
				// Reset flags and empty FIFO
				resetCommandsAndFIFO_BT();
#ifdef DEBUG_META
	  				// Send debug message via UART
	  				char string[] =
	  						"ERROR in main (BT):\n\tIncomplete command in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  				transmitUart_USB(string);
	  				transmitUart_BT(string);
	  #endif
			}
		} // END if (commandCompleteFlag)
		else if (requestCompleteFlag_BT) {
			// If the number of received characters is correct (5 for "Vext"+NL)
			if (uartRxFifo_BT->length == 5) {
				// Dequeue the first two characters
				volatile uint8_t char1 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char2 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char3 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char4 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char5 = dequeueFifo8Data(uartRxFifo_BT);

				// If request is correct
				if (char1 == 'V' && char2 == 'E' && char3 == 'X' && char4 == 'T'
						&& char5 == '\n') {
					// Sample external supply voltage
					float batteryVoltage = readADC();
					// Transmit result over UART (BT)
					transmitSupplyVoltage(&huart2, batteryVoltage);

					// Reset flag for request-is-complete indication
					requestCompleteFlag_BT = 0;
					// Reset flag for request-has-started indication
					requestStartFlag_BT = 0;
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string2[] =
	  							"Supply voltage sampled, flags are cleared\n\0";
	  					transmitUart_USB(string2);
	  					transmitUart_BT(string2);
	  #endif
				}
				// If request is incorrect
				else {
					// Reset flags and empty FIFO
					resetCommandsAndFIFO_BT();
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] =
	  							"ERROR in main (BT):\n\tInvalid request in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}

			}
			// If the number of received characters is correct (8 for "Pattern"+NL)
			else if (uartRxFifo_BT->length == 8) {
				// Dequeue the first two characters
				volatile uint8_t char1 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char2 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char3 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char4 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char5 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char6 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char7 = dequeueFifo8Data(uartRxFifo_BT);
				volatile uint8_t char8 = dequeueFifo8Data(uartRxFifo_BT);

				// If request is correct
				if (char1 == 'P' && char2 == 'A' && char3 == 'T' && char4 == 'T'
						&& char5 == 'E' && char6 == 'R' && char7 == 'N'
						&& char8 == '\n') {
					// Send current pattern configuration via UART
					transmitUart_BT(currentPattern);

					// Reset flag for request-is-complete indication
					requestCompleteFlag_BT = 0;
					// Reset flag for request-has-started indication
					requestStartFlag_BT = 0;
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] =
	  							"Current pattern transmitted, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}
				// If request is incorrect
				else {					// Reset flags and empty FIFO
					resetCommandsAndFIFO_BT();
#ifdef DEBUG_META
	  					// Send debug message via UART
	  					char string[] =
	  							"ERROR in main (BT):\n\tInvalid request in FIFO\n\tFIFO is now empty, flags are cleared\n\0";
	  					transmitUart_USB(string);
	  					transmitUart_BT(string);
	  #endif
				}
			}
			// If number of received characters is incorrect
			else {
				// Reset flags and empty FIFO
				resetCommandsAndFIFO_BT();
#ifdef DEBUG_META
	  				// Send debug message via UART
	  				char string[] =
	  						"ERROR in main (BT):\n\tInvalid request in FIFO - request is of incorrect length\n\tFIFO is now empty, flags are cleared\n\0";
	  				transmitUart_USB(string);
	  				transmitUart_BT(string);
	  #endif
			}
		} // END if (requestCompleteFlag_USB)

//		while (uartRxFifo_BT->length > 0) {
//			// Dequeue the first two characters
//			volatile uint8_t aChar = dequeueFifo8Data(uartRxFifo_BT);
//			char stringBT[] = "BT: _\n\0";
//			stringBT[4] = aChar;
//			transmitUart_USB(stringBT);
//		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, S8SCK_Pin|S8LCK_Pin|S8DATA1_Pin|S8DATA2_Pin
                          |S8DATA3_Pin|S8DATA4_Pin|S8DATA5_Pin|S8DATA6_Pin
                          |S8DATA7_Pin|S8DATA8_Pin|S8DATA9_Pin|S8DATA10_Pin
                          |S8DATA11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BT_BOOT_Pin|BT_WAKEUP_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BT_MODE_Pin|BT_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, S8DATA12_Pin|S8DATA13_Pin|LED_BL_Pin|LED_YE_Pin
                          |S8DATA14_Pin|S8DATA15_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(S8DATA16_GPIO_Port, S8DATA16_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : S8SCK_Pin S8LCK_Pin S8DATA1_Pin S8DATA2_Pin
                           S8DATA3_Pin S8DATA4_Pin S8DATA5_Pin S8DATA6_Pin
                           S8DATA7_Pin S8DATA8_Pin S8DATA9_Pin S8DATA10_Pin
                           S8DATA11_Pin */
  GPIO_InitStruct.Pin = S8SCK_Pin|S8LCK_Pin|S8DATA1_Pin|S8DATA2_Pin
                          |S8DATA3_Pin|S8DATA4_Pin|S8DATA5_Pin|S8DATA6_Pin
                          |S8DATA7_Pin|S8DATA8_Pin|S8DATA9_Pin|S8DATA10_Pin
                          |S8DATA11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BT_BOOT_Pin BT_WAKEUP_Pin BT_MODE_Pin BT_RESET_Pin */
  GPIO_InitStruct.Pin = BT_BOOT_Pin|BT_WAKEUP_Pin|BT_MODE_Pin|BT_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : S8DATA12_Pin S8DATA13_Pin LED_BL_Pin LED_YE_Pin
                           S8DATA14_Pin S8DATA15_Pin S8DATA16_Pin */
  GPIO_InitStruct.Pin = S8DATA12_Pin|S8DATA13_Pin|LED_BL_Pin|LED_YE_Pin
                          |S8DATA14_Pin|S8DATA15_Pin|S8DATA16_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BT_BUSY_Pin */
  GPIO_InitStruct.Pin = BT_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BT_BUSY_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/*
 * Function to transmit a string via UART (USB interface)
 */
static inline void transmitUart_USB(char string[]) {
	// Transmit string via UART and set timeout
	HAL_UART_Transmit(&huart1, (uint8_t*) string, strlen(string),
	UART_TX_TIMEOUT_MS);
}

/*
 * Function to transmit a string via UART (BT module)
 */
static inline void transmitUart_BT(char string[]) {
	HAL_UART_Transmit(&huart2, (uint8_t*) string, strlen(string),
	UART_TX_TIMEOUT_MS);
}

/*
 * Function to convert an ASCII coded hex-character (e.g. 'a') into an 8-bit unsigned integer value
 *
 * Returns 0 if c is not a valid hex value
 */
uint8_t hexCharToU8(char c) {
	// Initialize return value
	uint8_t returnValue = 0;

	// Convert letters to upper case
	if ((c >= 'a' && c <= 'f') || c == 'x') {
		c -= 32;
	}

	// If character contains a decimal number
	if (c >= '0' && c <= '9') {
		// Convert to integer value
		returnValue = (uint8_t) c - '0';
	}
	// If character contains a hex letter
	else if (c >= 'A' && c <= 'F') {
		// Convert to integer
		returnValue = (uint8_t) c - 'A' + 10;
	}

	// Return calculated integer value
	return returnValue;
}

/*
 * Function to reset the status flags and empty the FIFO (USB interface)
 */
void resetCommandsAndFIFO_USB(void) {
	// Dequeue all FIFO elements
	while (uartRxFifo_USB->length > 0) {
		dequeueFifo8Data(uartRxFifo_USB);
	}

	// Reset flags
	commandCompleteFlag_USB = 0;
	commandStartFlag_USB = 0;
	requestCompleteFlag_USB = 0;
	requestStartFlag_USB = 0;
}

/*
 * Function to reset the status flags and empty the FIFO (BT module)
 */
void resetCommandsAndFIFO_BT(void) {
	// Dequeue all FIFO elements
	while (uartRxFifo_BT->length > 0) {
		dequeueFifo8Data(uartRxFifo_BT);
	}

	// Reset flags
	commandCompleteFlag_BT = 0;
	commandStartFlag_BT = 0;
	requestCompleteFlag_BT = 0;
	requestStartFlag_BT = 0;
}

/*
 * Function to check for flag status (USB interface)
 *
 * Result:
 * 		0: One or more flags are set
 * 		1: All flags are cleared
 */
uint8_t areAllFlagsCleared_USB(void){

	if (commandCompleteFlag_USB | commandStartFlag_USB | requestCompleteFlag_USB
			| requestStartFlag_USB) {
		return ((uint8_t) 0x00);
	} else {
		return ((uint8_t) 0x01);
	}

}

/*
 * Function to check for flag status (BT module)
 *
 * Result:
 * 		0: One or more flags are set
 * 		1: All flags are cleared
 */
uint8_t areAllFlagsCleared_BT(void){

	if (commandCompleteFlag_BT | commandStartFlag_BT | requestCompleteFlag_BT
			| requestStartFlag_BT) {
		return ((uint8_t) 0x00);
	} else {
		return ((uint8_t) 0x01);
	}

}

/*
 * Function to shift 256 bits of data into shift registers
 *
 * Data from array "columnData" is used
 */
void shiftData256(void) {
	// Initialize index variables
	uint8_t i = 0;
	uint8_t bitIndex = ROW_COUNT-1;

	// For all bits
	for (i = 0; i < ROW_COUNT; i++) {

		if(columnData[0] & (1 << bitIndex)){
			HAL_GPIO_WritePin(S8DATA1_GPIO_Port, S8DATA1_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA1_GPIO_Port, S8DATA1_Pin, GPIO_PIN_RESET);
		}
		if (columnData[1] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA2_GPIO_Port, S8DATA2_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA2_GPIO_Port, S8DATA2_Pin, GPIO_PIN_RESET);
		}
		if (columnData[2] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA3_GPIO_Port, S8DATA3_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA3_GPIO_Port, S8DATA3_Pin, GPIO_PIN_RESET);
		}
		if (columnData[3] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA4_GPIO_Port, S8DATA4_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA4_GPIO_Port, S8DATA4_Pin, GPIO_PIN_RESET);
		}
		if (columnData[4] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA5_GPIO_Port, S8DATA5_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA5_GPIO_Port, S8DATA5_Pin, GPIO_PIN_RESET);
		}
		if (columnData[5] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA6_GPIO_Port, S8DATA6_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA6_GPIO_Port, S8DATA6_Pin, GPIO_PIN_RESET);
		}
		if (columnData[6] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA7_GPIO_Port, S8DATA7_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA7_GPIO_Port, S8DATA7_Pin, GPIO_PIN_RESET);
		}
		if (columnData[7] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA8_GPIO_Port, S8DATA8_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA8_GPIO_Port, S8DATA8_Pin, GPIO_PIN_RESET);
		}
		if (columnData[8] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA9_GPIO_Port, S8DATA9_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA9_GPIO_Port, S8DATA9_Pin, GPIO_PIN_RESET);
		}
		if (columnData[9] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA10_GPIO_Port, S8DATA10_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA10_GPIO_Port, S8DATA10_Pin, GPIO_PIN_RESET);
		}
		if (columnData[10] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA11_GPIO_Port, S8DATA11_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA11_GPIO_Port, S8DATA11_Pin, GPIO_PIN_RESET);
		}
		if (columnData[11] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA12_GPIO_Port, S8DATA12_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA2_GPIO_Port, S8DATA12_Pin, GPIO_PIN_RESET);
		}
		if (columnData[12] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA13_GPIO_Port, S8DATA13_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA13_GPIO_Port, S8DATA13_Pin, GPIO_PIN_RESET);
		}
		if (columnData[13] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA14_GPIO_Port, S8DATA14_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA14_GPIO_Port, S8DATA14_Pin, GPIO_PIN_RESET);
		}
		if (columnData[14] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA15_GPIO_Port, S8DATA15_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA15_GPIO_Port, S8DATA15_Pin, GPIO_PIN_RESET);
		}
		if (columnData[15] & (1 << bitIndex)) {
			HAL_GPIO_WritePin(S8DATA16_GPIO_Port, S8DATA16_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(S8DATA16_GPIO_Port, S8DATA16_Pin, GPIO_PIN_RESET);
		}

		// Generate one clock cycle
		DWT_Delay_us(1);
		HAL_GPIO_WritePin(S8SCK_GPIO_Port, S8SCK_Pin, GPIO_PIN_SET);
		DWT_Delay_us(2);
		HAL_GPIO_WritePin(S8SCK_GPIO_Port, S8SCK_Pin, GPIO_PIN_RESET);
		DWT_Delay_us(1);

		// Decrease bit index
		bitIndex--;
	}

}

/*
 * Function to latch the shift registers outputs
 */
static inline void latchData(void) {
	HAL_GPIO_WritePin(S8LCK_GPIO_Port, S8LCK_Pin, GPIO_PIN_RESET);
	DWT_Delay_us(1);
	HAL_GPIO_WritePin(S8LCK_GPIO_Port, S8LCK_Pin, GPIO_PIN_SET);
	DWT_Delay_us(1);
}

/*
 * Function to read the ADC value
 *
 * Result: External supply voltage
 */
static inline float readADC(void){

	float supplyVoltage = -1.0f;

	uint64_t referenceADCValue = 0;
	uint64_t supplyVoltageADCValue = 0;

	ADC_ChannelConfTypeDef sConfig = { 0 };

	// Configure reference channel
	sConfig.Channel = ADC_CHANNEL_VREFINT;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	// Start ADC
	HAL_ADC_Start(&hadc1);
	// Poll for conversion, timeout 1 ms
	HAL_ADC_PollForConversion(&hadc1, 1);
	// Read ADC value
	referenceADCValue = (uint64_t) HAL_ADC_GetValue(&hadc1);
	// Stop ADC
	HAL_ADC_Stop(&hadc1);

	// Configure input channel
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	// Start ADC
	HAL_ADC_Start(&hadc1);
	// Poll for conversion, timeout 1 ms
	HAL_ADC_PollForConversion(&hadc1, 1);
	// Read ADC value
	supplyVoltageADCValue = (uint64_t) HAL_ADC_GetValue(&hadc1);
	// Stop ADC
	HAL_ADC_Stop(&hadc1);

	supplyVoltage = (float) (8.0f * 3.3f * ((float) (supplyVoltageADCValue)) * ((float) vrefint_cal) / (referenceADCValue * 4095.0f));

	return supplyVoltage;
}

/*
 * Function to transmit the external supply voltage over UART (USB)
 *
 * Input: battery voltage (float)
 * Output: error (uint8_t)
 * 		0 = no error
 * 		1 = input variable is out of range (either negative or greater than 99.99 V)
 */
static inline uint8_t transmitSupplyVoltage(UART_HandleTypeDef *huart, float voltage){

	uint8_t error = 0;

	if(voltage < 0 || voltage > 99.99f){
		error = 1;
#ifdef DEBUG_META
		char resultString[] = "ERROR in transmitSupplyVoltage:\n\tInvalid supply voltage\n\tVoltage variable is either negative or greater than 99.99 V\n\0";
		transmitUart_USB(resultString);
		transmitUart_BT(resultString);
//		if (huart == &huart1) {
//			transmitUart_USB(resultString);
//		} else {
//			transmitUart_BT(resultString);
//		}
#endif
	} else {
		char resultString[] = "#00.00 V\n\0";
		resultString[1] = '0' + (uint8_t) (((uint16_t) floorf(voltage / 10.0f)) % 10);
		resultString[2] = '0' + (uint8_t) (((uint16_t) floorf(voltage)) % 10);
		resultString[4] = '0' + (uint8_t) (((uint16_t) floorf(voltage * 10.0f)) % 10);
		resultString[5] = '0' + (uint8_t) (((uint16_t) roundf(voltage * 100.0f)) % 10);
		if (huart == &huart1) {
			transmitUart_USB(resultString);
		} else {
			transmitUart_BT(resultString);
		}
	}

	return error;

}

/*
 * Function for resetting the BT module
 */
static inline void resetBT(void){
	HAL_GPIO_WritePin(BT_RESET_GPIO_Port, BT_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(BT_RESET_GPIO_Port, BT_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}

/*
 * Function for clearing the BT UART receive buffer
 */
static inline void clearUartRxBufferBT(uint16_t timeout_ms){
	uint8_t rxBuffer[64] = {0};
	HAL_UART_Receive(&huart2, rxBuffer, 64, timeout_ms);
}

/*
 * Function for reading the currently configured static pass key of the BT module
 *
 * Result:
 * 		no error: static pass key (range 0 to 999999)
 * 		   error: 0xffffffff
 */
static inline uint32_t readStaticPassKeyBT(void){

	uint32_t currentPassKey = 0xffffffff;

	uint8_t rxBuffer[64] = {0};

	// Assemble command array:
	// Start signal 0x02
	// Command CMD_GET_CNF 0x10
	// Length 0x01 0x00
	// Settings index for pass key 0x12
	// CHecksum 0x01
	uint8_t commandArray[6] = { 0x02, 0x10, 0x01, 0x00, 0x12, 0x01 };

	// Clear UART RX buffer
	clearUartRxBufferBT(200);

	// Send command
	HAL_UART_Transmit(&huart2, commandArray, 6, UART_TX_TIMEOUT_MS);

	// Receive response
	HAL_UART_Receive(&huart2, rxBuffer, 32, UART_TX_TIMEOUT_MS);

	// If response header is correct and status byte shows no error
	if(rxBuffer[0] == 0x02 && rxBuffer[1] == 0x50 && rxBuffer[2] == 0x07 && rxBuffer[3] == 0x00){
		// If status indicates no error
		if(rxBuffer[4] == 0x00){
			// Calculate checksum of received data
			uint8_t rxChecksum = rxBuffer[0]^rxBuffer[1]^rxBuffer[2]^rxBuffer[3]^rxBuffer[4]^rxBuffer[5]^rxBuffer[6]^rxBuffer[7]^rxBuffer[8]^rxBuffer[9]^rxBuffer[10];
			// If checksum is correct
			if(rxBuffer[11] == rxChecksum){
				currentPassKey = hexCharToU8(rxBuffer[10]);
				currentPassKey = currentPassKey + (hexCharToU8(rxBuffer[9]) * 10);
				currentPassKey = currentPassKey + (hexCharToU8(rxBuffer[8]) * 100);
				currentPassKey = currentPassKey + (hexCharToU8(rxBuffer[7]) * 1000);
				currentPassKey = currentPassKey + (hexCharToU8(rxBuffer[6]) * 10000);
				currentPassKey = currentPassKey + (hexCharToU8(rxBuffer[5]) * 100000);
			}
			// If checksum is incorrect
			else{
#ifdef DEBUG_META
				char string[] = "ERROR - Could not read BT modules static pass key.\n\tChecksum is incorrect.\n\0";
				transmitUart_USB(string);
#endif
			}

		} // If status indicates an error
		else{
#ifdef DEBUG_META
			char string[] = "ERROR - Could not read BT modules static pass key.\n\tStatus indicates an error.\n\0";
			transmitUart_USB(string);
#endif
		}
	}
	// If response header is incorrect
	else{
#ifdef DEBUG_META
		char string[] = "ERROR - Could not read BT modules static pass key.\n\tResponse header is incorrect.\n\0";
		transmitUart_USB(string);
#endif
	}

	return currentPassKey;
}

/*
 * Function for setting the BT module's static pass key
 *
 * passKey must be a number with 6 digits, thus ranging from 0 to 999999
 *
 * Current pass key is read first from BT module. If new pass key differs, the new key is written to the module.
 * This is to prevent unnecessary write cycles of the BT flash.
 *
 * Result:
 *		0x00: Static pass key set successfully
 *		0x01: Invalid parameter
 *		0x04: Fatal error - Factory reset BT module or re-flash the device
 *		0x05: Supply voltage too low
 *		0xff: Operation not permitted
 *
 *		0xf0: pass key out of range
 *		0xf1: no response from BT module
 *		0xf2: incorrect response header
 *		0xf3: incorrect checksum
 *		0xf4: unknown status
 */
static inline uint8_t setStaticPasskeyBT(uint32_t passKey) {

	uint8_t status = 0x00;
	uint8_t rxBuffer[32] = {0};

	// IF pass key is out of range
	if (passKey > 999999){
		status = 0x99;
	}
	// If pass key is in range
	else {

		// Disable UART interrupt for BT module
		CLEAR_BIT(huart2.Instance->CR1, USART_CR1_RXNEIE);
		huart2.RxState = HAL_UART_STATE_READY;

		// Reset BT module into command mode
		resetBT_commandMode();
		clearUartRxBufferBT(200);

		// Read the currently configured static pass key
		uint32_t currentPassKey = readStaticPassKeyBT();

		// If pass key is new
		if (currentPassKey != passKey) {
#ifdef DEBUG_META
			char string[] = "Setting new pass key.\n\0";
			transmitUart_USB(string);
			transmitUart_BT(string);
#endif

			// Start signal (constant value 0x02)
			uint8_t startSignal = 0x02;
			// CMD_SET_REQ command
			uint8_t command = 0x11;
			// Length bytes, constant value of 0x0007
			uint8_t length1 = 0x00;
			uint8_t length0 = 0x07;
			// Settings index (0x12 for RF_StaticPasskey)
			uint8_t settingsIndex = 0x12;

			// Pass key bytes
			uint8_t keyByte0 = '0' + ((uint8_t) ((passKey / 100000) % 10));
			uint8_t keyByte1 = '0' + ((uint8_t) ((passKey / 10000) % 10));
			uint8_t keyByte2 = '0' + ((uint8_t) ((passKey / 1000) % 10));
			uint8_t keyByte3 = '0' + ((uint8_t) ((passKey / 100) % 10));
			uint8_t keyByte4 = '0' + ((uint8_t) ((passKey / 10) % 10));
			uint8_t keyByte5 = '0' + ((uint8_t) ((passKey) % 10));

			// Checksum (XOR of all preceding bytes)
			uint8_t checksum = startSignal ^ command ^ length0 ^ length1
					^ settingsIndex ^ keyByte0 ^ keyByte1 ^ keyByte2 ^ keyByte3
					^ keyByte4 ^ keyByte5;

			uint8_t commandArray[12] = { startSignal, command, length0, length1,
					settingsIndex, keyByte0, keyByte1, keyByte2, keyByte3,
					keyByte4, keyByte5, checksum };

			// Clear UART RX buffer
			clearUartRxBufferBT(200);

			// Send command
			HAL_UART_Transmit(&huart2, commandArray, 12, UART_TX_TIMEOUT_MS);
			// Receive response
			HAL_UART_Receive(&huart2, rxBuffer, 32, UART_TX_TIMEOUT_MS);

			if (rxBuffer[0] == 0x00) {
				status = 0xf1;
			} else {
				// If response header is valid (0x02 0x51 0x01 0x00)
				if (rxBuffer[0] == 0x02 && rxBuffer[1] == 0x51
						&& rxBuffer[2] == 0x01 && rxBuffer[3] == 0x00) {
					uint8_t rxChecksum = rxBuffer[0] ^ rxBuffer[1] ^ rxBuffer[2]
							^ rxBuffer[3] ^ rxBuffer[4];
					// If checksum is correct
					if (rxBuffer[5] == rxChecksum) {
						// If status is valid
						if (rxBuffer[4] == 0x00 || rxBuffer[4] == 0x01
								|| rxBuffer[4] == 0x04 || rxBuffer[4] == 0x05
								|| rxBuffer[4] == 0xff) {
							status = rxBuffer[4];
						}
						// If status is invalid
						else {
							status = 0xf4;
						}

					} else {
						status = 0xf3;
					}
				}
				// If response header is incorrect
				else {
					status = 0xf2;
				}
			}
		}
#ifdef DEBUG_META
		// If pass key is already configured correctly
		else{
			char string[] = "Static pass key is already configured correctly.\n\0";
			transmitUart_USB(string);
		}
#endif
		// Reinitialize BT module
		initBT();
		// Re-enable UART interrupt for BT module
		SET_BIT(huart2.Instance->CR1, USART_CR1_RXNEIE);
		HAL_UART_Receive_IT(&huart2, &rxChar_BT, 1);
	}

	return status;

}

/*
 * Function for resetting of the BT module into command mode
 */
static inline void resetBT_commandMode(){
	// Start BT module into command mode
	// Reset MODE pin
	HAL_GPIO_WritePin(BT_MODE_GPIO_Port, BT_MODE_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	// Reset BT module
	resetBT();
}

/*
 * Function for resetting of the BT module into peripheral only mode
 */
static inline void resetBT_peripheralOnyMode(){
	// Restart BT module into peripheral only mode
	// Set MODE pin
	HAL_GPIO_WritePin(BT_MODE_GPIO_Port, BT_MODE_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	// Reset BT module
	resetBT();
	HAL_Delay(100);
}

/*
 * Function for initialization of the BT module
 */
static inline void initBT(void){

	// Reset BT module
	resetBT_peripheralOnyMode();

	// Clear UART RX buffer
	clearUartRxBufferBT(200);

}

/*
 * Function to enable the yellow LED
 */
static inline void enableLED_YE(void) {
	HAL_GPIO_WritePin(LED_YE_GPIO_Port, LED_YE_Pin, GPIO_PIN_SET);
}

/*
 * Function to disable the yellow LED
 */
static inline void disableLED_YE(void) {
	HAL_GPIO_WritePin(LED_YE_GPIO_Port, LED_YE_Pin, GPIO_PIN_RESET);
}

/*
 * Function to enable the blue LED
 */
static inline void enableLED_BL(void) {
	HAL_GPIO_WritePin(LED_BL_GPIO_Port, LED_BL_Pin, GPIO_PIN_SET);
}

/*
 * Function to disable the blue LED
 */
static inline void disableLED_BL(void) {
	HAL_GPIO_WritePin(LED_BL_GPIO_Port, LED_BL_Pin, GPIO_PIN_RESET);
}

/*
 * UART-receive-complete callback function
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	// USB interface
	if (huart == &huart1) {

		// Wrap received character to upper case
		if (rxChar_USB >= 'a' && rxChar_USB <= 'z') {
			rxChar_USB -= 32;
		}

		// If FIFO is not full
		if (uartRxFifo_USB->length < uartRxFifo_USB->maxLength) {
			// If command starts with '!' and all flags are cleared
			if (rxChar_USB == '!' && areAllFlagsCleared_USB()) {
				// Set command-has-started flag
				commandStartFlag_USB = 1;
			}
			// If request starts with '?' and all flags are cleared
			else if (rxChar_USB == '?' && areAllFlagsCleared_USB()) {
				// Set request-has-started flag
				requestStartFlag_USB = 1;
			}
			// If a NL character is received instead
			else if (rxChar_USB == '\n') {
				// If a command has been started
				if (commandStartFlag_USB) {
					// Set command-complete flag
					commandCompleteFlag_USB = 1;
					// Enqueue NL character into FIFO
					enqueueFifo8Data(uartRxFifo_USB, rxChar_USB);
				} else if (requestStartFlag_USB){
					// Set request-complete flag
					requestCompleteFlag_USB = 1;
					// Enqueue NL character into FIFO
					enqueueFifo8Data(uartRxFifo_USB, rxChar_USB);
				}
				// If no command has started so far
				else {
					// Reset flags and empty FIFO
					resetCommandsAndFIFO_USB();
				}
			}
			// If a vaid value has been received instead and commandStartFleg is set
			else if (((rxChar_USB >= '0' && rxChar_USB <= '9')
					|| (rxChar_USB >= 'A' && rxChar_USB <= 'Z') || rxChar_USB == '-' || rxChar_USB == '=') && commandStartFlag_USB) {
				// Enqueue received character into FIFO
				enqueueFifo8Data(uartRxFifo_USB, rxChar_USB);
			}
			// If a letter has been received instead and requestStartFleg is set
			else if ((rxChar_USB >= 'A' && rxChar_USB <= 'Z') && requestStartFlag_USB) {
				// Enqueue received character into FIFO
				enqueueFifo8Data(uartRxFifo_USB, rxChar_USB);
			}
			// If an invalid character has been received
			else {
#ifdef DEBUG_META
				// Send debug message containing flag status
				char string[] = "ERROR in UART-RX-Callback (USB):\n\0";
				transmitUart_USB(string);
//				transmitUart_BT(string);

				char stringC[] = "Char: _\n\0";
				stringC[6] = rxChar_USB;
				transmitUart_USB(stringC);
//				transmitUart_BT(stringC);

				if (commandCompleteFlag_USB) {
					char string2[] = "\tcommandCompleteFlag_USB = 1\n\0";
					transmitUart_USB(string2);
//					transmitUart_BT(string2);
				} else {
					char string2[] = "\tcommandCompleteFlag_USB = 0\n\0";
					transmitUart_USB(string2);
//					transmitUart_BT(string2);
				}

				if (commandStartFlag_USB) {
					char string3[] = "\tcommandStartFlag_USB = 1\n\0";
					transmitUart_USB(string3);
//					transmitUart_BT(string3);
				} else {
					char string3[] = "\tcommandStartFlag_USB = 0\n\0";
					transmitUart_USB(string3);
//					transmitUart_BT(string3);
				}

				if (requestCompleteFlag_USB) {
					char string4[] = "\trequestCompleteFlag_USB = 1\n\0";
					transmitUart_USB(string4);
//					transmitUart_BT(string4);
				} else {
					char string4[] = "\trequestCompleteFlag_USB = 0\n\0";
					transmitUart_USB(string4);
//					transmitUart_BT(string4);
				}

				if (requestStartFlag_USB) {
					char string4[] = "\trequestStartFlag_USB = 1\n\0";
					transmitUart_USB(string4);
//					transmitUart_BT(string4);
				} else {
					char string4[] = "\trequestStartFlag_USB = 0\n\0";
					transmitUart_USB(string4);
//					transmitUart_BT(string4);
				}
#endif

				resetCommandsAndFIFO_USB();
			}
		}
		// If FIFO is already full
		else {
			// Reset flags and empty FIFO
			resetCommandsAndFIFO_USB();
#ifdef DEBUG_META
			// Send debug message via UART
			char string[] =
					"ERROR in UART-RX-Callback (USB): FIFO overflow\n\tFIFO is now empty, flags are cleared\n\0";
			transmitUart_USB(string);
//			transmitUart_BT(string);
#endif
		}

		// Re-enable UART receive interrupt
		HAL_UART_Receive_IT(&huart1, &rxChar_USB, 1);

	}
	// BT module
	else if (huart == &huart2) {

		// Wrap received character to upper case
		if (rxChar_BT >= 'a' && rxChar_BT <= 'z') {
			rxChar_BT -= 32;
		}

		// If FIFO is not full
		if (uartRxFifo_BT->length < uartRxFifo_BT->maxLength) {
			// If command starts with '!' and all flags are cleared
			if (rxChar_BT == '!' && areAllFlagsCleared_BT()) {
				// Set command-has-started flag
				commandStartFlag_BT = 1;
			}
			// If request starts with '?' and all flags are cleared
			else if (rxChar_BT == '?' && areAllFlagsCleared_BT()) {
				// Set request-has-started flag
				requestStartFlag_BT = 1;
			}
			// If a NL character is received instead
			else if (rxChar_BT == '\n') {
				// If a command has been started
				if (commandStartFlag_BT) {
					// Set command-complete flag
					commandCompleteFlag_BT = 1;
					// Enqueue NL character into FIFO
					enqueueFifo8Data(uartRxFifo_BT, rxChar_BT);
				} else if (requestStartFlag_BT) {
					// Set request-complete flag
					requestCompleteFlag_BT = 1;
					// Enqueue NL character into FIFO
					enqueueFifo8Data(uartRxFifo_BT, rxChar_BT);
				}
				// If no command has started so far
				else {
					// Reset flags and empty FIFO
					resetCommandsAndFIFO_BT();
				}
			}
			// If a hex-value has been received instead and commandStartFleg is set
			else if (((rxChar_BT >= '0' && rxChar_BT <= '9')
					|| (rxChar_BT >= 'A' && rxChar_BT <= 'F')
					|| rxChar_BT == 'X') && commandStartFlag_BT) {
				// Enqueue received character into FIFO
				enqueueFifo8Data(uartRxFifo_BT, rxChar_BT);
			}
			// If a letter has been received instead and requestStartFleg is set
			else if ((rxChar_BT >= 'A' && rxChar_BT <= 'Z')
					&& requestStartFlag_BT) {
				// Enqueue received character into FIFO
				enqueueFifo8Data(uartRxFifo_BT, rxChar_BT);
			}
//			// Ignore 0x00
//			else if(rxChar_BT == 0x00){
//
//			}
			// If an invalid character has been received
			else {
#ifdef DEBUG_META
				// Send debug message containing flag status
				char string[] = "ERROR in UART-RX-Callback (BT):\n\0";
				transmitUart_USB(string);
//				transmitUart_BT(string);

				char stringC[] = "Char: _\n\0";
				stringC[6] = rxChar_BT;
				transmitUart_USB(stringC);
//				transmitUart_BT(stringC);

				if (commandCompleteFlag_BT) {
					char string2[] = "\tcommandCompleteFlag_BT = 1\n\0";
					transmitUart_USB(string2);
//					transmitUart_BT(string2);
				} else {
					char string2[] = "\tcommandCompleteFlag_BT = 0\n\0";
					transmitUart_USB(string2);
//					transmitUart_BT(string2);
				}

				if (commandStartFlag_BT) {
					char string3[] = "\tcommandStartFlag_BT = 1\n\0";
					transmitUart_USB(string3);
//					transmitUart_BT(string3);
				} else {
					char string3[] = "\tcommandStartFlag_BT = 0\n\0";
					transmitUart_USB(string3);
//					transmitUart_BT(string3);
				}

				if (requestCompleteFlag_BT) {
					char string4[] = "\trequestCompleteFlag_BT = 1\n\0";
					transmitUart_USB(string4);
//					transmitUart_BT(string4);
				} else {
					char string4[] = "\trequestCompleteFlag_BT = 0\n\0";
					transmitUart_USB(string4);
//					transmitUart_BT(string4);
				}

				if (requestStartFlag_BT) {
					char string4[] = "\trequestStartFlag_BT = 1\n\0";
					transmitUart_USB(string4);
//					transmitUart_BT(string4);
				} else {
					char string4[] = "\trequestStartFlag_BT = 0\n\0";
					transmitUart_USB(string4);
//					transmitUart_BT(string4);
				}
#endif

				resetCommandsAndFIFO_BT();
			}
		}
		// If FIFO is already full
		else {
			// Reset flags and empty FIFO
			resetCommandsAndFIFO_BT();
#ifdef DEBUG_META
			// Send debug message via UART
			char string[] =
					"ERROR in UART-RX-Callback (BT): FIFO overflow\n\tFIFO is now empty, flags are cleared\n\0";
			transmitUart_USB(string);
//			transmitUart_BT(string);
#endif
		}

		// Re-enable UART receive interrupt
		HAL_UART_Receive_IT(&huart2, &rxChar_BT, 1);
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	// Disable blue LED on fatal error
	disableLED_BL();

	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
