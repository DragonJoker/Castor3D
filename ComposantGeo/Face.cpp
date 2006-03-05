/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Face.h - Face.cpp

 Desc:   Classe gérant les faces, contient les arcs composant la face
         et la normale d'une face

*******************************************************************************/

#include "Face.h"
//______________________________________________________________________________
Face :: Face()
{
	_CoordMin = new Vertex();
	_CoordMax = new Vertex();
    NbPoints = 0;
}
//______________________________________________________________________________
Face :: ~Face ()
{
	vector <Arcs*>::iterator Iter1;
	for( Iter1 = vArcs.begin( );Iter1 != vArcs.end( ) ; Iter1++ ){					
		delete((*Iter1));				
	}
    vArcs.erase(vArcs.begin(), vArcs.end());
	delete _CoordMin;
	delete _CoordMax;
}
//______________________________________________________________________________
void Face :: Dessiner(Type_Affichage typeAff_p)
{  
	//_____________________________________________________________
/*
	//Si on a assez de point pour desinner une normale, on la dessine
	if(vArcs[0]->Size()>=3){
		
		glBegin(GL_LINES);
		Vecteur* v1 = new Vecteur(vArcs[0]->GetVertex(1),vArcs[0]->GetVertex(0));
		Vecteur* v2 = new Vecteur(vArcs[0]->GetVertex(1),vArcs[0]->GetVertex(2));

		Vecteur* pVecNormal = v1->GetNormal(v2); 
		glColor3f(1.0f, 0.0f, 0.0f);
		Vertex* CentreFace = CalculerCentreFace();

		glVertex3f(CentreFace->x, CentreFace->y, CentreFace->z);
		

		glVertex3f(CentreFace->x+pVecNormal->x,
			       CentreFace->y+pVecNormal->y,
				   CentreFace->z+pVecNormal->z);

		glEnd();

		delete v1;
		delete v2;
		delete pVecNormal;
		delete CentreFace;
	}
*/
	//__________________________________________________


	//Si on a assez de point pour calculer une normale, on la calcule
	if(vArcs[0]->Size()>=3){
		
		Vecteur* v1 = new Vecteur(vArcs[0]->GetVertex(1),vArcs[0]->GetVertex(0));
		Vecteur* v2 = new Vecteur(vArcs[0]->GetVertex(1),vArcs[0]->GetVertex(2));

		Vecteur* pVecNormal = v1->GetNormal(v2); 
		glNormal3f(pVecNormal->x, pVecNormal->y, pVecNormal->z);

		delete v1;
		delete v2;
		delete pVecNormal;
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	//__________________________________________________


	for(int i=0; i<vArcs.size();i++){
		vArcs[i]->Dessiner(typeAff_p);
	}
}

//______________________________________________________________________________
void Face :: AddArcs(Arcs * arc)
{
    vArcs.push_back(arc);

    NbPoints += arc->Size();
}
//______________________________________________________________________________
Vertex * Face :: CalculerCentreFace()
{
    Vertex * moyN = new Vertex();
    
    for (unsigned int i = 0 ; i < vArcs.size() ; i++) {
        for (unsigned int j = 0 ; j < vArcs[i]->Size() ; j++) {
			moyN->operator +=(vArcs[i]->GetVertex(j));
        }
    }
    moyN->operator /= (NbPoints);
    return moyN;
}
//______________________________________________________________________________

void Face :: CalculerExtremes()
{

	vArcs[0]->CalculerExtremes();

	_CoordMin->x = vArcs[0]->_CoordMin->x;
	_CoordMax->x = vArcs[0]->_CoordMax->x;
	_CoordMin->y = vArcs[0]->_CoordMin->y;
	_CoordMax->y = vArcs[0]->_CoordMax->y;
	_CoordMin->z = vArcs[0]->_CoordMin->z;
	_CoordMax->z = vArcs[0]->_CoordMax->z;

	for(int i=1; i< (int)(vArcs.size()) ;i++){
		vArcs[i]->CalculerExtremes();		
		MajExtremes(vArcs[i]->_CoordMax);
		MajExtremes(vArcs[i]->_CoordMin);
	}

}

//______________________________________________________________________________

Vertex* Face ::GetCoordMin()
{
	return _CoordMin;
}
//______________________________________________________________________________

Vertex* Face ::GetCoordMax()
{
	return _CoordMax;
}
//______________________________________________________________________________

void Face::MajExtremes(Vertex * pPoint_p)
{
	if(pPoint_p==NULL){
		return;
	}

	if(pPoint_p->x > _CoordMax->x){
		_CoordMax->x = pPoint_p->x;
	}

	if(pPoint_p->y > _CoordMax->y){
		_CoordMax->y= pPoint_p->y;
	}

	if(pPoint_p->z > _CoordMax->z){
		_CoordMax->z = pPoint_p->z;
	}

	if(pPoint_p->y < _CoordMin->y){
		_CoordMin->y = pPoint_p->y;
	}

	if(pPoint_p->x < _CoordMin->x){
		_CoordMin->x = pPoint_p->x;
	}

	if(pPoint_p->z < _CoordMin->z){
		_CoordMin->z = pPoint_p->z;
	}
}
//______________________________________________________________________________
