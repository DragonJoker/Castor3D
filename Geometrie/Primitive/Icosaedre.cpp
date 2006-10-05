/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Icosaedre.h - Icosaedre.cpp

 Desc:   Classe g�rant la primitive de l'icosaedre

*******************************************************************************/

#include "Icosaedre.h"
//______________________________________________________________________________
Icosaedre :: Icosaedre ()
{
	NbFaces = 0.0;
    Rayon = 0.0;
    _Centre = new Vertex();

}
//______________________________________________________________________________
Icosaedre :: Icosaedre (					  
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
Icosaedre :: Icosaedre (					  
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
Icosaedre :: ~Icosaedre ()
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
void Icosaedre::GenererPoints ()
{
	GenererPoints (NULL); 
}
//______________________________________________________________________________

void Icosaedre::GenererPoints (Vertex * Centre_p)
{

	if(Rayon<0){
		Rayon = -Rayon;
	}

	if (Centre_p == NULL)
		_Centre = new Vertex();
	else
		_Centre = Centre_p;

	// Dessin de l'icosa�dre
	float phi = (1+ sqrt(5)) / 2;
	float X = Rayon / sqrt(phi * sqrt(5));
	float Z = X * phi;

	//MessageBox(NULL,"", ,MB_OK);

	// on cr�e les 12 points le composant
	Vertex * pt1  = new Vertex(-X, 0, Z);
	pt1->operator += (_Centre);
	vMesVertex.push_back(pt1);
	Vertex * pt2  = new Vertex( X, 0, Z);
	pt2->operator += (_Centre);
	vMesVertex.push_back(pt2);
	Vertex * pt3  = new Vertex(-X, 0,-Z);
	pt3->operator += (_Centre);
	vMesVertex.push_back(pt3);
	Vertex * pt4  = new Vertex( X, 0,-Z);
	pt4->operator += (_Centre);
	vMesVertex.push_back(pt4);
	Vertex * pt5  = new Vertex( 0, Z, X);
	pt5->operator += (_Centre);
	vMesVertex.push_back(pt5);
	Vertex * pt6  = new Vertex( 0, Z,-X);
	pt6->operator += (_Centre);
	vMesVertex.push_back(pt6);
	Vertex * pt7  = new Vertex( 0,-Z, X);
	pt7->operator += (_Centre);
	vMesVertex.push_back(pt7);
	Vertex * pt8  = new Vertex( 0,-Z,-X);
	pt8->operator += (_Centre);
	vMesVertex.push_back(pt8);
	Vertex * pt9  = new Vertex( Z, X, 0);
	pt9->operator += (_Centre);
	vMesVertex.push_back(pt9);
	Vertex * pt10 = new Vertex(-Z, X, 0);
	pt10->operator += (_Centre);
	vMesVertex.push_back(pt10);
	Vertex * pt11 = new Vertex( Z,-X, 0);
	pt11->operator += (_Centre);
	vMesVertex.push_back(pt11);
	Vertex * pt12 = new Vertex(-Z,-X, 0);
	pt12->operator += (_Centre);
	vMesVertex.push_back(pt12);
	
	// on construit toutes les faces du isoca�dre
	Face * pTmp_l = new Face;
	pTmp_l->_v1 = pt1;
	pTmp_l->_v2 = pt2;
	pTmp_l->_v3 = pt5;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt10;
	pTmp_l->_v2 = pt1;
	pTmp_l->_v3 = pt5;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt10;
	pTmp_l->_v2 = pt5;
	pTmp_l->_v3 = pt6;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt5;
	pTmp_l->_v2 = pt9;
	pTmp_l->_v3 = pt6;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt5;
	pTmp_l->_v2 = pt2;
	pTmp_l->_v3 = pt9;
	vFaces.push_back(pTmp_l); 
	
	pTmp_l = new Face;
	pTmp_l->_v1 = pt2;
	pTmp_l->_v2 = pt11;
	pTmp_l->_v3 = pt9;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt4;
	pTmp_l->_v2 = pt9;
	pTmp_l->_v3 = pt11;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt4;
	pTmp_l->_v2 = pt6;
	pTmp_l->_v3 = pt9;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt3;
	pTmp_l->_v2 = pt6;
	pTmp_l->_v3 = pt4;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt8;
	pTmp_l->_v2 = pt3;
	pTmp_l->_v3 = pt4;
	vFaces.push_back(pTmp_l); 

	pTmp_l = new Face;
	pTmp_l->_v1 = pt4;
	pTmp_l->_v2 = pt11;
	pTmp_l->_v3 = pt8;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt7;
	pTmp_l->_v2 = pt8;
	pTmp_l->_v3 = pt11;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt7;
	pTmp_l->_v2 = pt12;
	pTmp_l->_v3 = pt8;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt12;
	pTmp_l->_v2 = pt7;
	pTmp_l->_v3 = pt1;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt2;
	pTmp_l->_v2 = pt1;
	pTmp_l->_v3 = pt7;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt2;
	pTmp_l->_v2 = pt7;
	pTmp_l->_v3 = pt11;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt1;
	pTmp_l->_v2 = pt10;
	pTmp_l->_v3 = pt12;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt12;
	pTmp_l->_v2 = pt10;
	pTmp_l->_v3 = pt3;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt10;
	pTmp_l->_v2 = pt6;
	pTmp_l->_v3 = pt3;
	vFaces.push_back(pTmp_l);

	pTmp_l = new Face;
	pTmp_l->_v1 = pt3;
	pTmp_l->_v2 = pt8;
	pTmp_l->_v3 = pt12;
	vFaces.push_back(pTmp_l);


	// Division des faces de l'icos�dre
	int i;
	for (i = 1 ; i < NbFaces ; i++) {
		Subdiviser();
	}

}

//______________________________________________________________________________
void Icosaedre :: Subdiviser () {
	Vecteur * pVecteurOA, * pVecteurOB, * pVecteurOC, * pVecteurOD, * pVecteurOE, * pVecteurOF;
	Vertex * ptA, * ptB, * ptC, * ptD, * ptE, * ptF;
	Face * pFace_l = new Face;
	vector <Face *> vFacesTmp;
	int i;

	for (i = 0 ; i < vFaces.size() ; i++) {
		vFacesTmp.push_back(vFaces[i]);
	}

	vFaces.clear();

	for (i = 0 ; i < vFacesTmp.size() ; i++) {
		pFace_l = vFacesTmp[i];

		// on copie les points de la face
		ptA = pFace_l->_v1;
		ptB = pFace_l->_v2;
		ptC = pFace_l->_v3;

		// on calcule les vecteurs entre l'origine et les points de la face
		pVecteurOA = new Vecteur(_Centre, ptA);
		pVecteurOB = new Vecteur(_Centre, ptB);
		pVecteurOC = new Vecteur(_Centre, ptC);

		// on additionne ces vecteurs 2 � 2
		pVecteurOD = pVecteurOA->operator +(pVecteurOB);
		pVecteurOE = pVecteurOB->operator +(pVecteurOC);
		pVecteurOF = pVecteurOA->operator +(pVecteurOC);
	
		// on normalise les vecteurs ainsi obtenus (on met leur longueur � 1)
		pVecteurOD->Normaliser();
		pVecteurOE->Normaliser();
		pVecteurOF->Normaliser();	
	
		// on ajuste la longueur au rayon
		pVecteurOD = pVecteurOD->operator * (Rayon);
		pVecteurOE = pVecteurOE->operator * (Rayon);
		pVecteurOF = pVecteurOF->operator * (Rayon);

		ptD = new Vertex(pVecteurOD->x, pVecteurOD->y, pVecteurOD->z);
		ptD->operator += (_Centre);
		vMesVertex.push_back(ptD);
		ptE = new Vertex(pVecteurOE->x, pVecteurOE->y, pVecteurOE->z);
		ptE->operator += (_Centre);
		vMesVertex.push_back(ptE);
		ptF = new Vertex(pVecteurOF->x, pVecteurOF->y, pVecteurOF->z);
		ptF->operator += (_Centre);
		vMesVertex.push_back(ptF);

 
		// on cr�e les faces obtenues � partir de ce d�coupage   
		pFace_l = new Face();
		pFace_l->_v1 = ptA;
		pFace_l->_v2 = ptD;
		pFace_l->_v3 = ptF;
		vFaces.push_back(pFace_l);

		pFace_l = new Face();
		pFace_l->_v1 = ptB;
		pFace_l->_v2 = ptE;
		pFace_l->_v3 = ptD;
		vFaces.push_back(pFace_l);

		pFace_l = new Face();
		pFace_l->_v1 = ptC;
		pFace_l->_v2 = ptF;
		pFace_l->_v3 = ptE;
		vFaces.push_back(pFace_l);

		pFace_l = new Face();
		pFace_l->_v1 = ptD;
		pFace_l->_v2 = ptE;
		pFace_l->_v3 = ptF;
		vFaces.push_back(pFace_l);
	}
}

//______________________________________________________________________________
void Icosaedre :: CreerListe(Type_Affichage typeAff_p)
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
void Icosaedre::Dessiner(Type_Affichage typeAff_p)
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
void Icosaedre::DessinerListe()
{   	
	glCallList(IdListe);
}
//______________________________________________________________________________

void Icosaedre::Modifier(double Rayon_p)
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

Figure* Icosaedre::Dupliquer()
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
