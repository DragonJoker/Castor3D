/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Cone.h - Cone.cpp

 Desc:   Classe gérant la primitive du cône

*******************************************************************************/
#ifndef Cone_H
#define Cone_H    
//______________________________________________________________________________
#include <iostream>
#include "../ComposantGeo/Vertex.h"
#include "Figure.h"
//______________________________________________________________________________
using namespace std;                                                            
//______________________________________________________________________________
class Cone : public Figure {
   
 public :

        int NbFaces;
        double Hauteur;
        double Rayon;      

        virtual Type_Figure GetTypeFig () {return cone_f;}
        Cone ();

		Cone (double Hauteur_p,
              double Rayon_p,              
              int NbFaces_p			  
              );

        Cone (double Hauteur_p,
              double Rayon_p,             
              int NbFaces_p,
			  Vertex * Centre_p
              );

        ~Cone ();

        void GenererPoints ();

		void GenererPoints (Vertex * Centre_p);

		void CreerListe1(Type_Affichage typeAff_p);

		void CreerListe2(Type_Affichage typeAff_p);

        void Dessiner(Type_Affichage typeAff_p);

		void DessinerKillBox();

		void DessinerListe1();

		void DessinerListe2();

		void GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l);

		void CalculerExtremes();

		void Modifier(double Rayon_p, double Hauteur_p);


};                                                                              
//______________________________________________________________________________
#endif
