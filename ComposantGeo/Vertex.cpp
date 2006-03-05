/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D
 Fichier: Vertex.h - Vertex.cpp
 Desc:   Classe gérant les sommets des géométries 
*******************************************************************************/

#include "Vertex.h"
//______________________________________________________________________________
Vertex :: Vertex ()
{
    x = 0;
    y = 0;
    z = 0;
}
//______________________________________________________________________________
Vertex :: Vertex(double dx, double dy, double dz)
{
    x = dx;
    y = dy;
    z = dz;
}
//______________________________________________________________________________
Vertex :: Vertex(Vertex * v)
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
Vertex :: ~Vertex ()
{
}                           
//______________________________________________________________________________
Vertex & Vertex :: operator + (const Vertex &pVertex)
{    
    Vertex * res = new Vertex();
    res->x = x + pVertex.x;
    res->y = y + pVertex.y;
    res->z = z + pVertex.z;
    return *res;
} 
//______________________________________________________________________________
Vertex * Vertex :: operator + (Vertex *pVertex)
{    
	return new Vertex(x + pVertex->x, y + pVertex->y, z + pVertex->z);
}
//______________________________________________________________________________
void Vertex :: operator += (Vertex *pVertex)
{    
	x += pVertex->x;
	y += pVertex->y;
	z += pVertex->z;
}    
//______________________________________________________________________________
Vertex & Vertex :: operator - (const Vertex &pVertex)
{    
    Vertex * res = new Vertex();
    res->x = x - pVertex.x;
    res->y = y - pVertex.y;
    res->z = z - pVertex.z;
    return *res;
} 
//______________________________________________________________________________
Vertex * Vertex :: operator - (Vertex *pVertex)
{    
    return new Vertex(x - pVertex->x, y - pVertex->y, z - pVertex->z);
} 
//______________________________________________________________________________
void Vertex :: operator -= (Vertex *pVertex)
{    
	x -= pVertex->x;
	y -= pVertex->y;
	z -= pVertex->z;
}
//______________________________________________________________________________
Vertex & Vertex :: operator * (const Vertex &pVertex)
{    
    Vertex * res = new Vertex();
    res->x = (x * pVertex.x);
    res->y = (y * pVertex.y);
    res->z = (z * pVertex.z);
    return *res;
} 
//______________________________________________________________________________
Vertex * Vertex :: operator * (Vertex *pVertex)
{    
    return new Vertex(x * pVertex->x, y * pVertex->y, z * pVertex->z);
} 
//______________________________________________________________________________

Vertex * Vertex ::operator / (int Valeur)
{
	if(Valeur==0){
		return NULL;
	}
	return new Vertex((x/Valeur), (y/Valeur), (z/Valeur));	
}
//______________________________________________________________________________

void Vertex ::operator /= (int Valeur)
{
	if(Valeur==0){
		return;
	}

	x/=Valeur;
	y/=Valeur;
	z/=Valeur;	
}

//______________________________________________________________________________
bool Vertex :: operator == (const Vertex &pVertex)
{
    return (x == pVertex.x) && (y == pVertex.y) && (z == pVertex.z);
}
//______________________________________________________________________________
bool Vertex :: operator == (Vertex *pVertex)
{
    return (x == pVertex->x) && (y == pVertex->y) && (z == pVertex->z);
}
//______________________________________________________________________________
bool Vertex :: operator != (const Vertex &pVertex)
{
    return (x != pVertex.x) || (y != pVertex.y) || (z != pVertex.z);
}
//______________________________________________________________________________
bool Vertex :: operator != (Vertex *pVertex)
{
    return (x != pVertex->x) || (y != pVertex->y) || (z != pVertex->z);
}
//______________________________________________________________________________