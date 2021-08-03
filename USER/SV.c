#include "SV.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"
#include "gait.h"
#include "uart.h"
#define INTEGRALNUM 6
u8 target_p;
float P[4] = {0};
float p;
float set_p;
float set_speed;
float speed;
float actual_p;
float err;//偏差值
float err_last;//上一个偏差值
float integral_list[INTEGRALNUM] = {0};
float integral = 0;//积分值
float Kp, Ki, Kd;

u8 sv_flag[4] = {0}; // lf, rf, lh, rh; 0: close, 1: open
 
void update_integral_list(float update_value)
{
	for(int i = 0; i<INTEGRALNUM; i++)
	{
		if(i != INTEGRALNUM-1)
		integral_list[i] = integral_list[i+1];
		else
		integral_list[i] = update_value;
	}
}

void SV_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();					//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|
										 GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|
	                   GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_12;			//PB0，1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         			//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  	//高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     		//初始化GPIOB.0和GPIOB.1

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);	//PB1置1 
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);	//PB1置1 
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET);
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_Initure.Pin=GPIO_PIN_8;
		GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         			//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_LOW;  	//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure); 
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);

}
void SV_test1(void)
{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);	//PB1置1 
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);	//PB1置1 
		HAL_Delay(500);
}
void SV_test2(void)
{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET);	//PB1置1 
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);	//PB1置1 
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	//PB0置0
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);	//PB1置1 		
		HAL_Delay(500);
}
void pid_value_init(void)
{
  Kp = 2;
  Ki = 0.2;
  Kd = 1.2;
  err = 0;
	err_last = 0;
	integral = 0;
	target_p = 30;
}

void SV_ESTIMATE(void)
{
	for(int i=0; i<4; i++)
	{
		if(sv_flag[i]==1)
		{
			
			P[3] = 50*(3.3*adc1/65536-2.5);
			P[1] = 50*(3.3*adc2/65536-2.5);
			P[0] = 50*(3.3*adc3/65536-2.5);
			P[2] = 50*(3.3*adc4/65536-2.5);
			actual_p = P[i];
			
			err = target_p - actual_p;
			update_integral_list(err);
			for(int i=0; i<INTEGRALNUM; i++)
			{
				integral += integral_list[i];
			}
			p = Kp * err + Ki*integral + Kd*(err - err_last);
			err_last = err;
			actual_p = p + actual_p;
			speed = 3*(actual_p-10);
			TIM_SetCompare1(&TIM16_Handler,speed);
			integral = 0;
			break;
		}
	}
}
void air_control_trot(void)//rf rh lf lh
{
	if(s<-0.2)
		{
			sv_flag[1]=1;
			SV_ESTIMATE();		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);	//rf0和别的是反的		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);	//rh0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);	//lf0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);	//lh0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	//all0	
		}
	if(s>=-0.2&&s<0)
	{
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);	//rf0和别的是反的		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);	//rh0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);	//lf0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);	//lh0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	//all0				
	}
	if(s>-T/16&&s<=T/16)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
			sv_flag[2]=0;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
	}
	if(s>T/16&&s<=3*T/32)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
	}
	if(s>11*T/32&&s<=15*T/32)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		sv_flag[1]=1;
		SV_ESTIMATE();
	}
	if(s>15*T/32&&s<=16*T/32)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
		sv_flag[1]=1;
		SV_ESTIMATE();
	}
	if(s>16*T/32&&s<=9*T/16)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
		sv_flag[1]=0;
		SV_ESTIMATE();
	}	
	if(s>15*T/16&&s<=T)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		sv_flag[0]=1;
		SV_ESTIMATE();
	}
}
	

void air_control_tripod(void)
{
	if(s<-0.2)
		{
			SV_ESTIMATE();
			sv_flag[1]=1;
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);	//rf0和别的是反的		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);	//rh0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);	//lf0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);	//lh0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	//all0	
		}
	if(s>=-0.2&&s<0)
	{
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);	//rf0和别的是反的		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);	//rh0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);	//lf0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);	//lh0		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	//all0				
	}
	if(s>-T/16&&s<=T/16)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
		sv_flag[2]=0;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
	}
	if(s>T/16&&s<=3*T/32)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
	}
	if(s>3*T/16&&s<=10*T/32)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		sv_flag[1]=1;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);
	}
	if(s>10*T/32&&s<=6*T/16)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
		sv_flag[1]=0;
	}
	if(s>7*T/16&&s<=17*T/32)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		sv_flag[3]=1;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);
	}
	if(s>17*T/32&&s<=9*T/16)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
	sv_flag[3]=0;
	}
	if(s>11*T/16&&s<=25*T/32)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		sv_flag[0]=1;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);
	}
	if(s>25*T/32&&s<=13*T/16)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
			sv_flag[0]=0;
	}
	if(s>15*T/16&&s<=T)
	{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		sv_flag[2]=1;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
	}

}
