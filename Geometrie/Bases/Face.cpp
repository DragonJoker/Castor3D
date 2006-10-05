/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Face.h - Face.cpp

 Desc:   Classe gérant les faces, contient les arcs composant la face
         et la normale d'une face

*******************************************************************************/

#include "Face.h"
//______________________________________________________________________________
Face::Face()
{
	Vertex* _v1 = NULL;
	Vertex* _v2 = NULL;
	Vertex* _v3 = NULL;
}
//______________________________________________________________________________
Face::~Face ()
{	
	//v1,v2,v3 Supprimés a un niveau superieur		
}
//______________________________________________________________________________
void Face::DessinerPlein()
{ 
	Vecteur* v1_l = new Vecteur(_v2,_v1);
	Vecteur* v2_l = new Vecteur(_v2,_v3);
	Vecteur* pVecNormal_l = v1_l->GetNormal(v2_l);
	
	glNormal3f(pVecNormal_l->x, pVecNormal_l->y, pVecNormal_l->z);

	delete v1_l;
	delete v2_l;
	delete pVecNormal_l;

	glBegin(GL_TRIANGLES);
    glVertex3f(_v1->x, _v1->y,	_v1->z );   
	glVertex3f(_v2->x, _v2->y,	_v2->z );   
    glVertex3f(_v3->x, _v3->y,	_v3->z );   
	glEnd();

}

//______________________________________________________________________________

void Face::DessinerFilaire()
{  

	/////////////////DESSINER NORMALES//////////////////////////////////
	/*
	Vecteur* v1_l = new Vecteur(_v2,_v1);
	Vecteur* v2_l = new Vecteur(_v2,_v3);
	Vecteur* pVecNormal_l = v1_l->GetNormal(v2_l);
	
	glNormal3f(pVecNormal_l->x, pVecNormal_l->y, pVecNormal_l->z);
		glColor3f(1.0f, 0.0f, 0.0f);
		Vertex* CentreFace = CalculerCentreFace();
		
		glBegin(GL_LINES);
		glVertex3f(CentreFace->x, CentreFace->y, CentreFace->z);
		

		glVertex3f(CentreFace->x+pVecNormal_l->x,
			       CentreFace->y+pVecNormal_l->y,
				   CentreFace->z+pVecNormal_l->z);

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnd();
		delete CentreFace;
	delete v1_l;
	delete v2_l;
	delete pVecNormal_l;
	*/
	///////////////////////////////////////////////////////////

	glBegin(GL_LINE_LOOP);
    glVertex3f(_v1->x, _v1->y,	_v1->z );   
	glVertex3f(_v2->x, _v2->y,	_v2->z );   
    glVertex3f(_v3->x, _v3->y,	_v3->z );   
	glEnd();
}

//______________________________________________________________________________
Vertex * Face::CalculerCentreFace()
{
    Vertex * moyN = new Vertex();
    
	moyN->operator+=(_v1);
	moyN->operator+=(_v2);
	moyN->operator+=(_v3);
    moyN->operator /= (3);

    return moyN;
}
//______________________________________________________________________________
