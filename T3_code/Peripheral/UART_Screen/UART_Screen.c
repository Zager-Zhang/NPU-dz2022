#include "UART_Screen.h"

int Alarm_Temp;

void US_Write_4Byte(uint16_t addr,int dat){
	uint8_t Dat[20]={0};
	Dat[0]=0xA5;
	Dat[1]=0x5A;
	Dat[2]=0x07;//长度
	Dat[3]=0x82;//指令  
	Dat[4]=addr>>8;    //两位地址 dizhi
	Dat[5]=addr&0xff;
	Dat[6]=dat>>24;
	Dat[7]=(dat>>16)&0xff;
	Dat[8]=(dat>>8)&0xff;
	Dat[9]=dat&0xff;
	HAL_UART_Transmit(&huart4,Dat,10,100);
	while(HAL_UART_GetState(&huart4) == HAL_UART_STATE_BUSY_TX);
}

void US_Read_4Byte(uint16_t addr){
	uint8_t Dat[20]={0};
	Dat[0]=0xA5;
	Dat[1]=0x5A;
	Dat[2]=0x04;
	Dat[3]=0x83;
	Dat[4]=addr>>8;//地址
	Dat[5]=addr&0xff;
	Dat[6]=0x02;
	HAL_UART_Transmit(&huart4,Dat,7,100);
	while(HAL_UART_GetState(&huart4) == HAL_UART_STATE_BUSY_TX);
}

void US_Read_2Byte(uint16_t addr){
	uint8_t Dat[20]={0};
	Dat[0]=0xA5;
	Dat[1]=0x5A;
	Dat[2]=0x04;
	Dat[3]=0x83;
	Dat[4]=addr>>8;//地址
	Dat[5]=addr&0xff;
	Dat[6]=0x01;
	HAL_UART_Transmit(&huart4,Dat,7,100);
}

void US_Write_RTC_7(uint16_t addr,int year,int month,int day,int week,int hour,int minute,int sec){
	uint8_t Dat[20]={0};
	Dat[0]=0xA5;
	Dat[1]=0x5A;
	Dat[2]=0x0A;//长度
	Dat[3]=0x82;
	Dat[4]=addr>>8;//地址
	Dat[5]=addr&0xff;
	Dat[6]=(year/10)*16+year%10;
	Dat[7]=(month/10)*16+month%10;
	Dat[8]=(day/10)*16+day%10;
	Dat[9]=(hour/10)*16+hour%10;
	Dat[10]=(hour/10)*16+hour%10;
	Dat[11]=(minute/10)*16+minute%10;
	Dat[12]=(sec/10)*16+sec%10;
	HAL_UART_Transmit(&huart4,Dat,13,100);
	while(HAL_UART_GetState(&huart4) == HAL_UART_STATE_BUSY_TX);
}

void US_Wirte_2Byte(uint16_t addr,int dat){
	uint8_t Dat[20]={0};
	Dat[0]=0xA5;
	Dat[1]=0x5A;
	Dat[2]=0x05;//长度
	Dat[3]=0x82;
	Dat[4]=addr>>8;//地址
	Dat[5]=addr&0xff;
	Dat[6]=dat>>8;
	Dat[7]=dat&0xff;
	HAL_UART_Transmit(&huart4,Dat,8,100);
}

void US_Write_Text(uint16_t addr,uint8_t tex[],uint16_t tex_len){
	uint8_t Dat[20]={0},i;
	Dat[0]=0xA5;
	Dat[1]=0x5A;
	Dat[2]=5+tex_len;//长度
	Dat[3]=0x82;
	Dat[4]=addr>>8;//地址
	Dat[5]=addr&0xff;
	for(i=0;i<tex_len;i++){
		Dat[6+i]=tex[i];
	}
	Dat[6+tex_len]=0xff;
	Dat[7+tex_len]=0xff;
	HAL_UART_Transmit(&huart4,Dat,8+tex_len,100);
}


