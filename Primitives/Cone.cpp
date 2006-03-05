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

	double angleRotation = (M_PI * 2) / NbFaces;

	Vertex * vTmp_l;
	Vertex * CentreBas; 
	Vertex * CentreHaut;
	int i = 0;
		
	if(Centre_p!=NULL){
		
		CentreBas = new Vertex();
		CentreHaut = new Vertex(0.0f, Hauteur, 0.0f);

		//etape 1 : On calcul  les coordonn�es des points
		for (double dAlphaI = 0 ; dAlphaI < M_PI * 2; dAlphaI += angleRotation) {
        
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
			Face * fBase_l;
			Arcs * aBase_l;
			Face * fCote_l;
			Arcs * aCote_l;

			for(i=0; i< NbFaces-1; i++){
				aBase_l = new Arcs();
				aCote_l = new Arcs();

				fBase_l = new Face();
				fCote_l = new Face();

				
				//Composition du bas
				aBase_l->vSuivants.push_back(vMesVertex[i]);	
				aBase_l->vSuivants.push_back(vMesVertex[i+1]);
				aBase_l->vSuivants.push_back(CentreBas);
				fBase_l->AddArcs(aBase_l);

				//Composition du cot�						
				aCote_l->vSuivants.push_back(vMesVertex[i+1]);
				aCote_l->vSuivants.push_back(vMesVertex[i]);
				aCote_l->vSuivants.push_back(CentreHaut);
				fCote_l->AddArcs(aCote_l);

				vFaces.push_back(fBase_l);
				vFaces.push_back(fCote_l);

			}

			aBase_l = new Arcs();
			aCote_l = new Arcs();

			fBase_l = new Face();			
			fCote_l = new Face();


			//Composition du bas
			aBase_l->vSuivants.push_back(vMesVertex[NbFaces-1]);		
			aBase_l->vSuivants.push_back(vMesVertex[0]);
			aBase_l->vSuivants.push_back(CentreBas);
			fBase_l->AddArcs(aBase_l);
					
			//Composition du cot�						
			aCote_l->vSuivants.push_back(vMesVertex[0]);
			aCote_l->vSuivants.push_back(vMesVertex[i]);
			aCote_l->vSuivants.push_back(CentreHaut);
			fCote_l->AddArcs(aCote_l);

			vFaces.push_back(fBase_l);
			vFaces.push_back(fCote_l);

		}else{

			Face * fBase_l;
			Arcs * aBase_l;
			Face * fCote_l;
			Arcs * aCote_l;

			for(i=0; i< NbFaces-1; i++){
				aBase_l = new Arcs();
				aCote_l = new Arcs();

				fBase_l = new Face();
				fCote_l = new Face();

				
				//Composition du bas !!				
				aBase_l->vSuivants.push_back(CentreBas);
				aBase_l->vSuivants.push_back(vMesVertex[i+1]);
				aBase_l->vSuivants.push_back(vMesVertex[i]);
				fBase_l->AddArcs(aBase_l);

				//Composition du cot� !!				
				aCote_l->vSuivants.push_back(CentreHaut);
				aCote_l->vSuivants.push_back(vMesVertex[i]);
				aCote_l->vSuivants.push_back(vMesVertex[i+1]);
				fCote_l->AddArcs(aCote_l);

				vFaces.push_back(fBase_l);
				vFaces.push_back(fCote_l);

			}

			aBase_l = new Arcs();
			aCote_l = new Arcs();

			fBase_l = new Face();			
			fCote_l = new Face();


			//Composition du bas !!			
			aBase_l->vSuivants.push_back(CentreBas);
			aBase_l->vSuivants.push_back(vMesVertex[0]);
			aBase_l->vSuivants.push_back(vMesVertex[NbFaces-1]);
			fBase_l->AddArcs(aBase_l);
					
			//Composition du cot� !!				
			aCote_l->vSuivants.push_back(CentreHaut);
			aCote_l->vSuivants.push_back(vMesVertex[i]);
			aCote_l->vSuivants.push_back(vMesVertex[0]);
			fCote_l->AddArcs(aCote_l);

			vFaces.push_back(fBase_l);
			vFaces.push_back(fCote_l);


		}


	}else{

		CentreBas = new Vertex();
		CentreHaut = new Vertex(0.0f, Hauteur, 0.0f);

		//etape 1 : On calcul  les coordonn�es des points
		for (double dAlphaI = 0 ; dAlphaI < M_PI * 2; dAlphaI += angleRotation) {
        
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
			Face * fBase_l;
			Arcs * aBase_l;
			Face * fCote_l;
			Arcs * aCote_l;

			for(i=0; i< NbFaces-1; i++){
				aBase_l = new Arcs();
				aCote_l = new Arcs();

				fBase_l = new Face();
				fCote_l = new Face();

				
				//Composition du bas
				aBase_l->vSuivants.push_back(vMesVertex[i]);		
				aBase_l->vSuivants.push_back(vMesVertex[i+1]);
				aBase_l->vSuivants.push_back(CentreBas);
				fBase_l->AddArcs(aBase_l);

				//Composition du cot�						
				aCote_l->vSuivants.push_back(vMesVertex[i+1]);
				aCote_l->vSuivants.push_back(vMesVertex[i]);
				aCote_l->vSuivants.push_back(CentreHaut);
				fCote_l->AddArcs(aCote_l);

				vFaces.push_back(fBase_l);
				vFaces.push_back(fCote_l);

			}

			aBase_l = new Arcs();
			aCote_l = new Arcs();

			fBase_l = new Face();			
			fCote_l = new Face();


			//Composition du bas
			aBase_l->vSuivants.push_back(vMesVertex[NbFaces-1]);		
			aBase_l->vSuivants.push_back(vMesVertex[0]);
			aBase_l->vSuivants.push_back(CentreBas);
			fBase_l->AddArcs(aBase_l);
					
			//Composition du cot�						
			aCote_l->vSuivants.push_back(vMesVertex[0]);
			aCote_l->vSuivants.push_back(vMesVertex[i]);
			aCote_l->vSuivants.push_back(CentreHaut);
			fCote_l->AddArcs(aCote_l);

			vFaces.push_back(fBase_l);
			vFaces.push_back(fCote_l);
		}else{

			Face * fBase_l;
			Arcs * aBase_l;
			Face * fCote_l;
			Arcs * aCote_l;

			for(i=0; i< NbFaces-1; i++){
				aBase_l = new Arcs();
				aCote_l = new Arcs();

				fBase_l = new Face();
				fCote_l = new Face();

				
				//Composition du bas !!				
				aBase_l->vSuivants.push_back(CentreBas);
				aBase_l->vSuivants.push_back(vMesVertex[i+1]);
				aBase_l->vSuivants.push_back(vMesVertex[i]);
				fBase_l->AddArcs(aBase_l);

				//Composition du cot� !!				
				aCote_l->vSuivants.push_back(CentreHaut);
				aCote_l->vSuivants.push_back(vMesVertex[i]);
				aCote_l->vSuivants.push_back(vMesVertex[i+1]);
				fCote_l->AddArcs(aCote_l);

				vFaces.push_back(fBase_l);
				vFaces.push_back(fCote_l);

			}

			aBase_l = new Arcs();
			aCote_l = new Arcs();

			fBase_l = new Face();			
			fCote_l = new Face();


			//Composition du bas !!			
			aBase_l->vSuivants.push_back(CentreBas);
			aBase_l->vSuivants.push_back(vMesVertex[0]);
			aBase_l->vSuivants.push_back(vMesVertex[NbFaces-1]);
			fBase_l->AddArcs(aBase_l);
					
			//Composition du cot� !!			
			aCote_l->vSuivants.push_back(CentreHaut);
			aCote_l->vSuivants.push_back(vMesVertex[i]);
			aCote_l->vSuivants.push_back(vMesVertex[0]);
			fCote_l->AddArcs(aCote_l);

			vFaces.push_back(fBase_l);
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

	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
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

	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
	}

	glEndList();
}  

//______________________________________________________________________________
void Cone::Dessiner (Type_Affichage typeAff_p)
{                                 
	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
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

void Cone::GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l)
{
	pCoordMin_l = _CoordMin; 
	pCoordMax_l = _CoordMax;
}

//______________________________________________________________________________

void Cone::CalculerExtremes()
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

void Cone::DessinerKillBox()
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
