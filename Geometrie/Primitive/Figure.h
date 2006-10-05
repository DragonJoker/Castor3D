/*******************************************************************************
 Auteur:  Sylvain DOREMUS + otr
 But:    Moteur 3D

 Fichier: Figure.h - Figure.cpp

 Desc:  Classe gérant les figures primitives (cylindres, cônes, sphères...)
        Contient les faces de la figure.

*******************************************************************************/
#ifndef Figure_H
#define Figure_H
//______________________________________________________________________________
#include "../../Math/Vertex.h"  
#include "../Bases/Face.h"
//______________________________________________________________________________
using namespace std;     
//______________________________________________________________________________
enum Type_Figure
{
 cone_f,
 cylindre_f,
 figure_f,
 sphere_f,
 tube_f,
 pave_f,
 projection_f,
 tourneur_f,
 plan_f,
 icosaedre_f,
 terrain_f,

 endEnum_f
};
//______________________________________________________________________________
class Figure{
  
protected:
		Vertex* _Centre;
		Vertex* _CoordMin; 
		Vertex* _CoordMax;
public:		
		vector < Vertex * > vMesVertex;
		float colorMaterial[3];
		float colorReflexion[3];
		float shininess[1];

public :
		int IdListe1;
		int IdListe2;

        vector < Face * > vFaces;

        Figure ();

        ~Figure();

        virtual Type_Geometrie GetTypeGeom () {return figure_g;}

        virtual Type_Figure GetTypeFig () {return figure_f;}

		virtual void Dessiner(Type_Affichage typeAff_p);

		virtual	void DessinerListe1();

		virtual	void DessinerListe2();

		void DessinerKillBox();

		virtual void CreerListe1(Type_Affichage typeAff_p);

		virtual void CreerListe2(Type_Affichage typeAff_p);

		void GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l);

		void CalculerExtremes();

		bool MemeCouleur (Figure * figure);

		virtual Figure* Dupliquer();

		virtual bool Ecrire(FILE* Fichier);
		
		virtual bool Lire(FILE* Fichier);

};
//______________________________________________________________________________
#endif
 