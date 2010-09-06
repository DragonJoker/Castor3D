/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Tourneur.h - Tourneur.cpp

 Desc:   Classe gérant la primitive composée d'une silhouette à
         laquelle on affecte une rotation

*******************************************************************************/

#ifndef Tourneur_H
#define Tourneur_H
//______________________________________________________________________________
#include <iostream>
#include <math.h>
#include "Vertex3d.h"
#include "Figure.h"  
#include "Arcs.h"
//______________________________________________________________________________
using namespace std;
//______________________________________________________________________________
class Tourneur : public Figure {
    public :
        Arcs * Silhouette;
        double Angle;
        int NbFaces;

        virtual Type_Figure GetTypeFig () {return tourneur_f;}
        Tourneur();
        Tourneur(double AlphaX_p,
                   double AlphaZ_p,
                   Vertex3d * Centre_p,
                   Arcs * Silhouette_p,
                   int nbFaces,
                   double angle_p,
                   Vertex3d * coul,
                   float shininess,
                   bool ferme);
        ~Tourneur();
        void GenererPoints ();
        virtual void Dessiner(int type, Referentiel * ref, bool norm);
        bool IsInPositions (int i, int nb, int * positions);
        Arcs * GetPointsBas ();
        Arcs * GetPointsHaut ();
        bool CentreDansSegment (Vertex3d * v0, Vertex3d * v1);
        bool CentreRotCrossSil ();
};
//______________________________________________________________________________
#endif
 