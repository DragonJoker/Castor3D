/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Sphere.h - Sphere.cpp

 Desc:   Classe g�rant la primitive de la sphere

*******************************************************************************/

#include "Sphere.h"
//______________________________________________________________________________
Sphere :: Sphere ()
{
	NbFaces = 0.0;
    Rayon = 0.0;
    _Centre = new Vertex();

}
//______________________________________________________________________________
Sphere :: Sphere (					  
                      double Rayon_p,
					  int NbFaces_p
                      ){
   
	NbFaces = NbFaces_p;
    Rayon = Rayon_p;
	_CoordMax = new Vertex();
	_CoordMin = new Vertex();
    _Centre = new Vertex();
    GenererPoints();
}
//______________________________________________________________________________
Sphere :: Sphere (					  
                      double Rayon_p,
					  int NbFaces_p,
					  Vertex * Centre_p
                      ){
   
	NbFaces = NbFaces_p;
    Rayon = Rayon_p;
	_CoordMax = new Vertex();
	_CoordMin = new Vertex();
    _Centre = Centre_p;

    GenererPoints(_Centre);
}
//______________________________________________________________________________
Sphere :: ~Sphere ()
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
void Sphere::GenererPoints ()
{
	GenererPoints (NULL); 
}
//______________________________________________________________________________

void Sphere::GenererPoints (Vertex * Centre_p)
{

	if(Rayon<0){
		Rayon = -Rayon;
	}

	if(NbFaces<3){
		return;
	}

	Arcs * ArcTmp_l = new Arcs();
	Vertex * CentreHaut;
	Vertex * CentreBas;

	Face * pFaceCourante_l = NULL;
	Arcs * pArcCourant_l = NULL;

	double angleRotation = (M_PI_FOIS_2) / NbFaces;

	if(NbFaces%2 == 0){

		//CALCUL DE LA POSITION DES POINTS
		
		double dAlphaI = 0 ;
			
		Vertex* vTmp_l=NULL;  
		ArcTmp_l = new Arcs();

		for (int i=0; i < (NbFaces/2)-1; i ++) {
        
			dAlphaI += angleRotation;
			
			vTmp_l = new Vertex();
			
			vTmp_l->x = Rayon * cos(dAlphaI);
			vTmp_l->y = 0.0;
			vTmp_l->z = Rayon * sin(dAlphaI);
				
			ArcTmp_l->vSuivants.push_back(vTmp_l);
					
		}

		CentreHaut = new Vertex(Rayon,0.0,0.0);
		CentreBas = new Vertex(-Rayon,0.0,0.0);

		CentreHaut->operator +=(Centre_p);
		CentreBas->operator +=(Centre_p);

		for(dAlphaI = 0 ; dAlphaI < M_PI_FOIS_2; dAlphaI += angleRotation){

			for(int j=0; j < ArcTmp_l->vSuivants.size(); j++){

				vTmp_l = new Vertex();
				
				vTmp_l->x = ArcTmp_l->vSuivants[j]->x;
				vTmp_l->y = ArcTmp_l->vSuivants[j]->z*sin(dAlphaI) ; 
				vTmp_l->z = ArcTmp_l->vSuivants[j]->z*cos(dAlphaI);
					
				vTmp_l->operator +=(Centre_p);
				vMesVertex.push_back(vTmp_l);
			}
		}

		//RECONSTITION DES FACES

		int TailleArc = ArcTmp_l->vSuivants.size();

		for (int i = 0 ; i < vMesVertex.size()-TailleArc ; i+=TailleArc) {

			pFaceCourante_l = new Face();			
			pFaceCourante_l->_v1 = vMesVertex[i+TailleArc];
			pFaceCourante_l->_v2 = vMesVertex[i];
			pFaceCourante_l->_v3 = CentreHaut;		
			vFaces.push_back(pFaceCourante_l);

			pFaceCourante_l = new Face();
			pFaceCourante_l->_v1 = CentreBas;
			pFaceCourante_l->_v2 = vMesVertex[i+TailleArc-1];
			pFaceCourante_l->_v3 = vMesVertex[i+TailleArc+TailleArc-1];		
			vFaces.push_back(pFaceCourante_l);
		}
		
		pFaceCourante_l = new Face();		
		pFaceCourante_l->_v1 = CentreHaut;
		pFaceCourante_l->_v2 = vMesVertex[0];
		pFaceCourante_l->_v3 = vMesVertex[vMesVertex.size()-TailleArc];	
		vFaces.push_back(pFaceCourante_l);

		pFaceCourante_l = new Face();		
		pFaceCourante_l->_v1 = vMesVertex[vMesVertex.size()-1];
		pFaceCourante_l->_v2 = vMesVertex[TailleArc-1];
		pFaceCourante_l->_v3 = CentreBas;	
		vFaces.push_back(pFaceCourante_l);


		for (int j = 0 ; j < TailleArc-1 ; j++) {	

				
			for (int i = 0 ; i < vMesVertex.size()-TailleArc ; i+=TailleArc) {	

				pFaceCourante_l = new Face();				
				pFaceCourante_l->_v1 = vMesVertex[j+i+1];
				pFaceCourante_l->_v2 = vMesVertex[j+i];
				pFaceCourante_l->_v3 = vMesVertex[j+i+TailleArc];				
				vFaces.push_back(pFaceCourante_l);
			
				pFaceCourante_l = new Face();
				pFaceCourante_l->_v1 = vMesVertex[j+i+TailleArc+1];
				pFaceCourante_l->_v2 = vMesVertex[i+j+1];
				pFaceCourante_l->_v3 = vMesVertex[j+i+TailleArc];			
				vFaces.push_back(pFaceCourante_l);
			}

			
			pFaceCourante_l = new Face();				
			pFaceCourante_l->_v1 = vMesVertex[j];
			pFaceCourante_l->_v2 = vMesVertex[j+1];
			pFaceCourante_l->_v3 = vMesVertex[vMesVertex.size()-TailleArc+1+j];
			vFaces.push_back(pFaceCourante_l);

			pFaceCourante_l = new Face();
			pFaceCourante_l->_v1 = vMesVertex[vMesVertex.size()-TailleArc+1+j];
			pFaceCourante_l->_v2 = vMesVertex[vMesVertex.size()-TailleArc+j];
			pFaceCourante_l->_v3 = vMesVertex[j];
			vFaces.push_back(pFaceCourante_l);			

		}

	}

	delete(ArcTmp_l);
	vMesVertex.push_back(CentreHaut);
	vMesVertex.push_back(CentreBas);
}

//______________________________________________________________________________
void Sphere::CreerListe(Type_Affichage typeAff_p)
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
void Sphere::Dessiner(Type_Affichage typeAff_p)
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
void Sphere::DessinerListe()
{   	
	glCallList(IdListe);
}
//______________________________________________________________________________

void Sphere::Modifier(double Rayon_p)
{

    Rayon = Rayon_p;    
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

Figure* Sphere::Dupliquer()
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
