/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Pave.h - Pave.cpp

 Desc:   Classe gérant la primitive de Pavé (cube ou parallélépipède rectangle)

*******************************************************************************/

#ifndef Pave_H
#define Pave_H
//______________________________________________________________________________
#include <iostream>
#include <math.h>
#include "../ComposantGeo/Vertex.h"
#include "../ComposantGeo/Arcs.h"
#include "Figure.h"

//______________________________________________________________________________
using namespace std;
//______________________________________________________________________________
class Pave : public Figure {

	int IdListe;

    public :
        double Largeur;
        double Hauteur;
        double Profondeur;


        Pave();

        Pave(double Largeur_p,
			 double Hauteur_p, 
			 double Profondeur_p); 

		Pave(double Largeur_p,
             double Hauteur_p,
             double Profondeur_p,
			 Vertex * Centre_p
             );

        ~Pave();

		virtual Type_Figure GetTypeFig () {return pave_f;}

        void GenererPoints ();

		void GenererPoints (Vertex * Centre_p);

        void Dessiner(Type_Affichage typeAff_p);

		void DessinerListe();

		void DessinerKillBox();

		void CreerListe(Type_Affichage typeAff_p);

		void GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l);

		void CalculerExtremes();

		void Modifier(double Largeur_p, double Hauteur_p, double Profondeur_p );



};
//______________________________________________________________________________
#endif
