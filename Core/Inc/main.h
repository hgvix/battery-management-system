/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define V0_Pin GPIO_PIN_0
#define V0_GPIO_Port GPIOA
#define V1_Pin GPIO_PIN_1
#define V1_GPIO_Port GPIOA
#define V2_Pin GPIO_PIN_2
#define V2_GPIO_Port GPIOA
#define V3_Pin GPIO_PIN_3
#define V3_GPIO_Port GPIOA
#define V_OUT_Pin GPIO_PIN_4
#define V_OUT_GPIO_Port GPIOA
#define Charge_Signal_Pin GPIO_PIN_5
#define Charge_Signal_GPIO_Port GPIOA
#define Battery_Current_Pin GPIO_PIN_6
#define Battery_Current_GPIO_Port GPIOA
#define DisCharge_Current_Pin GPIO_PIN_7
#define DisCharge_Current_GPIO_Port GPIOA
#define Temperature_Pin GPIO_PIN_0
#define Temperature_GPIO_Port GPIOB
#define BC0_Pin GPIO_PIN_4
#define BC0_GPIO_Port GPIOB
#define BC1_Pin GPIO_PIN_5
#define BC1_GPIO_Port GPIOB
#define BC2_Pin GPIO_PIN_6
#define BC2_GPIO_Port GPIOB
#define BC3_Pin GPIO_PIN_7
#define BC3_GPIO_Port GPIOB
#define ChargeControl_Pin GPIO_PIN_8
#define ChargeControl_GPIO_Port GPIOB
#define DischargeControl_Pin GPIO_PIN_9
#define DischargeControl_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
