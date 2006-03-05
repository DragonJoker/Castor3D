/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Pave.h - Pave.cpp

 Desc:   Classe gérant la primitive de Pavé (cube ou parallélépipède rectangle)

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

	
    //Calcul des coordonnées des 8 sommets du pavé

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
	Arcs * pArc_l = NULL;

	if(CptNegatif!=1 && CptNegatif!=3){

		//CONSTRUCTION FACES /!\ pour openGL le z est inverse

		// Faces du bas
		pFace_l = new Face();
		pArc_l = new Arcs();    
		pArc_l->vSuivants.push_back(vMesVertex[1]); 
		pArc_l->vSuivants.push_back(vMesVertex[3]); 
		pArc_l->vSuivants.push_back(vMesVertex[0]); 
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);
    
		pArc_l = new Arcs();
		pFace_l = new Face(); 
		pArc_l->vSuivants.push_back(vMesVertex[2]); 
		pArc_l->vSuivants.push_back(vMesVertex[3]); 
		pArc_l->vSuivants.push_back(vMesVertex[1]); 
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);


		// Face de devant
		pArc_l = new Arcs();
		pFace_l = new Face();    
		pArc_l->vSuivants.push_back(vMesVertex[6]);
		pArc_l->vSuivants.push_back(vMesVertex[1]);
		pArc_l->vSuivants.push_back(vMesVertex[0]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);


		pArc_l = new Arcs();
		pFace_l = new Face();	
		pArc_l->vSuivants.push_back(vMesVertex[0]);  
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pArc_l->vSuivants.push_back(vMesVertex[6]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);
		

		// Faces de droite
		pArc_l = new Arcs();
		pFace_l = new Face();    
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pArc_l->vSuivants.push_back(vMesVertex[2]);
		pArc_l->vSuivants.push_back(vMesVertex[1]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);


		pArc_l = new Arcs();
		pFace_l = new Face();	
		pArc_l->vSuivants.push_back(vMesVertex[1]);
		pArc_l->vSuivants.push_back(vMesVertex[6]);
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);
  		
		// Face du haut
		pArc_l = new Arcs();
		pFace_l = new Face();    
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pArc_l->vSuivants.push_back(vMesVertex[6]);
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

		pArc_l = new Arcs();
		pFace_l = new Face();
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pArc_l->vSuivants.push_back(vMesVertex[4]);
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

		// Face de gauche
		pArc_l = new Arcs();
		pFace_l = new Face();    
		pArc_l->vSuivants.push_back(vMesVertex[3]);
		pArc_l->vSuivants.push_back(vMesVertex[4]);
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

		pArc_l = new Arcs();
		pFace_l = new Face();    
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pArc_l->vSuivants.push_back(vMesVertex[0]);
		pArc_l->vSuivants.push_back(vMesVertex[3]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

		// Face de derrière
		pArc_l = new Arcs();
		pFace_l = new Face();    
		pArc_l->vSuivants.push_back(vMesVertex[2]);
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pArc_l->vSuivants.push_back(vMesVertex[4]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);
		
		pArc_l = new Arcs();
		pFace_l = new Face();	
		pArc_l->vSuivants.push_back(vMesVertex[4]);
		pArc_l->vSuivants.push_back(vMesVertex[3]);
		pArc_l->vSuivants.push_back(vMesVertex[2]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

	}else{

		//CONSTRUCTION FACES /!\ pour openGL le z est inverse

		// Faces du bas !!
		pFace_l = new Face();
		pArc_l = new Arcs();  		
		pArc_l->vSuivants.push_back(vMesVertex[0]); 
		pArc_l->vSuivants.push_back(vMesVertex[3]); 
		pArc_l->vSuivants.push_back(vMesVertex[1]); 
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);
    
		pArc_l = new Arcs();
		pFace_l = new Face();		 
		pArc_l->vSuivants.push_back(vMesVertex[1]);
		pArc_l->vSuivants.push_back(vMesVertex[3]);
		pArc_l->vSuivants.push_back(vMesVertex[2]); 
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);


		// Face de devant !!
		pArc_l = new Arcs();
		pFace_l = new Face(); 		
		pArc_l->vSuivants.push_back(vMesVertex[0]);
		pArc_l->vSuivants.push_back(vMesVertex[1]);
		pArc_l->vSuivants.push_back(vMesVertex[6]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);


		pArc_l = new Arcs();
		pFace_l = new Face();		
		pArc_l->vSuivants.push_back(vMesVertex[6]);
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pArc_l->vSuivants.push_back(vMesVertex[0]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);
		

		// Faces de droite !!
		pArc_l = new Arcs();
		pFace_l = new Face(); 		
		pArc_l->vSuivants.push_back(vMesVertex[1]);
		pArc_l->vSuivants.push_back(vMesVertex[2]);
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);


		pArc_l = new Arcs();
		pFace_l = new Face();		
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pArc_l->vSuivants.push_back(vMesVertex[6]);
		pArc_l->vSuivants.push_back(vMesVertex[1]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);
  		
		// Face du haut !!
		pArc_l = new Arcs();
		pFace_l = new Face(); 		
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pArc_l->vSuivants.push_back(vMesVertex[6]);
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

		pArc_l = new Arcs();
		pFace_l = new Face();		
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pArc_l->vSuivants.push_back(vMesVertex[4]);
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

		// Face de gauche !!
		pArc_l = new Arcs();
		pFace_l = new Face();		
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pArc_l->vSuivants.push_back(vMesVertex[4]);
		pArc_l->vSuivants.push_back(vMesVertex[3]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

		pArc_l = new Arcs();
		pFace_l = new Face();  		
		pArc_l->vSuivants.push_back(vMesVertex[3]);
		pArc_l->vSuivants.push_back(vMesVertex[0]);
		pArc_l->vSuivants.push_back(vMesVertex[5]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

		// Face de derrière !!
		pArc_l = new Arcs();
		pFace_l = new Face(); 		
		pArc_l->vSuivants.push_back(vMesVertex[4]);
		pArc_l->vSuivants.push_back(vMesVertex[7]);
		pArc_l->vSuivants.push_back(vMesVertex[2]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);
		
		pArc_l = new Arcs();
		pFace_l = new Face();		
		pArc_l->vSuivants.push_back(vMesVertex[2]);
		pArc_l->vSuivants.push_back(vMesVertex[3]);
		pArc_l->vSuivants.push_back(vMesVertex[4]);
		pFace_l->AddArcs(pArc_l);
		vFaces.push_back(pFace_l);

	}

    pArc_l = NULL;
    pFace_l = NULL;




 
}
//______________________________________________________________________________
void Pave::Dessiner(Type_Affichage typeAff_p)
{     
	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
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

	for(int i=0; i<vFaces.size();i++){
		vFaces[i]->Dessiner(typeAff_p);
	}

	glEndList();
}        
//______________________________________________________________________________

void Pave :: GetExtremes(Vertex* &pCoordMin_l, Vertex* &pCoordMax_l)
{
	pCoordMin_l = _CoordMin; 
	pCoordMax_l = _CoordMax;
}

//______________________________________________________________________________

void Pave :: CalculerExtremes()
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



//	vMesVertex.erase(vMesVertex.begin(), vMesVertex.end());
	GenererPoints (_Centre);

}
//______________________________________________________________________________

void Pave::DessinerKillBox()
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


	//les arretes latérales
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
