/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D
 Fichier: Vecteur.h - Vecteur.cpp
 Desc:   Classe gérant les vecteurs
*******************************************************************************/

#include "Vecteur.h"
//______________________________________________________________________________
Vecteur :: Vecteur ()
{
    x = 0;
    y = 0;
    z = 0;
}
//______________________________________________________________________________
Vecteur :: Vecteur(double dx, double dy, double dz)
{
    x = dx;
    y = dy;
    z = dz;
}
//______________________________________________________________________________
Vecteur :: Vecteur(Vertex *v)
{
    x = v->x;
    y = v->y;
    z = v->z;
}
//______________________________________________________________________________
Vecteur :: Vecteur(Vertex *v1, Vertex *v2)
{
    x = v2->x-v1->x;
    y = v2->y-v1->y;
    z = v2->z-v1->z;
}
//______________________________________________________________________________
Vecteur :: Vecteur(Vecteur * v)
{
    if (v != NULL) {
        x = v->x;
        y = v->y;
        z = v->z;
    } else {  
        x = 0;
        y = 0;
        z = 0;
    } 
}
//______________________________________________________________________________
Vecteur :: ~Vecteur ()
{
}
 
//______________________________________________________________________________
Vecteur & Vecteur :: operator + (const Vecteur &pVecteur)
{    
    Vecteur * res = new Vecteur();
    res->x = x + pVecteur.x;
    res->y = y + pVecteur.y;
    res->z = z + pVecteur.z;
    return *res;
}
//______________________________________________________________________________
  
Vecteur* Vecteur :: operator + (Vecteur *pVecteur)
{
	return new Vecteur(x+pVecteur->x, y+pVecteur->y, z+pVecteur->z);
}

//______________________________________________________________________________
Vecteur & Vecteur :: operator - (const Vecteur &pVecteur)
{    
    Vecteur * res = new Vecteur();
    res->x = x - pVecteur.x;
    res->y = y - pVecteur.y;
    res->z = z - pVecteur.z;
    return *res;
}               
//______________________________________________________________________________
  
Vecteur* Vecteur :: operator - (Vecteur *pVecteur)
{
	return new Vecteur(x-pVecteur->x, y-pVecteur->y, z-pVecteur->z);
}            
//______________________________________________________________________________
Vecteur & Vecteur :: operator ^ (const Vecteur &pVecteur)
{    
    Vecteur * res = new Vecteur();
    double x_l = pVecteur.x;
    double y_l = pVecteur.y;
    double z_l = pVecteur.z;
    res->x = (y * z_l) - (y_l * z);
    res->y = (z * x_l) - (z_l * x);
    res->z = (x * y_l) - (x_l * y);
    return *res;
}
//______________________________________________________________________________
  
Vecteur* Vecteur :: operator ^ (Vecteur *pVecteur)
{
	double x_l = pVecteur->x;
    double y_l = pVecteur->y;
    double z_l = pVecteur->z;
	return new Vecteur((y * z_l) - (y_l * z), (z * x_l) - (z_l * x), (x * y_l) - (x_l * y));
}
//______________________________________________________________________________

Vecteur * Vecteur ::operator * (int Valeur)
{
	return new Vecteur((x*Valeur), (y*Valeur), (z*Valeur));
}
//______________________________________________________________________________

Vecteur * Vecteur ::operator * (float Valeur)
{
	return new Vecteur((x*Valeur), (y*Valeur), (z*Valeur));	
}
//______________________________________________________________________________

Vecteur * Vecteur ::operator * (double Valeur)
{
	return new Vecteur((x*Valeur), (y*Valeur), (z*Valeur));
}
//______________________________________________________________________________

Vecteur * Vecteur ::operator / (int Valeur)
{
	if(Valeur==0){
		return NULL;
	}
	return new Vecteur((x/Valeur), (y/Valeur), (z/Valeur));	
}
//______________________________________________________________________________

Vecteur * Vecteur ::operator / (float Valeur)
{
	if(Valeur==0.0f){
		return NULL;
	}
	return new Vecteur((x/Valeur), (y/Valeur), (z/Valeur));		
}
//______________________________________________________________________________

Vecteur * Vecteur ::operator / (double Valeur)
{
	if(Valeur==0.0){
		return NULL;
	}
	return new Vecteur((x/Valeur), (y/Valeur), (z/Valeur));	
}
//______________________________________________________________________________
bool Vecteur :: operator == (const Vecteur &pVecteur)
{
    return (x == pVecteur.x) && (y == pVecteur.y) && (z == pVecteur.z);
}

//______________________________________________________________________________
bool Vecteur :: operator == (Vecteur *pVecteur)
{
    return (x == pVecteur->x) && (y == pVecteur->y) && (z == pVecteur->z);
}

//______________________________________________________________________________
bool Vecteur :: operator != (const Vecteur &pVecteur)
{
    return (x != pVecteur.x) || (y != pVecteur.y) || (z != pVecteur.z);
}
//______________________________________________________________________________
bool Vecteur :: operator != (Vecteur *pVecteur)
{
    return (x != pVecteur->x) || (y != pVecteur->y) || (z != pVecteur->z);
}
//______________________________________________________________________________
float Vecteur::GetLongueur()
{
	return sqrt(x*x+y*y+z*z);
}
//______________________________________________________________________________
Vecteur* Vecteur::GetNormalise()
{
	float Longeur = GetLongueur();
	
	if(Longeur == 0.0f){
		return NULL;
	}

	return new Vecteur(x/Longeur, y/Longeur, z/Longeur);
}
//______________________________________________________________________________
void Vecteur::Normaliser()
{
	float Longeur = GetLongueur();
	
	if(Longeur == 0.0f){
		return;
	}

	x/=Longeur;
	y/=Longeur;
	z/=Longeur;
}
//______________________________________________________________________________

Vecteur* Vecteur::GetNormal(Vecteur* pVecteur_l)
{
	Vecteur* vNormal = this->operator ^(pVecteur_l);
	vNormal->Normaliser();
	vNormal->Inverser();
	return vNormal;	
}
//______________________________________________________________________________

void Vecteur::Inverser()
{
    x = -x;
	y = -y;
	z = -z;
}
//______________________________________________________________________________
	
void Vecteur::TranslaterVertex(Vertex *&rVertex)
{
    rVertex->x += this->x;
	rVertex->y += this->y;
	rVertex->z += this->z;
}


//______________________________________________________________________________
