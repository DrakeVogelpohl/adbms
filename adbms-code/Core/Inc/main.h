/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define Current_ADC_Pin GPIO_PIN_2
#define Current_ADC_GPIO_Port GPIOA
#define SD_Contactors_IN_Pin GPIO_PIN_1
#define SD_Contactors_IN_GPIO_Port GPIOB
#define Comms_6822_State_GPIO_Pin GPIO_PIN_15
#define Comms_6822_State_GPIO_GPIO_Port GPIOB
#define Charger_GPIO_Pin GPIO_PIN_7
#define Charger_GPIO_GPIO_Port GPIOC
#define IMD_Status_GPIO_Pin GPIO_PIN_8
#define IMD_Status_GPIO_GPIO_Port GPIOC
#define BMS_Status_GPIO_Pin GPIO_PIN_9
#define BMS_Status_GPIO_GPIO_Port GPIOC
#define Contactor_N_Ctrl_GPIO_Pin GPIO_PIN_8
#define Contactor_N_Ctrl_GPIO_GPIO_Port GPIOA
#define Contactor_P_Ctrl_GPIO_Pin GPIO_PIN_9
#define Contactor_P_Ctrl_GPIO_GPIO_Port GPIOA
#define Contactor_Pre_Ctrl_GPIO_Pin GPIO_PIN_10
#define Contactor_Pre_Ctrl_GPIO_GPIO_Port GPIOA
#define LED1_GPIO_Pin GPIO_PIN_5
#define LED1_GPIO_GPIO_Port GPIOB
#define LED2_GPIO_Pin GPIO_PIN_6
#define LED2_GPIO_GPIO_Port GPIOB
#define LED3_GPIO_Pin GPIO_PIN_7
#define LED3_GPIO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
