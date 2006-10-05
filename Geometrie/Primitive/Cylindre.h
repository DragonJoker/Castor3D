/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Cylindre.h - Cylindre.cpp

 Desc:   Classe gérant la primitive du cylindre

*******************************************************************************/
#ifndef Cylindre_H
#define Cylindre_H
//______________________________________________________________________________
#include <iostream>
#include "../../Math/Vertex.h"
#include "Figure.h"
#include <math.h>

//______________________________________________________________________________
using namespace std;                                                            
//______________________________________________________________________________
class Cylindre : public Figure {

    public :

		int IdListe;    //Identifiant de la liste d'affichage
        double Hauteur;
        double Rayon;
		int NbFaces;

        virtual Type_Figure GetTypeFig () {return cylindre_f;}

        Cylindre ();

        Cylindre (double Hauteur_p,
                  double Rayon_p,
                  int NbFaces_p				  
                  );

        Cylindre (double Hauteur_p,
                  double Rayon_p,
                  int NbFaces_p,
				  Vertex * Centre_p
                  );


        ~Cylindre ();

		void CreerListe(Type_Affichage typeAff_p);

        void GenererPoints ();

		void GenererPoints (Vertex * Centre_p);

        void Dessiner(Type_Affichage typeAff_p);

		void DessinerListe();

		void Modifier(double Rayon, double Hauteur);
		
		Figure* Dupliquer();


};                                                                              
//______________________________________________________________________________
#endif
 



