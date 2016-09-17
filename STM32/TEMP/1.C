#include "fingerprint.h"

//=========È«¾Ö±äÁ¿==========
u8 gRsBuf[9];				 //´®ÐÐ½ÓÊÕ»º´æ
u8 gTxBuf[9];				 //´®ÐÐ·¢ËÍ»º´æ
u8 gRsLength;

static void delay()
{
	u16 i, j;
	for (i = 0; i < 1000; i++)
		for(j = 0; j < 10000; j++);
}


void TxByte(u8 temp)
{
	USART_SendData(FG_UART,temp);
	while (USART_GetFlagStatus(FG_UART, USART_FLAG_TXE) == RESET);
}

/******************´®¿Ú·¢ËÍ×Ó³ÌÐòbit SendUART(U8 Scnt,U8 Rcnt,U8 Delay)******/
/*¹¦ÄÜ£ºÏòDSP·¢ËÍÊý¾Ý********************************************************/
/*²ÎÊý£ºScnt·¢ËÍ×Ö½ÚÊý£»Rcnt½ÓÊÕ×Ö½ÚÊý£» DelayÑÓÊ±µÈ´ýÊý*********************/
/*·µ»ØÖµ£ºTRUE ³É¹¦£»FALSE Ê§°Ü**********************************************/
u8 TxAndRsCmd(u8 Scnt, u8 Rcnt, u8 Delay)
{
   u8  i, j, CheckSum;
	 u32 RsTimeCnt;
	 TxByte(CMD_HEAD);		//±êÖ¾Í·	 
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


/******************²éÑ¯ÓÃ»§×ÜÊý×Ó³ÌÐò******************/
/*¹¦ÄÜ£º²éÑ¯ÓÃ»§×ÜÊý **********************************/
/*²ÎÊý ÎÞ *********************************************/
/*·µ»ØÖµ£ºÓÃ»§×ÜÊý 0xFF:Ê§°Ü***************************/
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

//¶ÁÈ¡±È¶ÔµÈ¼¶
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

//ÉèÖÃ±È¶ÔµÈ¼¶
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

//¶ÁÈ¡Ö¸ÎÆ²É¼¯µÈ´ý³¬Ê±Ê±¼ä
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

/******************Ö¸ÎÆ´æ´¢×Ó³ÌÐò**********************/
/*¹¦ÄÜ£º´æ´¢ÓÃ»§Ö¸ÎÆ **********************************/
/*²ÎÊý£ºk ÓÃ»§Ö¸ÎÆ´æ´¢±àºÅ*****************************/
/*·µ»ØÖµ£ºTRUE³É¹¦£¬FALSEÊ§°Ü**************************/
u8 AddUser(u8 k)
{
	u8 m;
	
	m = GetUserCount();
	if (m >= USER_MAX_CNT)
		return ACK_FULL;


	gTxBuf[0] = CMD_ADD_1;
	gTxBuf[1] = 0;
	gTxBuf[2] = k;
	gTxBuf[3] = 3;
	gTxBuf[4] = 0;	
	
	LED1_ON;
	m = TxAndRsCmd(5, 8, 200);
	LED1_OFF;
	
	if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
	{
	    gTxBuf[0] = CMD_ADD_2;
			LED1_ON;
			m = TxAndRsCmd(5, 8, 200);
			LED1_OFF;
			if (m == ACK_SUCCESS && gRsBuf[4] == ACK_SUCCESS)
			{
					gTxBuf[0] = CMD_ADD_3;
					LED1_ON;
					m = TxAndRsCmd(5, 8, 200);
					LED1_OFF;
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
//É¾³ýËùÓÐÖ¸ÎÆ
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
		LED2_ON;
		delay();
		LED2_OFF;
	}
	else
	{
	 	LED3_ON;
		delay();
		LED3_OFF;
	}
}


u8 IsMasterUser(u8 UserID)
{
    if ((UserID == 1) || (UserID == 2) || (UserID == 3)) return TRUE;
			else  return FALSE;
}	 

//±È¶ÔÖ¸ÎÆ
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

