/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Lo�c DASSONVILLE
 But:     Moteur 3D
 Fichier: Constantes.h - Constantes.cpp
 Desc:   
*******************************************************************************/
#ifndef Constantes_H
#define Constantes_H

//______________________________________________________________________________

#include <math.h> 
#include <windows.h>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

//______________________________________________________________________________

#define M_PI_PAR_2  1.5707963267948966192313216916398
#define M_PI_FOIS_2 6.283185307179586476925286766559
#define M_PI        3.1415926535897932384626433832795                                                      

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
#endif
