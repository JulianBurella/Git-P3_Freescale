/**
* @file    SbLinkDefinitions.h
*
* @brief   Definitions for AT Starting Block
*
* @author  Michele Korell
*
*          Swiss Timing LTD
*
*          Copyright (c) Swiss Timing LTD 2013 All right reserved
*
******************************************************************/

#ifndef SbLinkDefinitions_H
#define SbLinkDefinitions_H

#include "Typedef.h"



#define STARTING_BLOCK_SAMPLE_SIZE      (500u)                  ///< Number of sample of the Starting Block force curve
#define STARTING_BLOCK_MSG_SIZE         (251u)                  ///< Size in bytes of a Starting Block message

#define STARTING_BLOCK_ADDRESS_MASTER           (0x0000)        ///< Master address of Starting Block link
#define STARTING_BLOCK_ADDRESS_BROADCAST        (0xFFFF)        ///< Broadcast addreass of Starting Block link

#define STARTING_BLOCK_DATABLOCK_SIZE   (125u)                  ///< Size in short of a Starting Block datablock


/**
* @enum EStartingBlockMsgID
*
* @brief IDs of messages on Starting Block link
**************************************/
typedef enum EStartingBlockMsgID
{
  E_STARTING_BLOCK_MSGID_GET_VERSION = 1,               ///< Request version of Starting Block firmware
  E_STARTING_BLOCK_MSGID_VERSION = 2,                   ///< Version of Staring Block firmware
  E_STARTING_BLOCK_MSGID_GET_STATUS = 3,                ///< Request status of a Starting Block
  E_STARTING_BLOCK_MSGID_STATUS = 4,                    ///< Status of a Starting Block
  E_STARTING_BLOCK_MSGID_GET_PRETENSION = 5,            ///< Request pretension of a Starting Block
  E_STARTING_BLOCK_MSGID_PRETENSION = 6,                ///< Pretension of a Starting Block
  E_STARTING_BLOCK_MSGID_CONFIGURATION = 10,            ///< Reaction time configuration of a Starting Block
  E_STARTING_BLOCK_MSGID_CONFIRM_CONFIGURATION = 11,    ///< Confirm the reaction time configuration of a Starting Block
  E_STARTING_BLOCK_MSGID_START = 12,                    ///< Race start message
  E_STARTING_BLOCK_MSGID_CONFIRM_START = 13,            ///< Confirmation of a race start message
  E_STARTING_BLOCK_MSGID_GET_FORCE_CURVE = 14,          ///< Request force curve of a Starting Block
  E_STARTING_BLOCK_MSGID_FORCE_CURVE = 15,              ///< Force curve databloack of a Starting Block
  E_STARTING_BLOCK_MSGID_RESET_START_DATA = 16,         ///< Reset of race start data in a starting block
  E_STARTING_BLOCK_MSGID_CONFIRM_RESET_START_DATA = 17, ///< Confirm reset of a start data
  E_STARTING_BLOCK_MSGID_TYMEBASE_SYNC = 21             ///< Synchronization of the time base
} EStartingBlockMsgID;



/**
* @enum     SSbMessage
*
* @brief   SbLink message structure
**************************************/
typedef struct SSbMessage
{
  UINT16 uAddress;                              ///< Unit address
  EStartingBlockMsgID eMsgID;                   ///< Message ID
  UINT8 *pData;                                 ///< Message buffer
  UINT8 uSize;                                  ///< Message Size
} SSbMessage;



/**
* @struct   SSbData
*
* @brief   Starting block data structure
**************************************/
typedef struct SSbData
{
  INT16 iPretension;                                    ///< Pretension configuration
  INT16 iFcSamples[STARTING_BLOCK_SAMPLE_SIZE];         ///< Force curve samples
  UINT16 uAddress;                                      ///< Unit Address
  UINT16 uTimeStamp;                                    ///< Last known timestamp
  UINT16 uPulseTimestamp;                               ///< Last pulse timestamp
  INT16 iReactionTime;                                  ///< Last reaction time
  UINT8 uStatus;                                        ///< Detailed status bitfield
  UINT8 uGeneralStatus;                                 ///< General status flag
  UINT8 uMajorVersion;                                  ///< Software major version
  UINT8 uMinorVersion;                                  ///< Software minor version
  UINT8 uLostMsg;                                       ///< Count lost messages
  BOOL bReactionTimeDownloaded;                         ///< Flag of reaction time downloaded
  BOOL bForceCurveDownloaded;                           ///< Flag of force curve downloaded
} SSbData;

#endif