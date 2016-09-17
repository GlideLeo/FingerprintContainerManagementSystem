/* 
ʵ����ָ�ƹ�
stm32rct6 
*/
#include "stm32f10x.h"
#include "usart.h"
#include "timer.h"
#include "fingerprint.h"
#include "r303.h"
#define DOOR1_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_0) 
#define DOOR1_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_0)
#define DOOR2_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_1) 
#define DOOR2_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_1)  
#define DOOR3_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_2) 
#define DOOR3_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_2)  
#define DOOR4_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_3)  
#define DOOR4_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_3) 
#define DOOR5_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_4) 
#define DOOR5_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_4)  
#define DOOR6_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_5)
#define DOOR6_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_5) 
#define DOOR7_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_6) 
#define DOOR7_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_6) 
#define DOOR8_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_7)  
#define DOOR8_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_7) 
#define DOOR9_ON	    GPIO_ResetBits(GPIOC, GPIO_Pin_8) 
#define DOOR9_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_8) 
#define DOOR10_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_9)  
#define DOOR10_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_9)
#define DOOR11_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_10) 
#define DOOR11_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_10) 
#define DOOR12_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_11) 
#define DOOR12_OFF	    GPIO_SetBits(GPIOC, GPIO_Pin_11) 
#define DOOR13_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_12)
#define DOOR13_OFF      GPIO_SetBits(GPIOC, GPIO_Pin_12)  
#define DOOR14_ON		GPIO_ResetBits(GPIOC, GPIO_Pin_13) 
#define DOOR14_OFF      GPIO_SetBits(GPIOC, GPIO_Pin_13)  
//===================ָ���=====================
#define VERIFY		0x30		     //ʶ��
#define ADD_CHECK	0x32		  	 //  
#define ADD		  	0x33		     //���ָ��
#define INIT      0x34
//===============ȫ�ֱ���========================
u8 uart1_rxbuf[9];				 //���н��ջ���
u8 uart1_txbuf[9];				 //���з��ͻ���
u8 uart1_rxlength;				 //���ڽ��ռ�������
u8 uart1_rxchar;				
u8 user_limit_get;         //��ȡ�����û�Ȩ��
u8 user_limit_now = 0;     //��ǰ�û�Ȩ��
unsigned int user_cnt = 0;		    //�û�������ʼ����ȡ  
unsigned char user_add_limit = 3; //Ĭ��ָ�����Ȩ�� 
unsigned char  uart1_rx_flag;
extern unsigned char r303_rxlength;
extern unsigned char r303_rxbuf[12];
//==============������Ļָ��=======================
unsigned char cmd1[ ]={"page 2"};		//��ʾ ��ȷ��ָ��
unsigned char cmd2[ ]={"page 3"};		//�������
unsigned char cmd3[ ]={"page 4"};		//��ʾ ʧ��
unsigned char cmd4[ ]={"page 6"};		//��ʾ ��¼��ָ��
unsigned char cmd5[ ]={"page 8"};		//¼��ɹ�
unsigned char cmd6[ ]={"page 9"};		//¼��ʧ��
//=================��ȡ�û�����===================
void Usercnt_Init()
{
	do{
	r303_ValidTempleteNum();
	if (r303_rxlength == 12) { r303_rxlength = 0;}	
	}while(r303_rxbuf[9]!=0x00);  //0x00ȷ����   	
 //10  11  ��λ����Чģ�����  
	user_cnt =r303_rxbuf[11]+1;		//��ǰ���û���
}
//==================��ʱ����======================
void delay()
{
	u16 i, j;
	for (i = 0; i < 1000; i++)
		for(j = 0; j < 10000; j++);
}
void delay1(u32 k)
{
	u16 i, j;
	for (i = 0; i < k; i++)
		for(j = 0; j < 10000; j++);
}
void delay2(u32 k)
{
	u16 i, j;
	for (i = 0; i < k; i++)
		for(j = 0; j < 10; j++);
}
//=================================================
void USART_SendStr(unsigned char *s)
{
 while(*s!='\0')
  {
  USART_SendData(USART1,*s);
  s++;
		delay2(700);
  }
}
/*
������ָ���β
*/
void USART_SendEnd(void)
{
	delay2(1000);
	USART_SendData(USART1,0xFF);
	delay2(1000);
	USART_SendData(USART1,0xFF);
	delay2(1000);
	USART_SendData(USART1,0xFF);
	delay2(1000);
}
/*
����IO�ڳ�ʼ�� 
*/
 void USERGPIO_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);	
	
 GPIO_InitStructure.GPIO_Pin = 0x3FFF;		            //	����0~13��	 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
 GPIO_Init(GPIOC, &GPIO_InitStructure);					
 GPIO_SetBits(GPIOC, 0x3FFF);						   
}


 int main(void)
 {	
	SystemInit();
	USERGPIO_Init();						//IO�ڳ�ʼ�� 
	usart_Configuration(9600,115200);		//������������9600 ָ��ģ�鲨����115200
	printf("Test Uart Fingerprint Reader\r\n");
 	//SetcompareLevel(5);
	//printf("�ȶԵȼ�Ϊ��%d\r\n", GetcompareLevel());
	//printf("��ǰ��ʱʱ�䣺%d\r\n",GetTimeOut());
	Usercnt_Init();							//��ʼ���û�������
	printf("User:%d\r\n",GetUserCount());
    //USART_SendData(USART1,0x22);			//���Դ���      
	GPIO_SetBits(GPIOC, 0x3FFF);            //IO��״̬��ʼ��	
	delay2(1000);			
	while(1)
	{	
		 if(uart1_rx_flag==1)
		 {
			 uart1_rx_flag= 0;
			 switch(uart1_rxchar)
			 {
				//==========���ָ��Ȩ������======
				 case 0x21:
					 user_add_limit = 1;   //���Ȩ��
				 case 0x22:
					 user_add_limit = 2;   //
				 case 0x23:
					 user_add_limit = 3;   //Ĭ��Ȩ��
				//=========���ָ��===============
				 case ADD :
					Usercnt_Init();        //��ʼ���û��� 
				  r303_Save(user_cnt);
//					switch(AddUser(user_cnt,user_add_limit))
//					{
//						case ACK_SUCCESS:
//						user_cnt++;
//						printf("%s",cmd5);
//						USART_SendEnd();
//						delay();
//						break;
//						case ACK_FAIL: 			
//						printf("%s",cmd6);
//						USART_SendEnd();
//							break;
//						case ACK_FULL:			
//						printf("%s",cmd6);
//						USART_SendEnd();
//							break;		
//					}	
					 break;
				//==========ָ��ʶ��================
				case VERIFY : 
				switch(r303_Search())   
					{
						 case ACK_SUCCESS:
							printf("%s", cmd2); 
						  USART_SendEnd();
//							 switch (user_limit_get)
//							 {
//							 case 1:
//								 printf("%s", cmd2);
//								 USART_SendEnd();
//								 DOOR9_ON;
//								 delay1(200);
//								 DOOR9_OFF;
//							   user_limit_now = 1;
//								 user_limit_get = 0;
//								 break;
//							 case 2:
//								 
//								 printf("%s", cmd2);
//								 USART_SendEnd();
//							   user_limit_now = 2;
//							   user_limit_get = 0;
//								 break;
//							 case 3:
//								 
//								 printf("%s", cmd2);
//								 USART_SendEnd();
//							   user_limit_now = 3;
//						  	 user_limit_get = 0;
//								 break;
//							 default:
//								 break;
//							 }
							 break;
//						case ACK_NO_USER:
//							 printf("%s",cmd3);
//						     USART_SendEnd();
//							break;
//						case ACK_TIMEOUT:	
//							 printf("%s",cmd3);
//						     USART_SendEnd();
//							break;	
//						case ACK_GO_OUT:
//							 printf("%s",cmd3);
//						     USART_SendEnd();
//							break;
						default:
							break;
					 };
						break;
					//=========�������ʼ��ָ�� ======
					case INIT:
							user_limit_now = 0;
							user_limit_get = 0;
						break;
					
					//=========����ָ��============
					case 0x41:
						DOOR1_ON;
						delay1(200);
						DOOR1_OFF;
						DOOR1_OFF;
						DOOR1_OFF;
						delay1(200);
						break;
					case 0x42:
						DOOR2_ON;
						delay1(200);
						DOOR2_OFF;
						break;
					case 0x43:
						DOOR3_ON;
						delay1(200);
						DOOR3_OFF;
						break;
					case 0x44:
						DOOR4_ON;
						delay1(200);
						DOOR4_OFF;
						break;
					case 0x45:
						DOOR5_ON;
						delay1(200);
						DOOR5_OFF;
						break;
					case 0x46:
						DOOR6_ON;
						delay1(200);
						DOOR6_OFF;
						break;
					case 0x47:
						DOOR7_ON;
						delay1(200);
						DOOR7_OFF;
						break;
					case 0x48:
						DOOR8_ON;
						delay1(200);
						DOOR8_OFF;
						break;
					case 0x49:
						DOOR9_ON;
						delay1(200);
						DOOR9_OFF;
						break;
					case 0x4A:
						//	if(user_limit==1)
						//	{										
						DOOR10_ON;
						delay1(200);
						DOOR10_OFF;
					//		}
						break;
					case 0x4B:
						DOOR11_ON;
						delay1(200);
						DOOR11_OFF;
						break;
					case 0x4C:
						DOOR12_ON;
						delay1(200);
						DOOR12_OFF;
						break;
					case 0x4D:
						DOOR13_ON;
						delay1(200);
						DOOR13_OFF;
						break;
					case 0x4E:
						DOOR14_ON;
						delay1(200);
						DOOR14_OFF;
						break;
			 }
			 
		 } 

	}
 }

