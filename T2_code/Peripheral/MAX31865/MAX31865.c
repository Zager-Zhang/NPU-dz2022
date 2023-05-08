#include "MAX31865.h"
#include "math.h"

void MAX31865_Init(void)
{
	MAX31865_Write(0x80,0xD3);	//三线模式
	HAL_Delay(20);
}

uint16_t MAX31865_Read(uint8_t addr)
{
	uint8_t dat;
	MAX31865_CS_LOW();
	HAL_SPI_Transmit(&hspi1,&addr,1,TIMEOUT);
	HAL_SPI_Receive(&hspi1,&dat,1,TIMEOUT);
	MAX31865_CS_HIGH();
	return dat;
}


void MAX31865_Write(uint8_t addr,uint8_t data)
{
	uint8_t dat[2];
	dat[0]=addr;
	dat[1]=data;
	MAX31865_CS_LOW();
	HAL_SPI_Transmit(&hspi1,dat,2,TIMEOUT);
	MAX31865_CS_HIGH();
}


double MAX31865_GetTemp(void)
{
	double temp,MAX31865_ADC,R_RTD;
	uint16_t dat;
	double A=3.90830*1e-3 , B=-5.77500*1e-7;
	dat=MAX31865_Read(0x01)<<8;
	dat|=MAX31865_Read(0x02);
	dat>>=1;
	MAX31865_ADC = (double)dat;
	
	R_RTD = (MAX31865_ADC * R_REF) / 32768.0;
	temp=R_RTD;
	
	//粗略计算温度 线性
//	temp = (MAX31865_ADC * 430.0 / 400.0 /32.0) - 256.0;
	
	//一元二次求解
	temp=(-A+sqrt(A*A-4*B*(1-R_RTD/100.0)))/(2*B);
	
	return temp;
}

double Get_Fina_Temp(double temp){
	double Fina_temp;
	if(temp>=(-126) && temp<=(-124)){
		Fina_temp=temp+0.63;
	}
	else if(temp>=(-102) && temp<=(-99)){
		Fina_temp=temp+1.0;
	}
	else if(temp>=(-77) && temp<=(-75)){
		Fina_temp=temp+1.1;
	}
	else if(temp>=(-52) && temp<=(-50)){
		Fina_temp=temp+1.02;
	}
	else if(temp>=(-26.5) && temp<=(-24.5)){
		Fina_temp=temp+0.82;
	}
	else if(temp>=(-1.5) && temp<=2){
		Fina_temp=0;
	}
	else if(temp>=24 && temp<=26){
		Fina_temp=temp+0.8;
	}
	else if(temp>=50 && temp<=52){
		Fina_temp=temp+0.95;
	}
	else if(temp>=76 && temp<=78){
		Fina_temp=temp+1.2-0.45;
	}
	else if(temp>=101 && temp<=104){
		Fina_temp=temp+1.08-0.45;
	}
	else if(temp>=128 && temp<=130){
		Fina_temp=temp+0.63-0.45;
	}
	else if(temp>=155 && temp<=156.5){
		Fina_temp=temp+1.05-0.45;
	}
	else if(temp>=182 && temp<=183.5){
		Fina_temp=temp+1.23-0.25;
	}
	else if(temp>=206 && temp<=212){
		Fina_temp=temp+1.15;
	}
	else if(temp>=236 && temp<=238){
		Fina_temp=temp+1.85;
	}
	else if(temp>=263 && temp<=265.2){
		Fina_temp=temp+1.25;
	}
	else Fina_temp=temp;
	return Fina_temp;
}




