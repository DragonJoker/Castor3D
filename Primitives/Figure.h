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
//______________________________________________________________________________
#include "../ComposantGeo/Geometrie.h"
#include "../ComposantGeo/Vertex.h"  
#include "../ComposantGeo/Face.h"
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

 endEnum_f
};
//______________________________________________________________________________
class Figure : public Geometrie {
  
protected:
		Vertex* _Centre;
		Vertex* _CoordMin; 
		Vertex* _CoordMax;
public:		
		vector < Vertex * > vMesVertex;

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

		virtual void DessinerKillBox();

		virtual void CreerListe1(Type_Affichage typeAff_p);

		virtual void CreerListe2(Type_Affichage typeAff_p);

		virtual void GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l);

		virtual void CalculerExtremes();

		

};
//______________________________________________________________________________
#endif
 