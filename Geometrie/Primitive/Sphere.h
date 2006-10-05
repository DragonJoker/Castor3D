/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Sphere.h - Sphere.cpp

 Desc:   Classe gérant la primitive de la Sphere

*******************************************************************************/
#ifndef Sphere_H
#define Sphere_H
//______________________________________________________________________________
#include <iostream>
#include "../../Math/Vertex.h"
#include "../Bases/Arcs.h"
#include "Figure.h"
//______________________________________________________________________________
using namespace std;                                                            
//______________________________________________________________________________
class Sphere : public Figure {

    public :

		int IdListe;    //Identifiant de la liste d'affichage
        double Rayon;
		int NbFaces;

        virtual Type_Figure GetTypeFig () {return sphere_f;}

        Sphere ();

        Sphere (double Rayon_p,
                  int NbFaces_p				  
                  );

        Sphere (double Rayon_p,
                  int NbFaces_p,
				  Vertex * Centre_p
                  );


        ~Sphere ();

		void CreerListe(Type_Affichage typeAff_p);

        void GenererPoints ();

		void GenererPoints (Vertex * Centre_p);

        void Dessiner(Type_Affichage typeAff_p);

		void DessinerListe();

		void Modifier(double Rayon);

		Figure* Sphere::Dupliquer();


};                                                                              
//______________________________________________________________________________
#endif
 



