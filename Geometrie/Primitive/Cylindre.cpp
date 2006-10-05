/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Cylindre.h - Cylindre.cpp

 Desc:   Classe g�rant la primitive du cylindre

*******************************************************************************/

#include "Cylindre.h"
//______________________________________________________________________________
Cylindre :: Cylindre ()
{
	NbFaces = 0.0;
    Hauteur = 0.0;
    Rayon = 0.0;
    _Centre = new Vertex();

}
//______________________________________________________________________________
Cylindre :: Cylindre (					  
                      double Hauteur_p,
                      double Rayon_p,
					  int NbFaces_p
                      ){
   
	NbFaces = NbFaces_p;
    Hauteur = Hauteur_p;
    Rayon = Rayon_p;
	_CoordMax = new Vertex();
	_CoordMin = new Vertex();
    _Centre = new Vertex();
    GenererPoints();
}
//______________________________________________________________________________
Cylindre :: Cylindre (					  
                      double Hauteur_p,
                      double Rayon_p,
					  int NbFaces_p,
					  Vertex * Centre_p
                      ){
   
	NbFaces = NbFaces_p;
    Hauteur = Hauteur_p;
    Rayon = Rayon_p;
	_CoordMax = new Vertex();
	_CoordMin = new Vertex();
    _Centre = Centre_p;

    GenererPoints(_Centre);
}
//______________________________________________________________________________
Cylindre :: ~Cylindre ()
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
void Cylindre::GenererPoints ()
{
	GenererPoints (NULL); 
}
//______________________________________________________________________________

void Cylindre::GenererPoints (Vertex * Centre_p)
{
	if(NbFaces<2){
		return;
	}

	//CALCUL DE LA POSITION DES POINTS
	double angleRotation = M_PI_FOIS_2 / NbFaces;

	Vertex * vTmp_l;
	Vertex * vTmp2_l;
	Vertex * vTmp3_l;

	Vertex * CentreHaut;
	Vertex * CentreBas;

	if(Centre_p!=NULL){

		for (double dAlphaI = 0 ; dAlphaI < M_PI_FOIS_2; dAlphaI += angleRotation) {
        
			vTmp_l = new Vertex();
		
			vTmp_l->x = Rayon * cos(dAlphaI);
			vTmp_l->y = 0.0;
			vTmp_l->z = Rayon * sin(dAlphaI);
			
			vTmp_l->operator +=(Centre_p);			
		
			vTmp2_l= new Vertex(vTmp_l);
			vTmp2_l->y+=Hauteur;

			vMesVertex.push_back(vTmp_l);
			vMesVertex.push_back(new Vertex(vTmp2_l));
		}

		
		CentreHaut =new Vertex(Centre_p);
		CentreHaut->y+=Hauteur;
		CentreBas = new Vertex(Centre_p);

		vMesVertex.push_back(CentreBas);
		vMesVertex.push_back(CentreHaut);


	}else{
		for (double dAlphaI = 0 ; dAlphaI < M_PI_FOIS_2; dAlphaI += angleRotation) {
        
			vTmp_l = new Vertex();
		
			vTmp_l->x = Rayon * cos(dAlphaI);
			vTmp_l->y = 0.0;
			vTmp_l->z = Rayon * sin(dAlphaI);
		
			vTmp2_l= new Vertex(vTmp_l);
			vTmp2_l->y+=Hauteur;

			vMesVertex.push_back(vTmp_l);
			vMesVertex.push_back(new Vertex(vTmp2_l));
		}
		
		CentreBas = new Vertex();
		CentreHaut = new Vertex(0.0f ,Hauteur, 0.0f);
		vMesVertex.push_back(CentreBas);
		vMesVertex.push_back(CentreHaut);
	}


	vTmp_l=NULL;
	vTmp2_l=NULL;
	vTmp3_l=NULL;



	//RECONSTITION DES FACES

	if(Hauteur > 0)
	{
		int i = 0;
		int Cpt = NbFaces+NbFaces;

		Face * fBase_l;
		Face * fSommet_l;
		Face * fCote1_l;
		Face * fCote2_l;
		
		for(; i<Cpt-2; i+=2){

			fBase_l = new Face();
			fSommet_l = new Face();
			fCote1_l = new Face();
			fCote2_l = new Face();

			//Composition du bas
			fBase_l->_v1 = vMesVertex[i];		
			fBase_l->_v2 = vMesVertex[i+2];
			fBase_l->_v3 = CentreBas;		

			//Composition du dessus
			fSommet_l->_v1 = vMesVertex[i+1];
			fSommet_l->_v2 = CentreHaut;
			fSommet_l->_v3 = vMesVertex[i+3];			

			//Composition du cote1
			fCote1_l->_v1 = vMesVertex[i];		
			fCote1_l->_v2 = vMesVertex[i+1];
			fCote1_l->_v3 = vMesVertex[i+2];			

			//Composition du cote2		
			fCote2_l->_v1 = vMesVertex[i+1];
			fCote2_l->_v2 = vMesVertex[i+3];
			fCote2_l->_v3 = vMesVertex[i+2];		

			vFaces.push_back(fSommet_l); 
			vFaces.push_back(fBase_l);
			vFaces.push_back(fCote1_l); 
			vFaces.push_back(fCote2_l);
		}
		

		fBase_l = new Face();
		fSommet_l = new Face();
		fCote1_l = new Face();
		fCote2_l = new Face();	

		//Composition du bas
		fBase_l->_v1 = vMesVertex[Cpt-2];		
		fBase_l->_v2 = vMesVertex[0];
		fBase_l->_v3 = CentreBas;

		//Composition du dessus
		fSommet_l->_v1 = vMesVertex[Cpt-1];
		fSommet_l->_v2 = CentreHaut;
		fSommet_l->_v3 = vMesVertex[1];		

		//Composition du cote1
		fCote1_l->_v1 = vMesVertex[i];		
		fCote1_l->_v2 = vMesVertex[i+1];
		fCote1_l->_v3 = vMesVertex[0];

		//Composition du cote2		
		fCote2_l->_v1 = vMesVertex[i+1];
		fCote2_l->_v2 = vMesVertex[1];
		fCote2_l->_v3 = vMesVertex[0];
		

		vFaces.push_back(fSommet_l); 
		vFaces.push_back(fBase_l);
		vFaces.push_back(fCote1_l);
		vFaces.push_back(fCote2_l);

		fBase_l = NULL;
		fSommet_l = NULL;
		fCote1_l = NULL;
		fCote2_l = NULL;

	}else{
		int i = 0;
		int Cpt = NbFaces+NbFaces;

		Face * fBase_l;
		Face * fSommet_l;
		Face * fCote1_l;
		Face * fCote2_l;
		
		for(; i<Cpt-2; i+=2){

			fBase_l = new Face();
			fSommet_l = new Face();
			fCote1_l = new Face();
			fCote2_l = new Face();

			//Composition du bas !!			
			fBase_l->_v1 = CentreBas;
			fBase_l->_v2 = vMesVertex[i+2];
			fBase_l->_v3 = vMesVertex[i];			

			//Composition du dessus !!			
			fSommet_l->_v1 = vMesVertex[i+3];
			fSommet_l->_v2 = CentreHaut;
			fSommet_l->_v3 = vMesVertex[i+1];			

			//Composition du cote1 !!			
			fCote1_l->_v1 = vMesVertex[i+2];
			fCote1_l->_v2 = vMesVertex[i+1];
			fCote1_l->_v3 = vMesVertex[i];
		
			//Composition du cote2 !!			
			fCote2_l->_v1 = vMesVertex[i+2];
			fCote2_l->_v2 = vMesVertex[i+3];
			fCote2_l->_v3 = vMesVertex[i+1];

			vFaces.push_back(fSommet_l); 
			vFaces.push_back(fBase_l);
			vFaces.push_back(fCote1_l); 
			vFaces.push_back(fCote2_l);
				

		}
		
		fBase_l = new Face();
		fSommet_l = new Face();
		fCote1_l = new Face();
		fCote2_l = new Face();	

		//Composition du bas !!		
		fBase_l->_v1 = CentreBas;
		fBase_l->_v2 = vMesVertex[0];
		fBase_l->_v3 = vMesVertex[Cpt-2];		

		//Composition du dessus !!		
		fSommet_l->_v1 = vMesVertex[1];
		fSommet_l->_v2 = CentreHaut;
		fSommet_l->_v3 = vMesVertex[Cpt-1];		

		//Composition du cote1 !!		
		fCote1_l->_v1 = vMesVertex[0];
		fCote1_l->_v2 = vMesVertex[i+1];
		fCote1_l->_v3 = vMesVertex[i];		

		//Composition du cote2 !!		
		fCote2_l->_v1 = vMesVertex[0];
		fCote2_l->_v2 = vMesVertex[1];
		fCote2_l->_v3 = vMesVertex[i+1];
	
		vFaces.push_back(fSommet_l); 
		vFaces.push_back(fBase_l);
		vFaces.push_back(fCote1_l);
		vFaces.push_back(fCote2_l);

		fBase_l = NULL;
		fSommet_l = NULL;
		fCote1_l = NULL;
		fCote2_l = NULL;


	}
}

//______________________________________________________________________________
void Cylindre::CreerListe(Type_Affichage typeAff_p)
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
void Cylindre::Dessiner(Type_Affichage typeAff_p)
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
void Cylindre::DessinerListe()
{   	
	glCallList(IdListe);
}
//______________________________________________________________________________

void Cylindre::Modifier(double Rayon_p, double Hauteur_p)
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
		Vertex * pt = *Iter;
		delete((*Iter));				
		pt = NULL;
	}

	vMesVertex.erase(vMesVertex.begin(), vMesVertex.end());
	GenererPoints (_Centre);
}

//______________________________________________________________________________

Figure* Cylindre::Dupliquer()
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
