/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Lo�c DASSONVILLE
 But:    Moteur 3D

 Fichier: Pave.h - Pave.cpp

 Desc:   Classe g�rant la primitive de Pav� (cube ou parall�l�pip�de rectangle)

*******************************************************************************/

#include "Pave.h"
//______________________________________________________________________________

Pave :: Pave()
{
	IdListe=0;
    Largeur = 0.0;
    Hauteur = 0.0;
    Profondeur = 0.0;
	_Centre = new Vertex();
	_CoordMin = new Vertex();
	_CoordMax = new Vertex();
    GenererPoints();

}

//______________________________________________________________________________
Pave :: Pave(double Largeur_p,
             double Hauteur_p,
             double Profondeur_p
             )
{                   
    Largeur = Largeur_p;
    Hauteur = Hauteur_p;    
    Profondeur = Profondeur_p;
	_CoordMin = new Vertex();
	_CoordMax = new Vertex();
    GenererPoints();

}

//______________________________________________________________________________
Pave :: Pave(double Largeur_p,
             double Hauteur_p,
             double Profondeur_p,
			 Vertex * Centre_p
             )
{                   
    Largeur = Largeur_p;
    Hauteur = Hauteur_p;    
    Profondeur = Profondeur_p;
	_CoordMin = new Vertex();
	_CoordMax = new Vertex();
    GenererPoints(Centre_p);

}
//______________________________________________________________________________
Pave :: ~Pave()
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
void Pave :: GenererPoints ()
{
	GenererPoints (NULL);
}
//______________________________________________________________________________

void Pave :: GenererPoints (Vertex * Centre_p)
{
	int CptNegatif = 0;
	if(Largeur <0){		CptNegatif++; }
	if(Hauteur <0){		CptNegatif++; }
	if(Profondeur <0){	CptNegatif++; }

	
    //Calcul des coordonn�es des 8 sommets du pav�

    vMesVertex.push_back(new Vertex(0,       0      , 0));
    vMesVertex.push_back(new Vertex(Largeur, 0      , 0));
    vMesVertex.push_back(new Vertex(Largeur, 0      ,  Profondeur));
    vMesVertex.push_back(new Vertex(0      , 0      ,  Profondeur));
    vMesVertex.push_back(new Vertex(0      , Hauteur,  Profondeur));
    vMesVertex.push_back(new Vertex(0      , Hauteur, 0));
    vMesVertex.push_back(new Vertex(Largeur, Hauteur, 0));
    vMesVertex.push_back(new Vertex(Largeur, Hauteur,  Profondeur));


	if(Centre_p!=NULL){
		vMesVertex[0]->operator +=(Centre_p);
		vMesVertex[1]->operator +=(Centre_p);
		vMesVertex[2]->operator +=(Centre_p);
		vMesVertex[3]->operator +=(Centre_p);
		vMesVertex[4]->operator +=(Centre_p);
		vMesVertex[5]->operator +=(Centre_p);
		vMesVertex[6]->operator +=(Centre_p);
		vMesVertex[7]->operator +=(Centre_p);
		_Centre = Centre_p;
	}else{
		_Centre = new Vertex();
	}
                            

	Face * pFace_l = NULL;

	if(CptNegatif!=1 && CptNegatif!=3){

		//CONSTRUCTION FACES /!\ pour openGL le z est inverse

		// Faces du bas
		pFace_l = new Face();  
		pFace_l->_v1 = vMesVertex[1]; 
		pFace_l->_v2 = vMesVertex[3]; 
		pFace_l->_v3 = vMesVertex[0]; 
		vFaces.push_back(pFace_l);
    

		pFace_l = new Face(); 
		pFace_l->_v1 = vMesVertex[2]; 
		pFace_l->_v2 = vMesVertex[3]; 
		pFace_l->_v3 = vMesVertex[1]; 
		vFaces.push_back(pFace_l);


		// Face de devant
		pFace_l = new Face();    
		pFace_l->_v1 = vMesVertex[6];
		pFace_l->_v2 = vMesVertex[1];
		pFace_l->_v3 = vMesVertex[0];
		vFaces.push_back(pFace_l);


		pFace_l = new Face();	
		pFace_l->_v1 = vMesVertex[0];  
		pFace_l->_v2 = vMesVertex[5];
		pFace_l->_v3 = vMesVertex[6];
		vFaces.push_back(pFace_l);
		

		// Faces de droite
		pFace_l = new Face();    
		pFace_l->_v1 = vMesVertex[7];
		pFace_l->_v2 = vMesVertex[2];
		pFace_l->_v3 = vMesVertex[1];
		vFaces.push_back(pFace_l);


		pFace_l = new Face();	
		pFace_l->_v1 = vMesVertex[1];
		pFace_l->_v2 = vMesVertex[6];
		pFace_l->_v3 = vMesVertex[7];
		vFaces.push_back(pFace_l);
  		
		// Face du haut
		pFace_l = new Face();    
		pFace_l->_v1 = vMesVertex[7];
		pFace_l->_v2 = vMesVertex[6];
		pFace_l->_v3 = vMesVertex[5];
		vFaces.push_back(pFace_l);

		pFace_l = new Face();
		pFace_l->_v1 = vMesVertex[5];
		pFace_l->_v2 = vMesVertex[4];
		pFace_l->_v3 = vMesVertex[7];
		vFaces.push_back(pFace_l);

		// Face de gauche
		pFace_l = new Face();    
		pFace_l->_v1 = vMesVertex[3];
		pFace_l->_v2 = vMesVertex[4];
		pFace_l->_v3 = vMesVertex[5];
		vFaces.push_back(pFace_l);

		pFace_l = new Face();    
		pFace_l->_v1 = vMesVertex[5];
		pFace_l->_v2 = vMesVertex[0];
		pFace_l->_v3 = vMesVertex[3];
		vFaces.push_back(pFace_l);

		// Face de derri�re
		pFace_l = new Face();    
		pFace_l->_v1 = vMesVertex[2];
		pFace_l->_v2 = vMesVertex[7];
		pFace_l->_v3 = vMesVertex[4];
		vFaces.push_back(pFace_l);
		
		pFace_l = new Face();	
		pFace_l->_v1 = vMesVertex[4];
		pFace_l->_v2 = vMesVertex[3];
		pFace_l->_v3 = vMesVertex[2];
		vFaces.push_back(pFace_l);

	}else{

		//CONSTRUCTION FACES /!\ pour openGL le z est inverse

		// Faces du bas !!
		pFace_l = new Face(); 		
		pFace_l->_v1 = vMesVertex[0]; 
		pFace_l->_v2 = vMesVertex[3]; 
		pFace_l->_v3 = vMesVertex[1]; 
		vFaces.push_back(pFace_l);
    
		pFace_l = new Face();		 
		pFace_l->_v1 = vMesVertex[1];
		pFace_l->_v2 = vMesVertex[3];
		pFace_l->_v3 = vMesVertex[2]; 
		vFaces.push_back(pFace_l);


		// Face de devant !!
		pFace_l = new Face(); 		
		pFace_l->_v1 = vMesVertex[0];
		pFace_l->_v2 = vMesVertex[1];
		pFace_l->_v3 = vMesVertex[6];
		vFaces.push_back(pFace_l);


		pFace_l = new Face();		
		pFace_l->_v1 = vMesVertex[6];
		pFace_l->_v2 = vMesVertex[5];
		pFace_l->_v3 = vMesVertex[0];
		vFaces.push_back(pFace_l);
		

		// Faces de droite !!
		pFace_l = new Face(); 		
		pFace_l->_v1 = vMesVertex[1];
		pFace_l->_v2 = vMesVertex[2];
		pFace_l->_v3 = vMesVertex[7];
		vFaces.push_back(pFace_l);


		pFace_l = new Face();		
		pFace_l->_v1 = vMesVertex[7];
		pFace_l->_v2 = vMesVertex[6];
		pFace_l->_v3 = vMesVertex[1];
		vFaces.push_back(pFace_l);
  		
		// Face du haut !!
		pFace_l = new Face(); 		
		pFace_l->_v1 = vMesVertex[5];
		pFace_l->_v2 = vMesVertex[6];
		pFace_l->_v3 = vMesVertex[7];
		vFaces.push_back(pFace_l);

		pFace_l = new Face();		
		pFace_l->_v1 = vMesVertex[7];
		pFace_l->_v2 = vMesVertex[4];
		pFace_l->_v3 = vMesVertex[5];
		vFaces.push_back(pFace_l);

		// Face de gauche !!
		pFace_l = new Face();		
		pFace_l->_v1 = vMesVertex[5];
		pFace_l->_v2 = vMesVertex[4];
		pFace_l->_v3 = vMesVertex[3];
		vFaces.push_back(pFace_l);

		pFace_l = new Face();  		
		pFace_l->_v1 = vMesVertex[3];
		pFace_l->_v2 = vMesVertex[0];
		pFace_l->_v3 = vMesVertex[5];
		vFaces.push_back(pFace_l);

		// Face de derri�re !!
		pFace_l = new Face(); 		
		pFace_l->_v1 = vMesVertex[4];
		pFace_l->_v2 = vMesVertex[7];
		pFace_l->_v3 = vMesVertex[2];
		vFaces.push_back(pFace_l);
		
		pFace_l = new Face();		
		pFace_l->_v1 = vMesVertex[2];
		pFace_l->_v2 = vMesVertex[3];
		pFace_l->_v3 = vMesVertex[4];
		vFaces.push_back(pFace_l);

	}

    pFace_l = NULL;




 
}
//______________________________________________________________________________
void Pave::Dessiner(Type_Affichage typeAff_p)
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
void Pave::DessinerListe()
{   	
	glCallList(IdListe);
}
//______________________________________________________________________________
void Pave::CreerListe(Type_Affichage typeAff_p)
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

	glEndList();
}        
//______________________________________________________________________________

void Pave :: Modifier(double Largeur_p, double Hauteur_p, double Profondeur_p )
{
    Largeur = Largeur_p;
    Hauteur = Hauteur_p;    
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

Figure* Pave::Dupliquer()
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

