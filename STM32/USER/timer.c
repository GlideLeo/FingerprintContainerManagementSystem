#include "timer.h"

void Timer7_Configuration(u16 arr, u16 psc)
{	
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
		NVIC_InitTypeDef NVIC_InitStructure; 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

		TIM_TimeBaseStructure.TIM_Prescaler =	psc; 		//设置用来作为TIMx时钟频率除数的预分频值
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//计数模式
		TIM_TimeBaseStructure.TIM_Period = arr; 		//设置在下一个更新事件装入活动的自动重装载寄存器周期的值		
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间

		NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01 ;//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

 		TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
 		TIM_Cmd(TIM7, ENABLE);  //使能TIM7						
}

void TIM7_Set(u8 mode)
{
	if (mode)
	{
			TIM_SetCounter(TIM7, 0); 	//清零计数器
			TIM_Cmd(TIM7, ENABLE);		//使能TIM7
	}
	else
	{
			TIM_Cmd(TIM7, DISABLE);  	//关闭定时器7			
	}
}

