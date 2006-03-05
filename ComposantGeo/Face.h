/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Face.h - Face.cpp

 Desc:   Classe gérant les faces, contient la normale d'une face

*******************************************************************************/
#ifndef Face_H
#define Face_H
//______________________________________________________________________________
//______________________________________________________________________________
#include "Geometrie.h"
#include "Arcs.h"   
#include "Vertex.h"
#include "Vecteur.h"
#include <iostream>
//______________________________________________________________________________
using namespace std;
//______________________________________________________________________________
class Face : public Geometrie {
    public :
        vector < Arcs * > vArcs;		
		Vertex* _CoordMin; 
		Vertex* _CoordMax;
       
        int NbPoints;


        Face();

        ~Face();

        virtual Type_Geometrie GetTypeGeom() {return face_g;}
        
        void Dessiner(Type_Affichage typeAff_p);

        void AddArcs(Arcs * arc);

		Vertex* CalculerCentreFace();

		void CalculerExtremes();

		void MajExtremes(Vertex * pPoint_p);

		Vertex* GetCoordMin();

		Vertex* GetCoordMax();


};
//______________________________________________________________________________
#endif
