/**
 * @file    stbp.h
 *
 * @brief   Swiss Timing Binary Protocol
 *			http://swtchcm006/redmine/projects/stbp/wiki
 *			
 * @author  Arnaud Brielmann
 * 
 * SVN:     $URL: svn://swtchcm006/Timing_&_Scoring/Components/SVN/CommonUtils/STBP/lib_c/trunk/stbp.h $
 *
 *          $Revision: 281 $
 *
 *          $LastChangedBy: arnaud_brielmann $
 *
 *          $LastChangedDate: 2014-09-09 10:31:01 +0200 (mar., 09 sept. 2014) $
 *
 *          SWISS TIMING Ltd. copyright 2011 (c).
 ******************************************************************/


#ifndef STBP_H_
#define STBP_H_

// TYPE DEFINITIONS
#include "stddef.h"
#include "def.h"

#define ESTBP_CRC_POLY  (UINT8)0x4D 				///< CRC Polynomial P(x)=x^8+x^6+x^3+x^2+1 = 0x14D

#define ESTBP_HEADER_LENGTH_NOSUB			4		///< Length in bytes of header when conf=0
#define ESTBP_HEADER_LENGTH_BUS8BIT			5		///< Length in bytes of header when conf=1
#define ESTBP_HEADER_LENGTH_BUS16BIT		6		///< Length in bytes of header when conf=2
#define ESTBP_HEADER_LENGTH_CHAIN16BIT		10		///< Length in bytes of header when conf=3

/**
* @enum EStbpControlBytes
*
* @brief Control bytes of STBP framing.
*/
typedef enum{
	ESTBP_CONTROLBYTE_END = (UINT8)0x00,			///< End of message flag
	ESTBP_CONTROLBYTE_START = (UINT8)0x01,			///< Start of message flag
	ESTBP_CONTROLBYTE_ESC = (UINT8)0x02				///< Escape byte flag
}EStbpControlBytes; 

/**
* @enum EStbpConf
*
* @brief Configuration byte of STBP.
*/
typedef enum{
	ESTBP_CONF_NOSUBHEADER				= (UINT8)0,
	ESTBP_CONF_BUS_8BIT					= (UINT8)1,
	ESTBP_CONF_BUS_16BIT				= (UINT8)2,		///< http://swtchcm006/redmine/projects/stbp/wiki/Bus_mode_16-bit
	ESTBP_CONF_CHAIN_16BIT				= (UINT8)3		///< http://swtchcm006/redmine/projects/stbp/wiki/Chain_mode_16-bit
}EStbpConf; 

/**
* @enum EStbpAppId
*
* @brief Application IDs
* http://swtchcm006/redmine/projects/stbp/wiki/AppID
*/
typedef enum{
	ESTBP_APPID_RESERVED							= (UINT16)0,
	ESTBP_APPID_AS_SB_STARTING_GATES	= (UINT16)1,
	ESTBP_APPID_QUANTUM_AS_SB_TIMING	= (UINT16)2,
	ESTBP_APPID_UPS12VDC 							= (UINT16)3,
	ESTBP_APPID_AIG059								= (UINT16)4,
	ESTBP_APPID_TRA_DECODER_PC				= (UINT16)5,
	ESTBP_APPID_QUANTUM_ENTRY_TERMINAL	= (UINT16)6,
	ESTBP_APPID_IH_TIME_ON_ICE					= (UINT16)7,
	ESTBP_APPID_TRA_ACTIVATOR_DECODER		= (UINT16)8,
	ESTBP_APPID_ASC3										= (UINT16)9,
	ESTBP_APPID_OSV8										= (UINT16)10,
	ESTBP_APPID_UNDERWATER_LAP_COUNTER	= (UINT16)11,
	ESTBP_APPID_TIMING_CY					= (UINT16)12,
	ESTBP_APPID_START_TIME_V			= (UINT16)13,
	ESTBP_APPID_AIG064						= (UINT16)14,
	ESTBP_APPID_CAP082						= (UINT16)15,
	ESTBP_APPID_FreescaleCup			= (UINT16)16,
	ESTBP_APPID_STD_MSG						= (UINT16)252
}EStbpAppId; 

/**
* @enum EStbpTxState
*
* @brief state of STBP TX state machine.
*/
typedef enum{
	ESTBP_TXSTATE_IDLE,
	ESTBP_TXSTATE_BUFFERINIT,
	ESTBP_TXSTATE_SENDBUFFER,
	ESTBP_TXSTATE_SEND_CRC,
	ESTBP_TXSTATE_TERMINATE,
	ESTBP_TXSTATE_SENDINGLASTBYTE
}EStbpTxState; 

/**
* @enum EStbpBusAddr
*
* @brief definitions of bus modes addresses.
*/
typedef enum{
	ESTBP_BUSADDR_BROADCAST = (UINT32)0
}EStbpBusAddr;

/**
* @enum EStbpChainAddr
*
* @brief definitions of chain modes addresses.
*/
typedef enum{
	ESTBP_CHAINADDR_BROADCAST = (UINT16)0
}EStbpChainAddr;

/**
* @struct SStbpChainFields
*
* @brief Definitions of sub-header fields for "Daisy Chain" mode.
*/
typedef struct{
	UINT16			SrcAddress;			///< Source Address
	UINT16			DestAddress;		///< Destination Address, 0 for broadcast
	UINT16			NodeDepth;			///< Node Depth,set to 0 when the message is sent at first and incremented when the message is forwarded
}SStbpChainFields;


/**
* @struct SStbpMsgInfo
*
* @brief RX message control fields.
*/
typedef struct {
	EStbpConf			Conf;			///< Defines what is contained in the sub-header field
	EStbpAppId			AppID;			///< Application ID (can be unique for sports or for type of device)
	UINT8 				MsgID;			///< Message ID
	UINT8				Crc;			///< CRC is computed from the encoded frame without "start" and "end" flag bytes.
	UINT8				Crc_t1;			///< CRC at previous iteration (shift register). Compared with sent value when CRC encoded over one byte
	UINT8				Crc_t2;			///< CRC 2 iterations before (shift register). Compared with sent value when CRC encoded over two bytes
	UINT32				BusAddress;		///< Used in bus modes only.  
	SStbpChainFields	ChainFields;	///< Used in Daisy Chain mode only.  
}SStbpMsgInfo;

/**
* @struct SStbpTxMsgCtrl
*
* @brief TX message control fields.
*/
typedef struct {
	UINT8			Crc;			///< CRC is computed from the encoded frame without "start" and "end" flag bytes.
	EStbpTxState	eTxState;		///< state of STBP TX state machine
}SStbpTxMsgCtrl;

/**
* @class CStbp
*
* @brief STBP Communication instance.
*/
typedef struct {
	UINT8* pRxBuffer;				///<  pointer to the reception buffer.
	UINT16 uRxBufferHead;           ///<  Byte position in current receive buffer to put next rxed byte	
	UINT16 uRxBufferTail;           ///<  Byte position in current receive buffer to read next field
	UINT16 uRxBufferSize;			///<  byte nb of RX buffer.
	UINT8* pTxBuffer;				///<  pointer to the emission buffer.
	UINT16 uTxBufferHead;			///<  byte position, in tx buffer, where to put the next byte
	UINT16 uTxBufferTail;			///<  position of the first byte in tx buffer
	UINT16 uTxBufferSize;			///<  byte nb of TX buffer.
	BOOL bIsReceiving;				///<  Current state of reception
	BOOL bEscapeByteRxed;			///<  Latest byte rxed was ESC
	BOOL bEscapeByteTxed;			///<  Latest byte txed was ESC
	BOOL bFrameOk;					///<  RXed frame is good (CRC, IDs, ...)
	SStbpMsgInfo RxMsgInfo;			///<  Filled during "on the fly" reception
	SStbpTxMsgCtrl TxMsgCtrl;		///<  Used by TX state machine
}CStbp;

// DECLARATION OF PUBLIC VARIABLES AND FUNCTIONS/METHODS

/**
* @brief Initialize the "object" STBP Communication instance.
*
* @param me : pointer to the "object" (STBP Communication instance)
* @param pRxBuffer : pointer to the reception buffer.
* @param uRxBufferSize : byte nb of RX buffer.
* @param pTxBuffer : pointer to the emission buffer.
* @param uTxBufferSize : byte nb of TX buffer.
*
* @return void
*/
void Stbp_InitSw(	CStbp * me, 
					UINT8 * pRxBuffer,
					UINT16 uRxBufferSize,
					UINT8 * pTxBuffer,
					UINT16 uTxBufferSize);

								
/**
* @brief Must be called for each received byte (expl: by ISR "UART Receive Complete")
*
* @param me : pointer to the "object" (STBP Communication instance)
* @param uRxByte : byte RXed 
*
* @return void
*/
void Stbp_ReceiveByte(CStbp * me, UINT8 uRxByte);

/**
* @brief returns current state of TX encoder. Do not call Stbp_NewMsg if Stbp_IsDataToTransmit returns TRUE
*
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return TRUE if TX encoder has data to be transmitted. FALSE otherwise
*/
BOOL Stbp_IsDataToTransmit(CStbp * me);

/**
* @brief returns TRUE if a new frame has been decoded
* Can be called in a loop and returns true only once per arriving frame 
*
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return TRUE if a new frame has been decoded
*/
BOOL Stbp_IsNewFrame(CStbp * me);

/**
* @brief Generates one byte of the encoded frame to be sent
* Must be called for every byte to send (expl: UART DataRegEmpty ISR) 
* Encoding in 253-basis and CRC computing are done "on-the-fly" 
*
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return byte to send
*/
UINT8 Stbp_GetNextByteToSend(CStbp * me);

/**
* @brief Fetch one byte from RX buffer and increment buffer index
* 
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return data byte 
*/
UINT8 Stbp_GetUint8(CStbp * me);

/**
* @brief Fetch one byte from RX buffer and increment buffer index
* 
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return data byte 
*/
INT8 Stbp_GetInt8(CStbp * me);

/**
* @brief Fetch one 16-bit word from RX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
* 
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return data word 
*/
UINT16 Stbp_GetUint16(CStbp * me);

/**
* @brief Fetch one 16-bit word from RX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
* 
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return data word 
*/
INT16 Stbp_GetInt16(CStbp * me);

/**
* @brief Fetch one 32-bit double word from RX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
* 
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return data word 
*/
UINT32 Stbp_GetUint32(CStbp * me);

/**
* @brief Fetch one 32-bit double word from RX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
* 
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return data double word 
*/
INT32 Stbp_GetInt32(CStbp * me);

/**
* @brief Fetch one 64-bit unsigned integer from RX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
*
* @param me : pointer to the "object" (STBP Communication instance)
*
* @return data word
*/
UINT64 Stbp_GetUint64(CStbp * me);

/**
* @brief Fetch a string from RX buffer and increment buffer index
* Convert "C string" terminated by a null char
* If called with pchDest=NULL : returns the length of the string but 
* doesn't increment the index and doesn't copy any data
* 
* @param me : pointer to the "object" (STBP Communication instance)
* @param pchDest : pointer to the destination
* @param uMaxLen : Size of allocated memory for pchDest
*
* @return length of the string
*/
UINT16 Stbp_GetString(CStbp * me, CHAR* pchDest, UINT16 uMaxLen);

/**
* @brief Initialize a new message in TX buffer
* Must not be called when TX encoder not ready
* 
* @param me : pointer to the "object" (STBP Communication instance)
* @param eConf : Protocol Configuration byte
* @param eAppId : Protocol Application ID
* @param uMsgId : Protocol Message ID
*
* @return true if successful, false if not
*/
BOOL Stbp_NewMsg(CStbp * me, EStbpConf eConf, EStbpAppId eAppId, UINT8 uMsgId);

/**
* @brief Initialize a new message in TX buffer with slave address
* Must not be called when TX encoder not ready
*
* @param me : pointer to the "object" (STBP Communication instance)
* @param eConf : Protocol Configuration byte
* @param eAppId : Protocol Application ID
* @param uMsgId : Protocol Message ID
* @param uSlaveAddress : Protocol Slave Address	
*
* @return true if successful, false if not
*/
BOOL Stbp_NewMsgBusMode(CStbp * me, EStbpConf eConf, EStbpAppId eAppId, UINT8 uMsgId, UINT32 uSlaveAddress);

/**
* @brief Initialize a new message in TX buffer with "Daisy Chain" Sub-header
* Must not be called when TX encoder not ready
*
* @param me : pointer to the "object" (STBP Communication instance)
* @param eConf : Protocol Configuration byte
* @param eAppId : Protocol Application ID
* @param uMsgId : Protocol Message ID
* @param uSrcAddress : Protocol Source Address
* @param uDestAddress : Protocol Destination Address, 0 for broadcasts
* @param uNodeDepth : Protocol Node Depth, set to 0 when the message is sent at first and incremented when the message is forwarded
*
* @return true if successful, false if not
*/
BOOL Stbp_NewMsgChainMode(	CStbp * me, EStbpConf eConf, EStbpAppId eAppId, UINT8 uMsgId,
							UINT16 uSrcAddress, UINT16 uDestAddress, UINT16 uNodeDepth);

/**
* @brief Put one byte in TX buffer and increment buffer index
* 
* @param me : pointer to the "object" (STBP Communication instance)
* @param uData : data byte
*
* @return void 
*/
void Stbp_PutUint8(CStbp * me, UINT8 uData);

/**
* @brief Put one byte in TX buffer and increment buffer index
* 
* @param me : pointer to the "object" (STBP Communication instance)
* @param iData : data byte
*
* @return void 
*/
void Stbp_PutInt8(CStbp * me, INT8 iData);

/**
* @brief Put one 16-bit word in TX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
* 
* @param me : pointer to the "object" (STBP Communication instance)
* @param uData : data 
*
* @return void 
*/
void Stbp_PutUint16(CStbp * me, UINT16 uData);

/**
* @brief Put one 16-bit word in TX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
* 
* @param me : pointer to the "object" (STBP Communication instance)
* @param iData : data 
*
* @return void 
*/
void Stbp_PutInt16(CStbp * me, INT16 iData);

/**
* @brief Put one 32-bit double word in TX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
* 
* @param me : pointer to the "object" (STBP Communication instance)
* @param uData : data 
*
* @return void 
*/
void Stbp_PutUint32(CStbp * me, UINT32 uData);

/**
* @brief Put one 32-bit double word in TX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
* 
* @param me : pointer to the "object" (STBP Communication instance)
* @param iData : data 
*
* @return void 
*/
void Stbp_PutInt32(CStbp * me, INT32 iData);

/**
* @brief Put one 64-bit unsigned integer in TX buffer and increment buffer index
* Endian-independent code (bytes are swapped for little-endian platforms)
*
* @param me : pointer to the "object" (STBP Communication instance)
* @param uData : data
*
* @return void
*/
void Stbp_PutUint64(CStbp * me, UINT64 uData);

/**
* @brief Put a string to TX buffer and increment buffer index
* Convert to "C# string" a "C string" terminated by a null char
*
* @param me : pointer to the "object" (STBP Communication instance)
* @param pchData : pointer to data
* @param uMaxLen : Size of allocated memory for pchData
*
*/
void Stbp_PutString(CStbp * me, CHAR* pchData, UINT16 uMaxLen);

/**
 * @brief Encodes the message in pTxBuffer to be sent. This function is an
 *        alternative to the on-the-fly encoding implemented in Stbp_GetNextByteToSend.
 *
 *        This function uses internally the Tx State Machine and Stbp_GetNextByteToSend
 *        to encode the message.
 *
 * @param me : pointer to the "object" (STBP Communication instance)
 * @param puBuffer : buffer to hold the encoded message. The buffer needs to have
 *                   a length of at least [StartByte + Header + 2*Payload + 2*CRC + StopByte] bytes
 *                   The function does not check if the the length is sufficient.
 *
 * @return Length of the encoded message
 */
UINT16 Stbp_EncodeMessage(CStbp* me, UINT8* puBuffer);

/**
 * @brief Decodes a message into RxMsgInfo and pRxBuffer. This function is an
 *        alternative to the on-the-fly decoding implemented in Stbp_ReceiveByte.
 *
 *        This function uses internally the Rx State Machine and Stbp_ReceiveByte
 *        to decode the message.
 *
 * @param me : pointer to the "object" (STBP Communication instance)
 * @param uLength : Length of the message to be decoded
 * @param puBuffer : Buffer that holds the message to be encoded.
 *
 * @return Flag indicating if the message was decoded sucessfully (i.e. CRC is correct)
 */
BOOL Stbp_DecodeMessage(CStbp* me, UINT16 uLength, UINT8* puBuffer);


#endif /* STBP_H_ */
