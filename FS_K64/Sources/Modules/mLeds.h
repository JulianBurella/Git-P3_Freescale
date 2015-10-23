/*
 * mLeds.h
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */

#ifndef MLEDS_H_
#define MLEDS_H_

#include"..\Utils\def.h"

// Masque des LED
typedef enum
{
  kMaskLed1=1,
  kMaskLed2=2,
  kMaskLed3=4,
  kMaskLed4=8
}LedMaskEnum;

// Etat des LED
typedef enum
{
  kLedOff,
  kLedOn
}LedStateEnum;

//-----------------------------------------------------------------------------
// Configuration des ports IO utilisés par les LEDS
//-----------------------------------------------------------------------------
void mLeds_Setup(void);

//-----------------------------------------------------------------------------
// Start du module
//-----------------------------------------------------------------------------
void mLeds_Open(void);

//-----------------------------------------------------------------------------
// On Off des LED
// aMask:   choix des LED sur lesquelles on veut agir (si bit à 1)
// aState:  choix de l'état des LED désirés
//-----------------------------------------------------------------------------
void mLeds_Write(LedMaskEnum aMask,LedStateEnum aState);

//-----------------------------------------------------------------------------
// On Off des LED
// aMask:   choix des LED sur lesquelles on veut agir (si bit à 1)
//-----------------------------------------------------------------------------
void mLeds_Toggle(LedMaskEnum aMask);

#endif
