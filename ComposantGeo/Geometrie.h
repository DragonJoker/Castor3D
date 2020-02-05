/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Lo�c DASSONVILLE
 But:    Moteur 3D
 Fichier: Geometrie.h - Geometrie.cpp
 Desc:   Classe g�rant les g�om�tries de base g�r�es par le moteur
         (points, lignes, figures 3D)
         Contient les coordonn�es du centre de la g�om�trie,
         le r�f�rentiel de cette g�om�trie, les angles par rapport au
         r�f�rentiel parent, les dimensions de la ComboBox de la g�om�trie
*******************************************************************************/
#ifndef Geometrie_H
#define Geometrie_H
//______________________________________________________________________________
#include <math.h> 
#include <windows.h>
#include <vector>
#include <GL/glut.h>
#include <iostream>
//______________________________________________________________________________
#include "Vertex.h"
//______________________________________________________________________________
#define M_PI_PAR_2 1.5707963267948966192313216916398
#define M_PI_FOIS_2 6.283185307179586476925286766559
#define M_PI 3.1415926535897932384626433832795                                                      
//______________________________________________________________________________
using namespace std;
//______________________________________________________________________________
enum Type_Geometrie
{
	geometrie_g,
	point_g,
	arcs_g,
	face_g,
	figure_g,

	endEnum_g

};
//______________________________________________________________________________
enum Type_Affichage
{ 
	points,
	lignesOuvertes,
	lignesfermees,
	faces,
	facesTriangle
};
//______________________________________________________________________________
class Geometrie {
  
public :

	Geometrie ();

    ~Geometrie ();

	virtual Type_Geometrie GetTypeGeom () {return geometrie_g;}

	virtual void Dessiner(Type_Affichage typeAff_p);

	virtual void GenererPoints();       
};
//______________________________________________________________________________
#endif