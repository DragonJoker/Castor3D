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
	double angleRotation = (M_PI * 2) / NbFaces;

	Vertex * vTmp_l;
	Vertex * vTmp2_l;
	Vertex * vTmp3_l;

	Vertex * CentreHaut;
	Vertex * CentreBas;

	if(Centre_p!=NULL){

		for (double dAlphaI = 0 ; dAlphaI < M_PI * 2; dAlphaI += angleRotation) {
        
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
		for (double dAlphaI = 0 ; dAlphaI < M_PI * 2; dAlphaI += angleRotation) {
        
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

		int Cpt = NbFaces+NbFaces;

		Arcs * aBase_l;
		Arcs * aSommet_l;
		Arcs * aCote1_l;
		Arcs * aCote2_l;

		Face * fBase_l;
		Face * fSommet_l;
		Face * fCote1_l;
		Face * fCote2_l;
		int i;
		
		for(i=0; i<Cpt-2; i+=2){

			aBase_l = new Arcs();
			aSommet_l = new Arcs();
			aCote1_l = new Arcs();
			aCote2_l = new Arcs();

			fBase_l = new Face();
			fSommet_l = new Face();
			fCote1_l = new Face();
			fCote2_l = new Face();

			//Composition du bas
			aBase_l->vSuivants.push_back(vMesVertex[i]);		
			aBase_l->vSuivants.push_back(vMesVertex[i+2]);
			aBase_l->vSuivants.push_back(CentreBas);
			fBase_l->AddArcs(aBase_l);

			//Composition du dessus
			aSommet_l->vSuivants.push_back(vMesVertex[i+1]);
			aSommet_l->vSuivants.push_back(CentreHaut);
			aSommet_l->vSuivants.push_back(vMesVertex[i+3]);		
			fSommet_l->AddArcs(aSommet_l);

			//Composition du cote1
			aCote1_l->vSuivants.push_back(vMesVertex[i]);		
			aCote1_l->vSuivants.push_back(vMesVertex[i+1]);
			aCote1_l->vSuivants.push_back(vMesVertex[i+2]);
			fCote1_l->AddArcs(aCote1_l);

			//Composition du cote2		
			aCote2_l->vSuivants.push_back(vMesVertex[i+1]);
			aCote2_l->vSuivants.push_back(vMesVertex[i+3]);
			aCote2_l->vSuivants.push_back(vMesVertex[i+2]);
			fCote2_l->AddArcs(aCote2_l);

			vFaces.push_back(fSommet_l); 
			vFaces.push_back(fBase_l);
			vFaces.push_back(fCote1_l); 
			vFaces.push_back(fCote2_l);
				

		}
		
		aBase_l = new Arcs();
		aSommet_l = new Arcs();
		aCote1_l = new Arcs();
		aCote2_l = new Arcs();

		fBase_l = new Face();
		fSommet_l = new Face();
		fCote1_l = new Face();
		fCote2_l = new Face();	

		//Composition du bas
		aBase_l->vSuivants.push_back(vMesVertex[Cpt-2]);		
		aBase_l->vSuivants.push_back(vMesVertex[0]);
		aBase_l->vSuivants.push_back(CentreBas);
		fBase_l->AddArcs(aBase_l);

		//Composition du dessus
		aSommet_l->vSuivants.push_back(vMesVertex[Cpt-1]);
		aSommet_l->vSuivants.push_back(CentreHaut);
		aSommet_l->vSuivants.push_back(vMesVertex[1]);		
		fSommet_l->AddArcs(aSommet_l);

		//Composition du cote1
		aCote1_l->vSuivants.push_back(vMesVertex[i]);		
		aCote1_l->vSuivants.push_back(vMesVertex[i+1]);
		aCote1_l->vSuivants.push_back(vMesVertex[0]);
		fCote1_l->AddArcs(aCote1_l);

		//Composition du cote2		
		aCote2_l->vSuivants.push_back(vMesVertex[i+1]);
		aCote2_l->vSuivants.push_back(vMesVertex[1]);
		aCote2_l->vSuivants.push_back(vMesVertex[0]);
		fCote2_l->AddArcs(aCote2_l);

		vFaces.push_back(fSommet_l); 
		vFaces.push_back(fBase_l);
		vFaces.push_back(fCote1_l);
		vFaces.push_back(fCote2_l);

		aBase_l = NULL;
		aSommet_l = NULL;
		aCote1_l = NULL;
		aCote2_l = NULL;

		fBase_l = NULL;
		fSommet_l = NULL;
		fCote1_l = NULL;
		fCote2_l = NULL;

	}else{
		int Cpt = NbFaces+NbFaces;

		Arcs * aBase_l;
		Arcs * aSommet_l;
		Arcs * aCote1_l;
		Arcs * aCote2_l;

		Face * fBase_l;
		Face * fSommet_l;
		Face * fCote1_l;
		Face * fCote2_l;

		int i = 0;
		
		for(i=0; i<Cpt-2; i+=2){

			aBase_l = new Arcs();
			aSommet_l = new Arcs();
			aCote1_l = new Arcs();
			aCote2_l = new Arcs();

			fBase_l = new Face();
			fSommet_l = new Face();
			fCote1_l = new Face();
			fCote2_l = new Face();

			//Composition du bas !!			
			aBase_l->vSuivants.push_back(CentreBas);
			aBase_l->vSuivants.push_back(vMesVertex[i+2]);
			aBase_l->vSuivants.push_back(vMesVertex[i]);
			fBase_l->AddArcs(aBase_l);

			//Composition du dessus !!			
			aSommet_l->vSuivants.push_back(vMesVertex[i+3]);
			aSommet_l->vSuivants.push_back(CentreHaut);
			aSommet_l->vSuivants.push_back(vMesVertex[i+1]);
			fSommet_l->AddArcs(aSommet_l);

			//Composition du cote1 !!			
			aCote1_l->vSuivants.push_back(vMesVertex[i+2]);
			aCote1_l->vSuivants.push_back(vMesVertex[i+1]);
			aCote1_l->vSuivants.push_back(vMesVertex[i]);
			fCote1_l->AddArcs(aCote1_l);

			//Composition du cote2 !!			
			aCote2_l->vSuivants.push_back(vMesVertex[i+2]);
			aCote2_l->vSuivants.push_back(vMesVertex[i+3]);
			aCote2_l->vSuivants.push_back(vMesVertex[i+1]);
			fCote2_l->AddArcs(aCote2_l);

			vFaces.push_back(fSommet_l); 
			vFaces.push_back(fBase_l);
			vFaces.push_back(fCote1_l); 
			vFaces.push_back(fCote2_l);
				

		}
		
		aBase_l = new Arcs();
		aSommet_l = new Arcs();
		aCote1_l = new Arcs();
		aCote2_l = new Arcs();

		fBase_l = new Face();
		fSommet_l = new Face();
		fCote1_l = new Face();
		fCote2_l = new Face();	

		//Composition du bas !!		
		aBase_l->vSuivants.push_back(CentreBas);
		aBase_l->vSuivants.push_back(vMesVertex[0]);
		aBase_l->vSuivants.push_back(vMesVertex[Cpt-2]);
		fBase_l->AddArcs(aBase_l);

		//Composition du dessus !!		
		aSommet_l->vSuivants.push_back(vMesVertex[1]);
		aSommet_l->vSuivants.push_back(CentreHaut);
		aSommet_l->vSuivants.push_back(vMesVertex[Cpt-1]);
		fSommet_l->AddArcs(aSommet_l);

		//Composition du cote1 !!		
		aCote1_l->vSuivants.push_back(vMesVertex[0]);
		aCote1_l->vSuivants.push_back(vMesVertex[i+1]);
		aCote1_l->vSuivants.push_back(vMesVertex[i]);
		fCote1_l->AddArcs(aCote1_l);

		//Composition du cote2 !!		
		aCote2_l->vSuivants.push_back(vMesVertex[0]);
		aCote2_l->vSuivants.push_back(vMesVertex[1]);
		aCote2_l->vSuivants.push_back(vMesVertex[i+1]);
		fCote2_l->AddArcs(aCote2_l);

		vFaces.push_back(fSommet_l); 
		vFaces.push_back(fBase_l);
		vFaces.push_back(fCote1_l);
		vFaces.push_back(fCote2_l);

		aBase_l = NULL;
		aSommet_l = NULL;
		aCote1_l = NULL;
		aCote2_l = NULL;

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

	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
	}

	glEndList();
}  
//______________________________________________________________________________
void Cylindre::Dessiner(Type_Affichage typeAff_p)
{     
	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
	}
} 

//______________________________________________________________________________
void Cylindre::DessinerListe()
{   	
	glCallList(IdListe);
}
//______________________________________________________________________________

void Cylindre::GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l)
{
	pCoordMin_l = _CoordMin; 
	pCoordMax_l = _CoordMax;
}

//______________________________________________________________________________

void Cylindre::CalculerExtremes()
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

void Cylindre::DessinerKillBox()
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
