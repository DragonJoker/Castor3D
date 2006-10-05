/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Plan.h - Plan.cpp

 Desc:   Classe gérant la primitive de Pavé (cube ou parallélépipède rectangle)

*******************************************************************************/

#ifndef Terrain_H
#define Terrain_H
//______________________________________________________________________________
#include <iostream>
#include <math.h>
#include "../../Math/Vertex.h"
#include "Figure.h"

//______________________________________________________________________________
using namespace std;

typedef struct tagPOINTMAP {
    BYTE Height;			
    BYTE Red;	
    BYTE Green;
    BYTE Blue;
} POINTMAP;

//______________________________________________________________________________
class Terrain : public Figure {

	int IdListe;

    public :
        double Largeur;      
        double Profondeur;
		int SubDivisionsX;
		int SubDivisionsY;
		POINTMAP ** HeightMap;

        Terrain();

        Terrain(double Largeur_p,			
			 double Profondeur_p,
			 int SubldivisionsX_p,
			 int SubldivisionsY_p			 
			 ); 

		Terrain(double Largeur_p,           
             double Profondeur_p,
			 int SubldivisionsX_p,
			 int SubldivisionsY_p,
			 Vertex * Centre_p
             );

		Terrain(std::string NomFichier);

        ~Terrain();

		virtual Type_Figure GetTypeFig () {return terrain_f;}

        void GenererPoints ();

		void GenererPoints (Vertex * Centre_p);

		void GenererPoints(std::string NomFichier);

		LONG LecteurLong(FILE *pFile);

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
