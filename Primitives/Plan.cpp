/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Lo�c DASSONVILLE
 But:    Moteur 3D

 Fichier: Plan.h - Plan.cpp

 Desc:   Classe g�rant la primitive de Pav� (cube ou parall�l�pip�de rectangle)

*******************************************************************************/

#include "Plan.h"
//______________________________________________________________________________

Plan :: Plan()
{
	IdListe=0;
    Largeur = 0.0;
    Profondeur = 0.0;
	SubDivisionsX = 0.0;
	SubDivisionsY = 0.0;
	_Centre = new Vertex();
	_CoordMin = new Vertex();
	_CoordMax = new Vertex();
    GenererPoints();

}

//______________________________________________________________________________
Plan :: Plan(double Largeur_p,
             double Profondeur_p,
			 int SubDivisionsX_p,
			 int SubDivisionsY_p
             )
{                   
    Largeur = Largeur_p;  
    Profondeur = Profondeur_p;
	SubDivisionsX = SubDivisionsX_p;
	SubDivisionsY = SubDivisionsY_p;
	_CoordMin = new Vertex();
	_CoordMax = new Vertex();
    GenererPoints();

}

//______________________________________________________________________________
Plan :: Plan(double Largeur_p,
             double Profondeur_p,
			 int SubDivisionsX_p,
			 int SubDivisionsY_p,
			 Vertex * Centre_p
             )
{                   
    Largeur = Largeur_p;     
    Profondeur = Profondeur_p;
	SubDivisionsX = SubDivisionsX_p;
	SubDivisionsY = SubDivisionsY_p;
	_CoordMin = new Vertex();
	_CoordMax = new Vertex();
	_Centre = Centre_p;
    GenererPoints(Centre_p);

}
//______________________________________________________________________________
Plan :: ~Plan()
{
	vector <Vertex*>::iterator Iter1;
	for( Iter1 = vMesVertex.begin( );Iter1 != vMesVertex.end( ) ; Iter1++ ){					
		delete((*Iter1));				
	}
	vMesVertex.erase(vMesVertex.begin(), vMesVertex.end());

	vector <Face*>::iterator Iter;
	for( Iter = vFaces.begin( );Iter != vFaces.end( ) ; Iter++ ){					
		delete((*Iter));				
	}
	vFaces.erase(vFaces.begin(), vFaces.end());
	delete _CoordMax;
	delete _CoordMin ;
    delete _Centre;
}
//______________________________________________________________________________
void Plan :: GenererPoints ()
{
	GenererPoints (NULL);
}
//______________________________________________________________________________

void Plan :: GenererPoints (Vertex * Centre_p)
{
	double EcartX= Largeur/SubDivisionsX;
	double EcartY= Profondeur/SubDivisionsY;

	Face* pFace_l = NULL;
	Arcs* pArc_l = NULL;
	Vertex* pVertexTmp = NULL;
		
	if(Centre_p != NULL)
	{
		for(int i=0; i<SubDivisionsX ;i++)
		{
			for(int j=0; j<SubDivisionsY ;j++)
			{
				pVertexTmp = new Vertex((i*EcartX),0.0,(j*EcartY));
				pVertexTmp->operator += (Centre_p);
				vMesVertex.push_back(pVertexTmp);	
			}
		}

		
		for( int i=0; i<vMesVertex.size()-SubDivisionsX ;i+=SubDivisionsX)
		{		
			for(int j=0; j<SubDivisionsY-1 ;j++)
			{
				pFace_l = new Face();
				pArc_l = new Arcs();   				
				
				pArc_l->vSuivants.push_back(vMesVertex[i+j+1]); 
				pArc_l->vSuivants.push_back(vMesVertex[i+SubDivisionsX+j]); 
				pArc_l->vSuivants.push_back(vMesVertex[i+j]); 
				pFace_l->AddArcs(pArc_l);
				vFaces.push_back(pFace_l);

				pFace_l = new Face();
				pArc_l = new Arcs();			
								
				pArc_l->vSuivants.push_back(vMesVertex[i+SubDivisionsX+j+1]); 
				pArc_l->vSuivants.push_back(vMesVertex[i+SubDivisionsX+j]); 
				pArc_l->vSuivants.push_back(vMesVertex[i+j+1]); 

				pFace_l->AddArcs(pArc_l);
				vFaces.push_back(pFace_l);		
			}
		}
	}
	pFace_l = NULL;
	pArc_l = NULL;	
	pVertexTmp = NULL;
 
}
//______________________________________________________________________________
void Plan::Dessiner(Type_Affichage typeAff_p)
{     
	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
	}
} 

//______________________________________________________________________________
void Plan::DessinerListe()
{   	
	glCallList(IdListe);
}
//______________________________________________________________________________
void Plan::CreerListe(Type_Affichage typeAff_p)
{   	

	if (glIsList(IdListe))
		glDeleteLists(IdListe,1);
	
	IdListe=glGenLists(1);
	
	glNewList(IdListe,GL_COMPILE);

	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
	}

	glEndList();
}        
//______________________________________________________________________________

void Plan :: GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l)
{
	pCoordMin_l = _CoordMin; 
	pCoordMax_l = _CoordMax;
}

//______________________________________________________________________________

void Plan :: CalculerExtremes()
{
	vFaces[0]->CalculerExtremes();

	_CoordMin->x = vFaces[0]->_CoordMin->x;
	_CoordMax->x = vFaces[0]->_CoordMax->x;
	_CoordMin->y = vFaces[0]->_CoordMin->y;
	_CoordMax->y = vFaces[0]->_CoordMax->y;
	_CoordMin->z = vFaces[0]->_CoordMin->z;
	_CoordMax->z = vFaces[0]->_CoordMax->z;


	for(int i=1; i< (int)(vFaces.size()) ;i++){

		vFaces[i]->CalculerExtremes();
		
		if(vFaces[i]->_CoordMax->x > _CoordMax->x){
			_CoordMax->x = vFaces[i]->_CoordMax->x;
		}

		if(vFaces[i]->_CoordMin->x < _CoordMin->x){
			_CoordMin->x = vFaces[i]->_CoordMin->x;
		}

		if(vFaces[i]->_CoordMax->y > _CoordMax->y){
			_CoordMax->y= vFaces[i]->_CoordMax->y;
		}

		if(vFaces[i]->_CoordMin->y < _CoordMin->y){
			_CoordMin->y = vFaces[i]->_CoordMin->y;
		}

		if(vFaces[i]->_CoordMax->z > _CoordMax->z){
			_CoordMax->z = vFaces[i]->_CoordMax->z;
		}

		if(vFaces[i]->_CoordMin->z < _CoordMin->z){
			_CoordMin->z = vFaces[i]->_CoordMin->z;
		}
	}

}

//______________________________________________________________________________


void Plan :: Modifier(double Largeur_p, double Profondeur_p)
{

    Largeur = Largeur_p;
	Profondeur = Profondeur_p;


	vector <Face*>::iterator Iter1;

	for( Iter1 = vFaces.begin( );Iter1 != vFaces.end( ) ; Iter1++ ){					
		delete((*Iter1));				
	}
	vFaces.erase(vFaces.begin(), vFaces.end());
	
	vector <Vertex*>::iterator Iter;

	for( Iter = vMesVertex.begin( );Iter != vMesVertex.end( ) ; Iter++ ){					
		delete((*Iter));				
	}

	vMesVertex.erase(vMesVertex.begin(), vMesVertex.end());

	GenererPoints (_Centre);


}
//______________________________________________________________________________

void Plan::DessinerKillBox()
{
	//Cube du bas
    glColor3f(0.0,0.7,0.7);
	glBegin(GL_LINE_LOOP);
	glVertex3f(_CoordMin->x, _CoordMin->y, _CoordMax->z);
	glVertex3f(_CoordMin->x, _CoordMin->y, _CoordMin->z);
	glVertex3f(_CoordMax->x, _CoordMin->y, _CoordMin->z);
	glVertex3f(_CoordMax->x, _CoordMin->y, _CoordMax->z);	
	glEnd();

	//Cube du haut
    glColor3f(0.0,0.7,0.7);
	glBegin(GL_LINE_LOOP);
	glVertex3f(_CoordMin->x, _CoordMax->y, _CoordMax->z);
	glVertex3f(_CoordMin->x, _CoordMax->y, _CoordMin->z);
	glVertex3f(_CoordMax->x, _CoordMax->y, _CoordMin->z);
	glVertex3f(_CoordMax->x, _CoordMax->y, _CoordMax->z);	
	glEnd();


	//les arretes lat�rales
	glBegin(GL_LINES);
	glVertex3f(_CoordMin->x, _CoordMin->y, _CoordMax->z);
	glVertex3f(_CoordMin->x, _CoordMax->y, _CoordMax->z);

	glVertex3f(_CoordMin->x, _CoordMin->y, _CoordMin->z);
	glVertex3f(_CoordMin->x, _CoordMax->y, _CoordMin->z);

	glVertex3f(_CoordMax->x, _CoordMin->y, _CoordMin->z);
	glVertex3f(_CoordMax->x, _CoordMax->y, _CoordMin->z);

	glVertex3f(_CoordMax->x, _CoordMin->y, _CoordMax->z);
	glVertex3f(_CoordMax->x, _CoordMax->y, _CoordMax->z);
	glEnd();
}
//______________________________________________________________________________
