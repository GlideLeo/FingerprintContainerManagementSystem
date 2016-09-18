/*
r303ָ��ʶ��ģ�� stm32����
Copyright(C) JassyLiu 2016
All rights reserved
*/

//=========��������===================
unsigned char r303_rxlength = 0;
unsigned char r303_rxbuf[12] = {0};

#include "r303.h"
//�������ݰ���ͷ   ��ͷ 0xEF01��2bytes��  Ĭ��ģ���ַ 0xFFFFFFFF ��4bytes�� 
unsigned char  FP_Pack_Head[6] = { 0xEF,0x01,0xFF,0xFF,0xFF,0xFF };
//��ȡָ��ͼ�� ����ʶ 0x01 ������ 0x0003 ָ���� 0x01 У��� 0x0005
unsigned char FP_Get_Img[6] = { 0x01,0x00,0x03,0x01,0x00,0x05 };
//����ָ������ ����CharBuffer1  ����ʶ 0x01 ������ 0x0004 ָ���� 0x02 �������� 0x01  У��� 0x0008
unsigned char  FP_Img_To_Buffer1[7] = { 0x01,0x00,0x04,0x02,0x01,0x00,0x08 };
//����ָ������ ����CharBuffer2 ����ʶ 0x01 ������ 0x0004 ָ���� 0x02 �������� 0x02  У��� 0x0008
unsigned char  FP_Img_To_Buffer2[7] = { 0x01,0x00,0x04,0x02,0x02,0x00,0x09 };
//ע��ģ��    ����ʶ 0x01 ������ 0x0003 ָ���� 0x05 У��� 0x0009
unsigned char  FP_RegModel[6] = {0x01,0x00,0x03,0x05,0x00,0x09};
//����ָ��    ����ʶ 0x01 ������ 0x0006 ָ���� 0x06   ������1  λ�ú�δ���壨2bytes�� У���δ���壨2bytes��
unsigned char  FP_Save_Finger[5] = { 0x01,0x00,0x06,0x06,0x01 };
//����Чģ���� ����ʶ0x01 ������ 0x0003 ָ���� 0x1D У��� 0x0021
unsigned char  FP_Templete_Num[6] = { 0x01,0x00,0x03,0x1D,0x00,0x21 };
//����ָ�ƿ�  ����ʶ 0x01 ������ 0x0006 ָ���� 0x04  ������1 ҳ��0~929 У��� 0x00B2
unsigned char  FP_Seach[11] = { 0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xA1,0x00,0xB2 };

void r303_delay(unsigned int k)
{
	unsigned int  i, j;
	for (i = 0; i < k; i++)
		for (j = 0; j < 10000; j++);
}
/*
��ָ��ģ�鷢��һ���ֽ�
��ڲ��� Ҫ���͵�����
���ڲ��� ��
*/
void r303_SendData(unsigned char data)
{
	USART_SendData(USART2, data);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/*
��ȡͼ��ָ��
��ڲ��� ��
���ڲ��� ��
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
����ָ������ ����CharBuffer1
��ڲ��� ��
���ڲ��� ��                         
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
����ָ������ ����CharBuffer2
��ڲ��� ��
���ڲ��� ��
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
�ϲ������ļ�  ע��ģ��
��ڲ��� �� 
���ڲ��� �� 
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
�����û�ָ��
��ڲ��� λ��ID ��0~999��
���ڲ��� ��
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
��ȡ��Чģ����
��ڲ��� ��
���ڲ��� ��
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
����ָ��
��ڲ��� ��
���ڲ��� ��
˵�� ʹ�û�����1���������бȶ�
*/
unsigned char r303_Search(void)
{
	unsigned int i;
	unsigned int j=0;
		do 
	{
		r303_rxlength = 0;
		r303_GetImage();		 //��һ�λ�ȡָ��ͼ�� 
		r303_delay(200);
		j++;
	} while ((r303_rxbuf[9]!=0x00)&&(j<=50));
  
	r303_delay(50);
	r303_rxlength = 0;
	r303_ImageToCharBuffer1();  //ͼ��ת����buffer1 
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
ע��ָ�� 
*/
unsigned char r303_Save(unsigned int PageID)
{
		do 
	{
		r303_rxlength = 0;
		r303_GetImage();		 //��һ�λ�ȡָ��ͼ�� 
		r303_delay(200);
	} while (r303_rxbuf[9]!=0x00);
	r303_delay(100);
	r303_rxlength = 0;
	r303_ImageToCharBuffer1();  //ͼ��ת����buffer1 
	r303_delay(1000); 
	if(r303_rxbuf[9]!=0x00){return 1;}
		do 
	{
		r303_rxlength = 0;
		r303_GetImage();		 //�ڶ���
		r303_delay(200);
	} while (r303_rxbuf[9]!=0x00);
	r303_delay(100);
	r303_rxlength = 0;
	r303_ImageToCharBuffer2();  //ͼ��ת����buffer2
	r303_delay(1000);
	if(r303_rxbuf[9]!=0x00){return 1;}
	r303_rxlength = 0;
	r303_RegModel();            //�ϲ�����ģ��  
	r303_delay(50);
	if(r303_rxbuf[9]!=0x00){return 1;}
  r303_rxlength = 0;
	r303_StoreChar(PageID);			//����ָ��     
  if(r303_rxbuf[9]!=0x00){return 1;}  
	return 0;
}
