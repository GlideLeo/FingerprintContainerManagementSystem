/*
ָ��ʶ��������ϵͳ
Copyright(C) JassyLiu 2016
All rights reserved
UART1  -->    ������
UART2  -->    ָ��ʶ��ģ��

Ȩ������ 
---------------------------------------------------------------------------
���� |        Ȩ��                           |  Ȩ��������                           | 
1       | �칫�� 						                  | �ò��Ų�Ա						              	 |
2       | �豸�� �칫��		            | �ò��Ų�Ա					��               |
3       | ���۲� �칫�� �豸��  | �ò��Ų�Ա ���в��� ���� |
-----------------------------------------------------------------------------
*/
#include "stm32f10x.h"
#include "usart.h"
#include "timer.h"
#include "r303.h"
#include "stmflash.h"    
#include "delay.h"
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
#define VERIFY		0x30		  																							   //ʶ��
#define ADD		  	0x33		   																							  //���ָ��
#define INIT        0x34			 																							  //��ʼ��
//===============ȫ�ֱ���========================
u8 uart1_rxbuf[9];			   																									 //���н��ջ���
u8 uart1_txbuf[9];			  																									 //���з��ͻ���
u8 uart1_rxlength;																												 //���ڽ��ռ�������
u8 uart1_rxchar;				    
unsigned char  uart1_rx_flag;	 																	 //����1���ձ�־λ
extern unsigned char r303_rxlength;											 //����2���ճ���
extern unsigned char r303_rxbuf[12];										 //���ڽ��ջ���
u8 user_limit_get;				 																									 //��ȡ�����û�Ȩ��
u8 user_limit_now = 0;																								 //��ǰ�û�Ȩ��
unsigned int user_cnt = 0;		 																			 //�û�������ʼ����ȡ  
unsigned char user_add_limit = 1; 													 //Ĭ��ָ�����Ȩ�� 
//================Flash����=======================
//Ҫд�뵽STM32 FLASH���ַ�������
const u8 level1[]={0,0,0,1};
const u8 level2[]={0,0,0,2};
const u8 level3[]={0,0,0,3};                                           //Ȩ��
#define SIZE sizeof(level1)	 																			  	//���鳤��
#define FLASH_SAVE_ADDR  0X08020000            	//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)
unsigned char  datatemp[SIZE];         												  //FLASH ��ȡ���� 
//==============������Ļָ��=======================
unsigned char cmd1[ ]={"page 2"};														//��ʾ ��ȷ��ָ��
unsigned char cmd2[ ]={"page 3"};														//�������
unsigned char cmd3[ ]={"page 4"};														//��ʾ ʧ��
unsigned char cmd4[ ]={"page 6"};														//��ʾ ��¼��ָ��
unsigned char cmd5[ ]={"page 8"};														//¼��ɹ�
unsigned char cmd6[ ]={"page 9"};														//¼��ʧ��
//=================��ȡ�û�����===================
//¼��ǰ������ ��ȡ�û��� 
void Usercnt_Init()
{
	do{
	  r303_ValidTempleteNum();
	  }while(r303_rxbuf[9]!=0x00);  																//0x00ȷ����   	
																																															//10  11  ��λ����Чģ�����  
	  user_cnt =r303_rxbuf[11]+1;																	//��ǰ���û���
	//r303_SendData(user_cnt);   																	  //������ 
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
  delay2(700);//��Ҫ����ʱ
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
	delay_init();	
	USERGPIO_Init();					        																   	//IO�ڳ�ʼ�� 
	usart_Configuration(9600,115200);		
	 //������������9600 ָ��ģ�鲨����115200
	Usercnt_Init();						                                             	//��ʼ���û�������
	GPIO_SetBits(GPIOC, 0x3FFF);     												   //IO��״̬��ʼ��	
	delay2(100);		
	while(1)
	{	
		 if(uart1_rx_flag==1)
		 {   uart1_rx_flag= 0;
			 switch(uart1_rxchar)
			 {
				//==========���ָ��Ȩ������======
				 case 0x21:
					 user_add_limit = 1;   //
				 break;
				 case 0x22:
					 user_add_limit = 2;   //
				 break;
				 case 0x23:
					 user_add_limit = 3;   //
				 break;
				//=========���ָ��===============
				 case ADD :
					Usercnt_Init();        //��ʼ���û��� 
				  switch(r303_Save(user_cnt))
					{
						case 0:
							switch(user_add_limit)
							{
								case 1:
									STMFLASH_Write(FLASH_SAVE_ADDR+(user_cnt-0x01)*0x20,(u16*)level1,SIZE);
							 	break;
								case 2:
									STMFLASH_Write(FLASH_SAVE_ADDR+(user_cnt-0x01)*0x20,(u16*)level2,SIZE);
								break;
								case 3:
									STMFLASH_Write(FLASH_SAVE_ADDR+(user_cnt-0x01)*0x20,(u16*)level3,SIZE);
								break;
							  default:
								break;
							}
							printf("%s",cmd5);
						  USART_SendEnd();
							delay();
						break;
						case 1:
							printf("%s",cmd6);
						  USART_SendEnd();
							delay();
						break;		
						default:
						break;						
					}

					break;
				//==========ָ��ʶ��================
				case VERIFY : 
					if(r303_Search()==0x00)
					{
						STMFLASH_Read(FLASH_SAVE_ADDR+(r303_rxbuf[11]*0x20-0x20),(u16*)datatemp,SIZE);
						user_limit_now = datatemp[3];
						r303_SendData(user_limit_now);
						printf("%s", cmd2); 
						USART_SendEnd();
					}
					else 
					{
						printf("%s", cmd3); 
						USART_SendEnd();						
					}
						break;
					//=========�������ʼ��ָ�� ======
					case INIT:
							user_limit_now = 1;
							user_limit_get = 1;
						break;
					
					//=========����ָ��============
					case 0x41:
						if(user_limit_now == 0x03){
						DOOR1_ON;
						delay1(200);
						DOOR1_OFF;
            }
						break;
					case 0x42:
						if(user_limit_now == 0x03){
						DOOR2_ON;
						delay1(200);
						DOOR2_OFF;
            }
						break;
					case 0x43:
						if(user_limit_now == 0x03){
						DOOR3_ON;
						delay1(200);
						DOOR3_OFF;
            }
						break;
					case 0x44:
						if(user_limit_now == 0x03){
						DOOR4_ON;
						delay1(200);
						DOOR4_OFF;
            }
						break;
					case 0x45:
						if(user_limit_now == 0x03){
						DOOR5_ON;
						delay1(200);
						DOOR5_OFF;
            }
						break;
					case 0x46:
						if(user_limit_now == 0x03){
						DOOR6_ON;
						delay1(200);
						DOOR6_OFF;
            }
						break;
					case 0x47:
						if(user_limit_now == 0x03){
						DOOR7_ON;
						delay1(200);
						DOOR7_OFF;
            }
						break;
					case 0x48:
						if(user_limit_now == 0x03){
						DOOR8_ON;
						delay1(200);
						DOOR8_OFF;
            }
						break;
					case 0x49:
						if(user_limit_now == 0x03){
						DOOR9_ON;
						delay1(200);
						DOOR9_OFF;
            }
						break;
					case 0x4A:
						if(user_limit_now == 0x03){
						DOOR10_ON;
						delay1(200);
						DOOR10_OFF;
            }
						break;
					case 0x4B:

						DOOR11_ON;
						delay1(200);
						DOOR11_OFF;
            
						break;
					case 0x4C:
						if(user_limit_now >= 0x02)
						{DOOR12_ON;
						delay1(200);
						DOOR12_OFF;}
						break;
					case 0x4D:
						if(user_limit_now >= 0x02)
						{DOOR13_ON;
						delay1(200);
						DOOR13_OFF;}
						break;
					case 0x4E:
						if(user_limit_now >= 0x02)
						{DOOR14_ON;
						delay1(200);
						DOOR14_OFF;
						}
						break;
			 }
			 
		 } 

	}
 }

