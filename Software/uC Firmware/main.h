/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define S8SCK_Pin GPIO_PIN_0
#define S8SCK_GPIO_Port GPIOC
#define S8LCK_Pin GPIO_PIN_1
#define S8LCK_GPIO_Port GPIOC
#define S8DATA1_Pin GPIO_PIN_2
#define S8DATA1_GPIO_Port GPIOC
#define S8DATA2_Pin GPIO_PIN_3
#define S8DATA2_GPIO_Port GPIOC
#define BT_BOOT_Pin GPIO_PIN_4
#define BT_BOOT_GPIO_Port GPIOA
#define BT_WAKEUP_Pin GPIO_PIN_5
#define BT_WAKEUP_GPIO_Port GPIOA
#define BT_MODE_Pin GPIO_PIN_6
#define BT_MODE_GPIO_Port GPIOA
#define BT_RESET_Pin GPIO_PIN_7
#define BT_RESET_GPIO_Port GPIOA
#define S8DATA3_Pin GPIO_PIN_4
#define S8DATA3_GPIO_Port GPIOC
#define S8DATA4_Pin GPIO_PIN_5
#define S8DATA4_GPIO_Port GPIOC
#define S8DATA12_Pin GPIO_PIN_1
#define S8DATA12_GPIO_Port GPIOB
#define S8DATA13_Pin GPIO_PIN_2
#define S8DATA13_GPIO_Port GPIOB
#define LED_BL_Pin GPIO_PIN_12
#define LED_BL_GPIO_Port GPIOB
#define LED_YE_Pin GPIO_PIN_13
#define LED_YE_GPIO_Port GPIOB
#define S8DATA5_Pin GPIO_PIN_6
#define S8DATA5_GPIO_Port GPIOC
#define S8DATA6_Pin GPIO_PIN_7
#define S8DATA6_GPIO_Port GPIOC
#define S8DATA7_Pin GPIO_PIN_8
#define S8DATA7_GPIO_Port GPIOC
#define S8DATA8_Pin GPIO_PIN_9
#define S8DATA8_GPIO_Port GPIOC
#define BT_BUSY_Pin GPIO_PIN_8
#define BT_BUSY_GPIO_Port GPIOA
#define S8DATA9_Pin GPIO_PIN_10
#define S8DATA9_GPIO_Port GPIOC
#define S8DATA10_Pin GPIO_PIN_11
#define S8DATA10_GPIO_Port GPIOC
#define S8DATA11_Pin GPIO_PIN_12
#define S8DATA11_GPIO_Port GPIOC
#define S8DATA14_Pin GPIO_PIN_4
#define S8DATA14_GPIO_Port GPIOB
#define S8DATA15_Pin GPIO_PIN_5
#define S8DATA15_GPIO_Port GPIOB
#define S8DATA16_Pin GPIO_PIN_6
#define S8DATA16_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
