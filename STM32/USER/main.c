/*
指纹识别货柜管理系统
Copyright(C) JassyLiu 2016
All rights reserved
UART1  -->    串口屏
UART2  -->    指纹识别模块

权限设置 
---------------------------------------------------------------------------
级别 |        权限                           |  权限所有人                           | 
1       | 办公室 						                  | 该部门部员						              	 |
2       | 设备部 办公室		            | 该部门部员					　               |
3       | 销售部 办公室 设备部  | 该部门部员 所有部长 其他 |
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
//===================指令定义=====================
#define VERIFY		0x30		  																							   //识别
#define ADD		  	0x33		   																							  //添加指纹
#define INIT        0x34			 																							  //初始化
//===============全局变量========================
u8 uart1_rxbuf[9];			   																									 //串行接收缓存
u8 uart1_txbuf[9];			  																									 //串行发送缓存
u8 uart1_rxlength;																												 //串口接收计数变量
u8 uart1_rxchar;				    
unsigned char  uart1_rx_flag;	 																	 //串口1接收标志位
extern unsigned char r303_rxlength;											 //串口2接收长度
extern unsigned char r303_rxbuf[12];										 //串口接收缓存
u8 user_limit_get;				 																									 //读取到的用户权限
u8 user_limit_now = 0;																								 //当前用户权限
unsigned int user_cnt = 0;		 																			 //用户数：初始化读取  
unsigned char user_add_limit = 1; 													 //默认指纹添加权限 
//================Flash保存=======================
//要写入到STM32 FLASH的字符串数组
const u8 level1[]={0,0,0,1};
const u8 level2[]={0,0,0,2};
const u8 level3[]={0,0,0,3};                                           //权限
#define SIZE sizeof(level1)	 																			  	//数组长度
#define FLASH_SAVE_ADDR  0X08020000            	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
unsigned char  datatemp[SIZE];         												  //FLASH 读取缓存 
//==============串口屏幕指令=======================
unsigned char cmd1[ ]={"page 2"};														//提示 请确认指纹
unsigned char cmd2[ ]={"page 3"};														//开柜界面
unsigned char cmd3[ ]={"page 4"};														//提示 失败
unsigned char cmd4[ ]={"page 6"};														//提示 请录入指纹
unsigned char cmd5[ ]={"page 8"};														//录入成功
unsigned char cmd6[ ]={"page 9"};														//录入失败
//=================读取用户总数===================
//录入前被调用 读取用户数 
void Usercnt_Init()
{
	do{
	  r303_ValidTempleteNum();
	  }while(r303_rxbuf[9]!=0x00);  																//0x00确认码   	
																																															//10  11  两位：有效模板个数  
	  user_cnt =r303_rxbuf[11]+1;																	//当前的用户数
	//r303_SendData(user_cnt);   																	  //调试用 
}
//==================延时函数======================
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
  delay2(700);//必要性延时
  }
}
/*
串口屏指令包尾
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
开关IO口初始化 
*/
 void USERGPIO_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);		
 GPIO_InitStructure.GPIO_Pin = 0x3FFF;		            //	设置0~13口	 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
 GPIO_Init(GPIOC, &GPIO_InitStructure);					
 GPIO_SetBits(GPIOC, 0x3FFF);						   
}


 int main(void)
 {	
	SystemInit();
	delay_init();	
	USERGPIO_Init();					        																   	//IO口初始化 
	usart_Configuration(9600,115200);		
	 //串口屏波特率9600 指纹模块波特率115200
	Usercnt_Init();						                                             	//初始化用户数变量
	GPIO_SetBits(GPIOC, 0x3FFF);     												   //IO口状态初始化	
	delay2(100);		
	while(1)
	{	
		 if(uart1_rx_flag==1)
		 {   uart1_rx_flag= 0;
			 switch(uart1_rxchar)
			 {
				//==========添加指纹权限设置======
				 case 0x21:
					 user_add_limit = 1;   //
				 break;
				 case 0x22:
					 user_add_limit = 2;   //
				 break;
				 case 0x23:
					 user_add_limit = 3;   //
				 break;
				//=========添加指纹===============
				 case ADD :
					Usercnt_Init();        //初始化用户数 
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
				//==========指纹识别================
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
					//=========主界面初始化指令 ======
					case INIT:
							user_limit_now = 1;
							user_limit_get = 1;
						break;
					
					//=========开门指令============
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

