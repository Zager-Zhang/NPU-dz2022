/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SHORT_PUSH 4
#define LONG_PUSH 200
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t My_Key1_Val,My_Key1_Time;
uint16_t My_Key2_Val,My_Key2_Time;
uint16_t My_Shift;	//Shift=0 自动模式  Shift=1 手动模式
uint16_t My_Mode_Num=1;
uint16_t My_Auto_Time;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

//方便配置电平
void My_Set(int num,int val){
	switch(num){
		case 1: 
			if(val==0) HAL_GPIO_WritePin(LA_GPIO_Port,LA_Pin,GPIO_PIN_RESET); 
			else HAL_GPIO_WritePin(LA_GPIO_Port,LA_Pin,GPIO_PIN_SET);
		break;
		
		case 2:
			if(val==0) HAL_GPIO_WritePin(LB_GPIO_Port,LB_Pin,GPIO_PIN_RESET); 
			else HAL_GPIO_WritePin(LB_GPIO_Port,LB_Pin,GPIO_PIN_SET);
		break;
		
		case 3:
			if(val==0) HAL_GPIO_WritePin(LC_GPIO_Port,LC_Pin,GPIO_PIN_RESET); 
			else HAL_GPIO_WritePin(LC_GPIO_Port,LC_Pin,GPIO_PIN_SET);			
		break;
		
		default: break;
	}
}

//模式转换
void My_Shift_Mode(int x){
	switch(x){
		case 1: 
			My_Set(1,0);My_Set(2,1);My_Set(3,0);
		break;
		
		case 2: 
			My_Set(1,0);My_Set(2,0);My_Set(3,1);
		break;
		
		case 3: 
			My_Set(1,0);My_Set(2,0);My_Set(3,0);
		break;
		
		case 4: 
			My_Set(1,1);My_Set(2,1);My_Set(3,0);
		break;
		
		default: break;
	}
}

//每5ms进行一次按键扫描
void My_Key_Scan(){
	My_Key1_Val=HAL_GPIO_ReadPin(Key1_GPIO_Port,Key1_Pin);
	My_Key2_Val=HAL_GPIO_ReadPin(Key2_GPIO_Port,Key2_Pin);
	if(!My_Key1_Val) My_Key1_Time++;
	else {
		//Shift=1手动模式 按键 My_Mode_Num++
		if(My_Key1_Time>=SHORT_PUSH && My_Key1_Time<=LONG_PUSH && My_Shift==1){
			HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
			if(My_Mode_Num==4) My_Mode_Num=1;
			else My_Mode_Num++;
		}
		//长按切换模式
		else if(My_Key1_Time>LONG_PUSH){
			HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);
			if(My_Shift==1) {
				My_Shift=0;
				My_Mode_Num=1;
				My_Auto_Time=0;
			}
			else {
				My_Shift=1;
				My_Mode_Num=1;
			}
		}
		My_Key1_Time=0;
	}
	
	if(!My_Key2_Val) My_Key2_Time++;
	else {
		//Shift=1手动模式 按键 num--
		if(My_Key2_Time>=SHORT_PUSH && My_Key2_Time<=LONG_PUSH && My_Shift==1){
			HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
			if(My_Mode_Num==1) My_Mode_Num=4;
			else My_Mode_Num--;
		}
		My_Key2_Time=0;
	}
}

// 5ms进入中断一次
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == (&htim4)){
		My_Key_Scan();
		if(!My_Shift) My_Auto_Time++;
		if(My_Auto_Time>=1000 && My_Shift==0){
			My_Auto_Time=0;
			if(My_Mode_Num==4) My_Mode_Num=1;
			else My_Mode_Num++;
		}
	}
} 
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
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim4);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		My_Shift_Mode(My_Mode_Num);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
