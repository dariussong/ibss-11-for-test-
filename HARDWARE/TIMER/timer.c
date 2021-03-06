#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "SV.h"
#include "gait.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//定时器中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/8/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

TIM_HandleTypeDef TIM5_Handler;      //定时器句柄 
TIM_HandleTypeDef TIM2_Handler;      //定时器句柄 
TIM_HandleTypeDef TIM7_Handler;      //定时器句柄 
TIM_OC_InitTypeDef TIM2_CH2Handler;
u8 mesg[4]={0X03,0X02,0XD0,0X12};	
u8 canbuf1[8]={0};
u8 canbuf2[8]={0};
u8 canbuf3[4]={0};


void TIM7_Init(u16 arr,u16 psc)
{  
    TIM7_Handler.Instance=TIM7;                          //通用定时器3
    TIM7_Handler.Init.Prescaler=psc;                     //分频
    TIM7_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM7_Handler.Init.Period=arr;                        //自动装载值
    TIM7_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
   
		HAL_TIM_Base_Init(&TIM7_Handler);    
    HAL_TIM_Base_Start_IT(&TIM7_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE    
}

void TIM5_Init(u16 arr,u16 psc)
{  
    TIM5_Handler.Instance=TIM5;                          //通用定时器3
    TIM5_Handler.Init.Prescaler=psc;                     //分频
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM5_Handler.Init.Period=arr;                        //自动装载值
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
   
		HAL_TIM_Base_Init(&TIM5_Handler);    
    HAL_TIM_Base_Start_IT(&TIM5_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE    
}

void TIM2_Init(u16 arr,u16 psc)
{  
    TIM2_Handler.Instance=TIM2;                          //通用定时器3
    TIM2_Handler.Init.Prescaler=psc;                     //分频
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM2_Handler.Init.Period=arr;                        //自动装载值
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
   
		HAL_TIM_Base_Init(&TIM2_Handler);    
    HAL_TIM_Base_Start_IT(&TIM2_Handler); //使能定时器6和定时器6更新中断：TIM_IT_UPDATE  
  
		HAL_TIM_PWM_ConfigChannel(&TIM2_Handler,&TIM2_CH2Handler,TIM_CHANNEL_2);//配置TIM1通道2
    HAL_TIM_PWM_Start(&TIM2_Handler,TIM_CHANNEL_2);//开启PWM通道2
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();            //使能TIM5时钟

		HAL_NVIC_SetPriority(TIM5_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启ITM3中断   
	}  
	   if(htim->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();            //使能TIM5时钟

		HAL_NVIC_SetPriority(TIM2_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM2_IRQn);          //开启ITM3中断   
	} 
		if(htim->Instance==TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();            //使能TIM5时钟

		HAL_NVIC_SetPriority(TIM7_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM7_IRQn);          //?  
	} 	
}

//定时器5中断服务函数
void TIM7_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM7_Handler);
}

void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handler);
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM2_Handler);
}

//定时器5中断服务函数调用
float s=-0.8;
float T=3;
float delta_t=0.02;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM5_Handler))
    {
			Read_All_Ad();	
			air_control_trot();
//			air_control_tripod();
//			movement_trot();	
//			movement_tilt();
//			movement_tripod();				
			s=s+delta_t;
		if(s>=T)
		{
			s=0;
		}
		}
		if(htim==(&TIM2_Handler))
    {			
			Read_All_Ad();			
			battery_indicatior();
		}
		if(htim==(&TIM7_Handler))
    {	
			
		}

		
		

			
		
}