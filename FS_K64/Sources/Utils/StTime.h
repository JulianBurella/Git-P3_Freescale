/**
 * @file    StTime.h
 *
 * @brief   C support for the data of the DRC class StTime
 *
 * @author  Reto Galli
 * 
 * SVN:     $URL: svn://swtchcm006/Timing_&_Scoring/Components/SVN/CommonUtils/STBP/lib_c/trunk/StTime.h $
 *
 *          $Revision: 234 $
 *
 *          $LastChangedBy: stephan_aronson $
 *
 *          $LastChangedDate: 2014-02-25 10:25:42 +0100 (mar., 25 févr. 2014) $
 *
 *          SWISS TIMING Ltd. copyright 2012-2013 (c)
 *
 *****************************************************************************/
#ifndef STTIME_H_
#define STTIME_H_

//#include "Typedef.h"
#include "def.h"
#include "stddef.h"

 

/**
 * @brief  Definition of available precisions for StTime
 */
typedef enum EStTimePrecision
{
	E_ST_TIME_PRECISION_1MS = 0,	///< Precision: 1ms
	E_ST_TIME_PRECISION_0P1MS = 1,  ///< Precision: 0.1ms
	E_ST_TIME_PRECISION_1US = 2     ///< Precision: 1us
} EStTimePrecision;




/**
 * @brief  Bit format of StTime compatible with C compilers. \n
 *         Note: In the C++ PC version the bits are defined as below. However, some C compiler do not
 *               accept bitfields based on UINT64. Thus, the definition was changed to UINT32.
 *               This should be supported starting from C standard definition C99. To access
 *               TimeHex it is necessary to use the functions StTime_GetTimeHex() and StTime_SetTimeHex().
 *
 *               @code
 *               // C++ definition of StTimeBits:
 *               typedef	struct StTimeBits
 *               {
 *                   UINT64 TimeHex      : 43;   // 43 bit value of the time represented by this instance
 *                   UINT64 Precision    : 3;    // Precision of the time represented by this instance (0: 1 ms, 1: 0.1 ms, 2: 1 us, 3..7: reserved)
 *                   UINT64 IsNull       : 1;    // Defines if this time is a NULL instance (0: No, 1: Yes)
 *                   UINT64 IsNegative   : 1;    // Sign of the time represented by this instance (0: Positive, 1: Negative)
 *               } StTimeBits;
 *               @endcode
 *********************************************************************************************/
typedef	struct StTimeBits
{
	UINT32 TimeHexLow;          ///< Lower 32 bit of the 43 bit value of the time represented by this instance

	UINT32 TimeHexHigh  : 11;   ///< Upper 11 bit of the 43 bit value of the time represented by this instance
	UINT32 Precision    : 3;    ///< Precision of the time represented by this instance (0: 1 ms, 1: 0.1 ms, 2: 1 us, 3..7: reserved)
	UINT32 IsNull       : 1;    ///< Defines if this time is a NULL instance (0: No, 1: Yes)
	UINT32 IsNegative   : 1;    ///< Sign of the time represented by this instance (0: Positive, 1: Negative)
} StTimeBits;




/**
 * @class StTime
 * @brief Definition of the class data of StTime
 *********************************************************************************************/
typedef union StTime
{
	StTimeBits f;     ///< Access via bit field
	UINT64     Val;   ///< Access via 64bit unsigned integer
} StTime;



/**
 * @brief  Initialize a StTime instance to 0
 *
 * @param  me : Context pointer to the current StTime instance
 * @param  ePrecision : Precision value that is set in StTime (0: 1ms, 1: 0.1ms 2: 1 us)
 *
 * @return void
 *********************************************************************************************/
void StTime_Init0(StTime* me, EStTimePrecision ePrecision);



/**
 * @brief  Initialize a StTime instance to a timer value
 *
 * @param  me : Context pointer to the current StTime instance
 * @param  ePrecision : Precision of the time value (0: 1ms, 1: 0.1ms 2: 1 us).
 *                      This precision is also set in the StTime structure.
 * @param  uTimestamp : Timer value
 * @param  bIsNegative : Flag indicating if this is a positive or a negative time
 *
 * @return void
 *********************************************************************************************/
void StTime_InitTimestamp(StTime* me, EStTimePrecision ePrecision, UINT64 uTimestamp, BOOL bIsNegative);



/**
 * @brief  Initialize a StTime instance to a time. \n
 *         Note: Values of hours, minutes, seconds,... are not verified on their validity.
 *
 * @param  me : Context pointer to the current StTime instance
 * @param  ePrecision : Precision / unit of the fractions field (0: 1ms, 1: 0.1ms 2: 1 us).
 *                      This precision is also set in the StTime structure.
 * @param  uHours : Hours value of the time
 * @param  uMinutes : Minutes value of the time
 * @param  uSeconds : Seconds value of the time
 * @param  uFractions : Fractions of a second, according to the value in uPrecision
 *
 * @return void
 *********************************************************************************************/
void StTime_InitTime(StTime* me, EStTimePrecision ePrecision, UINT8 uHours, UINT8 uMinutes, UINT8 uSeconds, UINT32 uFractions);



/**
 * @brief  Return the counter value TimeHex of the StTime structure. \n
 *         Note: This function is necessary, because it is not possible to define TimeHex
 *         directly as a 43 bit bitfield in some C compilers.
 *
 * @param  me : Context pointer to the current StTime instance
 *
 * @return UINT64 : 43 bit counter value
 *********************************************************************************************/
UINT64 StTime_GetTimeHex(StTime* me);



/**
 * @brief  Return the counter value TimeHex of the StTime structure. \n
 *         Note: This function is necessary, because it is not possible to define TimeHex
 *         directly as a 43 bit bitfield in some C compilers.
 *
 * @param  me : Context pointer to the current StTime instance
 * @param  uTimeHex : 43 bit counter value
 *
 * @return void
 *********************************************************************************************/
void StTime_SetTimeHex(StTime* me, UINT64 uTimeHex);



/**
 * @brief  Return the time (hours, minutes, seconds, fractions) from an StTime instance
 *
 * @param  me : Context pointer to the current StTime instance
 * @param  ePrecision : Desired precision / unit of the fractions field (0: 1ms, 1: 0.1ms 2: 1 us).
 *                      This can be different from the precision in StTime
 * @param  puHours : Variable to receive hours value of the time
 * @param  puMinutes : Variable to receive minutes value of the time
 * @param  puSeconds : Variable to receive seconds value of the time
 * @param  puFractions : Variable to receive fractions of a second, according to the value in uPrecision
 *
 * @return BOOL : true if successful (IsNull is not set in the instance of StTime)
 *********************************************************************************************/
BOOL StTime_GetTime(StTime* me, EStTimePrecision ePrecision, UINT8* puHours, UINT8* puMinutes, UINT8* puSeconds, UINT32* puFractions);



/**
 * @brief  Add another instance of StTime to the current instance. \n
 *         Note: The precision of the result will be identical to the precision of the
 *         me instance. Time will be shifted accordingly
 *
 * @param  me : Context pointer to the current StTime instance
 * @param  Time : Instance of StTime to add
 * @param  b24hWrapAround : Defines if the result wraps around when above 23:59:59.999999 or below 00:00:00.000000
 *                          TRUE: The result is always a time between 0h and 24h
 *                          FALSE: The result might be negative or above 24h
 *
 * @return BOOL : true if successful (IsNull is not set in both instances of StTime)
 *********************************************************************************************/
BOOL StTime_Add(StTime* me, StTime Time, BOOL b24hWrapAround);



/**
 * @brief  Invert the sign of an StTime instance
 *
 * @param  me : Context pointer to the current StTime instance
 *
 * @return  void
 *********************************************************************************************/
void StTime_InvertSign(StTime* me);



/**
 * @brief  Compare another instance of StTime to the current instance \n
 *         Note: The precision of the other instance is converted to the precision of the current instance
 *         before the comparison. In this conversion the value is always rounded towards 0
 *
 * @param  me : Context pointer to the current StTime instance
 * @param  Time : Instance of StTime to compare
 *
 * @return BOOL : TRUE if the times are identical, FALSE if not or any of the instances is not a valid time (isNull == TRUE)
 **********************************************************************************************/
BOOL StTime_Compare(StTime* me, StTime Time);



#endif /* STTIME_H_ */
