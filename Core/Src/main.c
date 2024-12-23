/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "string.h"
#include <stdio.h>
#include <math.h>

/* Private define ------------------------------------------------------------*/
#define NUNCHUK_ADDRESS (0x52 << 1)  // Adresse I2C du Nunchuk

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;
uint8_t init[2] = {0xF0, 0x55};  // Commande d'initialisation du Nunchuk
uint8_t data[6] = {0};           // Données reçues du Nunchuk

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    // Message de démarrage
    const char startMsg[] = "Starting initialization...\n\r";
    HAL_UART_Transmit(&huart2, (uint8_t*)startMsg, strlen(startMsg), HAL_MAX_DELAY);

    // Vérification de la disponibilité de la manette
    if (HAL_I2C_IsDeviceReady(&hi2c1, NUNCHUK_ADDRESS, 3, HAL_MAX_DELAY) != HAL_OK) {
        const char errMsg[] = "Nunchuk not responding. Check wiring and address.\n\r";
        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, strlen(errMsg), HAL_MAX_DELAY);
        Error_Handler();
    } else {
        const char readyMsg[] = "Nunchuk is ready for communication.\n\r";
        HAL_UART_Transmit(&huart2, (uint8_t*)readyMsg, strlen(readyMsg), HAL_MAX_DELAY);
    }

    // Initialisation du Nunchuk
    if (HAL_I2C_Master_Transmit(&hi2c1, NUNCHUK_ADDRESS, init, 2, HAL_MAX_DELAY) != HAL_OK) {
        const char errMsg[] = "Error in first transmission to initialize Nunchuk\n\r";
        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, strlen(errMsg), HAL_MAX_DELAY);
        Error_Handler();
    } else {
        const char initSuccess[] = "Initialization command sent successfully.\n\r";
        HAL_UART_Transmit(&huart2, (uint8_t*)initSuccess, strlen(initSuccess), HAL_MAX_DELAY);
    }
    HAL_Delay(100);

    // Envoi de la commande 0x00 pour démarrer la communication
    uint8_t command = 0x00;
    if (HAL_I2C_Master_Transmit(&hi2c1, NUNCHUK_ADDRESS, &command, 1, HAL_MAX_DELAY) != HAL_OK) {
        const char errMsg[] = "Error in second transmission (0x00 command)\n\r";
        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, strlen(errMsg), HAL_MAX_DELAY);
        Error_Handler();
    } else {
        const char commandSuccess[] = "0x00 command sent successfully.\n\r";
        HAL_UART_Transmit(&huart2, (uint8_t*)commandSuccess, strlen(commandSuccess), HAL_MAX_DELAY);
    }
    HAL_Delay(10);

    // Entrée dans la boucle principale
    const char loopMsg[] = "Entering main loop...\n\r";
    HAL_UART_Transmit(&huart2, (uint8_t*)loopMsg, strlen(loopMsg), HAL_MAX_DELAY);

    while (1)
    {
        // Demande de lecture des données
        if (HAL_I2C_Master_Transmit(&hi2c1, NUNCHUK_ADDRESS, &command, 1, HAL_MAX_DELAY) != HAL_OK) {
            const char errMsg[] = "Error in data request\n\r";
            HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, strlen(errMsg), HAL_MAX_DELAY);
            Error_Handler();
        }
        HAL_Delay(10);

        // Réception des données
        if (HAL_I2C_Master_Receive(&hi2c1, NUNCHUK_ADDRESS, data, 6, HAL_MAX_DELAY) != HAL_OK) {
            const char errMsg[] = "Error in data reception\n\r";
            HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, strlen(errMsg), HAL_MAX_DELAY);
            Error_Handler();
        }

        // Extraction des données
        uint8_t joystickX = data[0];
        uint8_t joystickY = data[1];
        uint16_t accelX = (data[2] << 2) | ((data[5] >> 2) & 0x03);
        uint16_t accelY = (data[3] << 2) | ((data[5] >> 4) & 0x03);
        uint16_t accelZ = (data[4] << 2) | ((data[5] >> 6) & 0x03);
        uint8_t buttonC = !(data[5] & 0x02);
        uint8_t buttonZ = !(data[5] & 0x01);

        float  roll = atan(accelY/sqrt(accelX*accelX + accelZ *accelZ)) * 180.0 / M_PI;
        float  pitch = atan(accelX/sqrt(accelY*accelY + accelZ *accelZ))* 180.0 / M_PI;

        // Affichage des données sur UART
        char dataMsg[120];
        snprintf(dataMsg, sizeof(dataMsg),
                 "Joystick X: %d, Y: %d\n\r"
                 "Accel X: %d, Y: %d, Z: %d\n\r"
                 "Button C: %d, Button Z: %d\n\r\n"
        		"roll : %.2f, pitch: %.2f\n\r\n",
                 joystickX, joystickY,
                 accelX, accelY, accelZ,
                 buttonC, buttonZ,
				 roll, pitch);

        HAL_UART_Transmit(&huart2, (uint8_t*)dataMsg, strlen(dataMsg), HAL_MAX_DELAY);

        HAL_Delay(100);  // Attente avant de relire les données
    }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
