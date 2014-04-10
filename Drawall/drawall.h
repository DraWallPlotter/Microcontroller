
/*
 * This file is part of Drawall, a project for a robot which draws on walls.
 * See http://drawall.cc/ and https://github.com/roipoussiere/Drawall/.
 * 
 * Copyright (c) 2012-2013 Nathana�l Jourdane
 * 
 * Drawall is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file	drawall.h
 * \author  Nathana�l Jourdane
 * \brief   Fichier d'en-t�te de la biblioth�que
 * \details L'ensemble du code a �t� indent� avec le programme indent (sudo apt-get install indent) avec les param�tres suivants :
indent -kr -bad -bbb -sc -ncdw -ss -bfda -blf -ts4 -ut -sob -nlp -ci12 -fc1 drawall.cpp drawall.h pins.h

// finit le dernier trajet au cas ou �a ne tombe pas juste

 * \mainpage Pr�sentation

Drawall est un projet libre de robot autonome qui dessine sur les murs.

Ce projet est libre : vous pouvez le redistribuer ou le modifier suivant les termes de la GNU GPL. L'ensemble du projet est publi� sous cette licence, ce qui inclut l'int�gralit� du code-source, les sch�mas �lectroniques, les sch�mas du mat�riel et toute la documentation. Pour plus de d�tails, consultez la GNU General Public License, dont vous trouverez une copie sur le fichier COPYING.txt dans le d�pot GitHub. La documentation d�taill�e du code source est disponible sur http://drawall.cc/. 

Ce robot utilise une carte Arduino et n�cessite donc le logiciel Arduino pour fonctionner. Vous trouverez de l'aide pour son installation et utilisation sur le site officiel http://arduino.cc/fr/.

La partie logicielle est une librairie Arduino. Elle est compos�e d'un fichier principal drawall.cpp, d'un fichier header drawall.h et d'un fichier de param�tres params.h. Ce dernier permet de sp�cifier tous les param�tres concernant le robot. Vous devrez l'�diter avant d'utiliser la librairie.
La librairie est utilis�e par l'interm�diaire d'un "sketch" Arduino, (fichier .ino), dont vous trouverez des exemples dans le r�pertoire de la librairie.

La librairie contient tous les calculs n�cessaire � l'execution du robot, les sketchs ne servent qu'� le commander, ils sont tr�s courts et simples � utiliser.

Il est possible de commander le robot par des fonctions simples (lignes, courbes, ...), ou par l'interm�diaire d'un fichier svg qu'il va interpr�ter.
Les fonctions svg ne sont pas encore toutes interpr�t�es, certains dessins ne seront don pas correctement reproduits. Vous pouvez vous r�f�rer au fichier d'exemple drawbot.svg dans le dossier examples.

Le projet comporte �galement un simulateur, permetant de tester le bon
fonctionnement d'un programe de dessin avant de le reproduire et faciliter le d�veloppement du projet.
Il n�cessite l'environnement de d�veloppement Processing : http://www.processing.org/. Ce simulateur reproduit le dessin que r�alise le robot, en interpretant en temps r�el les impulsions envoy�es aux moteurs.

Pour le faire fonctionner il vous faut donc connecter � votre ordinateur au minimum une carte arduino munie d'un lecteur SD et y ins�rer une carte contenant une image svg valide. Toutes les fonctions svg ne sont pas encore interpr�t�es. Pour plus d'informations sur la conformit� du fichier svg, r�f�rez-vous au document documentation/valid_svg.txt du d�pot GitHub. Une aide � l'installation sur Linux est �galement disponible sur le d�pot.

Ce projet est libre et �voluera en gr�ce aux retours des utilisateurs. Questions, demande d'informations et suggestions sont donc les bienvenues.

Copyright (c) 2012-2013 Nathana�l Jourdane

Adresse de contact : nathanael[AT]jourdane[DOT]net.

Site du projet : http://roipoussiere.github.io/Drawall

D�p�t GitHub : https://github.com/roipoussiere/Drawall

Documentation du projet : http://drawall.cc/

Vid�os de d�monstration du robot : http://www.youtube.com/watch?v=ewhZ9wcrR2s - 2�me version : http://www.youtube.com/watch?v=p4oQWtzjBI0&feature=youtu.be

\image html thedrawall.jpg
 */

#ifndef drawall
#define drawall

#include "pins.h"
#include <Arduino.h>
#include <math.h>
#include <SD.h>
#include <Servo.h>

/**
 * \brief Classe principale de la librairie
 * \todo Faire une simulation compl�te tr�s rapide au d�but avant de commencer le tra�� (possible ?) ou cr�er m�thode test(bool testing) pour activer le mode de test.
 * \todo Ajouter m�thode rect.
 * \todo Processing : Tracer les mouvement sur un calque s�par� (derri�re).
 * \todo Processing : Gestion des warnings.
 * \bug Processing : Affiche une mauvais position � la fin du tra�� (m�me pendant ?)
 * \bug Processing : Afficher toujours d�placement en cours � la fin du tra��
 * \bug Syst�me de limites tr�s foireux.
 * \bug Processing : L'affichage de la position de tient pas compte de la position de la feuille.
 * \bug Le crayon va tr�s en bas en fin de tra�� (en dehors de limite basse).
 * \bug Moteurs gauche et droit non synchronis�s pour les d�placements.
 * \bug Voir quel est le probl�me avec le dessin de Drawall, il fait n'importe quoi + non d�tection de la limite basse (cf. capture).
 * \bug Il y a une l�g�re marge en haut du dessin.
 * \bug Pour les surfaces larges, pas de marge en bas du dessin, sans doute li� au bug ci-dessus.
 * \bug Dessin pas centr� pour les surfaces larges.
 * \todo Afficher distance trac� + distance move + distance totale + taux d'optimisation (distance trac� / distance totale).
 * \todo Afficher dur�e + heure de fin estim�e.
 * \todo Processing : Mettre au clair variables globales.
 * \todo Processing : Afficher erreur de communication s�rie quand c'est le cas.
 * \todo Tester tous les fichiers avant de commencer � tracer.
 * \todo Utiliser un programme d'indentation, comme indent ou bcpp
 * \todo Avoir la possibilit� d'ins�rer une variable dans les codes d'erreurs
 */
class Drawall {

  private:

	/**
	 * \brief Liste des donn�es envoy�es au pc via le port s�rie.
	*/
	enum SerialData {
		BEGIN_INSTRUCTIONS,		// D�but d'envoi des donn�es d'initialisation.
		END_INSTRUCTIONS,		// Fin d'envoi des donn�es d'initialisation.
		BEGIN_MESSAGE,			// D�but d'envoi d'un message � afficher.
		END_MESSAGE,			// Fin d'envoi d'un message � afficher.
		ENABLE_MOTORS,			// Alimentation des moteurs.
		DISABLE_MOTORS,			// D�salimentation des moteurs.
		WRITE,					// Le stylo dessine.
		MOVE,					// Le stylo se d�place (ne dessine pas).
		PUSH_LEFT,				// Rel�che la courroie gauche d'un cran.
		PULL_LEFT,				// Tire la courroie gauche d'un cran.
		PUSH_RIGHT,				// Rel�che la courroie droite d'un cran.
		PULL_RIGHT,				// Tire la courroie droite d'un cran.
		ERROR,					// Erreur (suivie du code d'erreur).
		WARNING,				// Warning (suivie du code de warning).
		END						// Fin du trac�.
	};

	/**
	* \brief Liste des erreurs et warnings pouvant survenir pendant l'execution du programme.
	* \details Les erreurs commencent � l'indice 0 tandis que les warnings commencent � l'indice 100.
	*/
	enum Error {
		CARD_NOT_FOUND,			// La carte SD n'a pas �t� trouv�e ou est illisible.
		FILE_NOT_FOUND,			// Le fichier n'existe pas.
		FILE_NOT_READABLE,		// Erreur d'ouverture du fichier.
		TOO_LONG_SPAN,			// La distance entre les 2 moteurs est inf�rieure � la largeur de la feuille et sa position horizontale.
		INCOMPLETE_SVG,			// Le fichier svg est incomplet.
		NOT_SVG_FILE,			// Le fichier n'est pas un fichier svg.
		SVG_PATH_NOT_FOUND,		// Le fichier svg n'inclut aucune donn�e de dessin.

		// * Warnings *
		WRONG_LINE = 100,		// Ligne mal form�e dans le fichier de configuration.
		TOO_LONG_LINE,			// Ligne trop longue dans le fichier de configuration.
		UNKNOWN_SVG_FUNCTION,	// Fonction svg inconnue.
		UNKNOWN_SERIAL_CODE,	// Caract�re envoy� au port s�rie non reconnu. Utilis� uniquement sur le programme PC, jamais sur l'Arduino (mais il est pr�f�rable que les listes soient identiques).
		LEFT_LIMIT,				// Le crayon a atteint la limite gauche.
		RIGHT_LIMIT,			// Le crayon a atteint la limite droite.
		UPPER_LIMIT,			// Le crayon a atteint la limite haute.
		LOWER_LIMIT				// Le crayon a atteint la limite basse.
	};

	/**
	 * \brief Position sur la zone de dessin
	 * \details Les diff�rentes position pour acc�s rapide, correspondant aux 8 points cardinaux, plus le centre.
	 * \bug Ne fonctionne pas dans le sketch.
	 */
	enum Position {
		CENTER,
		UPPER_CENTER,
		LOWER_CENTER,

		LEFT_CENTER,
		RIGHT_CENTER,
		UPPER_LEFT,

		UPPER_RIGHT,
		LOWER_LEFT,
		LOWER_RIGHT
	};

	/************
	* Attributs *

	************/
	// Commencent par m, pour "member".

	/// Objet pour manipuler le servo-moteur, utilis� avec la librairie \a Servo.
	Servo mServo;

	/// Longueur du c�ble gauche, en pas.
	unsigned long mLeftLength;

	/// Longueur du c�ble droit, en pas.
	unsigned long mRightLength;

	/// Le fichier svg contenant le dessin vectoriel � reproduire.
	File mFile;

	/// �chelle g�n�r�e par les attributs width et height du fichier svg, afin que le dessin s'adapte aux dimentions de la zone de dessin.
	float mDrawingScale;

	/// Offset horizontal g�n�r� par l'attribut width du fichier svg, afin que le dessin soit centr� sur la zone de dessin.
	unsigned int mDrawingOffsetX;

	/// Offset vertical g�n�r� par l'attribut height du fichier svg, afin que le dessin soit centr� sur la zone de dessin.
	unsigned int mDrawingOffsetY;

	/// Longueur d'un pas (distance parcourue par le c�ble en 1 pas, en mm.
	float mStepLength;

	/// Delai initial entre chaque pas du moteur qui a la plus grande distance � parcourir, (en �s).
	/// \details Le d�lai de l'autre moteur est calcul� pour que les 2 moteurs arrivent au point de destination simultan�ment.
	float mDelay;

	/// Permet de d�terminer si le robot est en train d'�crire (\a true) ou non (\a false).   
	bool mWriting;

	/************
	* Positions *
	************/

	/// Position horizontale actuelle du crayon sur le plan.
	float mPositionX;

	/// Position verticale actuelle du crayon sur le plan.
	float mPositionY;

	/// Position horizontale du point d'arriv�e fictif qui n'est pas modifi� par les limites, de mani�re � ce que le tra�� ne subisse pas de transformation.
	float mFictivePosX;

	/// Position verticale du point d'arriv�e fictif qui n'est pas modifi� par les limites, de mani�re � ce que le tra�� ne subisse pas de transformation.    
	float mFictivePosY;

	/// Position horizontale du point de d�part d'une figure (ligne, courbe, ...).
	/// \details N�cessaire pour finir le tra�� avec endFigure().
	float mStartFigureX;

	/// Position verticale du point de d�part d'une figure (ligne, courbe, ...).
	/// \details N�cessaire pour finir le tra�� avec endFigure().
	float mStartFigureY;

	/// Position horizontale du point de contr�le P2 de la derni�re courbe de B�zier cubique tra��e.
	/// \details N�cessaire pour tracer une courbe de B�zier cubique qui d�pend de la pr�c�dente (appel de la fonction avec un seul point de contr�le).
	float mCubicCurveX;

	/// Position horizontale du point de contr�le P2 de la derni�re courbe de B�zier cubique tra��e.
	/// \details N�cessaire pour tracer une courbe de B�zier cubique qui d�pend de la pr�c�dente (avec un point de contr�le en moins).
	float mCubicCurveY;

	/// Position horizontale du point de contr�le P1 de la derni�re courbe de B�zier quadratique tra��e.
	/// \details N�cessaire pour tracer une courbe de B�zier quadratique qui d�pend de la pr�c�dente (appel de la fonction avec aucun point de contr�le).
	float mQuadraticCurveX;

	/// Position verticale du point de contr�le P1 de la derni�re courbe de B�zier quadratique tra��e.
	/// \details N�cessaire pour tracer une courbe de B�zier quadratique qui d�pend de la pr�c�dente (appel de la fonction avec aucun point de contr�le).
	float mQuadraticCurveY;

	/*************
	* Param�tres *
	*************/

	// Initialisation avant loadParameters()
	// Commencent par mp, pour "member" et "parameter".

	/// Nom du fichier ouvert par defaut
	char *mpFileName = "drawall.svg";

	/// Distance entre les 2 moteurs
	unsigned int mpSpan = 1000;

	/// Largeur de la feuille
	unsigned int mpAreaWidth = 650;

	/// Hauteur de la feuille
	unsigned int mpAreaHeight = 500;

	/// Position horizontale du coin sup�rieur gauche de la feuille par rapport au moteur gauche
	unsigned int mpAreaPositionX = 175;

	/// Position verticale du coin sup�rieur gauche de la feuille par rapport au moteur gauche
	unsigned int mpAreaPositionY = 250;

	// * Servo-moteur *

	/// Position lorsque le robot n'�crit pas (en degr�s)
	unsigned int mpServoOff = 36;

	/// Position lorsque le robot �crit (en degr�s)
	unsigned int mpServoOn = 50;

	/// Pause avant le d�placement du servo (en ms)
	unsigned int mpPreServoDelay = 100;

	/// Pause apr�s le d�placement du servo (en ms)
	unsigned int mpPostServoDelay = 500;

	// * Moteurs *

	/// Nombre de pas (prendre en compte les micros-pas effectu�s par le driver moteur)
	unsigned int mpSteps = 800;

	/// Diam�tre du pignon (en �m)
	float mpDiameter = 17.51;

	/// Direction du moteur gauche : \a true pour rel�cher la courroie lorsque le moteur tourne dans le sens horaire et \a false dans le cas contraire.
	boolean mpLeftDirection = false;

	/// Direction du moteur droit : \a true pour rel�cher la courroie lorsque le moteur tourne dans le sens horaire et \a false dans le cas contraire.
	boolean mpRightDirection = true;

	// * Divers *

	/// Pause avant de commencer � desiner (en ms)
	unsigned int mpInitialDelay = 5000;

	/// �chelle horizontale appliqu�e au dessin, permetant de le calibrer.
	float mpScaleX = 1;

	/// �chelle verticale appliqu�e au dessin, permetant de le calibrer.
	float mpScaleY = 1;

	/// Offset horizontal appliqu� au dessin, permetant de le d�caler.
	int mpOffsetX = -64;

	/// Offset vertical appliqu� au dessin, permetant de le d�caler.
	int mpOffsetY = 3;

	/// Vitesse par d�faut du crayon, avant l'appel �ventuel de setSpeed() (en m/s).
	unsigned int mpDefaultSpeed = 20;

	/// Position par d�faut du point de d�part du crayon, avant l'appel �ventuel de setPosition().
	/// Todo Prendre en charge dans le fichier de param�tres.
	Position mpDefaultPosition = CENTER;

	/// Position du point d'arriv�e du crayon
	/// Todo Prendre en charge dans le fichier de param�tres.
	Position mpEndPosition = CENTER;

	/***********
	* M�thodes *
	***********/

	/**
	 * \brief D�termine si la position d�sir�e se trouve dans la zone de dessin (\a true) ou non (\a false).
	 * \details Envoie un warning si c'est le cas.
	 * \return \a true si la position d�sir�e se trouve dans la zone de dessin, \a false sinon
	 * \todo Possibilit� de d�sactiver la correction de trajectoire si en dehors des limites.
	 */
	bool positionInsideArea(
				float x,
				float y);

	/**
	 * \brief Trace une ligne � bas niveau, de la position actuelle � la position absolue [\a x; \a y], en �crivant (\a writing) ou pas.
	 * \details Ne compense pas le courbure naturelle de la ligne. N'initialise pas les variables pour les courbes Bezier, � la diff�rence de line(float x, float y).
	 * \param pX La position absolue horizontale du point de destination.
	 * \param pY La position absolue verticale du point de destination.
	 * \param writing \a true pour �crire pendant le d�placement, \a false pour ne pas �crire.
	 * \todo Trop long, �clater la m�thode.
	 */
	void line(
				float pX,
				float pY,
				bool writing);

	/**
	 * \brief Initialise le ratio entre le nombre de pas et la distance.
	 * \details Le ratio est calcul� en fonction du diametre moteur et du nombre de pas.
	 * xx(mm)*ratio --> xx(pas)
	 * xx(pas)/ratio --> xx(mm)
	 * \todo Inverser le ratio car le nombre de pas est une valeur enti�re, �viter de le diviser.
	 */
	void initStepLength(
				);

	/**
	 * \brief Modifie l'�chelle pour s'adapter � la largeur \a width et la hauteur \a height du dessin.
	 * \details Les dimentions du dessin sont r�cup�r�es sur le fichier svg.
	 * \todo Changer le nom et retourner l'�chelle plut�t que de la modifier directement.
	 */
	void setDrawingScale(
				int width,
				int height);

	/**
	 * \brief Fonction appel�e lorsque une erreur se produit.
	 * \details �loigne le stylo de la paroi et stoppe le programme. Envoie le code d'erreur \a errNumber au PC, qui se charge d'afficher sa description. 
	 * \todo Mettre en pause le tra��, quand la pause sera op�rationnelle.
	 */
	void error(
				Error errorNumber);

	/**
	 * \brief Fonction appel�e lorsque un warning se produit.
	 * \details Envoie le code de warning \a errNumber � Processing, qui se charge d'afficher sa description sans affecter le d�roulement du programme.
	 * \todo Mettre en pause le tra��, quand la pause sera op�rationnelle.
	 */
	void warning(
				Error warningNumber);

	/***********************
	* Commande du mat�riel *
	***********************/

	/**
	 * \brief Rotation du moteur gauche d'un pas.
	 * \param pull Sens du pas a effectuer : \a true pour tirer, \a false pour relacher.
	 */
	void leftStep(
				bool pull);

	/**
	 * \brief Rotation du moteur droit d'un pas.
	 * \param pull Sens du pas a effectuer : \a true pour tirer, \a false pour relacher.
	 */
	void rightStep(
				bool pull);

	/**
	 * \brief Alimente ou d�salimente les moteurs.
	 * \details �loigne le stylo de la paroi avant la d�salimentation pour �viter de dessiner pendant la chute �ventuelle du moteur.
	 * \param power \a true pour alimenter le moteur, \a false pour le d�salimenter.
 	* \todo S�parer la d�sactivation moteur gauche et moteur droit.
	 */
	void power(
				bool power);

	/**************
	* Convertions *
	**************/

	/**
	 * \brief R�cup�re la position horizontale de \a position.
	 * \details Cf. type enum \a Position.
	 * \param position La position a convertir, de type \a Position.
	 * \return La position verticale de \a position.
	 */
	float positionToX(
				Position position);

	/**
	 * \brief R�cup�re la position verticale de \a position.
	 * \details Cf. type enum \a Position.
	 * \param position La position a convertir, de type \a Position.
	 * \return La position verticale de \a position.
	 */
	float positionToY(
				Position position);

	/**
	 * \brief Calcule la longueur du c�ble gauche pour la position [\a x ; \a y]
	 * \param x La position absolue horizontale du point.
	 * \param y La position absolue horizontale du point.
	 * \return La longueur du c�ble gauche pour cette position, en nombre de pas.
	 */
	long positionToLeftLength(
				float x,
				float y);

	/**
	 * \brief Calcule la longueur du c�ble droit pour la position [\a x ; \a y]
	 * \param x La position absolue horizontale du point.
	 * \param y La position absolue horizontale du point.
	 * \return La longueur du c�ble droit pour cette position, en nombre de pas.
	 */
	long positionToRightLength(
				float x,
				float y);

	/*******************
	* Lecture carte SD *
	*******************/

	/**
	 * \brief Initialise de la carte SD.
	 * \details Peut g�n�rer l'erreur 01 : Carte absente ou non reconnue.
	 * \param fileName Le nom du fichier � lire.
	 * \todo : si erreur et nom fichier > 8 car, proposer de v�rifer si carte format�e en fat16.
	 */
	void sdInit(
				const char *fileName);

	/**
	 * \brief R�cupere la valeur de l'attribut \a attribute du fichier svg sous forme textuelle.
	 * \details R�cup�re le prochain mot correspondant � la cha�ne sp�cifi�e.
	 * Ex.1 : "width" --> "21cm"
	 * Ex.2 : "style" --> "fill:none;stroke:#000000"
	 * \param attribute Le nom de l'attribut � lire, ex : "width".
	 * \param value Cha�ne dans laquelle copier la valeur de l'attribut.
	 * \todo D�tecter le '>' pour chercher uniquement dans la balise.
	 * \todo Voir pourquoi pas de char* en sortie.
	 */
	void getAttribute(
				const char *attribute,
				char *value);

	/**
	 * \brief R�cupere la valeur de l'attribut \a attribute du fichier svg sous forme num�rique (flotant).
	 * \details R�cup�re le prochain mot correspondant � la cha�ne sp�cifi�e et le convertit en flotant en prenant en compte son unit� (cm, mm, ...).
	 * Ex.1 : width --> 210
	 * \param attribute Le nom de l'attribut � lire.
	 * \todo Convertir les unit�s en mm, pas en px.
	 */
	float getNumericAttribute(
				const char *attribute);

	/**
	 * \brief Informe si le caract�re \a car est un chiffre.
	 * \param car Le caract�re � tester.
	 * \return Renvoie \a true si le caract�re est un chiffre, \a false sinon.
	 */
	bool isNumber(
				char car);

	/**
	 * \brief Cherche le mot \a word dans le fichier.
	 * \details Une fois trouv�, le curseur sera positionn� � la fin du mot.
	 * \param word Le mot � chercher.
	 * \return Renvoie \a false si le mot n'a pas �t� trouv�.
	 */
	bool sdFind(
				const char *word);

	/**
	 * \brief R�cup�re \a nbParams param�tres, les convertissent en float et les stoquent dans le tableau \a tabParams.
	 * \details Les param�tres sont des nombres s�par�s par un espace ou une virgule.
	 * Ex : d="m 150.0 10.0, l 20 20"
	 * \param tabParams Le tableau de param�tres dans lequel seront stoqu� les nombres.
	 * \param nbParams Le nombre de param�tres.
	 * \todo Voir si on peut renvoyer le tableau.
	 */
	void getParameters(
				float *tabParams,
				int nbParams);

	/**
	 * \brief Dessine une figure vectorielle correspondant au contenu de l'attribut d="..." d"une balise \a path.
	 * \details Le curseur doit �tre positionn� au d�but du contenu de d"..."
	 */
	void draw(
				);

	/**
	 * \brief Approche ou �loigne le crayon de la paroi.
	 * \param write \a true pour plaquer le crayon contre la paroi (tra��), \a false pour l'�loigner (d�placement)
	 */
	void write(
				bool write);

  public:

	/**
	 * \brief Initialise la librairie.
	 * \bug Corriger Warning
	 */
	 Drawall(
				);

	/**
	 * \brief D�marre la librairie.
	 * \details \b N�cessaire au fonctionnement de la librairie. R�alise les proc�dures d'initialisation du robot.
	 * \param fileName Nom du fichier de configuration � charger.
	 */
	void begin(
				const char *fileName);

	/**
	 * \brief Finit le tra��.
	 * \details Utilis� � la fin du programme. Cela positionne le crayon en bas de la zone de dessin, d�salimente les moteurs et met en pause le programme.
	 */
	void end(
				);

	/********************
	* Getters & setters *
	********************/

	/**
	 * \brief Renvoie la position horizontale actuelle du crayon.
	 * \return La position horizontale du crayon.
	 */
	float getPositionX(
				);

	/**
	 * \brief Renvoie la position verticale actuelle du crayon.
	 * \return La position verticale du crayon.
	 */
	float getPositionY(
				);

	/**
	 * \brief Sp�cifie la position initiale du crayon.
	 * \details � utiliser avant de commencer � tracer.
	 * \param x La position horizontale du crayon.
	 * \param y La position verticale du crayon.
	 */
	void setPosition(
				float x,
				float y);

	/**
	 * \brief Sp�cifie la position initiale du crayon.
	 * \details � utiliser avant de commencer � tracer.
	 * \param position La position du crayon (voir type enum \a Position).
	 * \bug Ne fonctionne pas sur le sketch.
	 */
	void setPosition(
				Position position);

	/**
	 * \brief Sp�cifie la vitesse du tra�� (en mm/s).
	 * \details Cette vitesse correspond � la vitesse de d�placement de la courroie qui effectue la plus grande distance entre 2 points (Cf. \a mDelay). La vitesse r�elle du dessin sera donc plus lente.

	 * \param speed La vitesse du tra��.
	 * \bug La vitesse diminue si on augmente le nombre de pas.
	 */
	void setSpeed(
				unsigned int speed);

	/**********************
	* Fonctions de dessin *
	**********************/

	/**
	 * \brief D�place le crayon � la position absolue [\a x ; \a y].
	 * \param x La position absolue horizontale du point de destination.
	 * \param y La position absolue verticale du point de destination.
	 */
	void moveAbs(
				float x,
				float y);

	/**
	 * \brief D�place le crayon � la position absolue \a position.
	 * \param position La position absolue du point de destination (Cf type enum \a Position)
	 */
	void move(
				Position position);

	/**
	 * \brief D�place le crayon � la position relative [\a x ; \a y].
	 * \details Une position relative correspond � la position d'un point par rapport � la position actuelle.
	 * \param x La position relative horizontale du point de destination.
	 * \param y La position relative verticale du point de destination.
	 */
	void moveRel(
				float x,
				float y);

	/**
	 * \brief Trace une ligne droite, de la position actuelle � la position absolue [\a x; \a y].
	 * \param x La position absolue horizontale du point de destination.
	 * \param y La position absolue verticale du point de destination.
	 * \bug Fait des escaliers dans certains cas (?).
	 */
	void lineAbs(
				float x,
				float y);

	/**
	 * \brief Trace une ligne droite, de la position actuelle � la position relative [\a x; \a y].
	 * \details Une position relative correspond � la position d'un point par rapport � la position actuelle.
	 * \param x La position relative horizontale du point de destination.
	 * \param y La position relative verticale du point de destination.
	 */
	void lineRel(
				float x,
				float y);

	/**
	 * \brief Trace un rectangle de largeur \a x et de hauteur \a y, avec pour point haut-gauche la position actuelle.
	 * \param x La taille des arr�tes horizontales.
	 * \param y La taille des arr�tes verticales.
	 */
	void rectangle(
				float x,
				float y);

	/**
	 * \brief Trace un rectangle repr�sentant les limites de la zone de dessin.
	 */
	void area(
				);

	/**
	 * \brief Trace un rectangle correspondant � la zone de dessin du fichier svg \a fileName.
	 * \details Le robot doit �tre muni d'un lecteur de carte SD et le fichier vectoriel doit �tre copi� sur celle-ci.
	 * \param fileName Le nom du fichier.
	 * \bug Le rectangle sort de la zone de dessin dans certains cas (test� aux valeurs 4810, 3950, 2830, 430, 640) + limite droite non-d�tect�e.

	 */
	void drawingArea(
				const char *fileName);

	/**
	 * \brief Trace une courbe de B�zier cubique en position absolue.
	 * \details La courbe est compos�e de 100 segments.
	 * Avec en point de d�part la position actuelle, en point de destination [\a pX ; \a pY] et en points de contr�le P1 [\a p1X ; \a p1Y] et P2 [\a p2X ; \a p2Y].
	 * Cf l'article Wikip�dia sur les courbes de B�zier : \a http://fr.wikipedia.org/wiki/Courbe_de_B%C3%A9zier .
	 * �quation de la courbe : B(t) = P0(1-t)^3 + 3*P1*t(1-t)^2 + 3*P2*t^2*(1-t) + P3*t^3. Avec t allant de 0 � 1.
	 * \param pX La position absolue horizontale du point de destination.
	 * \param pY La position absolue verticale du point de destination.

	 * \param p1X La position absolue horizontale du point de contr�le P1.
	 * \param p1Y La position absolue verticale du point de contr�le P1.
	 * \param p2X La position absolue horizontale du point de contr�le P2.
	 * \param p2Y La position absolue verticale du point de contr�le P2.
	 */
	void cubicCurveAbs(
				float pX,
				float pY,
				float p1X,
				float p1Y,
				float p2X,
				float p2Y);

	/**
	 * \brief Trace une courbe de B�zier cubique en position relative.
	 * \details La courbe est compos�e de 100 segments.
	 * Avec en point de d�part la position actuelle, en point de destination [\a pX ; \a pY] et en points de contr�le P1 [\a p1X ; \a p1Y] et P2 [\a p2X ; \a p2Y].
	 * Cf l'article Wikip�dia sur les courbes de B�zier : \a http://fr.wikipedia.org/wiki/Courbe_de_B%C3%A9zier .
	 * \param pX La position relative horizontale du point de destination.
	 * \param pY La position relative verticale du point de destination.
	 * \param p1X La position relative horizontale du point de contr�le P1.
	 * \param p1Y La position relative verticale du point de contr�le P1.
	 * \param p2X La position relative horizontale du point de contr�le P2.
	 * \param p2Y La position relative verticale du point de contr�le P2.
	 */
	void cubicCurveRel(
				float pX,
				float pY,
				float p1X,
				float p1Y,
				float p2X,
				float p2Y);

	/**
	 * \brief Trace une courbe de B�zier cubique en position absolue.
	 * \details La courbe est compos�e de 100 segments.
	 * Avec en point de d�part la position actuelle, en point de destination [\a pX ; \a pY] et point P2 [\a p2X ; \a p2Y].
	 * Le point de contr�le P1 est l'image du point P2 de la derni�re courbe de B�zier. Si la derni�re figure n'en �tait pas une, il correspond au point courant.
	 * Cf l'article Wikip�dia sur les courbes de B�zier : \a http://fr.wikipedia.org/wiki/Courbe_de_B%C3%A9zier .
	 * \param pX La absolue relative horizontale du point de destination.
	 * \param pY La absolue relative verticale du point de destination.
	 * \param p2X La absolue relative horizontale du point de contr�le P2.
	 * \param p2Y La absolue relative verticale du point de contr�le P2.
	 */
	void cubicCurveAbs(
				float pX,
				float pY,
				float p2X,
				float p2Y);

	/**
	 * \brief Trace une courbe de B�zier cubique en position relative.
	 * \details La courbe est compos�e de 100 segments.
	 * Avec en point de d�part la position actuelle, en point de destination [\a pX ; \a pY] et point P2 [\a p2X ; \a p2Y].
	 * Le point de contr�le P1 est l'image du point P2 de la derni�re courbe de B�zier. Si la derni�re figure n'en �tait pas une, il correspond au point courant.
	 * Cf l'article Wikip�dia sur les courbes de B�zier : \a http://fr.wikipedia.org/wiki/Courbe_de_B%C3%A9zier .
	 * \param pX La position relative horizontale du point de destination.
	 * \param pY La position relative verticale du point de destination.
	 * \param p2X La position relative horizontale du point de contr�le P2.
	 * \param p2Y La position relative verticale du point de contr�le P2.
	 */
	void cubicCurveRel(
				float pX,
				float pY,
				float p2X,
				float p2Y);

	/**
	 * \brief Trace une courbe de B�zier quadratique en position absolue.
	 * \details La courbe est compos�e de 100 segments.
	 * Avec en point de d�part la position actuelle, en point de destination [\a pX ; \a pY] et en point de contr�le P1 [\a p1X ; \a p1Y].
	 * Cf l'article Wikip�dia sur les courbes de B�zier : \a http://fr.wikipedia.org/wiki/Courbe_de_B%C3%A9zier .
	 * �quation de la courbe : B(t) = P0*(1-t)^2 + 2*P1*t(1-t) + P2*t^2. Avec t allant de 0 � 1.
	 * \param pX La position absolue horizontale du point de destination.
	 * \param pY La position absolue verticale du point de destination.
	 * \param p1X La position absolue horizontale du point de contr�le P1.
	 * \param p1Y La position absolue verticale du point de contr�le P1.
	 */
	void quadraticCurveAbs(
				float pX,
				float pY,
				float p1X,
				float p1Y);

	/**
	 * \brief Trace une courbe de B�zier quadratique en position relative.
	 * \details La courbe est compos�e de 100 segments.
	 * Avec en point de d�part la position actuelle, en point de destination [\a pX ; \a pY] et en point de contr�le P1 [\a p1X ; \a p1Y].
	 * Cf l'article Wikip�dia sur les courbes de B�zier : \a http://fr.wikipedia.org/wiki/Courbe_de_B%C3%A9zier .
	 * \param pX La position relative horizontale du point de destination.
	 * \param pY La position relative verticale du point de destination.
	 * \param p1X La position relative horizontale du point de contr�le P1.
	 * \param p1Y La position relative verticale du point de contr�le P1.
	 */
	void quadraticCurveRel(
				float pX,
				float pY,
				float p1X,
				float p1Y);

	/**
	 * \brief Trace une courbe de B�zier quadratique en position absolue.
	 * \details La courbe est compos�e de 100 segments.
	 * Avec en point de d�part la position actuelle et en point de destination [\a pX ; \a pY].
	 * Le point de contr�le P1 est l'image du point P2 de la derni�re courbe de B�zier. Si la derni�re figure n'en �tait pas une, il correspond au point courant.
	 * Cf l'article Wikip�dia sur les courbes de B�zier : \a http://fr.wikipedia.org/wiki/Courbe_de_B%C3%A9zier .

	 * \param pX La position absolue horizontale du point de destination.
	 * \param pY La position absolue verticale du point de destination.
	 */
	void quadraticCurveAbs(
				float pX,
				float pY);

	/**
	 * \brief Trace une courbe de B�zier quadratique en position relative.
	 * \details La courbe est compos�e de 100 segments.
	 * Avec en point de d�part la position actuelle, en point de destination [\a pX ; \a pY].
	 * Le point de contr�le P1 est l'image du point P2 de la derni�re courbe de B�zier. Si la derni�re figure n'en �tait pas une, il correspond au point courant.
	 * Cf l'article Wikip�dia sur les courbes de B�zier : \a http://fr.wikipedia.org/wiki/Courbe_de_B%C3%A9zier .
	 * \param pX La position relative horizontale du point de destination.
	 * \param pY La position relative verticale du point de destination.
	 */
	void quadraticCurveRel(
				float pX,
				float pY);

	/**
	 * \brief Fini la figure SVG en cours.
	 * \details Tr�ce une ligne droite jusqu'� la position du point cible du dernier d�placement, de mani�re a faire des formes "finies".
	 */
	void endFigure(
				);

	/**
	 * \brief Trace un dessin vectoriel svg correspondant au fichier \a fileName sur la carte SD.
	 * \details Le robot doit �tre muni d'un lecteur de carte SD et le fichier vectoriel doit �tre copi� sur celle-ci.
	 * \param fileName Le nom du fichier � dessiner.
	 * \todo Ajouter param�tre Position position (CENTER, ...).
	 */
	void svg(
				const char *fileName);

	/**
	 * \Brief Charge les param�tres � partir d'un fichier de configuration pr�sent sur la carte SD.
	 * \param fileName Le nom du fichier de configuration � charger.
	*/
	void loadParameters(
				const char *fileName);

	/**
	 * \Brief Imprime sur la liaison s�rie les param�tres actuels.
	*/
	void printParameters(
				);
};

#endif
