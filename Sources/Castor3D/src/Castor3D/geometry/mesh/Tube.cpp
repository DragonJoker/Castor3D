/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Tube.h - Tube.cpp

 Desc:   Classe gérant la primitive du Tube

*******************************************************************************/

#include "Tube.h"
//______________________________________________________________________________
Tube :: Tube ()
{
    NbFaces = 0;
    Rayon = 0.0;
    Centre = new Vertex3d();
}
//______________________________________________________________________________
Tube :: ~Tube ()
{

}
//______________________________________________________________________________
Tube :: Tube (Vertex3d * Centre_p,
                        double AlphaX_p,
                        double AlphaZ_p,
                        double Rayon_p,
                        Arcs * Arc_p,
                        int NbFaces_p,
                        Vertex3d * coul,
                        float shininess){
    Shininess = shininess;
    AlphaX = AlphaX_p;
    AlphaZ = AlphaZ_p;
    Rayon = Rayon_p;
    couleurMatiere = coul;
    Centre = new Vertex3d(Centre_p);
    NbFaces = NbFaces_p;
    GenererPoints(Arc_p);
}
//______________________________________________________________________________
void Tube :: CalculerAngle1Droite3D(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p, int Position_p)
{
        //On calcul les angles en radian

        Alpha_p = 0.0;  Beta_p  = 0.0;

        double X1 = (Arc_p->vSuivants[Position_p+1]->x - Arc_p->vSuivants[Position_p]->x);
        double Y1 = (Arc_p->vSuivants[Position_p+1]->y - Arc_p->vSuivants[Position_p]->y);
        double Z1 = (Arc_p->vSuivants[Position_p+1]->z - Arc_p->vSuivants[Position_p]->z);

        //Si les 2 point sont identiques on quitte avec des angles = 0
        if(X1 == 0 && Y1 == 0 && Z1 == 0){
                Alpha_p = 0;
                Beta_p = 0;
                return;
        }

        //Calcule de l'angle Alpha (rotation selon Z)
        double RayonVueDessus = sqrt(X1*X1+Z1*Z1);
        double LongeurDroite = sqrt(RayonVueDessus*RayonVueDessus + Y1*Y1);
        if(RayonVueDessus == 0){
                if( Y1 >0)
                        Alpha_p =  M_PI_Par_2;
                else
                        Alpha_p =  -M_PI_Par_2;
        }else{
                if( Y1 >0)
                        Alpha_p =  acos(Abs(RayonVueDessus) / Abs(LongeurDroite));
                else
                        Alpha_p = -acos(Abs(RayonVueDessus) / Abs(LongeurDroite));
        }

        //Calcul de l'angle Beta (rotation selon Y)

        if(X1 == 0){
                if( Z1 > 0)
                        Beta_p =   M_PI_Par_2;
                else if( Z1 < 0)
                        Beta_p =   -M_PI_Par_2;
                else    Beta_p =   0;

        }else{

                if( Z1 > 0){
                        if( X1 > 0 )
                                Beta_p = atan(Abs(Z1)/Abs(X1));
                        else
                                Beta_p = M_PI - atan(Abs(Z1)/Abs(X1));
                }else{
                        if( X1 > 0 )
                                Beta_p = -atan(Abs(Z1)/Abs(X1));
                        else
                                Beta_p = -M_PI + atan(Abs(Z1)/Abs(X1));
                }
        }

        cout<<"Angle Alpha de la droite "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Alpha_p)<<endl;
        cout<<"Angle Beta de la droite  "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Beta_p)<<endl;
}

//______________________________________________________________________________
void Tube :: CalculerAngle2Droite3D(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p, int Position_p)
{
        //On calcul les angles en radian

        Alpha_p = 0.0;  Beta_p  = 0.0;

        double X1 = (Arc_p->vSuivants[Position_p-1]->x - Arc_p->vSuivants[Position_p]->x);
        double Y1 = (Arc_p->vSuivants[Position_p-1]->y - Arc_p->vSuivants[Position_p]->y);
        double Z1 = (Arc_p->vSuivants[Position_p-1]->z - Arc_p->vSuivants[Position_p]->z);

        //Si les 2 point sont identiques on quitte avec des angles = 0
        if(X1 == 0 && Y1 == 0 && Z1 == 0){
                Alpha_p = 0;
                Beta_p = 0;
                return;
        }

        //Calcule de l'angle Alpha (rotation selon Z)
        double RayonVueDessus = sqrt(X1*X1+Z1*Z1);
        double LongeurDroite = sqrt(RayonVueDessus*RayonVueDessus + Y1*Y1);
        if(RayonVueDessus == 0){
                if( Y1 >0)
                        Alpha_p =  M_PI_Par_2;
                else
                        Alpha_p =  -M_PI_Par_2;
        }else{
                if( Y1 >0)
                        Alpha_p =  acos(Abs(RayonVueDessus) / Abs(LongeurDroite));
                else
                        Alpha_p = -acos(Abs(RayonVueDessus) / Abs(LongeurDroite));
        }

        //Calcul de l'angle Beta (rotation selon Y)

        if(X1 == 0){
                if( Z1 > 0)
                        Beta_p =   M_PI_Par_2;
                else if( Z1 < 0)
                        Beta_p =   -M_PI_Par_2;
                else    Beta_p =   0;

        }else{

                if( Z1 > 0){
                        if( X1 > 0 )
                                Beta_p = atan(Abs(Z1)/Abs(X1));
                        else
                                Beta_p = M_PI - atan(Abs(Z1)/Abs(X1));
                }else{
                        if( X1 > 0 )
                                Beta_p = -atan(Abs(Z1)/Abs(X1));
                        else
                                Beta_p = -M_PI + atan(Abs(Z1)/Abs(X1));
                }
        }

        cout<<"Angle Alpha de la droite "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Alpha_p)<<endl;
        cout<<"Angle Beta de la droite  "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Beta_p)<<endl;
}
//______________________________________________________________________________

void Tube :: AngleFormeParDeuxDroites3D(double AlphaD1, double BetaD1, double AlphaD2, double BetaD2, double &rAlphaResult, double &rBetaResult)
{
        //Si les 2 angles ont le même signe
        if( (AlphaD1 >= 0 && AlphaD2 >= 0) || (AlphaD1 < 0 && AlphaD2 < 0) ){
                rAlphaResult = (AlphaD1 + AlphaD2) / 2;
        }else{
                rAlphaResult = (AlphaD1 + AlphaD2) / 2;
        }

        //rAlphaResult  =  (-AlphaD1 + AlphaD2) / 2;
        //rBetaResult   = BetaD1             BetaD2

        //cout<<"Angle Alpha de la droite "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Alpha_p)<<endl;
        //cout<<"Angle Beta de la droite  "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Beta_p)<<endl;
}

//______________________________________________________________________________

void Tube :: CalculerAngle1Projection(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p, int Position_p)
{
        double X1 = (Arc_p->vSuivants[Position_p+1]->x - Arc_p->vSuivants[Position_p]->x);
        double Y1 = (Arc_p->vSuivants[Position_p+1]->y - Arc_p->vSuivants[Position_p]->y);
        double Z1 = (Arc_p->vSuivants[Position_p+1]->z - Arc_p->vSuivants[Position_p]->z);

        //Si les 2 point sont identiques on quitte avec des angles = 0
        if(X1 == 0 && Y1 == 0 && Z1 == 0){
                Alpha_p = 0;
                Beta_p = 0;
                return;
        }

        //On calcule l'angle Alpha
        if(X1 == 0){
                if( Y1 == 0)
                        Alpha_p = 0;
                else if(Y1 > 0)
                        Alpha_p = M_PI_Par_2;
                else
                        Alpha_p = -M_PI_Par_2;
        }else if( X1 > 0){
                if( Y1 == 0)
                        Alpha_p = 0;
                else if(Y1 > 0)
                        Alpha_p = atan(Abs(Y1)/Abs(X1));
                else
                        Alpha_p = -atan(Abs(Y1)/Abs(X1));
        }else{
                if( Y1 == 0)
                        Alpha_p = M_PI;
                else if(Y1 > 0)
                        Alpha_p = M_PI - atan(Abs(Y1)/Abs(X1));
                else
                        Alpha_p = -M_PI + atan(Abs(Y1)/Abs(X1));
        }

        //On calcule l'angle Beta
        if( X1 == 0){
                if(Z1 == 0)
                        Beta_p = 0;
                else if(Z1 > 0)
                        Beta_p = M_PI_Par_2;
                else
                        Beta_p = -M_PI_Par_2;
        }else if( X1 > 0){
                if(Z1 == 0)
                        Beta_p = 0;
                else if(Z1 > 0)
                        Beta_p = atan(Abs(Z1)/Abs(X1));
                else
                        Beta_p = -atan(Abs(Z1)/Abs(X1));
        }else{
                if(Z1 == 0)
                        Beta_p = -M_PI;
                else if(Z1 > 0)
                        Beta_p = M_PI - atan(Abs(Z1)/Abs(X1));
                else
                        Beta_p = -M_PI + atan(Abs(Z1)/Abs(X1));
        }

        cout<<"Angle projection 1 Alpha  "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Alpha_p)<<endl;
        cout<<"Angle projection 1 Beta   "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Beta_p)<<endl;
}
//______________________________________________________________________________

void Tube :: CalculerAngle2Projection(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p, int Position_p)
{
        double X1 = (Arc_p->vSuivants[Position_p-1]->x - Arc_p->vSuivants[Position_p]->x);
        double Y1 = (Arc_p->vSuivants[Position_p-1]->y - Arc_p->vSuivants[Position_p]->y);
        double Z1 = (Arc_p->vSuivants[Position_p-1]->z - Arc_p->vSuivants[Position_p]->z);

        //Si les 2 point sont identiques on quitte avec des angles = 0
        if(X1 == 0 && Y1 == 0 && Z1 == 0){
                Alpha_p = 0;
                Beta_p = 0;
                return;
        }

        //On calcule l'angle Alpha
        if(X1 == 0){
                if( Y1 == 0)
                        Alpha_p = 0;
                else if(Y1 > 0)
                        Alpha_p = M_PI_Par_2;
                else
                        Alpha_p = -M_PI_Par_2;
        }else if( X1 > 0){
                if( Y1 == 0)
                        Alpha_p = 0;
                else if(Y1 > 0)
                        Alpha_p = atan(Abs(Y1)/Abs(X1));
                else
                        Alpha_p = -atan(Abs(Y1)/Abs(X1));
        }else{
                if( Y1 == 0)
                        Alpha_p = M_PI;
                else if(Y1 > 0)
                        Alpha_p = M_PI - atan(Abs(Y1)/Abs(X1));
                else
                        Alpha_p = -M_PI + atan(Abs(Y1)/Abs(X1));
        }

        //On calcule l'angle Beta
        if( X1 == 0){
                if(Z1 == 0)
                        Beta_p = 0;
                else if(Z1 > 0)
                        Beta_p = M_PI_Par_2;
                else
                        Beta_p = -M_PI_Par_2;
        }else if( X1 > 0){
                if(Z1 == 0)
                        Beta_p = 0;
                else if(Z1 > 0)
                        Beta_p = atan(Abs(Z1)/Abs(X1));
                else
                        Beta_p = -atan(Abs(Z1)/Abs(X1));
        }else{
                if(Z1 == 0)
                        Beta_p = -M_PI;
                else if(Z1 > 0)
                        Beta_p = M_PI - atan(Abs(Z1)/Abs(X1));
                else
                        Beta_p = -M_PI + atan(Abs(Z1)/Abs(X1));
        }

        cout<<"Angle projection 2 Alpha  "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Alpha_p)<<endl;
        cout<<"Angle projection 2 Beta   "<<Position_p<<", "<<Position_p+1<<" : "<<RadianToDegre(Beta_p)<<endl;
}
//______________________________________________________________________________

void Tube :: CalculerAngleFirstFace(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p)
{
        CalculerAngle1Droite3D(Arc_p, Alpha_p, Beta_p, 0);
        Alpha_p -= M_PI_Par_2;
}
//______________________________________________________________________________

void Tube :: CalculerAngleLastFace(Arcs *Arc_p, Double &Alpha_p, Double &Beta_p)
{
        CalculerAngle1Droite3D(Arc_p, Alpha_p, Beta_p, Arc_p->vSuivants.size()-2);
        Alpha_p -= M_PI_Par_2;
}
//______________________________________________________________________________

double Tube :: CalculerAngleFaceMilieu(Arcs *Arc_p, Double &DecalageAlpha_p, Double &DecalageBeta_p, Double &Alpha_p, Double &Beta_p, int Position_p)
{
        double Alpha1, Beta1;
        double Alpha2, Beta2;
        double Alpha, Beta;
        bool AlphaObtus, BetaObtus;



        CalculerAngle1Droite3D(Arc_p, Alpha1, Beta1, Position_p);
        CalculerAngle2Droite3D(Arc_p, Alpha2, Beta2, Position_p);


}

//______________________________________________________________________________

double Tube :: Abs(double Valeur_p)
{
        if(Valeur_p >= 0)
                return Valeur_p;
        else
                return -Valeur_p;
}
//______________________________________________________________________________

double Tube :: GetAngleObtus(double Angle_p)
{

        if(Angle_p < M_PI)
                return M_PI_F2 - Angle_p;
        return Angle_p;
}
//______________________________________________________________________________

double Tube :: GetAngleAigu(double Angle_p)
{

        if(Angle_p < M_PI)
                return Angle_p;
        return M_PI_F2 - Angle_p;
}
//______________________________________________________________________________

bool IsAigu(double Angle_p)
{
        if(Angle_p < M_PI)
                return true;
        return false;
}

//______________________________________________________________________________

double Tube :: RadianToDegre( double AngleRad_p)
{
        return AngleRad_p*(360/M_PI_F2);
}

//______________________________________________________________________________


void Tube :: AngleFormeParDeuxDroites2D(double AlphaD1, double BetaD1, double AlphaD2, double BetaD2, double &rAlphaResult, double &rBetaResult)
{
        double AbsAlphaD1 = Abs(AlphaD1);
        double AbsAlphaD2 = Abs(AlphaD2);
        double AbsBetaD1 = Abs(BetaD1);
        double AbsBetaD2 = Abs(BetaD2);

        //On recupere l'angle formé par les 2 droites
        if( (AlphaD1 >= 0 && AlphaD2 >= 0) || (AlphaD1 <= 0 && AlphaD2 <= 0)){    //Si les 2 angles sont du même coté
                if(AbsAlphaD1 > AbsAlphaD2)
                        rAlphaResult = AbsAlphaD1 - AbsAlphaD2;
                else
                        rAlphaResult = AbsAlphaD2 - AbsAlphaD1;
        }else{
                rAlphaResult = AbsAlphaD1 + AbsAlphaD2;
        }

        //On recupere l'angle
        if( (BetaD1 >= 0 && BetaD2 >= 0) || (BetaD1 <= 0 && BetaD2 <= 0)){
                if(AbsBetaD1 > AbsBetaD2)
                        rBetaResult = AbsBetaD1 - AbsBetaD2;
                else
                        rBetaResult = AbsBetaD2 - AbsBetaD1;
        }else{
                rBetaResult = AbsBetaD1 + AbsBetaD2;
        }
}
//______________________________________________________________________________

bool Tube :: IsAlphaObtus(Arcs* Arc_p, int Position_p)
{
        //On regarde si l'angle est obtus ou aigü
        bool angleObtus1 = false;
        double Pente1;

        //On calcule la pente en X,Y de la droite
        double Tmp =  (Arc_p->vSuivants[Position_p+1]->x - Arc_p->vSuivants[Position_p-1]->x);

        if(Tmp != 0){
                 Pente1 =   (Arc_p->vSuivants[Position_p+1]->y - Arc_p->vSuivants[Position_p-1]->y)/Tmp;

                 //Calcul de y = ax+b
                 double b = Arc_p->vSuivants[Position_p+1]->y - (Pente1*Arc_p->vSuivants[Position_p+1]->x);
                 double y = (Pente1*Arc_p->vSuivants[Position_p]->x) + b;

                 //On regarde la position par rapport l'axe des X,Y
                 if(y > Arc_p->vSuivants[Position_p]->y){        //On est au dessus de la droite
                        angleObtus1 = true;
                 }else if(y < Arc_p->vSuivants[Position_p]->y){  //on est en dessous de la droite
                        angleObtus1 = false;
                 }else                                           //on est sur la droite
                        angleObtus1 = true;

        }
        else{   //Si la pente est infinie, on regarde si elle monte ou elle descend

                //On regarde la position par rapport à l'axe X,Y
                if(Arc_p->vSuivants[Position_p+1]->y > Arc_p->vSuivants[Position_p-1]->y){              //La droite monte
                        if( Arc_p->vSuivants[Position_p]->x < Arc_p->vSuivants[Position_p+1]->x){       //Si on est à gauche
                                angleObtus1 = false;
                        }else if( Arc_p->vSuivants[Position_p]->x > Arc_p->vSuivants[Position_p+1]->x){ //Si on est à droite
                                angleObtus1 = true;
                        }else
                                angleObtus1 = true;                                                     //Si on est sur la droite
                }else{                                                                                  //La droite descend
                        if( Arc_p->vSuivants[Position_p]->x < Arc_p->vSuivants[Position_p+1]->x){       //Si on est à gauche
                                angleObtus1 = true;
                        }else if( Arc_p->vSuivants[Position_p]->x > Arc_p->vSuivants[Position_p+1]->x){ //Si on est à droite
                                angleObtus1 = false;
                        }else
                                angleObtus1 = true;                                                     //Si on est sur la droite
                }
        }

        return angleObtus1;
}
//______________________________________________________________________________


bool Tube :: IsBetaObtus(Arcs* Arc_p, int Position_p)
{
        //On regarde si l'angle est obtus ou aigü
        bool angleObtus2 = false;
        double Pente2;

        //On calcule la pente en X,Y de la droite
        double Tmp =  (Arc_p->vSuivants[Position_p+1]->x - Arc_p->vSuivants[Position_p-1]->x);

        if(Tmp != 0){
                 Pente2 =   (Arc_p->vSuivants[Position_p+1]->z - Arc_p->vSuivants[Position_p-1]->z)/Tmp;

                 //Calcul de z = ax+b
                 double b2 = Arc_p->vSuivants[Position_p+1]->z - (Pente2*Arc_p->vSuivants[Position_p+1]->x);
                 double z = (Pente2*Arc_p->vSuivants[Position_p]->x) + b2;

                 //On regarde la position par rapport l'axe des X,Z
                 if(z > Arc_p->vSuivants[Position_p]->z){        //On est au dessus de la droite
                        angleObtus2 = true;
                 }else if(z < Arc_p->vSuivants[Position_p]->z){  //on est en dessous de la droite
                        angleObtus2 = false;
                 }else                                           //on est sur la droite
                        angleObtus2 = true;
        }
        else{   //Si la pente est infinie, on regarde si elle monte ou elle descend

                //On regarde la position par rapport à l'axe X,Z
                if(Arc_p->vSuivants[Position_p+1]->z > Arc_p->vSuivants[Position_p-1]->z){              //La droite monte
                        if( Arc_p->vSuivants[Position_p]->x < Arc_p->vSuivants[Position_p+1]->x){       //Si on est à gauche
                                angleObtus2 = false;
                        }else if( Arc_p->vSuivants[Position_p]->x > Arc_p->vSuivants[Position_p+1]->x){ //Si on est à droite
                                angleObtus2 = true;
                        }else
                                angleObtus2 = true;                                                     //Si on est sur la droite
                }else{                                                                                  //La droite descend
                        if( Arc_p->vSuivants[Position_p]->x < Arc_p->vSuivants[Position_p+1]->x){       //Si on est à gauche
                                angleObtus2 = true;
                        }else if( Arc_p->vSuivants[Position_p]->x > Arc_p->vSuivants[Position_p+1]->x){ //Si on est à droite
                                angleObtus2 = false;
                        }else
                                angleObtus2 = true;                                                     //Si on est sur la droite
                }
        }

        return angleObtus2;

}

//______________________________________________________________________________


Arcs * Tube :: CreerArc(double Alpha_p, double Beta_p, double angleRotation, double RayonX, double RayonZ, Arcs* Arc_p,  int positionArc)
{
        Arcs * arcTmp1 = new Arcs();
        double CosAlpha =  cos(Alpha_p);
        double SinAlpha =  sin(Alpha_p);
        double CosBeta =  cos(Beta_p);
        double SinBeta =  sin(Beta_p);

        for (double dAlphaI = 0 ; dAlphaI < M_PI_F2; dAlphaI += angleRotation) {

                Vertex3d * vTmp_l = new Vertex3d();

                //On dessine la face
                vTmp_l->x = RayonX * cos(dAlphaI);
                vTmp_l->y = 0.0;
                vTmp_l->z = RayonZ * sin(dAlphaI);

                //on tourne la face de l'angle Alpha
                vTmp_l->y = vTmp_l->x * SinAlpha;
                vTmp_l->x = vTmp_l->x * CosAlpha;

                //On calcule le rayon sur le plan X <-> Z
                double Rayon = sqrt(vTmp_l->x*vTmp_l->x + vTmp_l->z * vTmp_l->z);
                double AbsX1, AbsZ1;
                double Omega = 0.0;

                //On calcul les valeurs absolues
                if(vTmp_l->x < 0.0) { AbsX1 = -vTmp_l->x; } else { AbsX1 = vTmp_l->x; }
                if(vTmp_l->z < 0.0) { AbsZ1 = -vTmp_l->z; } else { AbsZ1 = vTmp_l->z; }

                //////////////////////////////////////////////////////////
                if( vTmp_l->x  == 0.0){
                        if(vTmp_l->z < 0)     { Omega = M_PI_Par_2; }
                        else if(vTmp_l->z > 0){ Omega = -M_PI_Par_2; }
                        else                  { Omega = 0.0; }
                }else{

                        Omega =  atan(AbsZ1/AbsX1);

                        if(vTmp_l->x > 0){
                                if(vTmp_l->z < 0){ Omega = -Omega; }
                                else             { Omega = Omega; }
                        }else{
                                if(vTmp_l->z < 0){ Omega = -M_PI + Omega;}
                                else             { Omega = M_PI - Omega;}
                        }
                }
                 ///////////////////////////////////////////////////////////

                //on tourne la face de l'angle Beta
                Omega += Beta_p;
                vTmp_l->x =  Rayon * cos(Omega);
                vTmp_l->z =  Rayon * sin(Omega);   


                //On place les point a l'emplacement
                vTmp_l->x +=  Arc_p->vSuivants[positionArc]->x;
                vTmp_l->y +=  Arc_p->vSuivants[positionArc]->y;
                vTmp_l->z +=  Arc_p->vSuivants[positionArc]->z;

                arcTmp1->vSuivants.insert(arcTmp1->vSuivants.begin(), vTmp_l);
                vTmp_l = NULL; 
        }

        return arcTmp1;
}

//______________________________________________________________________________

void Tube :: GenererPoints (Arcs *Arc_p)
{
    GenererReferentiel();

    double angleRotation = M_PI_F2 / NbFaces;
    vector < Arcs * > vArcsTempo;

    //On controle l'arc passé en paramétre
    if(Arc_p == NULL)
        return;

    int NbArcs = Arc_p->vSuivants.size();

    if(NbArcs < 2)
        return;

    double Alpha1, Beta1;
    Arcs * arcTmp1;
    Face * face1 = new Face();

    cout<<"______________________PREMIERE FACE __________________________"<<endl<<endl ;

    CalculerAngleFirstFace(Arc_p, Alpha1, Beta1);

    cout<<"Angle Alpha de la face = "<<Alpha1*(360/M_PI_F2)<<endl;
    cout<<"Angle Beta de la face  = "<<Beta1*(360/M_PI_F2)<<endl;

    arcTmp1 = CreerArc( Alpha1,  Beta1, angleRotation, Rayon, Rayon, Arc_p, 0);
    vArcsTempo.push_back(arcTmp1);


    //--------------------------------------------------------------
    cout<<"______________________MIDDLE FACE __________________________"<<endl<<endl ;
    /*
    double  AngleAlphaPrecedent = Alpha1;
    double  AngleBetaPrecedent = Beta1;

    for(int i=1; i<(NbArcs-1);i++){
        double DecalageAlpha, DecalageBeta;
        double AngleAlpha, AngleBeta;

        CalculerAngleFaceMilieu(Arc_p,DecalageAlpha ,DecalageBeta, AngleAlpha, AngleBeta, i);
        double RayonX = Rayon / cos(DecalageAlpha);
        double RayonZ = Rayon / cos(DecalageBeta);

        cout<<" Rayon : "<<Rayon;
        cout<<endl;

        cout<<" RayonX : "<<RayonX;
        cout<<endl;

        cout<<" RayonZ : "<<RayonZ;
        cout<<endl;

        Face * face = new Face();
        Arcs * arcTmp;

        AngleAlphaPrecedent +=  AngleAlpha/2;
        AngleAlphaPrecedent +=  DecalageAlpha;
        AngleBetaPrecedent  +=  AngleBeta/2;
        AngleBetaPrecedent  +=  DecalageBeta;

        arcTmp = CreerArc(AngleAlphaPrecedent, AngleBetaPrecedent, angleRotation, RayonX, RayonZ, Arc_p, i );

        AngleAlphaPrecedent +=  AngleAlpha/2;
        AngleAlphaPrecedent +=  DecalageAlpha;
        AngleBetaPrecedent  +=  AngleBeta/2;
        AngleBetaPrecedent  +=  DecalageBeta;
        vArcsTempo.push_back(arcTmp);


        arcTmp = NULL;


    }
    */
    //--------------------------------------------------------------
    cout<<"______________________DERNIERE FACE__________________________"<<endl<<endl;

    //On construit la derniere face
    double Alpha2, Beta2;
    Arcs * arcTmp2;
    Face * face2 = new Face();

    CalculerAngleLastFace(Arc_p, Alpha2, Beta2);
    arcTmp2 = CreerArc( Alpha2, Beta2, angleRotation, Rayon, Rayon, Arc_p, Arc_p->vSuivants.size()-1);
    vArcsTempo.push_back(arcTmp2);


   ///////////////////// On construit les faces du milieu  ///////////////////////////

   for(unsigned int i = 0; i< (vArcsTempo.size()-1); i++)
   {
        Face * faceFinal = NULL;
        Arcs * arcFinal = NULL;

        for(int j=0; j< (NbFaces-1); j++)
        {
                faceFinal = new Face();
                arcFinal = new Arcs();

                arcFinal->vSuivants.push_back((vArcsTempo.operator [](i))->vSuivants.operator [](j));
                arcFinal->vSuivants.push_back((vArcsTempo.operator [](i+1))->vSuivants.operator [](j));
                arcFinal->vSuivants.push_back((vArcsTempo.operator [](i+1))->vSuivants.operator [](j+1));
                arcFinal->vSuivants.push_back((vArcsTempo.operator [](i))->vSuivants.operator [](j+1));

                faceFinal->AddArcs(arcFinal);
                faceFinal->SetNorme(pReferentiel);
                vFaces.push_back(faceFinal);

                faceFinal = NULL;
                arcFinal = NULL;
        }

        faceFinal = new Face();
        arcFinal = new Arcs();

        arcFinal->vSuivants.push_back((vArcsTempo.operator [](i))->vSuivants.operator [](NbFaces-1));
        arcFinal->vSuivants.push_back((vArcsTempo.operator [](i+1))->vSuivants.operator [](NbFaces-1));
        arcFinal->vSuivants.push_back((vArcsTempo.operator [](i+1))->vSuivants.operator [](0));
        arcFinal->vSuivants.push_back((vArcsTempo.operator [](i))->vSuivants.operator [](0));

        faceFinal->AddArcs(arcFinal);
        faceFinal->SetNorme(pReferentiel);
        vFaces.push_back(faceFinal);

        faceFinal = NULL;
        arcFinal = NULL;

   }
   
   ///////////////////// On construit la premiere faces   ///////////////////////////

   Face * PremiereFace = new Face();
   PremiereFace->AddArcs(vArcsTempo.operator [](0));
   PremiereFace->SetNorme(pReferentiel);
   vFaces.push_back(PremiereFace);
   PremiereFace = NULL;

   ///////////////////// On construit la derniere faces   ///////////////////////////

   Face * DerniereFace = new Face();
   Arcs * arcFinal = vArcsTempo[vArcsTempo.size() - 1]->Inverse();  
   DerniereFace->AddArcs(arcFinal);
   DerniereFace->SetNorme(pReferentiel);
   vFaces.push_back(DerniereFace);
   DerniereFace = NULL;     
}
//______________________________________________________________________________
void Tube :: Dessiner (int type, Referentiel * ref, bool norm)
{
    if (ref != NULL)
        pReferentiel = ref;
    float * col = couleurMatiere->ToTab();
    float coulSpecular[] = {col[0], col[1], col[2]};
    float coulShininess[] = {Shininess};
    delete(col);       
    for (unsigned int i = 0 ; i < vFaces.size() ; i++) {
        glMaterialfv(GL_FRONT, GL_AMBIENT, coulSpecular);
        glMaterialfv(GL_FRONT, GL_SPECULAR, coulSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, coulShininess);
        vFaces[i]->Dessiner(type, pReferentiel, norm);
    }
}

