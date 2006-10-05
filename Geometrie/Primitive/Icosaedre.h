/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Icosaedre.h - Icosaedre.cpp

 Desc:   Classe gérant la primitive de l'icosaedre

*******************************************************************************/
#ifndef Icosaedre_H
#define Icosaedre_H
//______________________________________________________________________________
#include <iostream>
#include "../../Math/Vertex.h"
#include "../Bases/Arcs.h"
#include "Figure.h"
//______________________________________________________________________________
using namespace std;                                                            
//______________________________________________________________________________
class Icosaedre : public Figure {

    public :

		int IdListe;    //Identifiant de la liste d'affichage
        double Rayon;
		int NbFaces;

        virtual Type_Figure GetTypeFig () {return icosaedre_f;}

        Icosaedre ();

        Icosaedre (double Rayon_p,
                  int Precision_p				  
                  );

        Icosaedre (double Rayon_p,
                  int Precision_p,
				  Vertex * Centre_p
                  );


        ~Icosaedre ();

		void CreerListe(Type_Affichage typeAff_p);

        void GenererPoints ();

		void GenererPoints (Vertex * Centre_p);

        void Dessiner(Type_Affichage typeAff_p);

		void DessinerListe();

		void Modifier(double Rayon);

		Figure * Dupliquer();

		void Subdiviser ();


};                                                                              
//______________________________________________________________________________
#endif
 



