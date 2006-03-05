/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Figure.h - Figure.cpp

 Desc:  Classe gérant les figures primitives (cylindres, cônes, sphères...)
        Contient les faces de la figure.

*******************************************************************************/

#include "Figure.h"
//______________________________________________________________________________
Figure :: Figure ()
{
	IdListe1= -1;
	IdListe2= -1;
}
//______________________________________________________________________________
Figure :: ~Figure()
{
	vector <Vertex*>::iterator Iter1;
	for( Iter1 = vMesVertex.begin( );Iter1 != vMesVertex.end( ) ; Iter1++ ){					
		delete((*Iter1));				
	}
	vMesVertex.erase(vMesVertex.begin(), vMesVertex.end());

	vector <Face*>::iterator Iter;
	for( Iter = vFaces.begin( );Iter != vFaces.end( ) ; Iter++ ){					
		delete((*Iter));				
	}
	vFaces.erase(vFaces.begin(), vFaces.end());
}
//______________________________________________________________________________
void Figure :: Dessiner(Type_Affichage typeAff_p)
{

} 
//______________________________________________________________________________

void Figure::DessinerListe1()
{

}
//______________________________________________________________________________
void Figure::DessinerListe2()
{

}
//______________________________________________________________________________

void Figure::CreerListe1(Type_Affichage typeAff_p)
{

}
//______________________________________________________________________________

void Figure::CreerListe2(Type_Affichage typeAff_p)
{

}
//______________________________________________________________________________

void Figure::GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l)
{
	pCoordMin_l = _CoordMin; 
	pCoordMax_l = _CoordMax;
}
//______________________________________________________________________________

void Figure::CalculerExtremes()
{

}
//______________________________________________________________________________

void Figure::DessinerKillBox()
{

}
//______________________________________________________________________________
