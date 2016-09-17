#include "fingerprint.h"
//=========ȫ�ֱ���==========
u8 gRsBuf[9];				 //���н��ջ���
u8 gTxBuf[9];				 //���з��ͻ���
u8 gRsLength;
//=========��ʱ����================
static void delay()
{
	u16 i, j;  
	for (i = 0; i < 1000; i++)
		for(j = 0; j < 10000; j++);
}

//===========���ڷ��ͺ���============
void TxByte(u8 temp)
{
	USART_SendData(FG_UART,temp);
	while (USART_GetFlagStatus(FG_UART, USART_FLAG_TXE) == RESET);
}

/******************���ڷ����ӳ���bit SendUART(U8 Scnt,U8 Rcnt,U8 Delay)******/
/*���ܣ���DSP��������********************************************************/
/*������Scnt�����ֽ�����Rcnt�����ֽ����� Delay��ʱ�ȴ���*********************/
/*����ֵ��TRUE �ɹ���FALSE ʧ��**********************************************/
u8 TxAndRsCmd(u8 Scnt, u8 Rcnt, u8 Delay)
{
   u8  i, j, CheckSum;
	 u32 RsTimeCnt;
	 TxByte(CMD_HEAD);		//��־ͷ	 
	 CheckSum = 0;
	 for (i = 0; i < Scnt; i++)
	 {
			TxByte(gTxBuf[i]);		 
			CheckSum ^= gTxBuf[i];
	 }	
	 TxByte(CheckSum);
	 TxByte(CMD_TAIL);  
	 gRsLength = 0;
	 RsTimeCnt = Delay * 120000;
	 while (gRsLength < Rcnt && RsTimeCnt > 0)
	 RsTimeCnt--;
	 if (gRsLength != Rcnt)return ACK_TIMEOUT;
	 if (gRsBuf[0] != CMD_HEAD) return ACK_FAIL;
	 if (gRsBuf[Rcnt - 1] != CMD_TAIL) return ACK_FAIL;
	 if (gRsBuf[1] != (gTxBuf[0])) return ACK_FAIL;	 
	 CheckSum = 0;
	 for (j = 1; j < gRsLength - 1; j++) CheckSum ^= gRsBuf[j];
	 if (CheckSum != 0) return ACK_FAIL; 	  
 	 return ACK_SUCCESS;
}	 





//=======================����=========================
u8 GetUserLimit(u8 user)
{
	u8 m;
	gTxBuf[0] = CMD_USER_LIMIT;
	gTxBuf[1] = 0;
	gTxBuf[2] = user;
	gTxBuf[3] = 0;
	gTxBuf[4] = 0;
	m = TxAndRsCmd(5, 8, 10);
	if(m == ACK_SUCCESS)
	{
		return gRsBuf[4];
	} 
	else
	{
	 	return 0xFF;
	}	
}
/******************��ѯ�û������ӳ���******************/
/*���ܣ���ѯ�û����� **********************************/
/*���� �� *********************************************/
/*����ֵ���û����� 0xFF:ʧ��***************************/
u8 GetUserCount(void)
{
  u8 m;
	
	gTxBuf[0] = CMD_USER_CNT;
	gTxBuf[1] = 0;
	gTxBuf[2] = 0;
	gTxBuf[3] = 0;
	gTxBuf[4] = 0;	
	
	m = TxAndRsCmd(5, 8, 10);
		
	if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
	{
	    return gRsBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}



//��ȡ�ȶԵȼ�
u8 GetcompareLevel(void)
{
	u8 m;
	
	gTxBuf[0] = CMD_COM_LEV;
	gTxBuf[1] = 0;
	gTxBuf[2] = 0;
	gTxBuf[3] = 1;
	gTxBuf[4] = 0;	
	
	m = TxAndRsCmd(5, 8, 10);
		
	if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
	{
	    return gRsBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

//���ñȶԵȼ�
u8 SetcompareLevel(u8 temp)
{
	u8 m;
	
	gTxBuf[0] = CMD_COM_LEV;
	gTxBuf[1] = 0;
	gTxBuf[2] = temp;
	gTxBuf[3] = 0;
	gTxBuf[4] = 0;	
	
	m = TxAndRsCmd(5, 8, 10);
		
	if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
	{
	    return gRsBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

//��ȡָ�Ʋɼ��ȴ���ʱʱ��
u8 GetTimeOut(void)
{
	u8 m;
	
	gTxBuf[0] = CMD_TIMEOUT;
	gTxBuf[1] = 0;
	gTxBuf[2] = 0;
	gTxBuf[3] = 1;
	gTxBuf[4] = 0;	
	
	m = TxAndRsCmd(5, 8, 10);
		
	if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
	{
	    return gRsBuf[3];
	}
	else
	{
	 	return 0xFF;
	}
}

/******************ָ�ƴ洢�ӳ���**********************/
/*���ܣ��洢�û�ָ�� **********************************/
/*������k �û�ָ�ƴ洢���*****************************/
/*����ֵ��TRUE�ɹ���FALSEʧ��**************************/
u8 AddUser(u8 k,u8 limit)
{
	u8 m;
	
	m = GetUserCount();
	if (m >= USER_MAX_CNT)
		return ACK_FULL;


	gTxBuf[0] = CMD_ADD_1;   //0x01
	gTxBuf[1] = 0;
	gTxBuf[2] = k;
	gTxBuf[3] = limit;
	gTxBuf[4] = 0;	
	

	m = TxAndRsCmd(5, 8, 200);

	
	if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
	{
	    gTxBuf[0] = CMD_ADD_2;

			m = TxAndRsCmd(5, 8, 200);

			if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
			{
					gTxBuf[0] = CMD_ADD_3;

					m = TxAndRsCmd(5, 8, 200);

					if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
					{
						  
							return ACK_SUCCESS;
					}
					else
						return ACK_FAIL;
			}
			else
				return ACK_FAIL;
	}
	else
		return ACK_FAIL;
}
//ɾ������ָ��
void ClearAllUser(void)
{
 	u8 m;
	
	gTxBuf[0] = CMD_DEL_ALL;
	gTxBuf[1] = 0;
	gTxBuf[2] = 0;
	gTxBuf[3] = 0;
	gTxBuf[4] = 0;
	
	m = TxAndRsCmd(5, 8, 50);
	
	if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
	{	    
	//	LED2_ON;
		delay();
//		LED2_OFF;
	}
	else
	{
	// 	LED3_ON;
		delay();
//		LED3_OFF;
	}
}


u8 IsMasterUser(u8 UserID)
{
    if ((UserID == 1) || (UserID == 2) || (UserID == 3)) return TRUE;
			else  return FALSE;
}	 
extern u8 user_limit_get;
//�ȶ�ָ��
u8 VerifyUser(void)
{
	u8 m;
	
	gTxBuf[0] = CMD_MATCH;
	gTxBuf[1] = 0;
	gTxBuf[2] = 0;
	gTxBuf[3] = 0;
	gTxBuf[4] = 0;
	
	m = TxAndRsCmd(5, 8, 150);
	
	if ((m == ACK_SUCCESS) && (IsMasterUser(gRsBuf[4]) == TRUE))
	{	
		user_limit_get = gRsBuf[4];
		 return ACK_SUCCESS;
	}
	else if(gRsBuf[4] == ACK_NO_USER)
	{
		return ACK_NO_USER;
	}
	else if(gRsBuf[4] == ACK_TIMEOUT)
	{
		return ACK_TIMEOUT;
	}
	else
	{
			return ACK_GO_OUT;
	}
}

