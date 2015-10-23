/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier : 	mSwitch.h	
Auteur et Date :	Monnerat Serge 29 août 2013

But : Interface permettant l'utilisation des interrupteurs et poussoirs

-----------------------------------------------------------------------------
Historique:
-----------------------------------------------------------------------------

$History: $


-----------------------------------------------------------------------------
*/

#ifndef MSWITCH_H_
#define MSWITCH_H_

#include"..\Utils\def.h"

// Enuméré des interrupteurs 
typedef enum
{
	kSw1=0,
	kSw2=1,
	kSw3=2,
	kSw4=3
}SwitchEnum;

// Enuméré des boutons poussoirs
typedef enum
{
	kPushButSW1=0,
	kPushButSW2=1
}PushButEnum;

//-----------------------------------------------------------------------------
// Configuration des ports IO utilisés par les interrupteurs et les poussoirs
//-----------------------------------------------------------------------------
void mSwitch_Setup(void);

//-----------------------------------------------------------------------------
// Start du module
//-----------------------------------------------------------------------------
void mSwitch_Open(void);

//-------------------------------------------------------------------------
// Lecture des interrupteurs
// aSwitchNb: quel interrupteur veut-on lire
// Retour: état du switch
//-------------------------------------------------------------------------
bool mSwitch_ReadSwitch(SwitchEnum aSwitch);

//-------------------------------------------------------------------------
// Lecture des boutons poussoir
// aPushButNb: quel boutons poussoir veut-on lire
// Retour: état du bouton poussoir
//-------------------------------------------------------------------------
bool mSwitch_ReadPushBut(PushButEnum aPushButNb);

#endif
