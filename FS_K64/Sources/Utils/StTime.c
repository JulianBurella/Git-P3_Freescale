/**
 * @file    StTime.c
 *
 * @brief   C support for the data of the DRC class StTime
 *
 * @author  Reto Galli
 * 
 * SVN:     $URL: svn://swtchcm006/Timing_&_Scoring/Components/SVN/CommonUtils/STBP/lib_c/trunk/StTime.c $
 *
 *          $Revision: 234 $
 *
 *          $LastChangedBy: stephan_aronson $
 *
 *          $LastChangedDate: 2014-02-25 10:25:42 +0100 (mar., 25 f챕vr. 2014) $
 *
 *          SWISS TIMING Ltd. copyright 2012-2013 (c)
 *
 *****************************************************************************/
#include "StTime.h"


#define TICKS_24H_1_MS     (86400000)
#define TICKS_24H_0P1_MS   (864000000)
#define TICKS_24H_1_US     (86400000000)


void StTime_Init0(StTime* me, EStTimePrecision ePrecision)
{
	me->Val = 0;  // sets the fields IsNull, IsNegative to false and TimeHex to 0

	if (ePrecision <= E_ST_TIME_PRECISION_1US)
	{
		me->f.Precision = ePrecision;
	}
	else
	{
		me->f.Precision = E_ST_TIME_PRECISION_1MS;
	}
}




void StTime_InitTimestamp(StTime* me, EStTimePrecision ePrecision, UINT64 uTimestamp, BOOL bIsNegative)
{
	me->Val = 0;  // initialize all fields to 0

	if (ePrecision <= E_ST_TIME_PRECISION_1US)
	{
		me->f.IsNull = 0;

		if (bIsNegative)
		{
			me->f.IsNegative = 1;
		}
		else
		{
			me->f.IsNegative = 0;
		}

		me->f.Precision = ePrecision;

		StTime_SetTimeHex(me, uTimestamp);
	}
	else
	{
		me->f.IsNull = 1;
	}
}




void StTime_InitTime(StTime* me, EStTimePrecision ePrecision, UINT8 uHours, UINT8 uMinutes, UINT8 uSeconds, UINT32 uFractions)
{
	UINT32 uPrecisionFactor;

	me->Val = 0;  // initialize all fields to 0

	if (ePrecision <= E_ST_TIME_PRECISION_1US)
	{
		me->f.IsNull = 0;
		me->f.Precision = ePrecision;
		me->f.IsNegative = 0;

		switch (ePrecision)
		{
			case E_ST_TIME_PRECISION_1MS : uPrecisionFactor = 1000;
			break;

			case E_ST_TIME_PRECISION_0P1MS : uPrecisionFactor = 10000;
			break;

			case E_ST_TIME_PRECISION_1US : uPrecisionFactor = 1000000;
			break;
		}

		StTime_SetTimeHex(me, (((((UINT64)uHours * 60 + (UINT64)uMinutes) * 60) + (UINT64)uSeconds) * (UINT64)uPrecisionFactor) + (UINT64)uFractions);
	}
	else
	{
		me->f.IsNull = 1;
	}
}



UINT64 StTime_GetTimeHex(StTime* me)
{
	return (me->Val & 0x000007FFFFFFFFFF);
}



void StTime_SetTimeHex(StTime* me, UINT64 uTimeHex)
{
	me->f.TimeHexLow = (uTimeHex & 0x00000000FFFFFFFF);
	me->f.TimeHexHigh = ((uTimeHex & 0x000007FF00000000) >> 32);
}



BOOL StTime_GetTime(StTime* me, EStTimePrecision ePrecision, UINT8* puHours, UINT8* puMinutes, UINT8* puSeconds, UINT32* puFractions)
{
	UINT64 uTimestamp;
	UINT32 uPrecisionFactor;

	if ((!me->f.IsNull) && (ePrecision < 3))
	{
		uTimestamp = StTime_GetTimeHex(me);

		switch (ePrecision)
		{
			case E_ST_TIME_PRECISION_1MS :
			{
				uPrecisionFactor = 1000;

				if (me->f.Precision == E_ST_TIME_PRECISION_0P1MS)
				{
					uTimestamp /= 10;
				}
				else if (me->f.Precision == E_ST_TIME_PRECISION_1US)
				{
					uTimestamp /= 1000;
				}
			}
			break;

			case E_ST_TIME_PRECISION_0P1MS :
			{
				uPrecisionFactor = 10000;

				if (me->f.Precision == E_ST_TIME_PRECISION_1MS)
				{
					uTimestamp *= 10;
				}
				else if (me->f.Precision == E_ST_TIME_PRECISION_1US)
				{
					uTimestamp /= 100;
				}
			}
			break;

			case E_ST_TIME_PRECISION_1US :
			{
				uPrecisionFactor = 1000000;

				if (me->f.Precision == E_ST_TIME_PRECISION_1MS)
				{
					uTimestamp *= 1000;
				}
				else if (me->f.Precision == E_ST_TIME_PRECISION_0P1MS)
				{
					uTimestamp *= 100;
				}
			}
			break;
		}

		*puFractions = (UINT32) (uTimestamp % uPrecisionFactor);
		uTimestamp /= uPrecisionFactor;
		*puSeconds = (UINT8) (uTimestamp % 60);
		uTimestamp /= 60;
		*puMinutes = (UINT8) (uTimestamp % 60);
		*puHours = (UINT8) (uTimestamp / 60);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}




BOOL StTime_Add(StTime* me, StTime Time, BOOL b24hWrapAround)
{
	UINT64 uTime_TimeHex;
	INT64  iTemp;
	INT64  iTicks24h;

	uTime_TimeHex = StTime_GetTimeHex(&Time);

	if (!(me->f.IsNull || Time.f.IsNull))
	{
		// adjust precision of Time to the precision of me
		if (me->f.Precision != Time.f.Precision)
		{
			switch (me->f.Precision)
			{
				case E_ST_TIME_PRECISION_1MS : // 1ms
				{
					if (Time.f.Precision == E_ST_TIME_PRECISION_0P1MS) // 0.1ms
					{
						uTime_TimeHex /= 10;
					}
					else  // 1탎
					{
						uTime_TimeHex /= 1000;
					}
					iTicks24h = TICKS_24H_1_MS;
				}
				break;

				case E_ST_TIME_PRECISION_0P1MS : // 0.1ms
				{
					if (Time.f.Precision == E_ST_TIME_PRECISION_1MS) // 1ms
					{
						uTime_TimeHex *= 10;
					}
					else // 1탎
					{
						uTime_TimeHex /= 100;
					}
					iTicks24h = TICKS_24H_0P1_MS;
				}
				break;

				case E_ST_TIME_PRECISION_1US : // // 1탎
				{
					if (Time.f.Precision == E_ST_TIME_PRECISION_1MS) // 1ms
					{
						uTime_TimeHex *= 1000;
					}
					else // 0.1ms
					{
						uTime_TimeHex *= 100;
					}
					iTicks24h = TICKS_24H_1_US;
				}
				break;
			}
		}


		// Add time stamps
		if (!me->f.IsNegative)
		{
			iTemp = (INT64)StTime_GetTimeHex(me);;
		}
		else
		{
			iTemp = -(INT64)StTime_GetTimeHex(me);;
		}

		if (!Time.f.IsNegative)
		{
			iTemp += (INT64)uTime_TimeHex;
		}
		else
		{
			iTemp -= (INT64)uTime_TimeHex;
		}


		// put result into structure
		if (!b24hWrapAround)
		{
			if (iTemp >= 0)
			{
				StTime_SetTimeHex(me, (UINT64)iTemp);
				me->f.IsNegative = 0;
			}
			else
			{
				StTime_SetTimeHex(me, (UINT64)(-iTemp));
				me->f.IsNegative = 1;
			}
		}
		else
		{
			StTime_SetTimeHex(me, (UINT64)(iTemp % iTicks24h));
			me->f.IsNegative = 0;
		}

		return TRUE;
	}
	else	// one of the instances is null
	{
		me->Val = 0;
		me->f.IsNull = 1;
		return FALSE;
	}
}




void StTime_InvertSign(StTime* me)
{
	me->f.IsNegative ^= 1;
}




BOOL StTime_Compare(StTime* me, StTime Time)
{
	UINT64 uTime_TimeHex;

	if (!(me->f.IsNull || Time.f.IsNull))
	{

		uTime_TimeHex = StTime_GetTimeHex(&Time);

		// adjust precision of Time to the precision of me
		if (me->f.Precision != Time.f.Precision)
		{
			switch (me->f.Precision)
			{
				case E_ST_TIME_PRECISION_1MS : // 1ms
				{
					if (Time.f.Precision == E_ST_TIME_PRECISION_0P1MS) // 0.1ms
					{
						uTime_TimeHex /= 10;
					}
					else  // 1탎
					{
						uTime_TimeHex /= 1000;
					}
				}
				break;

				case E_ST_TIME_PRECISION_0P1MS : // 0.1ms
				{
					if (Time.f.Precision == E_ST_TIME_PRECISION_1MS) // 1ms
					{
						uTime_TimeHex *= 10;
					}
					else // 1탎
					{
						uTime_TimeHex /= 100;
					}
				}
				break;

				case E_ST_TIME_PRECISION_1US : // // 1탎
				{
					if (Time.f.Precision == E_ST_TIME_PRECISION_1MS) // 1ms
					{
						uTime_TimeHex *= 1000;
					}
					else // 0.1ms
					{
						uTime_TimeHex *= 100;
					}
				}
				break;
			}
		}
		return ((StTime_GetTimeHex(me) == uTime_TimeHex) && (me->f.IsNegative == Time.f.IsNegative));
	}
	else
	{
		return FALSE;
	}
}
