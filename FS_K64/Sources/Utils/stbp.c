/**
 * @file    stbp.c
 *
 * @brief   Swiss Timing Binary Protocol
 *			http://swtchcm006/redmine/projects/stbp/wiki
 *			
 * @author  Arnaud Brielmann
 * 
 * SVN:     $URL: svn://swtchcm006/Timing_&_Scoring/Components/SVN/CommonUtils/STBP/lib_c/trunk/stbp.c $
 *
 *          $Revision: 234 $
 *
 *          $LastChangedBy: stephan_aronson $
 *
 *          $LastChangedDate: 2014-02-25 10:25:42 +0100 (mar., 25 févr. 2014) $
 *
 *          SWISS TIMING Ltd. copyright 2011 (c).
 ******************************************************************/

// LOCAL INCLUDES
#include "stbp.h"

// DECLARATION OF PRIVATE VARIABLES AND FUNCTIONS/METHODS
static BOOL m_bIsBigEndian = FALSE;


// IMPLEMENTATION
void Stbp_InitSw(	CStbp * me,
					UINT8 * pRxBuffer,
					UINT16 uRxBufferSize,
					UINT8 * pTxBuffer,
					UINT16 uTxBufferSize)
{
	//Determine endianness. c.f. http://www.ibm.com/developerworks/aix/library/au-endianc
	UINT16 uDummyWord = 1;
	if( (*(UINT8*)&uDummyWord) == 0 )
	{
		m_bIsBigEndian = TRUE;
	}
	else
	{
		m_bIsBigEndian = FALSE;
	}
	//Init communication instance
	me->bFrameOk = FALSE;
	me->bIsReceiving = FALSE;
	me->pRxBuffer = pRxBuffer;
	me->pTxBuffer = pTxBuffer;
	me->uRxBufferHead = 0;
	me->uRxBufferTail = 0;
	me->uRxBufferSize = uRxBufferSize;
	me->uTxBufferHead = 0;
	me->uTxBufferSize = uTxBufferSize;
	me->uTxBufferTail = 0;
	
	me->bEscapeByteRxed = FALSE;
	me->bEscapeByteTxed = FALSE;
	
	me->RxMsgInfo.Conf = ESTBP_CONF_NOSUBHEADER;
	me->RxMsgInfo.AppID = ESTBP_APPID_RESERVED;
	me->RxMsgInfo.MsgID = 0;
	me->RxMsgInfo.BusAddress = ESTBP_BUSADDR_BROADCAST;
	me->RxMsgInfo.Crc = 0;

	me->TxMsgCtrl.Crc = 0;
	me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_IDLE;
}

void Stbp_ReceiveByte(CStbp * me, UINT8 uRxByte)
{	
	UINT8 uBitCnt, uCrc;
	uCrc = me->RxMsgInfo.Crc;
				
	if(!me->bIsReceiving) //wait for start byte
	{
		if (ESTBP_CONTROLBYTE_START == uRxByte)
		{
			//initialize control flags
			me->uRxBufferHead = 0;
			me->uRxBufferTail = 0;
			me->bIsReceiving = TRUE;
			me->bEscapeByteRxed = FALSE;
			uCrc = 0;
			me->bFrameOk = FALSE;
		}
	}
	else //start byte received
	{
		if(me->uRxBufferHead >= me->uRxBufferSize) //buffer overflow
		{
			me->bIsReceiving = FALSE;
		}
		else if (ESTBP_CONTROLBYTE_START == uRxByte)
		{
			//stay in reception but reset frame
			me->uRxBufferHead = 0;
			me->uRxBufferTail = 0;
			me->bEscapeByteRxed = FALSE;
			uCrc = 0;
		}
		else if (ESTBP_CONTROLBYTE_END == uRxByte) //End of frame
		{
			me->bFrameOk = FALSE;
			if (me->uRxBufferHead >= 5) //header+footer minimal length
			{
				//roll-back before crc update with its own encoded value
				if (me->pRxBuffer[me->uRxBufferHead - 1] > 252)
				{
					uCrc = me->RxMsgInfo.Crc_t2;//CRC was encoded over 2 bytes
				}
				else
				{
					uCrc = me->RxMsgInfo.Crc_t1;//CRC was encoded over 1 byte
				}
				if (uCrc == me->pRxBuffer[me->uRxBufferHead - 1])
				{
					me->RxMsgInfo.Conf = (EStbpConf)me->pRxBuffer[0];
					me->RxMsgInfo.AppID = (EStbpAppId)(((((UINT16)me->pRxBuffer[1])<<8)&0xFF00) + (((UINT16)me->pRxBuffer[2])&0x00FF)); //protocol is big-endian
					me->RxMsgInfo.MsgID = me->pRxBuffer[3];
					if(ESTBP_CONF_BUS_16BIT == me->RxMsgInfo.Conf)
					{
						me->RxMsgInfo.BusAddress = (UINT32)(((((UINT16)me->pRxBuffer[4])<<8)&0xFF00) + (((UINT16)me->pRxBuffer[5])&0x00FF)); //protocol is big-endian
						me->uRxBufferTail = ESTBP_HEADER_LENGTH_BUS16BIT;
					}
					else if(ESTBP_CONF_BUS_8BIT == me->RxMsgInfo.Conf)
					{
						me->RxMsgInfo.BusAddress = (UINT32)me->pRxBuffer[4]; 
						me->uRxBufferTail = ESTBP_HEADER_LENGTH_BUS8BIT;
					}
					else if(ESTBP_CONF_CHAIN_16BIT == me->RxMsgInfo.Conf)
					{
						me->RxMsgInfo.ChainFields.SrcAddress = (((((UINT16)me->pRxBuffer[4])<<8)&0xFF00) + (((UINT16)me->pRxBuffer[5])&0x00FF)); //protocol is big-endian
						me->RxMsgInfo.ChainFields.DestAddress = (((((UINT16)me->pRxBuffer[6])<<8)&0xFF00) + (((UINT16)me->pRxBuffer[7])&0x00FF)); //protocol is big-endian
						me->RxMsgInfo.ChainFields.NodeDepth = (((((UINT16)me->pRxBuffer[8])<<8)&0xFF00) + (((UINT16)me->pRxBuffer[9])&0x00FF)); //protocol is big-endian
						me->uRxBufferTail = ESTBP_HEADER_LENGTH_CHAIN16BIT;
					}
					else //No subheader
					{
						me->RxMsgInfo.BusAddress = 0;
						me->uRxBufferTail = ESTBP_HEADER_LENGTH_NOSUB;
					}
					
					me->bFrameOk = TRUE;
				}
			}
			me->bIsReceiving = FALSE;
		}
		else //Reception
		{
			//update CRC
			uCrc ^= uRxByte;
			for(uBitCnt=0;uBitCnt<8;uBitCnt++)
			{
				if (uCrc&0x80)
				{
					uCrc = (uCrc<<1)^ESTBP_CRC_POLY; 
				} 
				else
				{
					uCrc = uCrc<<1;
				}
			}
			//Decode 253-basis
			if (FALSE == me->bEscapeByteRxed)
			{
				if (ESTBP_CONTROLBYTE_ESC == uRxByte)
				{
					me->bEscapeByteRxed = TRUE;
				} 
				else
				{
					me->pRxBuffer[me->uRxBufferHead] = uRxByte-3;
					me->uRxBufferHead++;
				}
			}
			else //previous byte was ESC
			{
				me->pRxBuffer[me->uRxBufferHead] = uRxByte;
				me->uRxBufferHead++;
				me->bEscapeByteRxed = FALSE;
			}
		}
	}
	me->RxMsgInfo.Crc_t2 = me->RxMsgInfo.Crc_t1;
	me->RxMsgInfo.Crc_t1 = me->RxMsgInfo.Crc;
	me->RxMsgInfo.Crc = uCrc;	//update static member
}

BOOL Stbp_IsDataToTransmit(CStbp * me)
{
	BOOL bRetVal = FALSE;
	if ((me->TxMsgCtrl.eTxState > ESTBP_TXSTATE_IDLE) &&
		(me->TxMsgCtrl.eTxState < ESTBP_TXSTATE_SENDINGLASTBYTE))
	{
		bRetVal = TRUE;
	}
	else
	{
		me->uTxBufferHead = 0;
		me->uTxBufferTail = 0;
		me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_IDLE;
		bRetVal = FALSE;
	}
	return bRetVal;
}

BOOL Stbp_IsNewFrame(CStbp * me)
{
	BOOL bRetVal = FALSE;
	if (me->bFrameOk)
	{
		bRetVal = TRUE;
		me->bFrameOk = FALSE;
	}
	return bRetVal;
}

UINT8 Stbp_GetNextByteToSend(CStbp * me)
{
	UINT8 uNextDataByte, uCrc, uBitCnt;
	UINT8 uRetVal = ESTBP_CONTROLBYTE_END;
	
	uCrc = me->TxMsgCtrl.Crc;  //copy to local
	switch(me->TxMsgCtrl.eTxState)
	{
		case ESTBP_TXSTATE_BUFFERINIT :
		{
			if (me->uTxBufferHead > me->uTxBufferTail)
			{
				uRetVal = ESTBP_CONTROLBYTE_START;
				uCrc = 0;
				me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_SENDBUFFER;
			}
		}break;
		
		case ESTBP_TXSTATE_SENDBUFFER :
		{
			uNextDataByte = me->pTxBuffer[me->uTxBufferTail]; //copy to local
			//encode in 253-basis
			if(uNextDataByte>252)
			{
				if (me->bEscapeByteTxed)
				{
					uRetVal = uNextDataByte;
					me->bEscapeByteTxed = FALSE;
					me->uTxBufferTail++;
				}
				else
				{
					uRetVal = ESTBP_CONTROLBYTE_ESC;
					me->bEscapeByteTxed = TRUE;
				}
			}
			
			else 
			{
				uRetVal = uNextDataByte + 3; 
				me->uTxBufferTail++;
			}
			//update CRC
			uCrc ^= uRetVal;
			for(uBitCnt=0;uBitCnt<8;uBitCnt++)
			{
				if (uCrc&0x80)
				{
					uCrc = (uCrc<<1)^ESTBP_CRC_POLY;
				} 
				else
				{
					uCrc = uCrc<<1;
				}
			}
			//Check if buffer empty
			if (me->uTxBufferHead <= me->uTxBufferTail)
			{
				me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_SEND_CRC;
			}
		}break;
		
		case ESTBP_TXSTATE_SEND_CRC :
		{
			//encode crc in 253-basis
			if(uCrc>252)
			{
				if (me->bEscapeByteTxed)
				{
					uRetVal = uCrc;
					me->bEscapeByteTxed = FALSE;
					me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_TERMINATE;
				}
				else
				{
					uRetVal = ESTBP_CONTROLBYTE_ESC;
					me->bEscapeByteTxed = TRUE;
				}
			}
			else 
			{
				uRetVal = uCrc + 3; 
				me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_TERMINATE;
			}
		}break;
		
		case ESTBP_TXSTATE_TERMINATE :
		{
			uRetVal = ESTBP_CONTROLBYTE_END;
			me->uTxBufferHead = 0;
			me->uTxBufferTail = 0;
			me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_SENDINGLASTBYTE;
		}break;

		default:
		{
			uRetVal = ESTBP_CONTROLBYTE_END;
			me->uTxBufferHead = 0;
			me->uTxBufferTail = 0;
			me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_IDLE;
		}
	}
		
	me->TxMsgCtrl.Crc = uCrc; //update static member
	
	return uRetVal;
}

UINT8 Stbp_GetUint8(CStbp * me)
{
	UINT8 uRetVal = me->pRxBuffer[me->uRxBufferTail];
	me->uRxBufferTail += sizeof(UINT8);
	return uRetVal;
}

INT8 Stbp_GetInt8(CStbp * me)
{
	INT8 uRetVal = (INT8)me->pRxBuffer[me->uRxBufferTail];
	me->uRxBufferTail += sizeof(INT8);
	return uRetVal;
}

UINT16 Stbp_GetUint16(CStbp * me)
{
	UINT16 uRetVal;
    UINT8 *pRetVal = (UINT8 *)&uRetVal;
	if (m_bIsBigEndian)
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail + 1];
	} 
	else
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail + 1];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail];
	}
	me->uRxBufferTail += sizeof(UINT16);
	return uRetVal;
}

INT16 Stbp_GetInt16(CStbp * me)
{
	INT16 iRetVal;
    INT8 *pRetVal = (INT8 *)&iRetVal;
	if (m_bIsBigEndian)
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail + 1];
	} 
	else
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail + 1];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail];
	}
	me->uRxBufferTail += sizeof(INT16);
	return iRetVal;
}

UINT32 Stbp_GetUint32(CStbp * me)
{
	UINT32 uRetVal;
    UINT8 *pRetVal = (UINT8 *)&uRetVal;
	if (m_bIsBigEndian)
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail + 1];
		pRetVal[2] = me->pRxBuffer[me->uRxBufferTail + 2];
		pRetVal[3] = me->pRxBuffer[me->uRxBufferTail + 3];
	} 
	else
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail + 3];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail + 2];
		pRetVal[2] = me->pRxBuffer[me->uRxBufferTail + 1];
		pRetVal[3] = me->pRxBuffer[me->uRxBufferTail];
	}
	me->uRxBufferTail += sizeof(UINT32);
	return uRetVal;
}

INT32 Stbp_GetInt32(CStbp * me)
{
	INT32 iRetVal;
    INT8 *pRetVal = (INT8 *)&iRetVal;
	if (m_bIsBigEndian)
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail + 1];
		pRetVal[2] = me->pRxBuffer[me->uRxBufferTail + 2];
		pRetVal[3] = me->pRxBuffer[me->uRxBufferTail + 3];
	} 
	else
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail + 3];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail + 2];
		pRetVal[2] = me->pRxBuffer[me->uRxBufferTail + 1];
		pRetVal[3] = me->pRxBuffer[me->uRxBufferTail];
	}
	me->uRxBufferTail += sizeof(UINT32);
	return iRetVal;
}

UINT64 Stbp_GetUint64(CStbp * me)
{
	INT64 iRetVal;
    INT8 *pRetVal = (INT8 *)&iRetVal;
	if (m_bIsBigEndian)
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail + 1];
		pRetVal[2] = me->pRxBuffer[me->uRxBufferTail + 2];
		pRetVal[3] = me->pRxBuffer[me->uRxBufferTail + 3];
		pRetVal[4] = me->pRxBuffer[me->uRxBufferTail + 4];
		pRetVal[5] = me->pRxBuffer[me->uRxBufferTail + 5];
		pRetVal[6] = me->pRxBuffer[me->uRxBufferTail + 6];
		pRetVal[7] = me->pRxBuffer[me->uRxBufferTail + 7];
	}
	else
	{
		pRetVal[0] = me->pRxBuffer[me->uRxBufferTail + 7];
		pRetVal[1] = me->pRxBuffer[me->uRxBufferTail + 6];
		pRetVal[2] = me->pRxBuffer[me->uRxBufferTail + 5];
		pRetVal[3] = me->pRxBuffer[me->uRxBufferTail + 4];
		pRetVal[4] = me->pRxBuffer[me->uRxBufferTail + 3];
		pRetVal[5] = me->pRxBuffer[me->uRxBufferTail + 2];
		pRetVal[6] = me->pRxBuffer[me->uRxBufferTail + 1];
		pRetVal[7] = me->pRxBuffer[me->uRxBufferTail];
	}
	me->uRxBufferTail += sizeof(UINT64);
	return iRetVal;
}

UINT16 Stbp_GetString(CStbp * me, CHAR* pchDest, UINT16 uMaxLen)
{
	UINT16 uCnt, uStrLen;
	//get length and saturate it to uMaxLen
	uStrLen = Stbp_GetUint16(me);
	if ((NULL != pchDest) && (uStrLen <= uMaxLen))
	{
		for (uCnt=0;uCnt<uStrLen;uCnt++)
		{
			pchDest[uCnt] = me->pRxBuffer[me->uRxBufferTail + uCnt];
		}
		pchDest[uCnt] = 0x00; //null char to terminate the string
		me->uRxBufferTail += uCnt;
	}
	else
	{
		me->uRxBufferTail -= 2; //roll-back the "Stbp_GetUint16()" increment
	}
	return uStrLen;
}

BOOL Stbp_NewMsg(CStbp * me, EStbpConf eConf, EStbpAppId eAppId, UINT8 uMsgId)
{
	BOOL bRetVal = FALSE;
	
	if (me->TxMsgCtrl.eTxState == ESTBP_TXSTATE_IDLE)
	{
		me->pTxBuffer[0] = (UINT8)eConf;
		me->pTxBuffer[1] = (UINT8)((((UINT16)eAppId)>>8)&0xFF);
		me->pTxBuffer[2] = (UINT8)(((UINT16)eAppId)&0xFF);
		me->pTxBuffer[3] = uMsgId;
		me->uTxBufferHead = 4;
		me->uTxBufferTail = 0;
		me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_BUFFERINIT;
		bRetVal = TRUE;
	}
	
	return bRetVal;
}

BOOL Stbp_NewMsgBusMode(CStbp * me, EStbpConf eConf, EStbpAppId eAppId, UINT8 uMsgId, UINT32 uBusAddress)
{
	BOOL bRetVal = FALSE;
	
	if (me->TxMsgCtrl.eTxState == ESTBP_TXSTATE_IDLE)
	{
		me->pTxBuffer[0] = (UINT8)eConf;
		me->pTxBuffer[1] = (UINT8)((((UINT16)eAppId)>>8)&0xFF);
		me->pTxBuffer[2] = (UINT8)(((UINT16)eAppId)&0xFF);
		me->pTxBuffer[3] = uMsgId;
		switch(eConf)
		{
			case ESTBP_CONF_BUS_8BIT:
			{
				me->pTxBuffer[4] = (UINT8)(uBusAddress&0xFF);
				me->uTxBufferHead = 5;
				bRetVal = TRUE;
			}
			break;
			
			case ESTBP_CONF_BUS_16BIT:
			{
				me->pTxBuffer[4] = (UINT8)(((uBusAddress)>>8)&0xFF);
				me->pTxBuffer[5] = (UINT8)((uBusAddress)&0xFF);
				me->uTxBufferHead = 6;
				bRetVal = TRUE;
			}
			break;
			
			default:
			{
				me->uTxBufferHead = 4;
			}
		}
		
		me->uTxBufferTail = 0;
		me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_BUFFERINIT;
	}
	
	return bRetVal;
}

BOOL Stbp_NewMsgChainMode(	CStbp * me, EStbpConf eConf, EStbpAppId eAppId, UINT8 uMsgId,
							UINT16 uSrcAddress, UINT16 uDestAddress, UINT16 uNodeDepth)
{
	BOOL bRetVal = FALSE;

	if ((me->TxMsgCtrl.eTxState == ESTBP_TXSTATE_IDLE) && (ESTBP_CONF_CHAIN_16BIT == eConf))
	{
		me->pTxBuffer[0] = ESTBP_CONF_CHAIN_16BIT;
		me->pTxBuffer[1] = (UINT8)((((UINT16)eAppId)>>8)&0xFF);
		me->pTxBuffer[2] = (UINT8)(((UINT16)eAppId)&0xFF);
		me->pTxBuffer[3] = uMsgId;
		me->pTxBuffer[4] = (UINT8)(((uSrcAddress)>>8)&0xFF);
		me->pTxBuffer[5] = (UINT8)((uSrcAddress)&0xFF);
		me->pTxBuffer[6] = (UINT8)(((uDestAddress)>>8)&0xFF);
		me->pTxBuffer[7] = (UINT8)((uDestAddress)&0xFF);
		me->pTxBuffer[8] = (UINT8)(((uNodeDepth)>>8)&0xFF);
		me->pTxBuffer[9] = (UINT8)((uNodeDepth)&0xFF);
		me->uTxBufferHead = 10;
		bRetVal = TRUE;

		me->uTxBufferTail = 0;
		me->TxMsgCtrl.eTxState = ESTBP_TXSTATE_BUFFERINIT;
	}

	return bRetVal;
}

void Stbp_PutUint8(CStbp * me, UINT8 uData)
{
	me->pTxBuffer[me->uTxBufferHead] = uData;
	me->uTxBufferHead += sizeof(UINT8);
}

void Stbp_PutInt8(CStbp * me, INT8 iData)
{
	me->pTxBuffer[me->uTxBufferHead] = iData;
	me->uTxBufferHead += sizeof(INT8);
}

void Stbp_PutUint16(CStbp * me, UINT16 uData)
{
	UINT8 *pConvertVal = (UINT8 *)&uData;
	if (m_bIsBigEndian)
	{
		me->pTxBuffer[me->uTxBufferHead] = pConvertVal[0];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[1];
	} 
	else
	{
		me->pTxBuffer[me->uTxBufferHead] = pConvertVal[1];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[0];
	}
	me->uTxBufferHead += sizeof(UINT16);
}

void Stbp_PutInt16(CStbp * me, INT16 iData)
{
	INT8 *pConvertVal = (INT8 *)&iData;
	if (m_bIsBigEndian)
	{
		me->pTxBuffer[me->uTxBufferHead] = pConvertVal[0];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[1];
	} 
	else
	{
		me->pTxBuffer[me->uTxBufferHead] = pConvertVal[1];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[0];
	}
	me->uTxBufferHead += sizeof(INT16);
}

void Stbp_PutUint32(CStbp * me, UINT32 uData)
{
	UINT8 *pConvertVal = (UINT8 *)&uData;
	if (m_bIsBigEndian)
	{
		me->pTxBuffer[me->uTxBufferHead] = pConvertVal[0];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[1];
		me->pTxBuffer[me->uTxBufferHead + 2] = pConvertVal[2];
		me->pTxBuffer[me->uTxBufferHead + 3] = pConvertVal[3];
	} 
	else
	{
		me->pTxBuffer[me->uTxBufferHead] = pConvertVal[3];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[2];
		me->pTxBuffer[me->uTxBufferHead + 2] = pConvertVal[1];
		me->pTxBuffer[me->uTxBufferHead + 3] = pConvertVal[0];
	}
	me->uTxBufferHead += sizeof(UINT32);
}

void Stbp_PutInt32(CStbp * me, INT32 iData)
{
	INT8 *pConvertVal = (INT8 *)&iData;
	if (m_bIsBigEndian)
	{
		me->pTxBuffer[me->uTxBufferHead] = pConvertVal[0];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[1];
		me->pTxBuffer[me->uTxBufferHead + 2] = pConvertVal[2];
		me->pTxBuffer[me->uTxBufferHead + 3] = pConvertVal[3];
	} 
	else
	{
		me->pTxBuffer[me->uTxBufferHead] = pConvertVal[3];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[2];
		me->pTxBuffer[me->uTxBufferHead + 2] = pConvertVal[1];
		me->pTxBuffer[me->uTxBufferHead + 3] = pConvertVal[0];
	}
	me->uTxBufferHead += sizeof(INT32);
}

void Stbp_PutUint64(CStbp * me, UINT64 uData)
{
	UINT8 *pConvertVal = (UINT8 *)&uData;
	if (m_bIsBigEndian)
	{
		me->pTxBuffer[me->uTxBufferHead]     = pConvertVal[0];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[1];
		me->pTxBuffer[me->uTxBufferHead + 2] = pConvertVal[2];
		me->pTxBuffer[me->uTxBufferHead + 3] = pConvertVal[3];
		me->pTxBuffer[me->uTxBufferHead + 4] = pConvertVal[4];
		me->pTxBuffer[me->uTxBufferHead + 5] = pConvertVal[5];
		me->pTxBuffer[me->uTxBufferHead + 6] = pConvertVal[6];
		me->pTxBuffer[me->uTxBufferHead + 7] = pConvertVal[7];
	}
	else
	{
		me->pTxBuffer[me->uTxBufferHead]     = pConvertVal[7];
		me->pTxBuffer[me->uTxBufferHead + 1] = pConvertVal[6];
		me->pTxBuffer[me->uTxBufferHead + 2] = pConvertVal[5];
		me->pTxBuffer[me->uTxBufferHead + 3] = pConvertVal[4];
		me->pTxBuffer[me->uTxBufferHead + 4] = pConvertVal[3];
		me->pTxBuffer[me->uTxBufferHead + 5] = pConvertVal[2];
		me->pTxBuffer[me->uTxBufferHead + 6] = pConvertVal[1];
		me->pTxBuffer[me->uTxBufferHead + 7] = pConvertVal[0];
	}
	me->uTxBufferHead += sizeof(UINT64);
}

void Stbp_PutString(CStbp * me, CHAR* pchData, UINT16 uMaxLen)
{
	UINT16 uLengthData = 0;
	UINT16 uLoopIndex = 0;
	//get string length without using string.h
	while((uLengthData<uMaxLen) /*&& (*(pchData+uLengthData) != 0x00)*/)
	{
		uLengthData++;
	}
	//fill buffer
	Stbp_PutUint16(me, uLengthData);
	for (uLoopIndex = 0; uLoopIndex < uLengthData ; uLoopIndex++)
	{
		me->pTxBuffer[me->uTxBufferHead] = pchData[uLoopIndex];
		me->uTxBufferHead += sizeof(CHAR);
	}	
}

UINT16 Stbp_EncodeMessage(CStbp* me, UINT8* puBuffer)
{
	UINT16 uLength = 0;

	while (Stbp_IsDataToTransmit(me))
	{
		puBuffer[uLength] = Stbp_GetNextByteToSend(me);

		uLength++;
	}

	return uLength;
}

BOOL Stbp_DecodeMessage(CStbp* me, UINT16 uLength, UINT8* puBuffer)
{
	BOOL bRetVal = FALSE;
	UINT16 ii;

	for (ii = 0; ii < uLength; ii++)
	{
		Stbp_ReceiveByte(me, puBuffer[ii]);
	}
	//reset control flag
	if (me->bFrameOk)
	{
		bRetVal = TRUE;
		me->bFrameOk = FALSE;
	}

	return bRetVal;
}
