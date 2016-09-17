#include "timer.h"

void Timer7_Configuration(u16 arr, u16 psc)
{	
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
		NVIC_InitTypeDef NVIC_InitStructure; 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

		TIM_TimeBaseStructure.TIM_Prescaler =	psc; 		//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//����ģʽ
		TIM_TimeBaseStructure.TIM_Period = arr; 		//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ		
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ��

		NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01 ;//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

 		TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
 		TIM_Cmd(TIM7, ENABLE);  //ʹ��TIM7						
}

void TIM7_Set(u8 mode)
{
	if (mode)
	{
			TIM_SetCounter(TIM7, 0); 	//���������
			TIM_Cmd(TIM7, ENABLE);		//ʹ��TIM7
	}
	else
	{
			TIM_Cmd(TIM7, DISABLE);  	//�رն�ʱ��7			
	}
}

