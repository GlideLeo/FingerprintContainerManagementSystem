#include "usart.h"


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static void Rcc_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //USART1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); //USART2
}

static void UsartGPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//USART1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//USART2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//����ʸ���жϣ�ʸ������˼������˳�����Ⱥ����˼��
static void UsartNVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;	//�������ݽṹ��
  
 // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);//���ж�ʸ���ŵ�Flash��0��ַ

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

  //ʹ�ܴ����жϣ����������ȼ� USART1
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);	//���ṹ�嶪�����ú�������д�뵽��Ӧ�Ĵ�����

  //ʹ�ܴ����жϣ����������ȼ� USART2
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);	//���ṹ�嶪�����ú�������д�뵽��Ӧ�Ĵ�����
	
}

void usart_Configuration(u32 USART1_BaudRate,u32 USART2_BaudRate)
{
	USART_InitTypeDef USART_InitStruct;

	Rcc_Configuration();

	USART_InitStruct.USART_BaudRate = USART1_BaudRate;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
	USART_Init(USART1, &USART_InitStruct);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1, ENABLE);

	USART_InitStruct.USART_BaudRate = USART2_BaudRate;
	USART_Init(USART2, &USART_InitStruct);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART2, ENABLE);
	
	UsartGPIO_Configuration();
	UsartNVIC_Configuration();
}



//��ʹ�ð�����ģʽ
#if 1 //���û����Σ�����Ҫ��targetѡ����ѡ��ʹ��USE microLIB
#pragma import(__use_no_semihosting)
struct __FILE  
{  
	int handle;  
};  
FILE __stdout;  

_sys_exit(int x)  
{  
	x = x;  
}
#endif

PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1,(u8)ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

	return ch;
}
