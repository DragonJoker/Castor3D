/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Arcs.h - Arcs.cpp

 Desc:   Classe gérant les arcs ; une liste de points

*******************************************************************************/

#include "Arcs.h"                                                                               
//______________________________________________________________________________
Arcs :: Arcs ()
{
	_CoordMax=NULL;
	_CoordMin=NULL;
}
//______________________________________________________________________________
Arcs :: ~Arcs ()
{
	delete(_CoordMax);
	delete(_CoordMin);
    vSuivants.erase(vSuivants.begin(), vSuivants.end());
}
//______________________________________________________________________________
void Arcs :: Dessiner(Type_Affichage typeAff_p)
{   

	//On selectionne le type d'affichage necessaire
	switch(typeAff_p){

		case points:
			glBegin(GL_POINTS);
			break;

		case lignesOuvertes:
			glBegin(GL_LINE_STRIP);
			break;

		case lignesfermees:
			glBegin(GL_LINE_LOOP);
			break;

		case faces:
			glBegin(GL_POLYGON);
			break;

		case facesTriangle:
			glBegin(GL_TRIANGLES);
			break;	
	}


	//On dessine l'ensemble des points de la face
	for (unsigned int i = 0 ; i < vSuivants.size() ; i++) {
            glVertex3f(vSuivants[i]->x, vSuivants[i]->y,	vSuivants[i]->z );           
    }

	glEnd();
}
//______________________________________________________________________________
Arcs * Arcs :: Inverse ()
{
    Arcs * pRes_l = new Arcs();

    for (unsigned int i = 0 ; i < vSuivants.size() ; i++) {
        pRes_l->vSuivants.insert(pRes_l->vSuivants.begin(), vSuivants[i]);
    }

    return pRes_l;
}                                                                        
//______________________________________________________________________________
void Arcs :: AddVertex(Vertex * pPoint_p, int ind_p)
{
    vector < Vertex * > vTmp_l;
	int i;

    for (i = 0 ; i < ind_p ; i++){
        vTmp_l.push_back(vSuivants[i]);
	}

    vTmp_l.push_back(pPoint_p);

    for (i = ind_p ; i < (int)(vSuivants.size()) ; i++){
        vTmp_l.push_back(vSuivants[i]);
	}
    vSuivants = vTmp_l;

	MajExtremes(pPoint_p);

}
//______________________________________________________________________________

bool Arcs :: IsClosed()
{
	return vSuivants[0]->operator ==(vSuivants[vSuivants.size()-1]);
}
//______________________________________________________________________________

Vertex* Arcs ::GetCoordMin()
{
	return _CoordMin;
}
//______________________________________________________________________________

Vertex* Arcs ::GetCoordMax()
{
	return _CoordMax;
}
//______________________________________________________________________________

void Arcs ::CalculerExtremes()
{
	int NbVertex = vSuivants.size();
	delete _CoordMax;
	delete _CoordMin;
	_CoordMax = NULL;
	_CoordMin = NULL;

	for(int i=0; i<NbVertex; i++)
	{
		MajExtremes(vSuivants[i]);
	}
}


//______________________________________________________________________________

void Arcs::MajExtremes(Vertex * pPoint_p)
{
	if(pPoint_p==NULL){
		return;
	}

	if(_CoordMax==NULL){
		_CoordMax = new Vertex(pPoint_p);
		_CoordMin = new Vertex(pPoint_p);
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

int Arcs::Size()
{
	return vSuivants.size();
}
//______________________________________________________________________________

Vertex* Arcs::GetVertex(int i)
{
	return vSuivants[i];
}
//______________________________________________________________________________
