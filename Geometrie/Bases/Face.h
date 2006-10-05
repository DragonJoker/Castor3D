/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Face.h - Face.cpp

 Desc:   Classe gérant les faces, contient la normale d'une face

*******************************************************************************/
#ifndef Face_H
#define Face_H
//______________________________________________________________________________
#include "Constantes.h"
#include "../../Math/Vertex.h"
#include "../../Math/Vecteur.h"
//______________________________________________________________________________
class Face{
  
public :
		Vertex* _v1;
		Vertex* _v2;
		Vertex* _v3;
       
        Face();

        ~Face();

        virtual Type_Geometrie GetTypeGeom() {return face_g;}
        
        void DessinerPlein();

        void DessinerFilaire();

  		Vertex* CalculerCentreFace();


};
//______________________________________________________________________________
#endif
