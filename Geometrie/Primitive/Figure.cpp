/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Lo�c DASSONVILLE
 But:    Moteur 3D

 Fichier: Figure.h - Figure.cpp

 Desc:  Classe g�rant les figures primitives (cylindres, c�nes, sph�res...)
        Contient les faces de la figure.

*******************************************************************************/

#include "Figure.h"
//______________________________________________________________________________
Figure :: Figure ()
{
	IdListe1= -1;
	IdListe2= -1;
	_Centre = new Vertex();
	_CoordMin = new Vertex();
	_CoordMax = new Vertex();
	colorMaterial[0] = 0.5;
	colorMaterial[1] = 0.5;
	colorMaterial[2] = 0.5;

	colorReflexion[0] = 0.5;
	colorReflexion[1] = 0.5;
	colorReflexion[2] = 0.5;

	shininess[0] = 5.0;
}
//______________________________________________________________________________
Figure :: ~Figure()
{
	delete	_Centre;
	delete _CoordMin;
	delete _CoordMax;

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
}
//______________________________________________________________________________
void Figure::Dessiner(Type_Affichage typeAff_p)
{
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
} 
//______________________________________________________________________________

void Figure::DessinerListe1()
{

}
//______________________________________________________________________________
void Figure::DessinerListe2()
{

}
//______________________________________________________________________________

void Figure::CreerListe1(Type_Affichage typeAff_p)
{

}
//______________________________________________________________________________

void Figure::CreerListe2(Type_Affichage typeAff_p)
{

}
//______________________________________________________________________________

void Figure::GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l)
{
	pCoordMin_l = _CoordMin; 
	pCoordMax_l = _CoordMax;
}
//______________________________________________________________________________

void Figure::CalculerExtremes()
{
	delete(_CoordMin); 
	delete(_CoordMax); 

	_CoordMin = new Vertex(vMesVertex[0]);
	_CoordMax = new Vertex(vMesVertex[0]);

	int NbVertex = vMesVertex.size();


	for(int i=1; i< NbVertex;i++){

	
		if(vMesVertex[i]->x > _CoordMax->x){
			_CoordMax->x = vMesVertex[i]->x;
		}

		if(vMesVertex[i]->x < _CoordMin->x){
			_CoordMin->x = vMesVertex[i]->x;
		}

		if(vMesVertex[i]->y > _CoordMax->y){
			_CoordMax->y= vMesVertex[i]->y;
		}

		if(vMesVertex[i]->y < _CoordMin->y){
			_CoordMin->y = vMesVertex[i]->y;
		}

		if(vMesVertex[i]->z > _CoordMax->z){
			_CoordMax->z = vMesVertex[i]->z;
		}

		if(vMesVertex[i]->z < _CoordMin->z){
			_CoordMin->z = vMesVertex[i]->z;
		}
	}

}
//______________________________________________________________________________

void Figure::DessinerKillBox()
{
	glColor3f(0.0,0.7,0.7);

	//Cube du bas    
	glBegin(GL_LINE_LOOP);
	glVertex3f(_CoordMin->x, _CoordMin->y, _CoordMax->z);
	glVertex3f(_CoordMin->x, _CoordMin->y, _CoordMin->z);
	glVertex3f(_CoordMax->x, _CoordMin->y, _CoordMin->z);
	glVertex3f(_CoordMax->x, _CoordMin->y, _CoordMax->z);	
	glEnd();

	//Cube du haut
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
	glColor3f(1.0f, 1.0f, 1.0f);
	
}
//______________________________________________________________________________

Figure* Figure::Dupliquer()
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


bool Figure::Ecrire(FILE* pFile)
{
	int TailleDouble = sizeof(float);
	int TailleInt    = sizeof(int);
	int NbVertex = vMesVertex.size();
	int NbFaces = vFaces.size();
	Face* FaceTmp = NULL;
	bool Trouve_l = false;
	int cpt=0;

	if ( pFile == NULL ){		
		return false;
	}

	//On ecrit le nombre de vertex
	fwrite(	&NbVertex, 1, TailleInt, pFile );

	if(ferror( pFile )){
		return false;
	}


	for(int i=0; i<NbVertex; i++)
	{
		fwrite(	&vMesVertex[i]->x, 1, TailleDouble, pFile );
		fwrite(	&vMesVertex[i]->y, 1, TailleDouble, pFile );
		fwrite(	&vMesVertex[i]->z, 1, TailleDouble, pFile );
	}


	//On ecrit le nombre de faces
	fwrite(	&NbFaces, 1, TailleInt, pFile );

	if(ferror( pFile )){
		return false;
	}

	for(int i=0; i<NbFaces; i++){
		
		FaceTmp = vFaces[i];		
		
		Trouve_l = false;
		cpt=0;

		while(!Trouve_l){

			if(FaceTmp->_v1 == vMesVertex[cpt])
			{
				fwrite(	&cpt, 1, TailleInt, pFile );	
				Trouve_l = true;
			}
			cpt++;
		}
	
		Trouve_l = false;
		cpt=0;

		while(!Trouve_l){

			if(FaceTmp->_v2 == vMesVertex[cpt])
			{
				fwrite(	&cpt, 1, TailleInt, pFile );	
				Trouve_l = true;
			}
			cpt++;
		}
		
		Trouve_l = false;
		cpt=0;

		while(!Trouve_l){

			if(FaceTmp->_v3 == vMesVertex[cpt])
			{
				fwrite(	&cpt, 1, TailleInt, pFile );	
				Trouve_l = true;
			}
			cpt++;
		}

	}

	return true;
}

//______________________________________________________________________________


bool Figure::Lire(FILE* pFile)
{
	
	int TailleDouble = sizeof(float);
	int TailleInt    = sizeof(int);
	Vertex* VertexTmp = NULL;
	Vertex* AdrVertex = NULL;
	Face* FaceTmp = NULL;
	int NbVertex = 0;
	int NbFaces = 0;
	int Pos = 0;


	if ( pFile == NULL ){		
		return false;
	}

	//On lit le nombre de vertex
	fread( &NbVertex, 1, sizeof(int), pFile );

	if(ferror( pFile )){
		return false;
	}


	for(int i=0; i<NbVertex; i++)
	{
		VertexTmp = new Vertex();	
		fread(	&VertexTmp->x, 1, TailleDouble, pFile );
		fread(	&VertexTmp->y, 1, TailleDouble, pFile );
		fread(	&VertexTmp->z, 1, TailleDouble, pFile );
		vMesVertex.push_back(VertexTmp);
	
	}
	
	//On lit le nombre de faces
	fread(	&NbFaces, 1, sizeof(int), pFile );

	if(ferror( pFile )){
		return false;
	}

	for(int i=0; i<NbFaces; i++){
		
		FaceTmp= new Face();
		
		fread(	&Pos, 1, TailleInt, pFile );
		FaceTmp->_v1 = vMesVertex[Pos];	

		fread(	&Pos, 1, TailleInt, pFile );
		FaceTmp->_v2 = vMesVertex[Pos];				
	
		fread(	&Pos, 1, TailleInt, pFile );
		FaceTmp->_v3 = vMesVertex[Pos];	
		
		vFaces.push_back(FaceTmp);

	}
	return true;

}
//______________________________________________________________________________

bool Figure :: MemeCouleur (Figure * figure)
{
	return (colorMaterial[0] == figure->colorMaterial[0]) &&
		   (colorMaterial[1] == figure->colorMaterial[1]) &&
		   (colorMaterial[2] == figure->colorMaterial[2]) &&
		   (colorReflexion[0] == figure->colorReflexion[0]) &&
		   (colorReflexion[1] == figure->colorReflexion[1]) &&
		   (colorReflexion[2] == figure->colorReflexion[2]) &&
		   (shininess[0] == figure->shininess[0]);
}

//______________________________________________________________________________
