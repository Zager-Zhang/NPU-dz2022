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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MAX31865.h"
#include "UART_Screen.h"
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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
double Temperature,Last_Temp;
int Send_Temp,Last_Alarm_Temp,Now_Alarm_Temp,Flag_Alarm;
int T_Count,My_Sec=40,My_Min=40,My_Hour=14;
int flag_Beep,Tcount_Beep;
uint8_t RxData;
int History_Num;
int Flag_History;//历史记录满5个了
struct Node{
	int sec,minute,hour;
	int temp,alarm_temp;
}History_Data[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//10ms进入一次中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim==(&htim2)){
		T_Count++;
		//时间计算
		if(T_Count==100){
			T_Count=0;My_Sec++;
			HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		}
		if(My_Sec==60){
			My_Sec=0;My_Min++;
			HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
		}
		if(My_Min==60){
			My_Min=0;My_Hour++;
			HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);
		}
		//蜂鸣器响5s
		if(flag_Beep){
			Tcount_Beep++;
			if(Tcount_Beep>=500){
				flag_Beep=0; Tcount_Beep=0;
				HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET);
			}
		}
		
	}
}
void copy_history(int i,int j){
//	struct Node Tmp;
//	Tmp.alarm_temp=History_Data[i].alarm_temp;
//	Tmp.hour=History_Data[i].hour;
//	Tmp.minute=History_Data[i].minute;
//	Tmp.sec=History_Data[i].sec;
//	Tmp.temp=History_Data[i].temp;
	
	History_Data[i].alarm_temp=History_Data[j].alarm_temp;
	History_Data[i].hour=History_Data[j].hour;
	History_Data[i].minute=History_Data[j].minute;
	History_Data[i].sec=History_Data[j].sec;
	History_Data[i].temp=History_Data[j].temp;
	
//	History_Data[j].alarm_temp=Tmp.alarm_temp;
//	History_Data[j].hour=Tmp.hour;
//	History_Data[j].minute=Tmp.minute;
//	History_Data[j].sec=Tmp.sec;
//	History_Data[j].temp=Tmp.temp;
}
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
  MX_SPI1_Init();
  MX_UART4_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	MAX31865_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_UART_Receive_IT(&huart4,&RxData,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		US_Write_RTC_7(0x0080,22,5,15,7,My_Hour,My_Min,My_Sec);
		int i;
		if(Flag_History){
			for(i=0;i<5;i++){
				if(i==0) {
					US_Write_4Byte(0x0200,History_Data[i].temp);
					US_Write_4Byte(0x0400,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0300,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
				else if(i==1) {
					US_Write_4Byte(0x0210,History_Data[i].temp);
					US_Write_4Byte(0x0410,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0310,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
				else if(i==2) {
					US_Write_4Byte(0x0220,History_Data[i].temp);
					US_Write_4Byte(0x0420,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0320,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
				else if(i==3) {
					US_Write_4Byte(0x0230,History_Data[i].temp);
					US_Write_4Byte(0x0430,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0330,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
				else if(i==4) {
					US_Write_4Byte(0x0240,History_Data[i].temp);
					US_Write_4Byte(0x0440,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0340,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
			}
		}
		else{
			for(i=0;i<History_Num;i++){
				if(i==0) {
					US_Write_4Byte(0x0200,History_Data[i].temp);
					US_Write_4Byte(0x0400,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0300,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
				else if(i==1) {
					US_Write_4Byte(0x0210,History_Data[i].temp);
					US_Write_4Byte(0x0410,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0310,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
				else if(i==2) {
					US_Write_4Byte(0x0220,History_Data[i].temp);
					US_Write_4Byte(0x0420,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0320,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
				else if(i==3) {
					US_Write_4Byte(0x0230,History_Data[i].temp);
					US_Write_4Byte(0x0430,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0330,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
				else if(i==4) {
					US_Write_4Byte(0x0240,History_Data[i].temp);
					US_Write_4Byte(0x0440,History_Data[i].alarm_temp);
					US_Write_RTC_7(0x0340,22,5,15,7,History_Data[i].hour,History_Data[i].minute,History_Data[i].sec);
				}
			}
		}
		//MAX31865芯片 读取温度等
		if(MAX31865_RDY()==0)
		{
			Last_Temp=Temperature;
			Temperature=MAX31865_GetTemp();
			if(Temperature>270) Temperature=Last_Temp;
			Temperature=Get_Fina_Temp(Temperature);
			Send_Temp=Temperature*1000;
			
			US_Write_4Byte(0x0010,Send_Temp);//向串口屏 发送温度
			Last_Alarm_Temp = Now_Alarm_Temp;
			Now_Alarm_Temp=US_Read_4Byte(0x0020)/10;//从串口屏 读入报警温度
			if(Last_Alarm_Temp!=Now_Alarm_Temp || fabs(Last_Temp-Temperature)>=13 )
				Flag_Alarm=1;
			else Flag_Alarm=0;
			
			//温度超警告 蜂鸣器未开启 警报温度已经修改
			if(Temperature>=Now_Alarm_Temp && Flag_Alarm==1){
				copy_history(4,3);
				copy_history(3,2);
				copy_history(2,1);
				copy_history(1,0);
				
				History_Data[0].sec=My_Sec;
				History_Data[0].minute=My_Min;
				History_Data[0].hour=My_Hour;
				History_Data[0].temp=Temperature*1000;
				History_Data[0].alarm_temp=Now_Alarm_Temp*10;
				History_Num++;
				if(History_Num==5){
					History_Num=0; Flag_History=1;
				}
				flag_Beep=1; //蜂鸣器开启标志
				Flag_Alarm=0; //消除
				Tcount_Beep=0;
				HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_SET);
//				US_Wirte_2Byte(0x0500,0x0001);
			}
			if(Temperature<Now_Alarm_Temp){
//				flag_Beep=0; Tcount_Beep=0; Flag_Alarm=1;//温度降下去了回头还是得报警的
////				US_Wirte_2Byte(0x0500,0x0000);
////				US_Wirte_2Byte(0x0500,0x000C);
//				HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET);
				Flag_Alarm=1;//温度降下去了回头还是得报警的
				
			}
			HAL_Delay(300);
		}
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
