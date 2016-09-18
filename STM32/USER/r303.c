/*
r303指纹识别模块 stm32驱动
Copyright(C) JassyLiu 2016
All rights reserved
*/

//=========变量定义===================
unsigned char r303_rxlength = 0;
unsigned char r303_rxbuf[12] = {0};

#include "r303.h"
//所有数据包包头   包头 0xEF01（2bytes）  默认模块地址 0xFFFFFFFF （4bytes） 
unsigned char  FP_Pack_Head[6] = { 0xEF,0x01,0xFF,0xFF,0xFF,0xFF };
//获取指纹图像 包标识 0x01 包长度 0x0003 指令码 0x01 校验和 0x0005
unsigned char FP_Get_Img[6] = { 0x01,0x00,0x03,0x01,0x00,0x05 };
//生成指纹特征 存于CharBuffer1  包标识 0x01 包长度 0x0004 指令码 0x02 缓冲区号 0x01  校验和 0x0008
unsigned char  FP_Img_To_Buffer1[7] = { 0x01,0x00,0x04,0x02,0x01,0x00,0x08 };
//生成指纹特征 存于CharBuffer2 包标识 0x01 包长度 0x0004 指令码 0x02 缓冲区号 0x02  校验和 0x0008
unsigned char  FP_Img_To_Buffer2[7] = { 0x01,0x00,0x04,0x02,0x02,0x00,0x09 };
//注册模板    包标识 0x01 包长度 0x0003 指令码 0x05 校验和 0x0009
unsigned char  FP_RegModel[6] = {0x01,0x00,0x03,0x05,0x00,0x09};
//储存指纹    包标识 0x01 包长度 0x0006 指令码 0x06   缓冲区1  位置号未定义（2bytes） 校验和未定义（2bytes）
unsigned char  FP_Save_Finger[5] = { 0x01,0x00,0x06,0x06,0x01 };
//读有效模板数 包标识0x01 包长度 0x0003 指令码 0x1D 校验和 0x0021
unsigned char  FP_Templete_Num[6] = { 0x01,0x00,0x03,0x1D,0x00,0x21 };
//搜索指纹库  包标识 0x01 包长度 0x0006 指令码 0x04  缓冲区1 页数0~929 校验和 0x00B2
unsigned char  FP_Seach[11] = { 0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xA1,0x00,0xB2 };

void r303_delay(unsigned int k)
{
	unsigned int  i, j;
	for (i = 0; i < k; i++)
		for (j = 0; j < 10000; j++);
}
/*
向指纹模块发送一个字节
入口参数 要发送的数据
出口参数 无
*/
void r303_SendData(unsigned char data)
{
	USART_SendData(USART2, data);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/*
获取图像指令
入口参数 无
出口参数 无
*/
void r303_GetImage(void)
{
	unsigned char i = 0;
	for (i = 0; i < 6; i++)
	{
		r303_SendData(FP_Pack_Head[i]);
	}
	for ( i = 0; i < 6; i++)
	{
		r303_SendData(FP_Get_Img[i]);
	}
}

/*
生成指纹特征 存入CharBuffer1
入口参数 无
出口参数 无                         
*/
void r303_ImageToCharBuffer1(void)
{
	unsigned char i = 0;
	for (i = 0; i < 6; i++)
	{
		r303_SendData(FP_Pack_Head[i]);
	}
	for (i = 0; i < 7;i++ )
	{
		r303_SendData(FP_Img_To_Buffer1[i]);
	}
}

/*
生成指纹特征 存入CharBuffer2
入口参数 无
出口参数 无
*/
void r303_ImageToCharBuffer2(void)
{
	unsigned char i = 0;
	for (i = 0; i < 6; i++)
	{
		r303_SendData(FP_Pack_Head[i]);
	}
	for (i = 0; i < 7; i++)
	{
		r303_SendData(FP_Img_To_Buffer2[i]);
	}
}

/*
合并特征文件  注册模板
入口参数 无 
出口参数 无 
*/

void r303_RegModel(void)
{
	unsigned char i = 0;
	for (i = 0; i < 6; i++)
	{
		r303_SendData(FP_Pack_Head[i]);
	}
	for ( i = 0; i < 6; i++)
	{
		r303_SendData(FP_RegModel[i]);                                                                                                                                                                                                                                                                                                                       
	}
}

/*
储存用户指纹
入口参数 位置ID （0~999）
出口参数 无
*/

void r303_StoreChar(unsigned int PageID)
{
	unsigned char i = 0;
	unsigned char id_h = 0;
	unsigned char id_l = 0;
	unsigned int sum = 0;
	unsigned char sum_h = 0;
	unsigned char sum_l = 0;
	id_l = PageID & 0x00FF;
	id_h = (PageID & 0xFF00) >> 8;
	sum = 0x0E + id_l + id_h;
	sum_l = sum & 0x00FF;
	sum_h = (sum & 0xFF00) >> 8;

	for (i = 0; i < 6; i++)
	{
		r303_SendData(FP_Pack_Head[i]);
	}
	for ( i = 0; i < 5; i++)
	{
		r303_SendData(FP_Save_Finger[i]);
	}
	r303_SendData(id_h);
	r303_SendData(id_l); 
	r303_SendData(sum_h);
	r303_SendData(sum_l);
}

/*
读取有效模板数
入口参数 无
出口参数 无
*/
void r303_ValidTempleteNum(void)
{
	unsigned int i;
	r303_rxlength = 0;
	for (i = 0; i < 6; i++)
	{
		r303_SendData(FP_Pack_Head[i]);
	}
	for (i = 0; i < 6; i++)
	{
		r303_SendData(FP_Templete_Num[i]);
	}
	r303_delay(1000);
}

/*
搜索指纹
入口参数 无
出口参数 无
说明 使用缓冲区1的特征进行比对
*/
unsigned char r303_Search(void)
{
	unsigned int i;
	unsigned int j=0;
		do 
	{
		r303_rxlength = 0;
		r303_GetImage();		 //第一次获取指纹图像 
		r303_delay(200);
		j++;
	} while ((r303_rxbuf[9]!=0x00)&&(j<=50));
  
	r303_delay(50);
	r303_rxlength = 0;
	r303_ImageToCharBuffer1();  //图像转换到buffer1 
	r303_delay(1000); 

	r303_rxlength = 0;
	for (i = 0; i < 6; i++)
	{
		r303_SendData(FP_Pack_Head[i]);
	}
	for (i = 0; i < 11; i++)
	{
		r303_SendData(FP_Seach[i]);
	}
	r303_delay(200);
	return r303_rxbuf[9];
}

/*
注册指纹 
*/
unsigned char r303_Save(unsigned int PageID)
{
		do 
	{
		r303_rxlength = 0;
		r303_GetImage();		 //第一次获取指纹图像 
		r303_delay(200);
	} while (r303_rxbuf[9]!=0x00);
	r303_delay(100);
	r303_rxlength = 0;
	r303_ImageToCharBuffer1();  //图像转换到buffer1 
	r303_delay(1000); 
	if(r303_rxbuf[9]!=0x00){return 1;}
		do 
	{
		r303_rxlength = 0;
		r303_GetImage();		 //第二次
		r303_delay(200);
	} while (r303_rxbuf[9]!=0x00);
	r303_delay(100);
	r303_rxlength = 0;
	r303_ImageToCharBuffer2();  //图像转换到buffer2
	r303_delay(1000);
	if(r303_rxbuf[9]!=0x00){return 1;}
	r303_rxlength = 0;
	r303_RegModel();            //合并特征模板  
	r303_delay(50);
	if(r303_rxbuf[9]!=0x00){return 1;}
  r303_rxlength = 0;
	r303_StoreChar(PageID);			//储存指纹     
  if(r303_rxbuf[9]!=0x00){return 1;}  
	return 0;
}
