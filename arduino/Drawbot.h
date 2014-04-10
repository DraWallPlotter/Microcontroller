#ifndef Drawbot
#define Drawbot

#include <math.h>
#include <SD.h>
#include <Servo.h> 

#include "Arduino.h"

// d�finition des pins
#define PIN_CS 10
#define PIN_SERVO A5

// position du servo-moteur (en degr�s)
// lorsque le robot n'�crit pas (MIN) - lorsque il �crit (MAX)
#define MIN_SERVO 81
#define MAX_SERVO 91

// sp�cifications m�caniques du robot
#define NBPAS 200
#define DIAMETRE 20

// diff�rentes �tapes des pins du moteur en mode demi-pas:
const int tabMot[8] = {B0101, B0100, B0110, B0010, B1010, B1000, B1001, B0001};

class Draw {
	public:

	/****************
	** construceurs **
	****************/

	// dimentions de la surface (en mm)
	Draw(float surfaceL, float surfaceH);

	// Ajout du nom du fichier .svg si en mode svg
	Draw(float surfaceL, float surfaceH, char *nomFichier);

	/************
	** getters **
	************/

	// renvoie la position courante en X du crayon
	float getaX(void);
	// renvoie la position courante en Y du crayon
	float getaY(void);
	// renvoie la longueur courante de la chaine gauche
	long getA(void);
	// renvoie la longueur courante de la chaine droite
	long getB(void);
	
	/************
	** setters **
	************/

	// initialise la position du curseur, � n'utiliser qu'au d�but.
	// Par d�faut: au centre de la surface
	void setaXY(float aX, float aY);
    
	// initialise le ratio pour le calcul de distance, � n'utiliser qu'au d�but.
	// Par d�faut: 100
	void setRatioDist(float ratioDist);

	// initialise le ratio pour le calcul de vitesse, � n'utiliser qu'au d�but).
	// Par d�faut: 100
	void setRatioVitesse(float ratioVitesse);

	// initialise la vitesse du crayon
	// Par d�faut: 100
    void setVitesse(float vitesse);

	// initialise les limites que le crayon ne franchira pas, de chaque cot�s de la surface
	// Limite haute: Pr�voir une bonne marge, minimum 1/10 de la hauteur de la surface, 
	// pour �viter que le crayon ne se fasse pas tirer dans 2 sens oppos�s.
	// Une valeur proche de 0 entrainerait un tra�� impr�cis et risquerait de d�teriorer le crayon.
	// Limite gauche et droite: Pr�voir une l�g�re marge pour qu'aucune des 2 chaines ne soient
	// jamais � la verticale (dans ce cas l'autre chaine ne serait pas tendue).
	void setlimG(float limG);	// Par d�faut: 10
	void setlimD(float limD);	// Par d�faut: 10
	void setlimH(float limH);	// Par d�faut: 50
	void setlimB(float limB);	// Par d�faut: 10
	
	/*****************
	** alimentation **
	*****************/
	
	// alimente ou d�salimente le moteur.
	// Attention, selon le poids du crayon, la d�salimentation peut entrainer la chute du robot.
	void alimenter(bool alimenter);

	// obligatoire, � placer en d�but de programme:
	// r�alise les proc�dures d'initialisation n�cessaires au fonctionnement du robot.
	void commencer(void);
    
	/*****************
	** deplacements **
	*****************/

	// Quasiment toutes ces fonctions sont doubl�e,
	// de mani�re � effectuer le tra��: 
	// - en position absolue (ABS)
	// - en position relative (REL)

	// deplacements (sans �crire)

	// place le crayon au point [x, y]
	void deplacerABS(float x, float y);
	void deplacerREL(float x, float y);

	// place le crayon au centre de la surface.
	void centrer(void);

	// lignes droite:
	
	// trace une ligne droite, du point courant au point [x, y]
	void ligneABS(float x, float y);
	void ligneREL(float x, float y);
	
	// fini le tra�� courant:
	// Revient au point cible du dernier d�placement.
	// Ceci permet de faire des formes "finies"
	void finir(void);

	// lignes horizontales/verticales

	// trace une ligne horizontale sur toute la largeur de la surface, � la coordon�e y.
	void horizABS(float y);
	void horizREL(float y);

	// trace une ligne verticale sur toute la hauteur de la surface,
	// � la coordon�e x.
	void vertiABS(float x);
	void vertiREL(float x);

	// courbes de Bezier cubiques
	
	// trace une courbe de B�zier cubique
	// dont les points de controle P1 et P2 sont respectivement
	// en [x1, y1] et [x2, y2],
	// et dont le point de destination est en [x, y].
	void bezierCubABS(float x, float y, float x1, float y1, float x2, float y2);
	void bezierCubREL(float x, float y, float x1, float y1, float x2, float y2);

	// idem, le point de controle P1 est l'image du point P2 de la derni�re courbe de B�zier
	// (cubique ou quadratique) par sym�trie avec le point courant.
	// Si la derni�re figure n'�tait pas une courbe de B�zier, il correspond au point courant.
	void bezierCubABS(float x, float y, float x2, float y2);
	void bezierCubREL(float x, float y, float x2, float y2);

	//courbes de Bezier quadratiques

	// trace une courbe de B�zier quadratique dont le point de controle P1 est en [x1, y1]
	// et dont le point de destination est en [x, y].
	void bezierQuadABS(float x, float y, float x1, float y1);
	void bezierQuadREL(float x, float y, float x1, float y1);

	// idem, le point de controle est l'image du point de controle de la derni�re courbe de B�zier
	// (cubique ou quadratique) par sym�trie avec le point courant.
	// Si la derni�re figure n'�tait pas une courbe de B�zier, il correspond au point courant.
	void bezierQuadABS(float x, float y);
	void bezierQuadREL(float x, float y);
	
	// arcs et cercles
	
	// trace un arc de cercle
	void arcABS(float a1, float a2, float a3, float a4, float a5, float a6, float a7); // non fini
	void arcREL(float a1, float a2, float a3, float a4, float a5, float a6, float a7);
	
	// trace une ellipse dont le centre est les point courant,
	// le rayon horizontal est rx et le rayon vertical est ry.
	void ellipse(float rx, float ry);

	// trace un cercle dont le centre est les point courant et de rayon r.
	void cercle(float r);
	
	void svg(void);

	
	
	private:
	
	
	
	/**********
	** noyau ** (classe � l'origine de tout d�placement)
	**********/

	Servo servo;

	// ligne() n'initialise pas les variables pour les courbes Bezier, � la diff�rence de ligneABS()
	void ligne(float x, float y, bool ecrit);
	
	/*********************
	** lecture carte SD **
	*********************/
	
	// initialisation de la carte SD
	int initSD(char *nomFichier);

	char * nomFichier;

	// recupere une valeur du fichier XML
	// EX: width --> (21cm) OU style --> (fill:none;stroke:#000000)
	void val(const char *requette, char *valeur);

	// recupere une valeur du fichier XML qui est un nombre et le converti en float,
	// en prenant en compte les unit�s (cm, mm, ...)
	// EX: width --> 210 OU height --> 320
	float valNb(const char *requette);

	// teste si le car est un chiffre ou non
	bool estChiffre(char car);

	// trouve un mot dans le fichier
	// renvoie faux si non trouv� (le curseur sera � la fin du mot)
	boolean trouveSD(const char *mot);

	// r�cup�re nbParams param�tres, les convertis en float et les stoquent dans le tableau tNb
	void params(float *tNb, int nbParams);

	// dessine les tra��s en lisant le contenu du fichier .svg
	void dessiner(void);

	// le fichier XML � lire
	File fichier;

	long filG(float x, float y);
	long filD(float x, float y);

	unsigned int aG;
	unsigned int aD;

	// vrai si l'utilisateur souhaite dessiner un .svg
	// faux sinon
	bool modeSVG;

	// masque
	int masqueAlim;

	// si ecrire est vrai, le stylo est contre la paroi, sinon il est �loign�
	void ecrire(bool ecrire);

	//variables:

	float ratioVitesse;
	
	float aX;
	float aY; // position sur le plan
	
	float aXf; // position voulue (fictive) avant qu'elle soit modifi�e par les limites
	float aYf;
	
	float surfaceH;
	float surfaceL; // taille du plan (en mm)
	
	float largeur; // surface o� l'on peu �crire (limites comprises)
	float hauteur;
	
	int nbPas; // nb de pas des moteurs (on suppose qu'ils sont de m�me type)
	float diametre;
	
	float ratioDist; // ratio entre le nb de pas et les mm
	
	float delaiBase;
	
	bool ecrireOk;
	
	float ptDepartX;	
	float ptDepartY;
	
	float ptBezierCubX;
	float ptBezierCubY;
	float ptBezierQuadX;
	float ptBezierQuadY;
	
	float limG;
	float limD;
	float limH;
	float limB; //limites que le stylo ne franchira pas, de chaque cot� de la surface
};

#endif
