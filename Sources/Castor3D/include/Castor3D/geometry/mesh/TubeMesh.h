/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Tube.h - Tube.cpp

 Desc:   Classe gérant la primitive du Tube

*******************************************************************************/
#ifndef Tube_H
#define Tube_H
//______________________________________________________________________________
#include <iostream>
#include "Vertex3d.h"
#include "Figure.h"
//______________________________________________________________________________
using namespace std;                                                            
//______________________________________________________________________________
class Tube : public Figure {
    public :
        int NbFaces;
        double Rayon;


        virtual Type_Figure GetTypeFig () {return tube_f;}
        Tube ();
        Tube (    Vertex3d * Centre_p, 
                  double AlphaX_p,
                  double AlphaZ_p,
                  double RayonX_p,
                  Arcs * Arc_p,
                  int NbFaces_p,
                  Vertex3d * coul,
                  float shininess);
        ~Tube ();
        void GenererPoints (Arcs *Arc_p);
        virtual void Dessiner(int type, Referentiel * ref, bool norm);

    private :
        double Abs(double Valeur_p);
        double GetAngleObtus(double Angle_p);
        double GetAngleAigu(double Angle_p);

        double RadianToDegre( double AngleRad_p);

        bool IsAigu(double Angle_p);
        bool IsAlphaObtus(Arcs* Arc_p, int Position_p);
        bool IsBetaObtus(Arcs* Arc_p, int Position_p);

        //Calcule les angles de la rotation d'une droite selon X et Z dans un espace 3D
        void CalculerAngle1Droite3D(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p, int Position_p);
        void CalculerAngle2Droite3D(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p, int Position_p);

        //Calcul les angles de rotation formé par de droite dans un espace 3D
        void AngleFormeParDeuxDroites3D(double AlphaD1, double BetaD1, double AlphaD2, double BetaD2, double &rAlphaResult, double &rBetaResult);


        //Calcule les angles formé par rotation d'une droite selon X et Z selon une vue 2D
        void CalculerAngle1Projection(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p, int Position_p);
        void CalculerAngle2Projection(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p, int Position_p);

        void CalculerAngleFirstFace(Arcs *Arc_p, Double &Alpha, Double &Beta);
        void CalculerAngleLastFace(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p);
        double CalculerAngleFaceMilieu(Arcs *Arc_p,Double &DecalageAlpha_p, Double &DecalageBeta_p, Double &Alpha_p, Double &Beta_p, int position);

        void AngleFormeParDeuxDroites2D(double AlphaD1, double BetaD1, double AlphaD2, double BetaD2, double &rAlphaResult, double &rBetaResult);

        Arcs * CreerArc(double Alpha_p, double Beta_p, double angleRotation, double RayonX, double RayonZ,Arcs* Arc_p, int positionArc);



};
//______________________________________________________________________________
#endif

 