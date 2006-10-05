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

Vertex* Plan ::GetVertex(int i, int j)
{
	return vMesVertex[i*SubDivisionsY+j];
}

//______________________________________________________________________________


void Plan :: GenererPoints (Vertex * Centre_p)
{
	float EcartX= Largeur/SubDivisionsX;
	float EcartY= Profondeur/SubDivisionsY;

	Face* pFace_l = NULL;
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


		if(Largeur >=0 && Profondeur >= 0 || Largeur <0 && Profondeur < 0)
		{
			for(int i=0; i<SubDivisionsX-1 ;i++)
			{
				for(int j=0; j<SubDivisionsY-1 ;j++)
				{
					pFace_l = new Face();												
					pFace_l->_v1 = GetVertex(i,j+1);
					pFace_l->_v2 = GetVertex(i+1,j); 
					pFace_l->_v3 = GetVertex(i,j); 
					vFaces.push_back(pFace_l);

					
					pFace_l = new Face();						  
					pFace_l->_v1 = GetVertex(i,j+1); 
					pFace_l->_v2 = GetVertex(i+1,j+1);
					pFace_l->_v3 = GetVertex(i+1,j); 
					vFaces.push_back(pFace_l);
					
				}
			}
		}else{
			for(int i=0; i<SubDivisionsX-1 ;i++)
			{
				for(int j=0; j<SubDivisionsY-1 ;j++)
				{
					pFace_l = new Face();										
					pFace_l->_v1 = GetVertex(i,j); 
					pFace_l->_v2 = GetVertex(i+1,j); 
					pFace_l->_v3 = GetVertex(i,j+1);
					vFaces.push_back(pFace_l);

					
					pFace_l = new Face();					
					pFace_l->_v1 = GetVertex(i+1,j);
					pFace_l->_v2 = GetVertex(i+1,j+1);
					pFace_l->_v3 = GetVertex(i,j+1); 
					vFaces.push_back(pFace_l);
					
				}
			}

		}

	}
	pFace_l = NULL;
	pVertexTmp = NULL;
 
}
//______________________________________________________________________________
void Plan::Dessiner(Type_Affichage typeAff_p)
{     
	int i;

	switch(typeAff_p){

		case lignesfermees:
			for(i=0; i<vFaces.size();i++){
				vFaces[i]->DessinerFilaire();
			}
			break;

		case facesTriangle:	
			glMaterialfv(GL_FRONT, GL_SPECULAR, colorReflexion);
			glMaterialfv(GL_FRONT, GL_AMBIENT, colorMaterial);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, colorMaterial);
			//glMaterialfv(GL_FRONT, GL_EMISSION, MatEmmision);
			glMaterialfv(GL_FRONT, GL_SHININESS, shininess);	
			for(i=0; i<vFaces.size();i++){
				vFaces[i]->DessinerPlein();
			}
			break;	
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

	int i;

	switch(typeAff_p){

		case lignesfermees:
			for(i=0; i<vFaces.size();i++){
				vFaces[i]->DessinerFilaire();
			}
			break;

		case facesTriangle:		
			for(i=0; i<vFaces.size();i++){
				vFaces[i]->DessinerPlein();
			}
			break;	
	}

	glEndList();
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
Figure* Plan::Dupliquer()
{
	
	Figure* CopyFig = new Figure ();
	Face* FaceTmp = NULL;
	bool Trouve_l;
	int cpt;

	int NbVertex_l = vMesVertex.size();
	int NbFaces_l = vFaces.size();


	//On effectue une copie des vertex
	for(int i=0; i<NbVertex_l ;i++){
		CopyFig->vMesVertex.push_back(new Vertex(vMesVertex[i]));
	}
	
	
	//Copie de chaque face de la figure
	for(int i=0; i< NbFaces_l; i++){

		FaceTmp = new Face();

		Trouve_l = false;
		cpt = 0;

		while(!Trouve_l){

			if(vFaces[i]->_v1 ==vMesVertex[cpt])
			{
				FaceTmp->_v1 = CopyFig->vMesVertex[cpt];
				Trouve_l = true;
			}				
			cpt++;
		}

		Trouve_l = false;
		cpt = 0;

		while(!Trouve_l){

			if(vFaces[i]->_v2 ==vMesVertex[cpt])
			{
				FaceTmp->_v2 = CopyFig->vMesVertex[cpt];
				Trouve_l = true;
			}				
			cpt++;
		}
	
		Trouve_l = false;
		cpt = 0;

		while(!Trouve_l){

			if(vFaces[i]->_v3 ==vMesVertex[cpt])
			{
				FaceTmp->_v3 = CopyFig->vMesVertex[cpt];
				Trouve_l = true;
			}				
			cpt++;
		}
		
		CopyFig->vFaces.push_back(FaceTmp);
	}


	FaceTmp = NULL;

	return CopyFig;

}
//______________________________________________________________________________

