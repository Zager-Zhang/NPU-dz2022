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
#include "adc.h"
#include "dac.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "UART_Screen.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CY_COUNT 500.0
#define LIMIT_I 27000000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
double Output_Voltage;
uint16_t Counter10ms;
uint16_t SWITCH_DATA,Vout_Data,FLAG_WARNING;
int Vout_yxl;
uint8_t Rx_Tmp;
uint8_t Sw_Text[3][8];
double VDA;
double I_out,I_tmp_ave;
int I_tmp,I_tmp_sum;
//uint8_t Rx_Start[3];
//uint8_t Rx_Data[20];
//uint16_t addr;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int Get_ADC(){
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,10);
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC)){
		return HAL_ADC_GetValue(&hadc1);
	}
	return 0;
}

double Get_ADC_Voltage(int V_ADC){
	return (double)V_ADC*3.3/4096.0;
}

void Set_Voltage(double Voltage){
	uint16_t DAC_num=Voltage*4096.0/3.3;
	HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_num);
}

void Write_Sw_State(){
	if(FLAG_WARNING){
		US_Write_Text(0x0040,Sw_Text[2],4);
		return;
	}
	if(SWITCH_DATA)
		US_Write_Text(0x0040,Sw_Text[1],3);
	else
		US_Write_Text(0x0040,Sw_Text[0],4);
}

//tim2 10ms进入一次中断   ADC:PA5  DAC:PA4
//tim1
int Sum_ADC=0,T3_Counter;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim==(&htim3)){
		T3_Counter++;
		I_tmp=Get_ADC();
		I_tmp_sum+=I_tmp;
		if(T3_Counter==CY_COUNT){
			T3_Counter=0;
			I_tmp_ave=I_tmp_sum/CY_COUNT;
			if(SWITCH_DATA)
				I_out=Get_ADC_Voltage((I_tmp_sum-2.95*CY_COUNT)*10.0*100.0)*1000.0/CY_COUNT;//*10 0.1电阻 *100 串口屏精度两位小数
			else 
				I_out=0;
			if(I_out<500) {
				I_out=0;
				return;
			}
			else if(I_out<=3000) I_out=I_out;
			else if(I_out<=10000) I_out=I_out*0.961;
			else I_out=I_out*0.901;	//由于采样电阻的
			
			if(I_out!=0&&I_out<=10000){
				Vout_yxl=Vout_yxl*100;
				I_out=Vout_yxl+(I_out-Vout_yxl)*400/Vout_yxl;
			}
			else if(I_out>=50000){
				Vout_yxl=Vout_yxl*3333.3;
				I_out=Vout_yxl+(I_out-Vout_yxl)*100000/Vout_yxl;
			}
			I_tmp_sum=0;
//			if(I_out>LIMIT_I){
//				US_Wirte_2Byte(0x0030,0);
//				FLAG_WARNING=1;
//			}
		}
	}
	if(htim==(&htim2)){
		Counter10ms++;
		Write_Sw_State();
		//每100ms读取一次
		//0x0010 输出电压读取
		if(Counter10ms%10==0)
			US_Read_2Byte(0x0010);
		//0x0030 开关状态读取
		if(Counter10ms%10==5)
			US_Read_2Byte(0x0030);
		if(Counter10ms==100){
			US_Write_4Byte(0x0000,I_out);
			if(SWITCH_DATA==0) printf("当前电源状态为OFF状态\r\n \r\n");
			else {
				printf("当前电源输出电流值为：");
				printf("%d",(int)I_out/100);
				printf(".");
				printf("%d",(int)(I_out/10)%10);
				printf("%d mA\r\n \r\n",(int)I_out%10);
			}
			Counter10ms=0;
			HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		}
	}
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
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  MX_UART4_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_UART_Receive_IT(&huart4,&Rx_Tmp,1);
	Sw_Text[0][0]=' ';
	Sw_Text[0][1]='O';
	Sw_Text[0][2]='F';
	Sw_Text[0][3]='F';
	
	Sw_Text[1][0]=' ';
	Sw_Text[1][1]='O';
	Sw_Text[1][2]='N';
	
	Sw_Text[2][0]='W';
	Sw_Text[2][1]='A';
	Sw_Text[2][2]='R';
	Sw_Text[2][3]='N';
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
uint8_t n,len,data_len;
uint8_t Rx_Data[20];
uint16_t addr;

void Clear_RxData(){
	n=0;
	len=0;
	addr=0;
	memset(Rx_Data,0,sizeof(Rx_Data));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == UART4)
	{
		if(n==0){
			if(Rx_Tmp==0xA5){
				Rx_Data[n++]=Rx_Tmp;
			}
			else Clear_RxData();
		}
		else if(n==1){
			if(Rx_Tmp==0x5A){
				Rx_Data[n++]=Rx_Tmp;
			}
			else Clear_RxData();
		}
		//Rx_Data[2]为长度
		else if(n==2){
			Rx_Data[n++]=Rx_Tmp;
			len=Rx_Tmp;
		}
		else if(n==3){
			if(Rx_Tmp==0x83){
				Rx_Data[n++]=Rx_Tmp;
			}
			else Clear_RxData();
		}
		else if(n==4){
			Rx_Data[n++]=Rx_Tmp;
		}
		else if(n==5){
			Rx_Data[n++]=Rx_Tmp;
			addr=(Rx_Data[4]<<8)+Rx_Data[5];
		}
		else if(n==6){
			if((Rx_Tmp*2+4)==len){
				Rx_Data[n++]=Rx_Tmp;
				data_len=Rx_Tmp;
				if(data_len>2) Clear_RxData();
			}
			else Clear_RxData();
		}
		else{
			if(n<len+2)
				Rx_Data[n++]=Rx_Tmp;
			else{
				Rx_Data[n++]=Rx_Tmp;
				//开关
				if(addr==0x0030){
					SWITCH_DATA=(Rx_Data[7]<<8)+Rx_Data[8];
					if(SWITCH_DATA){
						HAL_GPIO_WritePin(SW_GPIO_Port,SW_Pin,GPIO_PIN_RESET);
						FLAG_WARNING=0;
					}
					else{
						HAL_GPIO_WritePin(SW_GPIO_Port,SW_Pin,GPIO_PIN_SET);
						Set_Voltage(0);
					}
				}
				//电压
				if(addr==0x0010 && SWITCH_DATA==1){
					double Vo_tmp;
					Vout_Data=(Rx_Data[7]<<8)+Rx_Data[8];
					Vout_yxl=Vout_Data;
					Output_Voltage=(double)Vout_Data/10.0;
					Vo_tmp=(double)Vout_Data/10.0 - 0.1 + 0.02;
					if(I_out>50000){
						Vo_tmp=Vo_tmp*31.7/30.0;			//不算线阻						
					}
					
					if(I_out>190000){
						Vo_tmp=Vo_tmp+0.03;
					}
					if(I_out>65000 && I_out<=126000){
						Vo_tmp=Vo_tmp-0.03;
					}
					if(I_out>1000 && I_out<4000){
						Vo_tmp=Vo_tmp;
					}
					if(I_out>4000 && I_out<7000){
						Vo_tmp=Vo_tmp+0.03;
					}
					if(I_out>7000 && I_out<8000){
						Vo_tmp=Vo_tmp+0.04;
					}
					if(I_out>250000){
						Vo_tmp=Vo_tmp+0.1;
					}
					VDA=(8.61-Vo_tmp)/3.0;
					Set_Voltage(VDA);
				}
				Clear_RxData();
			}
		}
		while(HAL_UART_Receive_IT(&huart4,&Rx_Tmp,1) != HAL_OK);
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
