/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Tourneur.h - Tourneur.cpp

 Desc:   Classe gérant la primitive composée d'une silhouette à
         laquelle on affecte une rotation

*******************************************************************************/

#include "Tourneur.h"
//______________________________________________________________________________

Tourneur :: Tourneur()
{
    AlphaX = 0.0;
    AlphaZ = 0.0;
    Silhouette = new Arcs();
    Angle = 0.0;
    Centre = new Vertex3d(); 
    GenererPoints();
}
//______________________________________________________________________________
Tourneur :: Tourneur(double AlphaX_p,
                     double AlphaZ_p,
                     Vertex3d * Centre_p,
                     Arcs * Silhouette_p, 
                     int nbFaces,
                     double Angle_p,
                     Vertex3d * coul,
                     float shininess,
                     bool ferme)
{                   
    AlphaX = AlphaX_p;
    AlphaZ = AlphaZ_p;
    Silhouette = Silhouette_p;
    while (Angle_p > 360.0)
        Angle_p -= 360.0;
    while (Angle_p < 0)
        Angle_p += 360.0;
    Angle = Angle_p * M_PI * 2 / 360;
    Centre = Centre_p;
    NbFaces = nbFaces;  
    Shininess = shininess;
    couleurMatiere = coul;
    Ferme = ferme;
    GenererPoints();
}
//______________________________________________________________________________
Tourneur :: ~Tourneur()
{
    delete(Centre);
    delete(couleurMatiere);
    delete(Silhouette);
}
//______________________________________________________________________________
Arcs * Tourneur :: GetPointsBas ()
{
    int nbElem = (int)(Silhouette->vSuivants.size());
    double yMin = 10e24; 
    double xMin = 10e24;
    double xMax = -10e24;
    Arcs * pRes_l = new Arcs();
    for (int i = 0 ; i < nbElem ; i++) {
        Vertex3d * pTmp_l = Silhouette->vSuivants[i];
        if (pTmp_l->y < yMin) {
            pRes_l = new Arcs();
            yMin = pTmp_l->y;
            xMin = pTmp_l->x;
            xMax = pTmp_l->x;
            pRes_l->vSuivants.push_back(pTmp_l);
        } else if (pTmp_l->y == yMin) {
            if (pTmp_l->x < xMin) {
                xMin = pTmp_l->x;
                if (pRes_l->vSuivants.size() > 1)
                    pRes_l->vSuivants[0] = pTmp_l;
                else
                    pRes_l->vSuivants.insert(pRes_l->vSuivants.begin(), pTmp_l);
            }
            if (pTmp_l->x > xMax) {
                xMax = pTmp_l->x;
                if (pRes_l->vSuivants.size() > 1)
                    pRes_l->vSuivants[1] = pTmp_l;
                else
                    pRes_l->vSuivants.push_back(pTmp_l);
            }
        }
    }
    return pRes_l;
}    
//______________________________________________________________________________
Arcs * Tourneur :: GetPointsHaut ()
{
    int nbElem = (int)(Silhouette->vSuivants.size());
    double yMax = -10e24;
    double xMin = 10e24;
    double xMax = -10e24;
    Arcs * pRes_l = new Arcs();
    for (int i = 0 ; i < nbElem ; i++) {
        Vertex3d * pTmp_l = Silhouette->vSuivants[i];
        if (pTmp_l->y > yMax) {
            pRes_l = new Arcs();
            yMax = pTmp_l->y;
            xMin = pTmp_l->x;
            xMax = pTmp_l->x;
            pRes_l->vSuivants.push_back(pTmp_l);
        } else if (pTmp_l->y == yMax) {
            if (pTmp_l->x < xMin) {
                xMin = pTmp_l->x;
                if (pRes_l->vSuivants.size() > 1)
                    pRes_l->vSuivants[0] = pTmp_l;
                else
                    pRes_l->vSuivants.insert(pRes_l->vSuivants.begin(), pTmp_l);
            }
            if (pTmp_l->x > xMax) {
                xMax = pTmp_l->x;
                if (pRes_l->vSuivants.size() > 1)
                    pRes_l->vSuivants[1] = pTmp_l;
                else
                    pRes_l->vSuivants.push_back(pTmp_l);
            }
        }
    }
    return pRes_l;
}
//______________________________________________________________________________
void Tourneur :: GenererPoints ()
{

    double angleRotation = Angle / ((float)NbFaces);

    int nbElem = Silhouette->vSuivants.size();
    Arcs * pFond_l = new Arcs();
    Arcs * pDevant_l = new Arcs();   
    Arcs * pHaut_l = new Arcs();
    Arcs * pBas_l = new Arcs();
    int NbTmp = nbElem;
    int PositionCentre = nbElem;
    bool NormalesEndroit = true;
    bool bComplet;
    Silhouette->Ferme = Silhouette->IsClosed();

    // définition du centre de la silhouette
    Vertex3d * centreRotation = NULL;
    if (Silhouette->Centre == NULL)
        Silhouette->Centre = Silhouette->vSuivants[0];

    // définition des points de la silhouette en fonction du centre du Tourneur
    for (int i = 0 ; i < nbElem ; i++) {
        Vertex3d * point = Silhouette->vSuivants[i];
        point = (point->AddVect(Centre));
        Silhouette->vSuivants[i] = point;
    }


    Centre = Silhouette->Centre->AddVect(Centre);
    centreRotation = Centre;

    Silhouette->Centre = Centre;

    // récupération des 2 points extrêmes en bas (gauche-droite)
    //et des 2 points extrêmes en haut (gauche-droite)
    Arcs * pPointsBas = GetPointsBas();
    Arcs * pPointsHaut = GetPointsHaut();

    GenererReferentiel();

    if (nbElem != 0) {
    // Si on a bien des points dans la silhouette

        int * PositionsCentreRotation;
        int NbPosCentrRot = 0;

        if (Ferme) {
        // Si la figure est demandée fermée par l'utilisateur
            if (!Silhouette->Ferme) {
            // si la silhouette n'est pas fermée
                if (!CentreRotCrossSil()) {
                    Vertex3d * v0 = Silhouette->vSuivants[0];
                    Vertex3d * vn = Silhouette->vSuivants[nbElem - 1];
                    // on calcule la pente de la droite entre le premier et
                    // le dernier point de la silhouette
                    double pente = (vn->y - v0->y) / (vn->x - v0->x);
                    // pour chaque point de la silhouette autre que
                    // le premier ou le dernier, en partant de la fin
                    for (int i = nbElem - 2 ; i > 0 ; i--) {
                        Vertex3d * vi = new Vertex3d(Silhouette->vSuivants[i]);
                        // on calcule la pente de la droite entre ce point et
                        // le premier point de la silhouette
                        double penteBis = (vi->y - v0->y) / (vi->x - v0->x);
                        if (pente == penteBis) {
                        // si les 2 pentes sont égales, ça signifie que vi se trouve
                        // sur la droite entre le premier point et le dernier et que
                        // donc des faces vont se superposer. On renverse donc la
                        // figure pour considérer que vi+1 est le dernier point
                            Silhouette->InverserSuivants(i);
                            if (!CentreRotCrossSil())
                                Silhouette->vSuivants.insert(Silhouette->vSuivants.begin(), vi);
                            break;
                        }
                        delete(vi);
                    } // fin for i
                } else {
                    // Ajout des points nécessaires à la cohérence de la figure si
                    // le centre de rotation se trouve dans les limites de la
                    // silhouette
                    for (int i = 1 ; i < nbElem-1 ; i++) {
                        Vertex3d * v1 = Silhouette->vSuivants[i];
                        Vertex3d * v0 = Silhouette->vSuivants[i-1];
                        // Si le centre de rotation est entre v0 et v1
                        if (CentreDansSegment(v0,v1))
                        /*
                            (Centre->x < v0->x && Centre->x >= v1->x) ||
                                (Centre->x < v1->x && Centre->x >= v0->x))
                        */
                        {
                            if ( NbPosCentrRot != 0) {
                            // si on a déjà eu des croisements, on en ajoute un
                                int * positions_l = new int[NbPosCentrRot];
                                for (int k = 0 ; k < NbPosCentrRot ; k++)
                                    positions_l[k] = PositionsCentreRotation[k];
                                NbPosCentrRot++;
                                PositionsCentreRotation = new int[NbPosCentrRot];
                                for (int k = 0 ; k < NbPosCentrRot - 1 ;k++)
                                    PositionsCentreRotation[k] = positions_l[k];
                                PositionsCentreRotation[NbPosCentrRot - 1] = i;
                                delete(positions_l);
                            } else {
                            // sinon on crée le premier croisement
                                NbPosCentrRot = 1;
                                PositionsCentreRotation = new int[NbPosCentrRot];
                                PositionsCentreRotation[0] = i;
                            }
                            double penteXY = (v0->y - v1->y) / (v0->x - v1->x);
                            double penteXZ = (v0->z - v1->z) / (v0->x - v1->x);
                            // on calcule les coordonnées du point de croisement
                            Vertex3d * pointRotation = new Vertex3d();
                            pointRotation->x = Centre->x;
                            pointRotation->y = v0->y+(penteXY*(Centre->x - v0->x));
                            pointRotation->z = Centre->z;//v0->z+(penteXZ*(Centre->x - v0->x));

                            // et on l'ajoute à la silhouette, à la bonne place
                            Silhouette->AddPoint(pointRotation, i);
                            // on positionne le curseur à la nouvelle position
                            // du i-ème point (passé i+1-ème suite à l'ajout)
                            i++;
                            nbElem++;
                        } // fin if (CentreDansSegment(v0,v1))
                    } // fin for i
                } // fin if (!CentreRotCrossSil)
                
                // Si le centre de rotation est dans la silhouette et
                // qu'il n'est pas le dernier point de la silhouette
                if (NbPosCentrRot > 0 /*&&
                        Silhouette->vSuivants.size() - 1 !=
                                PositionsCentreRotation[NbPosCentrRot-1] /*&&
                        PositionsCentreRotation[0] != 0*/)
                {
                    nbElem += 2;
                    int nbCentrePosAnc = NbPosCentrRot;

                    int * positions_l = new int[NbPosCentrRot];
                    for (int k = 0 ; k < NbPosCentrRot ; k++)
                        positions_l[k] = PositionsCentreRotation[k];
                    NbPosCentrRot += 2;
                    PositionsCentreRotation = new int[NbPosCentrRot];
                    for (int k = 1 ; k < NbPosCentrRot - 1 ;k++)
                        PositionsCentreRotation[k] = positions_l[k-1];
                    PositionsCentreRotation[0] = 0;
                    PositionsCentreRotation[NbPosCentrRot - 1] = nbElem-1;
                    delete(positions_l);

                    Vertex3d * pPoint_l = Silhouette->vSuivants[PositionsCentreRotation[NbPosCentrRot-2]];
                    Vertex3d * pPoint2_l = Silhouette->vSuivants[PositionsCentreRotation[1]];
                    Vertex3d * pPoint3_l = new Vertex3d(pPoint2_l->x,
                                                        Silhouette->vSuivants[0]->y,
                                                        pPoint2_l->z);          
                    Silhouette->vSuivants.insert(Silhouette->vSuivants.begin(),
                                                 pPoint3_l);

                    // on ajoute le premier croisement au début de la silhouette
                    Silhouette->vSuivants.insert(Silhouette->vSuivants.begin(),
                                                 pPoint2_l);
                    // Et le dernier croisement en dernier point de la silhouette
                    Silhouette->vSuivants.push_back(pPoint_l);
                    
                    // on met à jour les indices des points appartenant
                    // à l'axe de rotation
                    for (int i = 1 ; i < NbPosCentrRot - 1 ; i++)
                        PositionsCentreRotation[i]++;
                } else /*if (Silhouette->vSuivants.size() - 1 ==
                                PositionsCentreRotation[NbPosCentrRot-1])*/ {
                    Silhouette->vSuivants.push_back(Silhouette->vSuivants[0]);
                    nbElem++;
                } /*else {

                }// fin if (NbPosCentrRot > 0)*/

                if (NbPosCentrRot <= 1)
                    bComplet = true;
                else if (NbPosCentrRot > 1)
                    bComplet = false;
                    
                // Même si ça peut ne pas être vrai, on considère que
                //la silhouette est fermée 
                Silhouette->Ferme = true;
            }  // fin if (!Silhouette->Ferme)


        } // fin if (Ferme)
                             
        bool bVIPasse = false;
        // on construit les faces constituant le tourneur
        for (int i = 1 ; i < nbElem ; i++) {
            Vertex3d * v1 = Silhouette->vSuivants[i-1];
            Vertex3d * v2 = Silhouette->vSuivants[i];
            Vertex3d * v0Bas = pPointsBas->vSuivants[0];
            Vertex3d * v1Bas = pPointsBas->vSuivants[pPointsBas->vSuivants.size() - 1];
            Vertex3d * v0Haut = pPointsHaut->vSuivants[0];
            Vertex3d * v1Haut = pPointsHaut->vSuivants[pPointsHaut->vSuivants.size() - 1];
            Vertex3d * CentreBas = NULL;
            bool bInPos = IsInPositions(i, NbPosCentrRot, PositionsCentreRotation);
            // si on rencontre un point de l'axe de rotation, on inverse
            // la façon dont on tourne
            if ( NbPosCentrRot != 0 && bInPos)
                bVIPasse = !bVIPasse;

            double rayon1 = v1->NormeVectXZ(centreRotation);
            double rayon2 = v2->NormeVectXZ(centreRotation);

            for (int j = 0 ; j <= NbFaces ; j++) {
                double dAlphaI = j * angleRotation;
                double SinAlpha = sin(dAlphaI);
                double CosAlpha = cos(dAlphaI);

                if (bVIPasse && bInPos) {
                    SinAlpha = -SinAlpha;
                    CosAlpha = -CosAlpha;
                }

                Vertex3d * v1Rot = new Vertex3d(rayon1 * CosAlpha,
                                                v1->y,
                                                rayon1 * SinAlpha);
                Vertex3d * v2Rot = new Vertex3d(rayon2 * CosAlpha,
                                                v2->y,
                                                rayon2 * SinAlpha);

                if (j == 0) {
                    if (i == 1)
//                        if (
                        pDevant_l->vSuivants.push_back(v1Rot);
                    pDevant_l->vSuivants.push_back(v2Rot);
                }

                if (v1->z != Centre->z ||
                    v2->z != Centre->z ||
                    v1Rot->z != Centre->z ||
                    v2Rot->z != Centre->z ||
                    v1->z != v2->z || v1->z != v1Rot->z || v1->z != v2Rot->z ||
                    v2->z != v1Rot->z || v2->z != v2Rot->z || v1Rot->z != v2Rot->z)
                {
                    Arcs * pArc_l = new Arcs();
                    if (bVIPasse && bInPos) {
                        pArc_l->vSuivants.push_back(v2);
                        if (!v2Rot->Equals(v2))
                            pArc_l->vSuivants.push_back(v2Rot);
                        if (!v1Rot->Equals(v1))
                            pArc_l->vSuivants.push_back(v1Rot);
                        pArc_l->vSuivants.push_back(v1);
                    } else {                            
                        pArc_l->vSuivants.push_back(v1);
                        if (!v1Rot->Equals(v1))
                            pArc_l->vSuivants.push_back(v1Rot);
                        if (!v2Rot->Equals(v2))
                            pArc_l->vSuivants.push_back(v2Rot);
                        pArc_l->vSuivants.push_back(v2);
                    }

                    if ( pArc_l->vSuivants.size() > 2 ) {
                    // si on a plus de 2 points, on construit la face
                        Face * pFace_l = new Face();
                        pFace_l->AddArcs(pArc_l);
                        pFace_l->SetNorme(pReferentiel);
                        vFaces.push_back(pFace_l);

                        // un petit affichage au cas où on a des normes nulles
                        if (pFace_l->Norme->x == 0 &&
                            pFace_l->Norme->y == 0 &&
                            pFace_l->Norme->z == 0)
                        {
                            cout<<"i="<<i<<" j="<<j
                                <<" dAlphaI="<<dAlphaI<<" "<<endl
                                <<"v1 : x="<<v1->x<<" y="
                                <<v1->y<<" z="<<v1->z<<endl
                                <<"v1Rot : x="<<v1Rot->x<<" y="
                                <<v1Rot->y<<" z="<<v1Rot->z<<endl
                                <<"v2 : x="<<v2->x<<" y="
                                <<v2->y<<" z="<<v2->z<<endl
                                <<"v2Rot : x="<<v2Rot->x<<" y="
                                <<v2Rot->y<<" z="<<v2Rot->z<<endl<<endl;
                        }

                        if (!Ferme) {
                        // on construit la face opposée
                            Vertex3d * n = new Vertex3d(pFace_l->Norme);
                            n->x = -n->x;
                            n->y = -n->y;
                            n->z = -n->z;
                            pFace_l = new Face();
                            pFace_l->AddArcs(pArc_l->Inverse());
                            pFace_l->Norme = n;
                            vFaces.push_back(pFace_l);
                        }
                    }
                } else {
                    cout<<"Tourneur l.180 : coucou"<<endl;
                }
                v1 = v1Rot;
                v2 = v2Rot;

                if (j == NbFaces) {
                    if (i == 1)
                        pFond_l->vSuivants.push_back(v1);
                    pFond_l->vSuivants.push_back(v2);
                }
                
            } // fin for j

        } // fin for i

        // Si la figure est fermée on ajoute les faces haut, bas et de coupe     
        if (Ferme) { /*
            if (Angle != 0.0 && Angle != M_PI * 2) {
                Face * pFace_l = new Face();
                pFace_l->AddArcs(pFond_l);
                pFace_l->SetNorme(pReferentiel);
                if (NbPosCentrRot > 1) {
                    pFace_l->Norme->x = -pFace_l->Norme->x;
                    pFace_l->Norme->y = -pFace_l->Norme->y;
                    pFace_l->Norme->z = -pFace_l->Norme->z;
                }
                vFaces.push_back(pFace_l);
                pFace_l = NULL;

                pFace_l = new Face();
                pFace_l->AddArcs(pDevant_l->Inverse());
                pFace_l->SetNorme(pReferentiel);
                vFaces.push_back(pFace_l);
                pFace_l = NULL;
            }  */
        } // fin if (Ferme)
    } //fin if (nbElem != 0)
}                                                                               
//______________________________________________________________________________
bool Tourneur :: CentreRotCrossSil ()
{
    for (unsigned int i = 1 ; i < Silhouette->vSuivants.size() ; i++) {
        Vertex3d * v0 = Silhouette->vSuivants[i-1];
        Vertex3d * v1 = Silhouette->vSuivants[0];
        if (CentreDansSegment(v0,v1))
            return true;
    }
    return false;
}
//______________________________________________________________________________
bool Tourneur :: CentreDansSegment (Vertex3d * v0, Vertex3d * v1)
{
    double penteXZ = (v0->z - v1->z) / (v0->x - v1->x);
    double penteCentreXZ = (v0->z - Centre->z) / (v0->x - Centre->x);

    return penteCentreXZ == penteXZ;
}
//______________________________________________________________________________
void Tourneur :: Dessiner(int type, Referentiel * ref, bool norm)
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

    glBegin(GL_LINE_STRIP);
    Silhouette->Dessiner(2, pReferentiel, false);
    glEnd(); 

}          
//______________________________________________________________________________
bool Tourneur :: IsInPositions (int i, int nb, int * positions)
{
    for (int j = 0 ; j < nb ; j++)
        if (positions[j] == i)
            return true;
    return false;
}
//______________________________________________________________________________

