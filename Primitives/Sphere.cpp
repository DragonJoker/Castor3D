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

	if(NbFaces<3){
		return;
	}

	Arcs * ArcTmp_l = new Arcs();
	Vertex * CentreHaut;
	Vertex * CentreBas;

	Face * pFaceCourante_l = NULL;
	Arcs * pArcCourant_l = NULL;

	double angleRotation = (M_PI * 2) / NbFaces;

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

		for(dAlphaI = 0 ; dAlphaI < M_PI*2; dAlphaI += angleRotation){

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

		for ( int i = 0 ; i < vMesVertex.size()-TailleArc ; i+=TailleArc) {

			pFaceCourante_l = new Face();
			pArcCourant_l = new Arcs();			
			
			pArcCourant_l->vSuivants.push_back(vMesVertex[i+TailleArc]);
			pArcCourant_l->vSuivants.push_back(vMesVertex[i]);
			pArcCourant_l->vSuivants.push_back(CentreHaut);

			pFaceCourante_l->vArcs.push_back(pArcCourant_l);
			vFaces.push_back(pFaceCourante_l);

			pFaceCourante_l = new Face();
			pArcCourant_l = new Arcs();

			pArcCourant_l->vSuivants.push_back(CentreBas);
			pArcCourant_l->vSuivants.push_back(vMesVertex[i+TailleArc-1]);
			pArcCourant_l->vSuivants.push_back(vMesVertex[i+TailleArc+TailleArc-1]);

			pFaceCourante_l->vArcs.push_back(pArcCourant_l);
			vFaces.push_back(pFaceCourante_l);
		}
		
		pFaceCourante_l = new Face();
		pArcCourant_l = new Arcs();	
		
		pArcCourant_l->vSuivants.push_back(CentreHaut);
		pArcCourant_l->vSuivants.push_back(vMesVertex[0]);
		pArcCourant_l->vSuivants.push_back(vMesVertex[vMesVertex.size()-TailleArc]);

		pFaceCourante_l->vArcs.push_back(pArcCourant_l);
		vFaces.push_back(pFaceCourante_l);

		pFaceCourante_l = new Face();
		pArcCourant_l = new Arcs();	
		
		pArcCourant_l->vSuivants.push_back(vMesVertex[vMesVertex.size()-1]);
		pArcCourant_l->vSuivants.push_back(vMesVertex[TailleArc-1]);
		pArcCourant_l->vSuivants.push_back(CentreBas);

		pFaceCourante_l->vArcs.push_back(pArcCourant_l);
		vFaces.push_back(pFaceCourante_l);

	

	
		for (int j = 0 ; j < TailleArc-1 ; j++) {	

				
			for ( int i = 0 ; i < vMesVertex.size()-TailleArc ; i+=TailleArc) {

				pFaceCourante_l = new Face();
				pArcCourant_l = new Arcs();				
				
				pArcCourant_l->vSuivants.push_back(vMesVertex[j+i+1]);
				pArcCourant_l->vSuivants.push_back(vMesVertex[j+i]);
				pArcCourant_l->vSuivants.push_back(vMesVertex[j+i+TailleArc]);

				pFaceCourante_l->vArcs.push_back(pArcCourant_l);
				vFaces.push_back(pFaceCourante_l);

				
				pFaceCourante_l = new Face();
				pArcCourant_l = new Arcs();

				pArcCourant_l->vSuivants.push_back(vMesVertex[j+i+TailleArc+1]);
				pArcCourant_l->vSuivants.push_back(vMesVertex[i+j+1]);
				pArcCourant_l->vSuivants.push_back(vMesVertex[j+i+TailleArc]);

				pFaceCourante_l->vArcs.push_back(pArcCourant_l);
				vFaces.push_back(pFaceCourante_l);			

			}

			
			pFaceCourante_l = new Face();
			pArcCourant_l = new Arcs();				
				
			pArcCourant_l->vSuivants.push_back(vMesVertex[j]);
			pArcCourant_l->vSuivants.push_back(vMesVertex[j+1]);
			pArcCourant_l->vSuivants.push_back(vMesVertex[vMesVertex.size()-TailleArc+1+j]);

			pFaceCourante_l->vArcs.push_back(pArcCourant_l);
			vFaces.push_back(pFaceCourante_l);


			pFaceCourante_l = new Face();
			pArcCourant_l = new Arcs();

			pArcCourant_l->vSuivants.push_back(vMesVertex[vMesVertex.size()-TailleArc+1+j]);
			pArcCourant_l->vSuivants.push_back(vMesVertex[vMesVertex.size()-TailleArc+j]);
			pArcCourant_l->vSuivants.push_back(vMesVertex[j]);

			pFaceCourante_l->vArcs.push_back(pArcCourant_l);
			vFaces.push_back(pFaceCourante_l);			


		}

	}


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

	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
	}

	glEndList();
}  
//______________________________________________________________________________
void Sphere::Dessiner(Type_Affichage typeAff_p)
{     
	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
	}
} 

//______________________________________________________________________________
void Sphere::DessinerListe()
{   	
	glCallList(IdListe);
}
//______________________________________________________________________________

void Sphere::GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l)
{
	pCoordMin_l = _CoordMin; 
	pCoordMax_l = _CoordMax;
}

//______________________________________________________________________________

void Sphere::CalculerExtremes()
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

void Sphere::DessinerKillBox()
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
