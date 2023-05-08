#ifndef __MAX31865_H
#define __MAX31865_H

#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

#define MAX31865_CS_HIGH() HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_SET)
#define MAX31865_CS_LOW() HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_RESET)
#define MAX31865_RDY() HAL_GPIO_ReadPin(MAX31865_RDY_GPIO_Port,MAX31865_RDY_Pin)

#define TIMEOUT 100
#define R_REF 430.0

void MAX31865_Init(void);
uint16_t MAX31865_Read(uint8_t addr);
void MAX31865_Write(uint8_t addr,uint8_t data);
double MAX31865_GetTemp(void);
double Get_Fina_Temp(double temp);

#endif

