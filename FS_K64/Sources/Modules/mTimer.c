/*
------------------------------------------------------------
Copyright 2003-201x Haute �cole ARC Ing�ni�rie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	mTimer.c
Author and date :	Monnerat Serge 29 ao�t 2015

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/
#include "mTimer.h"
#include "../Interfaces/iFlextimer.h"
#include "../Interfaces/iDio.h"

static double mTimer_ForwardRegulationDirection(double aOldValuePosition, Int8 aDifferenceWithCenter, UInt16 aSizeTabBrowse, UInt8 aSizeBeetwenTheLines);


//-----------------------------------------------------------------------------
// Speed Mes module setup
//-----------------------------------------------------------------------------
void mTimer_Setup(void)
{
	
	// Flextimer setup
	iFlextimer_Config();
	
	// Pin direction et enable du pont en H en sortie
	iDio_SetPortDirection(kPortA,kMaskIo15+kMaskIo16+kMaskIo17,kIoOutput);
	
}

//-----------------------------------------------------------------------------
// Speed Mes module start
//-----------------------------------------------------------------------------
void mTimer_Open(void)
{
	mTimer_SetServoDuty(0, kServoDefault);
	mTimer_SetServoDuty(1, kServoDefault);
	mTimer_SetMotorDuty(0 ,0);
	// Enable des ponts en H
	iDio_SetPort(kPortA,kMaskIo17,kIoOff);
}

//-----------------------------------------------------------------------------
// Speed Mes module stop
//-----------------------------------------------------------------------------
void mTimer_Close(void)
{
	
}

//-----------------------------------------------------------------------------
// Lecture de la vitesse de rotation des moteurs
// Moteur A = moteur gauche --> valeur n�gative = en arri�re, valeur pos=en avant
// Moteur B = moteur droite
//----------------------------------------------------------------------------
void mTimer_GetSpeed(float *aSpeedMotLeft,float *aSpeedMotRight)
{
	iFlextimer_GetSpeed(aSpeedMotLeft, aSpeedMotRight);
}

//---------------------------------------------------------------------------
// Modifie la position du servo
// Position entre -1.0 to 1.0
//---------------------------------------------------------------------------
void mTimer_SetServoDuty(UInt8 aServoNumber, float aPosition)
{
	iFlextimer_SetServoDuty(aServoNumber,aPosition);
}

//---------------------------------------------------------------------------
// Modifie le duty du PWM
// Moteur A = moteur gauche
// Moteur B = moteur droite
//---------------------------------------------------------------------------
void mTimer_SetMotorDuty(float aMotorLeft, float aMotorRight)
{
	iFlextimer_SetMotorDuty(aMotorLeft, aMotorRight);	
}

//---------------------------------------------------------------------------
// Enable des ponts en H des moteurs DC
//---------------------------------------------------------------------------
void mTimer_EnableHBridge(void)
{
	// Enable des ponts en H
	iDio_SetPort(kPortA,kMaskIo17,kIoOff);		
}

//---------------------------------------------------------------------------
// Disable des ponts en H des moteurs DC
//---------------------------------------------------------------------------
void mTimer_DisableHBridge(void)
{
	// Enable des ponts en H
	iDio_SetPort(kPortA,kMaskIo17,kIoOn);		
}

//---------------------------------------------------------------------------
// Fait aller le robot tout droit
//---------------------------------------------------------------------------
/*
 * Inputs:
 * -	tabPicture: correspond au tableau contenant directement les d�riv�s de la lumi�re pour �viter tout traitement d'image
 * -	aSizeTabBrowser: correspond au nombre de cases � parcourir
 */
//TODO JE DOIS ENCORE r�fl�chir aux param�tres
void mTimer_MotorMoveStraight(UInt8* tabPicture, Int16 aSizeTabBrowse, MotorSpeed aMotorSpeed){
		//Me permet de positioner ma voiture pour qu'elle aille le plus droit possible
		static float sServoPosition = kServoDefault; //Premi�re fois que je vais dans la fonction je mets les roues droites
		
		//Va me permmetre de savoir o� ce trouvent les deux lignes
		Int16 aLeftLine=0;
		Int16 aRightLine=0;
		Int16 aDifferenceWithCenter=0;
		
		//Pour contr�ler que la lecture des deux lignes est correcte
		bool aValidityLeftLine=false, aValidityRightLine=false;
		
		//Variables pour ma boucle de recherche des d�riv�es
		UInt16 i;
		StateMachineMoveForward aState= kfindLeftLine;
		
		//Va me permmetre de trouver la ligne de gauche et la ligne de droite.
		//J'ignore les 3 premiers pixels
		for(i=3; (i<(aSizeTabBrowse)) && (aState != kEnd); i++){
				switch(aState){
				case kfindLeftLine:
					if(tabPicture[i]>kSeuilDerive){
						aLeftLine = i+1;//Je compte � partir de 1!!!
						
						if((aLeftLine<(aSizeTabBrowse/2)) && (aLeftLine!=0)) aValidityLeftLine = true;
						else{
								aValidityLeftLine = false;
								aValidityRightLine=true;
								aRightLine = i+1;
								aState = kEnd;
						}
						
						aState = kwaitoffset;
					}
					break;
				case kwaitoffset://j'aurai peut-�tre besoin de cet offset si je d�tecte 2 d�riv� pour une seule ligne !?
					i += (UInt16)(aSizeTabBrowse*0.2);//J'incr�mente de 10% pour �viter l'�ventuelle deuxi�me d�riv�e pour la m�me ligne,
					//Contr�ler que 10% est suffisant !!!!!!!
					aState = kfindRightLine;
					break;
				case kfindRightLine:
					if(tabPicture[i]>kSeuilDerive){
							aRightLine = i+1;
							
							if((aRightLine!=0) && (aRightLine>(aSizeTabBrowse/2))) aValidityRightLine=true;
							else 																									 aValidityRightLine=false;
							
							aState = kEnd;
					}
					break;
				case kEnd:
					//Nothing
					break;
				default: aState = kfindLeftLine;
				}
		}
		
		if(aValidityLeftLine==true && aValidityRightLine==true){
			//Calcul pour savoir si je suis trop � gauche ou � droite du centre
			//Une valeur n�gative: trop � droite
			//Une valeur positive: trop � gauche
			//La valeur absolue d�termine l'�cart avec le centre
			//Le but est d'avoir la plus proche valeur de 0
			//TODO Contr�ler le taille du tableau et rajouter +1 si le nombre et impair et apparamment d'apr�s mes tests, +2 si c'est pair !!!
			aDifferenceWithCenter = (aSizeTabBrowse%2)==0?2:1;
			aDifferenceWithCenter += aSizeTabBrowse;
			aDifferenceWithCenter = aLeftLine - (aDifferenceWithCenter - aRightLine);
			
			//Si ma valeur est n�gative je dois aller � gauche
			//Si ma valeur est positive je dois aller � droite
			sServoPosition = mTimer_ForwardRegulationDirection(sServoPosition, aDifferenceWithCenter, aSizeTabBrowse, (aRightLine-aLeftLine));
			
		//Si je ne vois que la ligne de gauche	
		}else if(aValidityLeftLine==true){
				//Je dois me d�placer vers la gauche
				sServoPosition -= kServoDefault*0.3;
			//Si je ne vois que la ligne de droite			
		}else if(aValidityRightLine==true){
				sServoPosition += kServoDefault*0.3;
		//Si je ne vois aucune ligne
		}else{
				sServoPosition = kServoDefault;//Je fait aller la voiture tout droit
		}
		
		//Contr�le des valeurs extr�mes pour �viter que le servo aille dans ses limites
		if(sServoPosition > kServoMaxPosition)
			sServoPosition = kServoMaxPosition;
		else if(sServoPosition <kServoMinPosition){
			sServoPosition = kServoMinPosition;
		}
		
		//Je mets les servos dans la position corrig�e pour avancer tout droit
		mTimer_SetServoDuty(0, sServoPosition);
		
		//Voir la n�cessit� de faire tourner les roues dans une vitesse diff�rente que la vitesse maximale
		//TODO Contr�ler lequel des moteur tourne � vitesse max en n�gatif !!!
		//mTimer_SetMotorDuty(0, 0);
}

//TODO terter � tout prix cette fonction !!!
static double mTimer_ForwardRegulationDirection(double aOldValuePosition, Int8 aDifferenceWithCenter, UInt16 aSizeTabBrowse, UInt8 aSizeBeetwenTheLines){
		//Variable pour estimer le pourcentage entre la surface entre les deux lignes
		double aPercentAreaBetweenLinesAndAreaTot = (double)aSizeBeetwenTheLines/(double)aSizeTabBrowse;
		
		//Variable pour repr�senter la grandeur de l'�cart entre le centre et la voiture
		//TODO !!! � tester, j'effectue un algorythme qui va calculer une valeur bizarre
		aSizeBeetwenTheLines = aSizeBeetwenTheLines==0?1:aSizeBeetwenTheLines;
		double aPercentdifferenceWithCenter = aDifferenceWithCenter*(double)aPercentAreaBetweenLinesAndAreaTot/(double)aSizeBeetwenTheLines;
		
		double aNewValueServoPosition = (double)(aOldValuePosition*aPercentdifferenceWithCenter+kServoDefault);
			
		return aNewValueServoPosition;
}

//---------------------------------------------------------------------------
// permet de faire tourner les deux moteurs � la m�me vitesse
// Fontion non-bloquante, donc plusieurs appels pour r�guler la vitesse
//---------------------------------------------------------------------------
/*
 * Inputs:
 * -	aCmdVitesse: Correspond � la vitesse en pourcent � vouloir atteindre valeur entre (-100 -> 100)
 * -	aMotorSpeed: Correspond � la vitesse des deux moteurs
 * 
 * Output:
 * -	bool: Vrai si les deux moteurs tournent � la m�me vitesse
 */
bool mTimer_SetSameVitesseMotor(float aCmdVitesse, MotorSpeed aMotorSpeed){
		//Valeurs de la commande de la vitesse des moteurs.
		static float aSpeedLeftCmd=0;
		static float aSpeedRightCmd=0;
		
		bool valueReturn=true;
		
		//S�curit�
		if(aCmdVitesse > 100)
			aCmdVitesse = 100;
		
		if(aCmdVitesse < 0)
			aCmdVitesse = 0;
		
		//Moteur Gauche
		//La vitesse max est de 5500
		//Je vais contr�ler si ma valeur est � +- 1% de la vitesse d�sir�e
		if(aMotorSpeed.VitesseMoteurGauche < ((kMotorMaxSpeed*aCmdVitesse/100.)-55)){ //Plus petit que 1% en positif
				aSpeedLeftCmd += ((abs((kMotorMaxSpeed*aCmdVitesse/100.)-55.)-aMotorSpeed.VitesseMoteurGauche))*kMotorMaxSpeedCmd/kMotorMaxSpeed;
				valueReturn = false;
		}else if(aMotorSpeed.VitesseMoteurGauche > ((kMotorMaxSpeed*aCmdVitesse/100.)-55)){ //Plus grand que 1% en positif
				aSpeedLeftCmd -= ((abs((kMotorMaxSpeed*aCmdVitesse/100.)-55.)-aMotorSpeed.VitesseMoteurGauche))*kMotorMaxSpeedCmd/kMotorMaxSpeed;
				valueReturn = false;
		}else 
			valueReturn = (true==valueReturn)?true:false;//Contr�le que la variable ne soit pas false d'avant
			
		//Moteur Droite
		//Je vais contr�ler si ma valeur est � +- 1% de la vitesse d�sir�e
		if(aMotorSpeed.VitesseMoteurDroite < ((kMotorMaxSpeed*aCmdVitesse/100.)-55)){ //Plus petit que 1% en positif
				aSpeedRightCmd += ((abs((kMotorMaxSpeed*aCmdVitesse/100.)-55.)-aMotorSpeed.VitesseMoteurDroite))*kMotorMaxSpeedCmd/kMotorMaxSpeed;
		}else if(aMotorSpeed.VitesseMoteurDroite > ((kMotorMaxSpeed*aCmdVitesse/100.)-55)){ //Plus grand que 1% en positif
				aSpeedRightCmd -= ((abs((kMotorMaxSpeed*aCmdVitesse/100.)-55.)-aMotorSpeed.VitesseMoteurDroite))*kMotorMaxSpeedCmd/kMotorMaxSpeed;
		}else
			valueReturn = (true==valueReturn)?true:false;//Contr�le que la variable ne soit pas false d'avant
		
		
		mTimer_SetMotorDuty(aSpeedLeftCmd, aSpeedRightCmd);
		
		return valueReturn;
}
