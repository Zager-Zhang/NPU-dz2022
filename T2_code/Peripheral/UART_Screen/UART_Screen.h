#ifndef __UART_SCREEN_H
#define __UART_SCREEN_H

#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

void US_Write_4Byte(uint16_t addr,int dat);
void US_Write_RTC_7(uint16_t addr,int year,int month,int day,int week,int hour,int minute,int sec);
int US_Read_4Byte(uint16_t addr);
void US_Wirte_2Byte(uint16_t addr,int dat);

#endif
