#include "fingerprint.h"
//=========全局变量==========
u8 gRsBuf[9];				 //串行接收缓存
u8 gTxBuf[9];				 //串行发送缓存
u8 gRsLength;
//=========延时函数================
static void delay()
{
	u16 i, j;  
	for (i = 0; i < 1000; i++)
		for(j = 0; j < 10000; j++);
}

//===========串口发送函数============
void TxByte(u8 temp)
{
	USART_SendData(FG_UART,temp);
	while (USART_GetFlagStatus(FG_UART, USART_FLAG_TXE) == RESET);
}

/******************串口发送子程序bit SendUART(U8 Scnt,U8 Rcnt,U8 Delay)******/
/*功能：向DSP发送数据********************************************************/
/*参数：Scnt发送字节数；Rcnt接收字节数； Delay延时等待数*********************/
/*返回值：TRUE 成功；FALSE 失败**********************************************/
u8 TxAndRsCmd(u8 Scnt, u8 Rcnt, u8 Delay)
{
   u8  i, j, CheckSum;
	 u32 RsTimeCnt;
	 TxByte(CMD_HEAD);		//标志头	 
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





//=======================新增=========================
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
/******************查询用户总数子程序******************/
/*功能：查询用户总数 **********************************/
/*参数 无 *********************************************/
/*返回值：用户总数 0xFF:失败***************************/
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



//读取比对等级
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

//设置比对等级
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

//读取指纹采集等待超时时间
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

/******************指纹存储子程序**********************/
/*功能：存储用户指纹 **********************************/
/*参数：k 用户指纹存储编号*****************************/
/*返回值：TRUE成功，FALSE失败**************************/
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
//删除所有指纹
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
//比对指纹
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

