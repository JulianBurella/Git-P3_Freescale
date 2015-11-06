/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name : 	mTimer.h	
Author and date :	Monnerat Serge 29 août 2015

Goal : 

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $


-----------------------------------------------------------------------------
*/
#ifndef __MTIMER__
#define __MTIMER__

#include "..\Utils\def.h"

typedef enum{
	kfindLeftLine,
	kwaitoffset,//j'aurai peut-être besoin de cet offset si je détecte 2 dérivé pour une seule ligne !?
	kfindRightLine,
	kEnd
}StateMachineMoveForward;

#define kServoDefault (float)(0)
#define kSeuilDerive 60 //La valeur reste à definir avec les tests sur circuit.
#define kMotorMaxSpeed 5500.
#define kServoMaxPosition (0.9)
#define kServoMinPosition (-0.85)
#define kMotorMaxSpeedCmd 1.

//-----------------------------------------------------------------------------
// Speed Mes module setup
//-----------------------------------------------------------------------------
void mTimer_Setup(void);

//-----------------------------------------------------------------------------
// Speed Mes module start
//-----------------------------------------------------------------------------
void mTimer_Open(void);

//-----------------------------------------------------------------------------
// Speed Mes module stop
//-----------------------------------------------------------------------------
void mTimer_Close(void);

//-----------------------------------------------------------------------------
// Lecture de la vitesse de rotation des moteurs
// Moteur A = moteur gauche --> valeur négative = en arrière, valeur pos=en avant
// Moteur B = moteur droite
//-----------------------------------------------------------------------------
void mTimer_GetSpeed(float *aSpeedMotLeft,float *aSpeedMotRight);

//---------------------------------------------------------------------------
// Modifie la position du servo
// Position entre -1.0 to 1.0
//---------------------------------------------------------------------------
void mTimer_SetServoDuty(UInt8 aServoNumber, float aPosition);

//---------------------------------------------------------------------------
// Modifie le duty du PWM
// Moteur A = moteur gauche
// Moteur B = moteur droite
//---------------------------------------------------------------------------
void mTimer_SetMotorDuty(float aMotorLeft , float aMotorRight);

//---------------------------------------------------------------------------
// Enable des ponts en H des moteurs DC
//---------------------------------------------------------------------------
void mTimer_EnableHBridge(void);

//---------------------------------------------------------------------------
// Disable des ponts en H des moteurs DC
//---------------------------------------------------------------------------
void mTimer_DisableHBridge(void);

void mTimer_MotorMoveStraight(UInt8* tabPicture, Int16 aSizeTabBrowse, MotorSpeed aMotorSpeed);

bool mTimer_SetSameVitesseMotor(MotorCmd aCmd, MotorSpeed aMotorSpeed);

#endif
