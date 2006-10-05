/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Cone.h - Cone.cpp

 Desc:   Classe g�rant la primitive du c�ne

*******************************************************************************/

#include "Cone.h"
//______________________________________________________________________________
Cone::Cone ()
{
    NbFaces = 0;
    Hauteur = 0.0;
    Rayon = 0.0; 
	IdListe1 = -1;    
	IdListe2 = -1;    
    _Centre = new Vertex();
}
//______________________________________________________________________________
Cone::~Cone ()
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
    delete _Centre;
	delete _CoordMax;
	delete _CoordMin; 

}
//______________________________________________________________________________

Cone::Cone(double Hauteur_p,
           double Rayon_p,           
           int NbFaces_p			  
           )
{
	_CoordMax = new Vertex();
	_CoordMin = new Vertex();
	NbFaces = NbFaces_p;
    Hauteur = Hauteur_p;
    Rayon = Rayon_p;
	IdListe1 = -1;    
	IdListe2 = -1;   
    _Centre = new Vertex();
    GenererPoints();


}
//______________________________________________________________________________

Cone::Cone(double Hauteur_p,
           double Rayon_p,          
           int NbFaces_p,
		   Vertex* Centre_p
           )
{

	_CoordMax = new Vertex();
	_CoordMin = new Vertex();
	NbFaces = NbFaces_p;
    Hauteur = Hauteur_p;
    Rayon = Rayon_p;
	IdListe1 = -1;    
	IdListe2 = -1;   
    _Centre = Centre_p;
    GenererPoints(_Centre);


}

//______________________________________________________________________________
void Cone::GenererPoints ()
{
	GenererPoints (NULL); 
}

//______________________________________________________________________________
void Cone::GenererPoints (Vertex * Centre_p)
{

	double angleRotation = (M_PI_FOIS_2) / NbFaces;

	Vertex * vTmp_l;
	Vertex * CentreBas; 
	Vertex * CentreHaut; 
		
	if(Centre_p!=NULL){
		
		CentreBas = new Vertex();
		CentreHaut = new Vertex(0.0f, Hauteur, 0.0f);

		//etape 1 : On calcul  les coordonn�es des points
		for (double dAlphaI = 0 ; dAlphaI < M_PI_FOIS_2; dAlphaI += angleRotation) {
        
			vTmp_l = new Vertex();
		
			vTmp_l->x = Rayon * cos(dAlphaI);
			vTmp_l->y = 0.0;
			vTmp_l->z = Rayon * sin(dAlphaI);
			vMesVertex.push_back(vTmp_l);
			
			vTmp_l->operator +=(Centre_p);
		}
		
		CentreBas = new Vertex(Centre_p);
		CentreHaut = new Vertex(0.0f ,Hauteur, 0.0f);
		CentreHaut->operator +=(Centre_p);
		vMesVertex.push_back(CentreBas);
		vMesVertex.push_back(CentreHaut);

		//etape 2 : on reconstitue les faces

		if(Hauteur > 0){
			int i = 0;
			Face * fBase_l;		
			Face * fCote_l;

			for(; i< NbFaces-1; i++){

				fBase_l = new Face();
				fCote_l = new Face();
				
				//Composition du bas
				fBase_l->_v1 = vMesVertex[i];	
				fBase_l->_v2 = vMesVertex[i+1];
				fBase_l->_v3 = CentreBas;
				vFaces.push_back(fBase_l);

				//Composition du cot�						
				fCote_l->_v1 = vMesVertex[i+1];
				fCote_l->_v2 = vMesVertex[i];
				fCote_l->_v3 = CentreHaut;				
				vFaces.push_back(fCote_l);

			}

			fBase_l = new Face();			
			fCote_l = new Face();

			//Composition du bas
			fBase_l->_v1 = vMesVertex[NbFaces-1];		
			fBase_l->_v2 = vMesVertex[0];
			fBase_l->_v3 = CentreBas;
			vFaces.push_back(fBase_l);		
					
			//Composition du cot�						
			fCote_l->_v1 = vMesVertex[0];
			fCote_l->_v2 = vMesVertex[i];
			fCote_l->_v3 = CentreHaut;			
			vFaces.push_back(fCote_l);

		}else{
			int i = 0;
			Face * fBase_l;		
			Face * fCote_l;		

			for(; i< NbFaces-1; i++){
			
				fBase_l = new Face();
				fCote_l = new Face();

				
				//Composition du bas !!				
				fBase_l->_v1 = CentreBas;
				fBase_l->_v2 = vMesVertex[i+1];
				fBase_l->_v3 = vMesVertex[i];
				vFaces.push_back(fBase_l);

				//Composition du cot� !!				
				fCote_l->_v1 = CentreHaut;
				fCote_l->_v2 = vMesVertex[i];
				fCote_l->_v3 = vMesVertex[i+1];			
				vFaces.push_back(fCote_l);

			}

			fBase_l = new Face();			
			fCote_l = new Face();


			//Composition du bas !!			
			fBase_l->_v1 = CentreBas;
			fBase_l->_v2 = vMesVertex[0];
			fBase_l->_v3 = vMesVertex[NbFaces-1];
			vFaces.push_back(fBase_l);
					
			//Composition du cot� !!				
			fCote_l->_v1 = CentreHaut;
			fCote_l->_v2 = vMesVertex[i];
			fCote_l->_v3 = vMesVertex[0];
			vFaces.push_back(fCote_l);
		}


	}else{

		CentreBas = new Vertex();
		CentreHaut = new Vertex(0.0f, Hauteur, 0.0f);

		//etape 1 : On calcul  les coordonn�es des points
		for (double dAlphaI = 0 ; dAlphaI < M_PI_FOIS_2; dAlphaI += angleRotation) {
        
			vTmp_l = new Vertex();
		
			vTmp_l->x = Rayon * cos(dAlphaI);
			vTmp_l->y = 0.0;
			vTmp_l->z = Rayon * sin(dAlphaI);
			vMesVertex.push_back(vTmp_l);				
		}
		
		CentreBas = new Vertex();
		CentreHaut = new Vertex(0.0f ,Hauteur, 0.0f);
		vMesVertex.push_back(CentreBas);
		vMesVertex.push_back(CentreHaut);

		//etape 2 : on reconstitue les faces

		if(Hauteur >0){
			int i = 0;
			Face * fBase_l;		
			Face * fCote_l;		

			for(; i< NbFaces-1; i++){
			
				fBase_l = new Face();
				fCote_l = new Face();
				
				//Composition du bas
				fBase_l->_v1 = vMesVertex[i];		
				fBase_l->_v2 = vMesVertex[i+1];
				fBase_l->_v3 = CentreBas;
				vFaces.push_back(fBase_l);

				//Composition du cot�						
				fCote_l->_v1 = vMesVertex[i+1];
				fCote_l->_v2 = vMesVertex[i];
				fCote_l->_v3 = CentreHaut;					
				vFaces.push_back(fCote_l);

			}

			fBase_l = new Face();			
			fCote_l = new Face();


			//Composition du bas
			fBase_l->_v1 = vMesVertex[NbFaces-1];		
			fBase_l->_v2 = vMesVertex[0];
			fBase_l->_v3 = CentreBas;
			vFaces.push_back(fBase_l);
					
			//Composition du cot�						
			fCote_l->_v1 = vMesVertex[0];
			fCote_l->_v2 = vMesVertex[i];
			fCote_l->_v3 = CentreHaut;
			vFaces.push_back(fCote_l);

			
			
		}else{
			int i = 0;
			Face * fBase_l;		
			Face * fCote_l;		

			for(; i< NbFaces-1; i++){				

				fBase_l = new Face();
				fCote_l = new Face();

				
				//Composition du bas !!				
				fBase_l->_v1 = CentreBas;
				fBase_l->_v2 = vMesVertex[i+1];
				fBase_l->_v3 = vMesVertex[i];
				vFaces.push_back(fBase_l);

				//Composition du cot� !!				
				fCote_l->_v1 = CentreHaut;
				fCote_l->_v2 = vMesVertex[i];
				fCote_l->_v3 = vMesVertex[i+1];
				vFaces.push_back(fCote_l);
			}

			fBase_l = new Face();			
			fCote_l = new Face();


			//Composition du bas !!			
			fBase_l->_v1 = CentreBas;
			fBase_l->_v2 = vMesVertex[0];
			fBase_l->_v3 = vMesVertex[NbFaces-1];
			vFaces.push_back(fBase_l);
					
			//Composition du cot� !!			
			fCote_l->_v1 = CentreHaut;
			fCote_l->_v2 = vMesVertex[i];
			fCote_l->_v3 = vMesVertex[0];
			vFaces.push_back(fCote_l);
		}

	}

	vTmp_l;
    CentreBas = NULL; 
	CentreHaut = NULL; 


}
//______________________________________________________________________________
void Cone::CreerListe1(Type_Affichage typeAff_p)
{   	
  
	if (glIsList(IdListe1))
		glDeleteLists(IdListe1,1);
	
	IdListe1=glGenLists(1);
	
	glNewList(IdListe1,GL_COMPILE);

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
void Cone::CreerListe2(Type_Affichage typeAff_p)
{   	
  
	if (glIsList(IdListe2))
		glDeleteLists(IdListe2,1);
	
	IdListe2=glGenLists(1);
	
	glNewList(IdListe2,GL_COMPILE);

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
void Cone::Dessiner (Type_Affichage typeAff_p)
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
void Cone::DessinerListe1()
{   	
	glCallList(IdListe1);
}
//______________________________________________________________________________
void Cone::DessinerListe2()
{   	
	glCallList(IdListe2);
}
//______________________________________________________________________________

void Cone::Modifier(double Rayon_p, double Hauteur_p)
{
    Rayon = Rayon_p;
    Hauteur = Hauteur_p;    

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

Figure* Cone::Dupliquer()
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
