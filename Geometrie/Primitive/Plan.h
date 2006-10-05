/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Plan.h - Plan.cpp

 Desc:   Classe gérant la primitive de Pavé (cube ou parallélépipède rectangle)

*******************************************************************************/

#ifndef Plan_H
#define Plan_H
//______________________________________________________________________________
#include <iostream>
#include <math.h>
#include "../../Math/Vertex.h"
#include "Figure.h"

//______________________________________________________________________________
using namespace std;
//______________________________________________________________________________
class Plan : public Figure {

	int IdListe;

    public :
        double Largeur;      
        double Profondeur;
		int SubDivisionsX;
		int SubDivisionsY;

        Plan();

        Plan(double Largeur_p,			
			 double Profondeur_p,
			 int SubldivisionsX_p,
			 int SubldivisionsY_p			 
			 ); 

		Plan(double Largeur_p,           
             double Profondeur_p,
			 int SubldivisionsX_p,
			 int SubldivisionsY_p,
			 Vertex * Centre_p
             );

        ~Plan();

		virtual Type_Figure GetTypeFig () {return plan_f;}

        void GenererPoints ();

		void GenererPoints (Vertex * Centre_p);

        void Dessiner(Type_Affichage typeAff_p);

		void DessinerListe();

		void CreerListe(Type_Affichage typeAff_p);

		void Modifier(double Largeur_p, double Hauteur_p);

		Figure* Dupliquer();


	private:
		Vertex* GetVertex(int i, int j);


};
//______________________________________________________________________________
#endif
