#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <stdlib.h> 
#include "Math.h"     

#include "resource.h"
#include "./src/global.h"
#include "./Geometrie/Bases/Constantes.h"

//_____________________________________________________________________________________________
enum Type_Action
{
 modifier_f,
 selectionner_f,
 creer_f
};
//_____________________________________________________________________________________________
enum Type_Selection
{
 objet_f,
 vertex_f,
 aucune_f,
 tous_f,
 dupliquer_f
};
//_____________________________________________________________________________________________
enum Type_Modification
{
 translation_f,
 rotation_f
};
//_____________________________________________________________________________________________
typedef struct {
	float y;
	float x;
} point2D;

//_____________________________________________________________________________________________

#define NONE       0
#define VUE_FACE   1
#define VUE_COTE   2
#define VUE_DESSUS 3
#define VUE_3D     4

//_____________________________________________________________________________________________
HINSTANCE  _Instance;
HWND _HwndFenFils;
HWND _HwndFen;
//_____________________________________________________________________________________________
char NomClassPrincipal[] = "ClassPrincipal";
char NomClassFils[] = "ClassFils";


float g_fCubeRotationX, g_fCubeRotationY;      
float g_fZoomDessus=25, g_fZoomFace=25, g_fZoomCote=25;

bool g_bMouseDrag = 0;                         // Mouse capture flag
int g_iMouseLastX;                             // Holds previous X position of mouse
int g_iMouseLastY;                             // Holds previous Y position of mouse
int g_iMouseDeltaX;                            // Holds the mouse increment in X since LastX
int g_iMouseDeltaY;                            // Holds the mouse increment in Y since LastY

int g_translationX;
int g_translationY;
int g_translationZ;

int PosMenuPrimitive =3;

int g_nbFacesCone = 80;
int g_nbFacesCylindre = 40;
int g_nbFacesSphere = 50;
int g_nbSubdivisionPlanX = 2;
int g_nbSubdivisionPlanY = 2;
int g_nbFacesIco = 4;

int g_fenSelection = NONE;

float g_reculCam = -5;

//Coordonnées des point de la souris pour les selection
point2D g_pointDebut;
point2D g_pointFin;

//Action qui sont actuellement en cours 
Type_Action ActionActu = creer_f;
Type_Figure FigureActu = pave_f;
Type_Selection SelectionActu = objet_f;
Type_Modification ModificationActu = translation_f;

//Id de l'element selectionner en cours
UINT g_lastChecked = ID_CUBE;

//Variable des elements globals
CGlobal * objets = new CGlobal;


#define ID_MDI_CLIENT      4999             
#define ID_MDI_PREMIER_FENETRE  50000   

typedef struct FILS                       
{
	int type;
    HDC hDC;                                
    HGLRC hRC;                              
    HWND hWnd;                             
} FILS;


int nbreFils = 0;
int maxFils = 4;
RECT _rectFils;
FILS* tabFils = new FILS[maxFils];         // Create the CHILD structures

///////////////////////////////////////////////////////////////////////////////////////////

//Initialisation de la librairie graphique
void InitGL()
{

    glCullFace(GL_BACK);
    glClearColor(0.7, 0.8, 1.0, 0.4);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);

    float MatAmbient[] = {0.8f, 0.8f, 0.8f, 1.0f};
    float MatDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    float MatSpecular[] = {0.9f, 0.9f, 0.9f, 1.0f};
    float MatEmmision[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float MatShininess[] = {100.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MatDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, MatEmmision);
    glMaterialfv(GL_FRONT, GL_SHININESS, MatShininess);

    float LightAmbient[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
    float LightPosition[] = {0.0, 35.0, 0.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	   
   // Culling	
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
}

///////////////////////////////////////////////////////////////////////////////////////////

void SetProjection(int iWidth, int iHeight, int Type)
{
    if(iHeight == 0){
        iHeight = 1; //on evite la division par 0
    }

    glViewport(0, 0, iWidth, iHeight);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	switch(Type){

		case VUE_3D:
			 
			gluPerspective(45.0,(float)iWidth/(float)iHeight,0.1,2000.0);
			gluLookAt(g_reculCam,0,0,0,0,0,0,1,0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		break;
	
		case VUE_FACE:
			glOrtho(-(iWidth/2) / g_fZoomFace, (iWidth/2) / g_fZoomFace, -(iHeight/2)/ g_fZoomFace, (iHeight/2) / g_fZoomFace, -2000, 2000);
		break;

		case VUE_DESSUS:
			glOrtho(-(iWidth/2) / g_fZoomDessus, (iWidth/2) / g_fZoomDessus, -(iHeight/2) / g_fZoomDessus, (iHeight/2) / g_fZoomDessus, -2000, 2000);
		break;

		case VUE_COTE:
			glOrtho(-(iWidth/2) / g_fZoomCote, (iWidth/2) / g_fZoomCote, -(iHeight/2) / g_fZoomCote, (iHeight/2) / g_fZoomCote, -2000, 2000);
		break;
	}

}
///////////////////////////////////////////////////////////////////////////////////////////

void DessinerGrille(int TypeVue){

	glColor3f(0.7,0.7,0.7);
	int i;

	switch(TypeVue){
		case VUE_FACE:
			for(i=-1000; i<1001;i++){		
					glBegin(GL_LINES);
						glVertex3i(-1000, i, -2000);
						glVertex3i(1000, i, -2000);				
					glEnd();
					
					glBegin(GL_LINES);
						glVertex3i(i, -1000, -2000);
						glVertex3i(i, 1000, -2000);				
					glEnd();
			}
			break;
		case VUE_COTE:
			for(i=-1000; i<1001;i++){		
					glBegin(GL_LINES);
						glVertex3i(2000, i, -1000);
						glVertex3i(2000, i, 1000);				
					glEnd();
					
					glBegin(GL_LINES);
						glVertex3i(2000, -1000, i);
						glVertex3i(2000, 1000, i);				
					glEnd();
			}
			break;
		case VUE_DESSUS:
			for(i=-1000; i<1001;i++){		
					glBegin(GL_LINES);
						glVertex3i(-1000, 2000, i);
						glVertex3i(1000, 2000, i );				
					glEnd();
					
					
					glBegin(GL_LINES);
						glVertex3i(i, 2000, -1000);
						glVertex3i(i, 2000, 1000);				
					glEnd();
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

void DessinerRepere(){

	
    float coulSpecular[] = {1.0, 0.0, 0.0};
	float coulSpecular2[] = {0.0,1.0,0.0};
	float coulSpecular3[] = {0.0,0.0,1.0};
	float coulSpecular4[] = {1.0,1.0,1.0};
    float coulShininess[] = {50.0};

	//Axe des X en rouge 
	glBegin(GL_LINES);

    glMaterialfv(GL_FRONT, GL_AMBIENT, coulSpecular);
    glMaterialfv(GL_FRONT, GL_SPECULAR, coulSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, coulShininess);
	glColor3f(1.0,0.0,0.0);
	glVertex3f(0.0, 0.0,  0.0);
	glVertex3f(1.0, 0.0,  0.0);
	glEnd();

	//Axe des Y en vert
	glBegin(GL_LINES);
	glColor3f(0.0,1.0,0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, coulSpecular2);
    glMaterialfv(GL_FRONT, GL_SPECULAR, coulSpecular2);
    glMaterialfv(GL_FRONT, GL_SHININESS, coulShininess);
	glVertex3f(0.0, 0.0,  0.0);
	glVertex3f( 0.0,1.0,  0.0);
	glEnd();

	//Axe des Z en bleu
	glBegin(GL_LINES);
	glColor3f(0.0,0.0,1.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, coulSpecular3);
    glMaterialfv(GL_FRONT, GL_SPECULAR, coulSpecular3);
    glMaterialfv(GL_FRONT, GL_SHININESS, coulShininess);
	glVertex3f(0.0, 0.0,  0.0);
	glVertex3f( 0.0,  0.0, 1.0);
	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT, coulSpecular4);
    glMaterialfv(GL_FRONT, GL_SPECULAR, coulSpecular4);
	glColor3f(1.0,1.0,1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////

void DrawSelectionFace()
{
    glColor3f(0.6,0.6,0.6);
	glBegin(GL_LINE_LOOP);
	glVertex3f(g_pointDebut.x, g_pointDebut.y,  0.0);
	glVertex3f(g_pointFin.x,   g_pointDebut.y,  0.0);
	glVertex3f(g_pointFin.x,   g_pointFin.y,    0.0);
	glVertex3f(g_pointDebut.x, g_pointFin.y,    0.0);	
	glEnd();
}

///////////////////////////////////////////////////////////////////////////////////////////

void DrawSelectionDessus()
{
    glColor3f(0.6,0.6,0.6);
	glBegin(GL_LINE_LOOP);
	glVertex3f(g_pointDebut.x, 0.0,g_pointDebut.y);
	glVertex3f(g_pointFin.x,   0.0,g_pointDebut.y);
	glVertex3f(g_pointFin.x,   0.0,g_pointFin.y  );
	glVertex3f(g_pointDebut.x, 0.0,g_pointFin.y  );	
	glEnd();
}
///////////////////////////////////////////////////////////////////////////////////////////

void DrawSelectionCote()
{
    glColor3f(0.6,0.6,0.6);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0,g_pointDebut.y, g_pointDebut.x );
	glVertex3f(0.0,g_pointDebut.y, g_pointFin.x   );
	glVertex3f(0.0,g_pointFin.y  , g_pointFin.x    );
	glVertex3f(0.0,g_pointFin.y  , g_pointDebut.x   );	
	glEnd();
}
///////////////////////////////////////////////////////////////////////////////////////////

void DrawGLScene(int Type)
{
	  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

  	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	switch(Type){
		case VUE_FACE:
			glPushMatrix();	
			if(g_fenSelection == VUE_FACE && ActionActu==selectionner_f){
				DrawSelectionFace();
			}			
			DessinerRepere();
			DessinerGrille(VUE_FACE);
		    objets->Dessiner(lignesfermees);
			glPopMatrix();
			break;
		case VUE_COTE:
			glPushMatrix();
			glRotatef(90,0.0,1.0,0.0);
			if(g_fenSelection == VUE_COTE && ActionActu==selectionner_f){
				DrawSelectionCote();
			}
			DessinerRepere();
			DessinerGrille(VUE_COTE);
			objets->Dessiner(lignesfermees);
			glPopMatrix();
			break;
		case VUE_DESSUS:
			glPushMatrix();
			glRotatef(-90,1.0,0.0,0.0);	
			if(g_fenSelection == VUE_DESSUS && ActionActu==selectionner_f){
				DrawSelectionDessus();
			}
			DessinerRepere();
			DessinerGrille(VUE_DESSUS);
			objets->Dessiner(lignesfermees);
			glPopMatrix();
			break;
		case VUE_3D:
			glPushMatrix();

			glEnable(GL_LIGHTING);

			glRotatef(g_fCubeRotationY,0.0,0.0,1.0);
			glRotatef(-g_fCubeRotationX,0.0,1.0,0.0);
			

			DessinerRepere();
			objets->Dessiner(facesTriangle);			

			glDisable(GL_LIGHTING);
			glPopMatrix();			
			break;
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

}

///////////////////////////////////////////////////////////////////////////////////////////

int GetType(HWND hWnd){
	for(int i=0; i< nbreFils;i++){

		if(tabFils[i].hWnd == hWnd){
			return tabFils[i].type; 
		}
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////

float CalculerCoor(int MouseCoord, int DimScreen, float Diviseur){

	float Taille = DimScreen / 2.0f;
	Taille = MouseCoord - Taille;
	Taille /= Diviseur; 
	return Taille;
}

///////////////////////////////////////////////////////////////////////////////////////////

float CalculerAngle(point2D point_p)
{
	float angle_l = 0.0;

	if(0 < point_p.y){
		if(0 <point_p.x){
			angle_l = atan(point_p.y / point_p.x);			
		}else if(point_p.x < 0){
			angle_l = -atan(point_p.x / point_p.y)+M_PI_PAR_2;	
		}else{
			angle_l = M_PI_PAR_2;
		}
	}else if(point_p.y < 0){
		if(0 < point_p.x){
			angle_l = -atan(point_p.x / point_p.y)+M_PI+M_PI_PAR_2;	
		}else if(point_p.x < 0){
			angle_l = atan(point_p.y/ point_p.x)+M_PI;
		}else{
			angle_l = M_PI+M_PI_PAR_2;
		}		

	}else{
		if(0 < point_p.x){
			angle_l = 0.0;	
		}else if(point_p.x < 0){
			angle_l = M_PI;
		}else{
			angle_l = 0.0;
		}
	}

	return angle_l;
}

///////////////////////////////////////////////////////////////////////////////////////////

//fonction de creation d'une fenetre client
void CreerFenFils(int TypeVue)
{
	HWND HwndFenFils_l;

	if(nbreFils>=maxFils){
		return;
	}

	nbreFils++;

	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));

	char *nomFenetre;


	switch(TypeVue){

	case VUE_FACE:
		nomFenetre="Vue de Face";
	break;

	case VUE_COTE:
		nomFenetre="Vue de Coté (droit)";
	break;

	case VUE_DESSUS:
		nomFenetre="Vue de Dessus";
	break;

	case VUE_3D:
		nomFenetre="Vue 3D";
	break;

	}

	HwndFenFils_l = CreateWindowEx(
		                    WS_EX_MDICHILD,
						    NomClassFils,
						    nomFenetre,
						    WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
						    CW_USEDEFAULT,
						    CW_USEDEFAULT,
						    CW_USEDEFAULT,
						    CW_USEDEFAULT,
						    _HwndFenFils,
						    NULL,
						    _Instance,
						    &cs
						    );

	
	if(!HwndFenFils_l){
		MessageBox(NULL, "Impossible de creer la fenetre", "Erreur", MB_ICONERROR | MB_OK);
	}

	//On place la fenetre dans le tableau de fils
	tabFils[nbreFils - 1].hWnd = HwndFenFils_l;         
    tabFils[nbreFils - 1].hDC = GetDC(HwndFenFils_l);   
	tabFils[nbreFils - 1].type = TypeVue;


	//redefinition du format de pixel
	PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int iFormat = ChoosePixelFormat(tabFils[nbreFils - 1].hDC, &pfd);
    SetPixelFormat(tabFils[nbreFils - 1].hDC, iFormat, &pfd);              

    tabFils[nbreFils - 1].hRC = wglCreateContext(tabFils[nbreFils - 1].hDC);  
    wglMakeCurrent(tabFils[nbreFils - 1].hDC, tabFils[nbreFils - 1].hRC);   
	InitGL();		  

}

///////////////////////////////////////////////////////////////////////////////////////////

void LButtonDownOnFace()
{ 
	//primitives temporaires
	Pave* p     = NULL;
	Cylindre* c = NULL;
	Cone* co    = NULL;
	Sphere* sp  = NULL;
	Plan* pl    = NULL;
	Icosaedre* ico = NULL;
	Terrain* ter = NULL;


	//On verifie que le clique de la souris s'effectue dans la fenetre
	if(0<=g_iMouseLastX && g_iMouseLastX<=_rectFils.right &&  0<=g_iMouseLastY && g_iMouseLastY<=_rectFils.bottom	){

		//On calcule la veritable coordonnée de la souris (coord 3D)
		float nbre1 = CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomFace);
		float nbre2 = CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomFace);		
	
		switch(ActionActu)
		{		
			case creer_f:
			{
				switch(FigureActu){

					case pave_f:
						p = new Pave(1.0, 1.0, 1.0, new Vertex(nbre1,-nbre2,0.0f) );
						objets->GeoActu =((Figure * )(p));
						break;
										
					case cylindre_f:
						c = new Cylindre(1.0, 1.0, g_nbFacesCylindre,new Vertex(nbre1,-nbre2,0.0f));
						objets->GeoActu = ((Figure * )(c));
						break;

					case cone_f:
						co = new Cone(1.0, 1.0, g_nbFacesCone,new Vertex(nbre1,-nbre2,0.0f));
						objets->GeoActu = ((Figure * )(co));
						break;

					case sphere_f:
						sp = new Sphere(1.0, g_nbFacesSphere,new Vertex(nbre1,-nbre2,0.0f));
						objets->GeoActu = ((Figure * )(sp));
						break;

					case plan_f:
						pl = new Plan(1.0, 1.0, g_nbSubdivisionPlanX,g_nbSubdivisionPlanY,new Vertex(nbre1,-nbre2,0.0f));
						objets->GeoActu = ((Figure * )(pl));
						break;

					case icosaedre_f:
						ico = new Icosaedre(1.0, g_nbFacesIco,new Vertex(nbre1,-nbre2,0.0f));
						objets->GeoActu = ((Figure * )(ico));
						break;
					
					case terrain_f:
                        ter = new Terrain("map.map");
						objets->GeoActu = ((Figure * )(ter));
					//	Terrain::GenererPoints(std::string NomFichier)
						break;

				}
 
			}break;
			
			case selectionner_f:
			{
				g_fenSelection = VUE_FACE;
				g_pointDebut.x =  nbre1;
				g_pointDebut.y = -nbre2;
				g_pointFin.x   =  nbre1; 
				g_pointFin.y   = -nbre2;
			}break;

			case modifier_f:
			{
				g_fenSelection = VUE_FACE;
				g_pointDebut.x =  nbre1;
				g_pointDebut.y = -nbre2;
				g_pointFin.x   =  nbre1; 
				g_pointFin.y   = -nbre2;
			}break;
		}

		g_translationX = 0;
		g_translationY = 0;

	} //if(clique zone ecran)
	
}
///////////////////////////////////////////////////////////////////////////////////////////

void LButtonDownOnCote()
{
	
	Pave* p     = NULL;
	Cylindre* c = NULL;
	Cone* co    = NULL;
	Sphere* sp  = NULL;
	Plan* pl    = NULL;
	Icosaedre* ico = NULL;

	//On verifie que le clique de la souris s'effectue dans la fenetre
	if(0<=g_iMouseLastX && g_iMouseLastX<=_rectFils.right &&  0<=g_iMouseLastY && g_iMouseLastY<=_rectFils.bottom	)
	{
		//On calcule la veritable coordonnée de la souris (coord 3D)
		float nbre1 = CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomCote);
		float nbre2 = CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomCote);
		
		switch(ActionActu)
		{
			case creer_f:
			{
				switch(FigureActu){

					case pave_f:
						p = new Pave(1.0, 1.0, 1.0, new Vertex(0.0f,-nbre2,nbre1) );
						objets->GeoActu =((Figure * )(p));
						break;
											
					case cylindre_f:
						c = new Cylindre(1.0, 1.0, g_nbFacesCylindre, new Vertex(0.0f,-nbre2,nbre1) );
						objets->GeoActu = ((Figure * )(c));
						break;

					case cone_f:
						co = new Cone(1.0, 1.0, g_nbFacesCone,new Vertex(0.0f,-nbre2,nbre1));
						objets->GeoActu = ((Figure * )(co));
						break;

					case sphere_f:
						sp = new Sphere(1.0, g_nbFacesSphere,new Vertex(0.0f,-nbre2,nbre1));
						objets->GeoActu = ((Figure * )(sp));
						break;

					case plan_f:
						pl = new Plan(1.0, 1.0, g_nbSubdivisionPlanX, g_nbSubdivisionPlanY,new Vertex(0.0f,-nbre2,nbre1));
						objets->GeoActu = ((Figure * )(pl));
						break;

					case icosaedre_f:
						ico = new Icosaedre(1.0, g_nbFacesIco,new Vertex(0.0f,-nbre2,nbre1));
						objets->GeoActu = ((Figure * )(ico));
						break;
				}

			}break;
			
			case selectionner_f:
			{
				g_fenSelection = VUE_COTE;
				g_pointDebut.x =  nbre1;
				g_pointDebut.y = -nbre2;
				g_pointFin.x   =  nbre1; 
				g_pointFin.y   = -nbre2;
			}break;

			case modifier_f:
			{
				g_fenSelection = VUE_COTE;
				g_pointDebut.x =  nbre1;
				g_pointDebut.y = -nbre2;
				g_pointFin.x   =  nbre1; 
				g_pointFin.y   = -nbre2;
			}break;
		}

		g_translationX = 0;
		g_translationY = 0; 

	} //if(clique zone ecran)

}
///////////////////////////////////////////////////////////////////////////////////////////

void LButtonDownOnDessus()
{
	
	Pave* p     = NULL;
	Cylindre* c = NULL;
	Cone* co    = NULL;
	Sphere* sp  = NULL;
	Plan* pl    = NULL;
	Icosaedre* ico = NULL;
					
	//On verifie que le clique de la souris s'effectue dans la fenetre
	if(0<=g_iMouseLastX && g_iMouseLastX<=_rectFils.right &&  0<=g_iMouseLastY && g_iMouseLastY<=_rectFils.bottom	)
	{
		//On calcule la veritable coordonnée de la souris (coord 3D)
		float nbre1 = CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomDessus);
		float nbre2 = CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomDessus);

		switch(ActionActu)
		{
			case creer_f:
			{
				switch(FigureActu){

					case pave_f:
						p = new Pave(1.0, 1.0, 1.0, new Vertex(nbre1,0.0f,-nbre2) );
						objets->GeoActu =((Figure * )(p));
						break;
											
					case cylindre_f:
						c = new Cylindre( 1.0, 1.0, g_nbFacesCylindre,new Vertex(nbre1,0.0f,-nbre2));
						objets->GeoActu = ((Figure * )(c));
						break;

					case cone_f:
						co = new Cone(1.0, 1.0, g_nbFacesCone,new Vertex(nbre1,0.0f,-nbre2));
						objets->GeoActu = ((Figure * )(co));
						break;

					case sphere_f:
						sp = new Sphere(1.0, g_nbFacesSphere,new Vertex(nbre1,0.0f,-nbre2));
						objets->GeoActu = ((Figure * )(sp));
						break;

					case plan_f:
						pl = new Plan(1.0, 1.0, g_nbSubdivisionPlanX, g_nbSubdivisionPlanY,new Vertex(nbre1,0.0f,-nbre2));
						objets->GeoActu = ((Figure * )(pl));
						break;

					case icosaedre_f:
						ico = new Icosaedre(1.0, g_nbFacesIco,new Vertex(nbre1,0.0f,-nbre2));
						objets->GeoActu = ((Figure * )(ico));
						break;				

					}			
				
			}break;

			case selectionner_f:
			{
				g_fenSelection = VUE_DESSUS;
				g_pointDebut.x =  nbre1;
				g_pointDebut.y = -nbre2;
				g_pointFin.x   =  nbre1; 
				g_pointFin.y   = -nbre2;
			}break;

			case modifier_f:
			{
				g_fenSelection = VUE_DESSUS;
				g_pointDebut.x =  nbre1;
				g_pointDebut.y = -nbre2;
				g_pointFin.x   =  nbre1; 
				g_pointFin.y   = -nbre2;
			}break;
		}
		
		g_translationX = 0;
		g_translationY = 0;

	} //if(clique zone ecran)

}

///////////////////////////////////////////////////////////////////////////////////////////

void OnLButtonDown(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	g_bMouseDrag = 1;                
	g_iMouseLastX = LOWORD(lParam);  
	g_iMouseLastY = HIWORD(lParam);

	//On regarde si on clique dans la fenetre openGL
	GetClientRect(hWnd, &_rectFils);

	int Type = GetType(hWnd);

	switch(Type){

		case VUE_FACE:
			LButtonDownOnFace();				
			break;
					
		case VUE_COTE:				
			LButtonDownOnCote();				
			break;
					
		case VUE_DESSUS:
			LButtonDownOnDessus();
			break;
	}
				
    SetCapture(hWnd);    

}
///////////////////////////////////////////////////////////////////////////////////////////

void LButtonUpOnFace()				
{

	switch(ActionActu)
	{
		case selectionner_f:
		{
			
			float maxX;
			float maxY;
			float minX;
			float minY;

			if(g_pointDebut.x < g_pointFin.x){
				minX = g_pointDebut.x;
				maxX = g_pointFin.x;
			}else{
				maxX = g_pointDebut.x;
				minX = g_pointFin.x;
			}

			if(g_pointDebut.y < g_pointFin.y){
				minY = g_pointDebut.y;
				maxY = g_pointFin.y;
			}else{
				maxY = g_pointDebut.y;
				minY = g_pointFin.y;
			}

			switch(SelectionActu)
			{
				//On procede à une selection par objet entier
				case objet_f:
				{
					objets->SelectionnerGeoXY(minX,maxX,minY,maxY);
				}break;

				//On procede à une selection par vertex
				case vertex_f:
				{
					objets->SelectionnerVertexXY(minX,maxX,minY,maxY);
				}break;

			}//switch(SelectionActu)

		
		}break;

		case modifier_f:
		{
			switch(ModificationActu)
			{
				case translation_f:
				{
					Vecteur* Vec = new Vecteur(g_pointFin.x - g_pointDebut.x,
											   g_pointFin.y - g_pointDebut.y,	
											   0.0);

					objets->TranslaterSelection(Vec);
				}break;

				case rotation_f:
				{


				}break;
			}
		}break;
	}

}
///////////////////////////////////////////////////////////////////////////////////////////
										
void LButtonUpOnCote()				
{
	switch(ActionActu)
	{
		case selectionner_f:
		{
			float maxZ;
			float maxY;
			float minZ;
			float minY;

			if(g_pointDebut.x < g_pointFin.x){
				minZ = g_pointDebut.x;
				maxZ = g_pointFin.x;
			}else{
				maxZ = g_pointDebut.x;
				minZ = g_pointFin.x;
			}

			if(g_pointDebut.y < g_pointFin.y){
				minY = g_pointDebut.y;
				maxY = g_pointFin.y;
			}else{
				maxY = g_pointDebut.y;
				minY = g_pointFin.y;
			}

			switch(SelectionActu)
			{
				//On procede à une selection par objet entier
				case objet_f:
				{
					objets->SelectionnerGeoZY(minZ,maxZ,minY,maxY);
				}break;

				//On procede à une selection par vertex
				case vertex_f:
				{
					objets->SelectionnerVertexZY(minZ,maxZ,minY,maxY);
				}break;

			}//switch(SelectionActu)

		}break;

		case modifier_f:
		{
			switch(ModificationActu)
			{
				case translation_f:
				{
					Vecteur* Vec = new Vecteur(0.0,
											   g_pointFin.y - g_pointDebut.y,	
											   g_pointFin.x - g_pointDebut.x);

					objets->TranslaterSelection(Vec);
				}break;

				case rotation_f:
				{


				}break;
			}

		}break;
	}


}
///////////////////////////////////////////////////////////////////////////////////////////				

void LButtonUpOnDessus()
{
	switch(ActionActu)
	{
		case selectionner_f:
		{
			float maxX;
			float maxZ;
			float minX;
			float minZ;

			if(g_pointDebut.x < g_pointFin.x){
				minX = g_pointDebut.x;
				maxX = g_pointFin.x;
			}else{
				maxX = g_pointDebut.x;
				minX = g_pointFin.x;
			}

			if(g_pointDebut.y < g_pointFin.y){
				minZ = g_pointDebut.y;
				maxZ = g_pointFin.y;
			}else{
				maxZ = g_pointDebut.y;
				minZ = g_pointFin.y;
			}

			switch(SelectionActu)
			{
				case objet_f:
				{
					objets->SelectionnerGeoXZ(minX,maxX,minZ,maxZ);				
				}break;

				case vertex_f:
				{
					objets->SelectionnerVertexXZ(minX,maxX,minZ,maxZ);
				}break;

			}//switch(SelectionActu)

		}break;

		case modifier_f:
		{
			switch(ModificationActu)
			{
				case translation_f:
				{
					Vecteur* Vec = new Vecteur(g_pointFin.x - g_pointDebut.x,
											   0.0,	
											   g_pointFin.y - g_pointDebut.y);

					objets->TranslaterSelection(Vec);
				}break;

				case rotation_f:
				{


				}break;
			}


		}break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

void OnLButtonUp(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	int Type=0;

	switch(ActionActu)
	{
		case creer_f:
		{
			if(objets->GeoActu !=NULL){
				objets->GeoActu->CreerListe1(lignesfermees);
				objets->GeoActu->CreerListe2(faces);
				objets->GeoActu->CalculerExtremes();
				objets->AjouterGeo(objets->GeoActu);				
			}
			objets->GeoActu = NULL; 			         	
		}break;

		case selectionner_f:
		{
			Type = GetType(hWnd);

			switch(Type){

				case VUE_FACE:
					LButtonUpOnFace();				
					break;
							
				case VUE_COTE:				
					LButtonUpOnCote();				
					break;
							
				case VUE_DESSUS:
					LButtonUpOnDessus();
					break;
			}

			g_fenSelection = NONE;

		}break;

		case modifier_f:
		{
			Type = GetType(hWnd);

			switch(Type){

				case VUE_FACE:
					LButtonUpOnFace();				
					break;
							
				case VUE_COTE:				
					LButtonUpOnCote();				
					break;
							
				case VUE_DESSUS:
					LButtonUpOnDessus();
					break;
			}

			g_fenSelection = NONE;
		
		}break;

	}

	g_bMouseDrag = 0;
	ReleaseCapture();     

}

///////////////////////////////////////////////////////////////////////////////////////////

void MouseMoveOn3D()
{
	g_fCubeRotationX += g_iMouseDeltaX;
	g_fCubeRotationY += g_iMouseDeltaY;
}

///////////////////////////////////////////////////////////////////////////////////////////

void MouseMoveOnFace()
{

	int Profondeur_l; 
	Pave * p     = NULL;
	Cylindre * c = NULL;
	Cone* co     = NULL;
	Sphere* sp   = NULL;
	Plan* pl     = NULL;
	Icosaedre * ico;

	g_translationX += g_iMouseDeltaX;
	g_translationY += g_iMouseDeltaY;
						
	switch(ActionActu)
	{
		case creer_f:
		{
			switch(objets->GeoActu->GetTypeFig())
			{
				case pave_f:
					p = (Pave *)objets->GeoActu;					
					Profondeur_l = p->Profondeur;
					p->Modifier(g_translationX/g_fZoomFace, -g_translationY/g_fZoomFace, Profondeur_l );
					break;	

				case cylindre_f:
					c = (Cylindre*)objets->GeoActu;																
					c->Modifier(g_translationX/g_fZoomFace,-g_translationY/g_fZoomFace); 
					break;

				case cone_f:
					co = (Cone*)objets->GeoActu;	
					co->Modifier(g_translationX/g_fZoomFace,-g_translationY/g_fZoomFace); 
					break;

				case sphere_f:
					sp = (Sphere*)objets->GeoActu;	
					sp->Modifier(g_translationX/g_fZoomFace); 
					break;

				case plan_f:
					pl = (Plan*)objets->GeoActu;
					Profondeur_l = pl->Profondeur;
					pl->Modifier(g_translationX/g_fZoomFace, Profondeur_l);
					break;

				case icosaedre_f:
					ico = (Icosaedre*)objets->GeoActu;
					ico->Modifier(-g_translationX/g_fZoomFace);
					break;
			}
		}break;

		case selectionner_f:
		{		//On calcule la veritable coordonnée de la souris 
				g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomFace);
				g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomFace);
		}break;

		case modifier_f:
		{

			switch(ModificationActu)
			{
				case translation_f:
				{
					//On calcule la veritable coordonnée de la souris 
					g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomFace);
					g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomFace);	

					Vecteur* Vec = new Vecteur(g_pointFin.x - g_pointDebut.x, g_pointFin.y - g_pointDebut.y,  0.0);
					objets->TranslaterSelectionXY(Vec);
					delete Vec;
					
					//On calcule la veritable coordonnée de la souris 
					g_pointDebut.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomFace);
					g_pointDebut.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomFace);
				}break;

				case rotation_f:
				{
					//On calcule la veritable coordonnée de la souris 
					g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomFace);
					g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomFace);	

					float angle1 = CalculerAngle(g_pointDebut);
					float angle2 = CalculerAngle(g_pointFin);
						
					float angleRotation = angle2 - angle1;	
			     	objets->RotationSelectionZ(angleRotation);

					//On calcule la veritable coordonnée de la souris 
					g_pointDebut.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomFace);
					g_pointDebut.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomFace);

				}break;
			}


		}break;

	}
}
///////////////////////////////////////////////////////////////////////////////////////////

void MouseMoveOnCote()
{
	int Largeur_l;
	Pave * p;
	Cylindre * c;
	Cone* co;
	Sphere* sp;
	Plan* pl;
	Icosaedre * ico;

	g_translationX += g_iMouseDeltaX;
	g_translationY += g_iMouseDeltaY;

	switch(ActionActu)
	{
		case creer_f:
		{
			switch(objets->GeoActu->GetTypeFig())
			{
				case pave_f:
					p = (Pave *)objets->GeoActu;					
					Largeur_l = p->Largeur;
					p->Modifier(Largeur_l,  -g_translationY/g_fZoomCote, g_translationX/g_fZoomCote); 
					break;	

				case cylindre_f:
					c = (Cylindre*)objets->GeoActu;																
					c->Modifier(g_translationX/g_fZoomCote,-g_translationY/g_fZoomCote); 
					break;

				case cone_f:
					co = (Cone*)objets->GeoActu;																
					co->Modifier(g_translationX/g_fZoomCote,-g_translationY/g_fZoomCote); 
					break;

				case sphere_f:
					sp = (Sphere*)objets->GeoActu;	
					sp->Modifier(g_translationX/g_fZoomCote); 
					break;

				case plan_f:
					pl = (Plan*)objets->GeoActu;
					Largeur_l = pl->Largeur;
					pl->Modifier(Largeur_l, g_translationX/g_fZoomCote);
					break;

				case icosaedre_f:
					ico = (Icosaedre*)objets->GeoActu;
					ico->Modifier(g_translationX/g_fZoomCote);
					break;

			}

		}break;

		case selectionner_f:
		{   //On calcule la veritable coordonnée de la souris 
			g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomCote);
			g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomCote);
		}break;

		case modifier_f:
		{

			switch(ModificationActu)
			{
				case translation_f:
				{
					//On calcule la veritable coordonnée de la souris 
					g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomCote);
					g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomCote);

					Vecteur* Vec = new Vecteur(0.0, g_pointFin.y - g_pointDebut.y, g_pointFin.x - g_pointDebut.x);
					objets->TranslaterSelectionYZ(Vec);

					g_pointDebut.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomCote);
					g_pointDebut.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomCote);
				}break;

				case rotation_f:
				{
					//On calcule la veritable coordonnée de la souris 
					g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomCote);
					g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomCote);
				
					float angle1 = CalculerAngle(g_pointDebut);
					float angle2 = CalculerAngle(g_pointFin);
						
					float angleRotation = angle2 - angle1;	
			     	objets->RotationSelectionX(-angleRotation);

					g_pointDebut.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomCote);
					g_pointDebut.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomCote);

				}break;
			}


		}break;

	}
}


///////////////////////////////////////////////////////////////////////////////////////////

void MouseMoveOnDessus()
{
	int Hauteur_l;
	Pave * p;
	Cylindre * c;
	Cone* co;
	Sphere* sp;
	Plan* pl;
	Icosaedre * ico;

	g_translationX += g_iMouseDeltaX;
	g_translationY += g_iMouseDeltaY;

	switch(ActionActu)
	{
		case creer_f:
		{	
			switch(objets->GeoActu->GetTypeFig())
			{
				case pave_f:
				{
					p = (Pave *)objets->GeoActu;					
					Hauteur_l = p->Hauteur;
					p->Modifier(g_translationX/g_fZoomDessus,  Hauteur_l, -g_translationY/g_fZoomDessus);
					break;	

				case cylindre_f:
					c = (Cylindre*)objets->GeoActu;	
					Hauteur_l = c->Hauteur;
					c->Modifier(-g_translationX/g_fZoomDessus,Hauteur_l); 
					break;

				case cone_f:
					co = (Cone*)objets->GeoActu;
					Hauteur_l = co->Hauteur;
					co->Modifier(g_translationX/g_fZoomDessus,Hauteur_l); 
					break;
				
				case sphere_f:
					sp = (Sphere*)objets->GeoActu;	
					sp->Modifier(g_translationX/g_fZoomDessus); 
					break;

				case plan_f:
					pl = (Plan*)objets->GeoActu;
					pl->Modifier(g_translationX/g_fZoomDessus,-g_translationY/g_fZoomDessus);
					break;

				case icosaedre_f:
					ico = (Icosaedre*)objets->GeoActu;
					ico->Modifier(g_translationX/g_fZoomDessus);
					break;

				}
			}

		}break;

		case selectionner_f:
		{    //On calcule la veritable coordonnée de la souris 
			g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomDessus);
			g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomDessus);
		}break;

		case modifier_f:
		{ 

			switch(ModificationActu)
			{
				case translation_f:
				{
					//On calcule la veritable coordonnée de la souris 
					g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomDessus);
					g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomDessus);

					Vecteur* Vec = new Vecteur(g_pointFin.x - g_pointDebut.x,  0.0,	 g_pointFin.y - g_pointDebut.y);
					objets->TranslaterSelectionXZ(Vec);

					g_pointDebut.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomDessus);
					g_pointDebut.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomDessus);
				}break;

				case rotation_f:
				{
					//On calcule la veritable coordonnée de la souris 
					g_pointFin.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomDessus);
					g_pointFin.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomDessus);

					float angle1 = CalculerAngle(g_pointDebut);
					float angle2 = CalculerAngle(g_pointFin);
						
					float angleRotation = angle2 - angle1;	
			     	objets->RotationSelectionY(angleRotation);

					g_pointDebut.x =  CalculerCoor(g_iMouseLastX, _rectFils.right, g_fZoomDessus);
					g_pointDebut.y = -CalculerCoor(g_iMouseLastY, _rectFils.bottom, g_fZoomDessus);

				}break;
			}

		}break;

	}

}

///////////////////////////////////////////////////////////////////////////////////////////
void OnMouseMove(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{

	//Si on deplace la souris et que le clique gauche est enfoncé
	if(g_bMouseDrag)                  
    {
	    int	Type = GetType(hWnd);

		g_iMouseDeltaX = LOWORD(lParam) - g_iMouseLastX;   // Get mouse deltas
		g_iMouseDeltaY = HIWORD(lParam) - g_iMouseLastY;
		g_iMouseLastX  = LOWORD(lParam);                   // Get new mouse position
		g_iMouseLastY  = HIWORD(lParam);

		switch(Type){

			case VUE_3D:
				MouseMoveOn3D();
				break;

			case VUE_FACE:
				MouseMoveOnFace();				
				break;

			case VUE_COTE:
				MouseMoveOnCote();
				break;

			case VUE_DESSUS:
				MouseMoveOnDessus(); 	
				break;
		}

     }

}

///////////////////////////////////////////////////////////////////////////////////////////
void KeyDownOn3D()
{
	if(GetKeyState(VK_UP) & 0x80){
		g_reculCam+=1;
	}else if(GetKeyState(VK_DOWN) & 0x80){
		g_reculCam-=1;
		if (g_reculCam == 0){
			g_reculCam = .01f;
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////
void KeyDownOnFace()
{
	if(GetKeyState(VK_UP) & 0x80){
		g_fZoomFace+=1;
	}else if(GetKeyState(VK_DOWN) & 0x80){
		g_fZoomFace-=1;
		if (g_fZoomFace <= 0){
			g_fZoomFace = .01f;
		}
	}else if(GetKeyState(VK_DELETE) & 0x80){
		objets->SupprimerGeoSel();	
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
void KeyDownOnCote()
{
	if(GetKeyState(VK_UP) & 0x80){
		g_fZoomCote+=1;
	}else if(GetKeyState(VK_DOWN) & 0x80){
		g_fZoomCote-=1;
		if (g_fZoomCote <= 0){
			g_fZoomCote = .01f;
		}
	}else if(GetKeyState(VK_DELETE) & 0x80){
		objets->SupprimerGeoSel();	
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
void KeyDownOnDessus()
{
	if(GetKeyState(VK_UP) & 0x80){
		g_fZoomDessus+=1;
	}else if(GetKeyState(VK_DOWN) & 0x80){
		g_fZoomDessus-=1;
		if (g_fZoomDessus <= 0){
			g_fZoomDessus = .01f;
		}
	}else if(GetKeyState(VK_DELETE) & 0x80){
		objets->SupprimerGeoSel();	
	}
}
///////////////////////////////////////////////////////////////////////////////////////////

void OnKeyDown(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{

	int Type = GetType(hWnd);

	switch(Type){
				
		case VUE_3D:	
			KeyDownOn3D();		
			break;

		case VUE_FACE:
			KeyDownOnFace();
			break;

		case VUE_COTE:
			KeyDownOnCote();
  			break;

		case VUE_DESSUS:
			KeyDownOnDessus();
			break;
		}
}

///////////////////////////////////////////////////////////////////////////////////////////
void OnClose(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	int i = 0;

	while (i<nbreFils && tabFils[i].hWnd!= hWnd){
		i++;
	}

	for(i; i < nbreFils-1; i++) {
		tabFils[i] = tabFils[i+1]; 		
	}

	nbreFils--;

}


///////////////////////////////////////////////////////////////////////////////////////////

//gestion des messages de fenetre client
LRESULT CALLBACK WndProcFils(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{

	GetClientRect(hWnd, &_rectFils);

	int Type =0;

    switch(uMessage)
    {
        case WM_LBUTTONDOWN:
			OnLButtonDown(hWnd, uMessage, wParam, lParam);
			break;

        case WM_MOUSEMOVE: 
			OnMouseMove(hWnd, uMessage, wParam, lParam);
			break;

        case WM_LBUTTONUP:      
			OnLButtonUp(hWnd, uMessage, wParam, lParam);
			break;

		case WM_KEYDOWN:
			OnKeyDown(hWnd, uMessage, wParam, lParam);
			break;

		case WM_CLOSE:
			OnClose(hWnd, uMessage, wParam, lParam);
			break;
	}
		
	return DefMDIChildProc(hWnd, uMessage, wParam, lParam);

}
// Gestion des messages reçus par la boite dialogue de personnalisation de sphere
LRESULT CALLBACK DlgAProposProc(HWND Dlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message)	{
		
		case WM_COMMAND:

			switch(LOWORD(wParam)) {
			
				case IDOK:					
					EndDialog(Dlg,0);
					return TRUE;
			}

		default :
			return FALSE;
			
	}

	return TRUE;
}

// Gestion des messages reçus par la boite dialogue de personnalisation de la couleur
LRESULT CALLBACK DlgCouleursProc(HWND Dlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int Select;
	float material[3];
	float reflexion[3];
	float shininess[1];
	char * txt = new char[25];
	int succes;
	Figure * fTmp_l;

/*	if (objets->GeoActu == NULL) {
		EndDialog(Dlg,0);
		return TRUE;		
	}*/

	switch(message)	{
		case WM_COMMAND:
			Select=LOWORD(wParam);
			switch(Select) {
				case IDOK:
					// Récupération des spécificités
					GetDlgItemText(Dlg,IDC_EDITSHININESS, txt, 25);
					shininess[0] = atof(txt);
					
					material[0] = GetDlgItemInt(Dlg,IDC_MREDEDIT, &succes, false) / 255.0;
					material[1] = GetDlgItemInt(Dlg,IDC_MGREENEDIT, &succes, false) / 255.0;
					material[2] = GetDlgItemInt(Dlg,IDC_MBLUEEDIT, &succes, false) / 255.0;

					reflexion[0] = GetDlgItemInt(Dlg,IDC_RREDEDIT, &succes, false) / 255.0;
					reflexion[1] = GetDlgItemInt(Dlg,IDC_RGREENEDIT, &succes, false) / 255.0;
					reflexion[2] = GetDlgItemInt(Dlg,IDC_RBLUEEDIT, &succes, false) / 255.0;

					if (!succes) {
						MessageBox(Dlg, "Les couleurs doivent être entieres", "Problème", MB_OK);
					} else {
						int i, taille = objets->GetTailleFigEnModif();
						for (i = 0 ; i < taille ; i++) {
							fTmp_l = objets->GetGeo(i);
							fTmp_l->colorMaterial[0] = material[0];
							fTmp_l->colorReflexion[0] = reflexion[0];
							fTmp_l->colorMaterial[1] = material[1];
							fTmp_l->colorReflexion[1] = reflexion[1];
							fTmp_l->colorMaterial[2] = material[2];
							fTmp_l->colorReflexion[2] = reflexion[2];
							fTmp_l->shininess[0] = shininess[0];
						}
						EndDialog(Dlg,0);
						return TRUE;	
					}
				case IDCANCEL:
					EndDialog(Dlg,Select);
					return TRUE;
			}
		default:
			return FALSE;
	}
}


// Gestion des messages reçus par la boite dialogue de personnalisation de sphere
LRESULT CALLBACK DlgSphrProc(HWND Dlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int Select;
	float rayonX = 0;
	float rayonZ = 0;
	float alphaX = 0;
	float alphaZ = 0;
	float portionX = 0;
	float portionZ = 0;
	int prec = 0;
	//Sphere * sph = NULL;
	int succes;
	char * txt = new char[25];

	switch(message)	{
		case WM_COMMAND:
			Select=LOWORD(wParam);
			switch(Select) {
				case IDOK:
					// Récupération des spécificités
					GetDlgItemText(Dlg,IDC_RAYONXEDIT, txt, 25);
					rayonX = atof(txt);	
					GetDlgItemText(Dlg,IDC_RAYONZEDIT, txt, 25);
					rayonZ = atof(txt);
					GetDlgItemText(Dlg,IDC_ANGLEXEDIT, txt, 25);
					alphaX = atof(txt);
					GetDlgItemText(Dlg,IDC_ANGLEZEDIT, txt, 25);
					alphaZ = atof(txt);
					GetDlgItemText(Dlg,IDC_PORTIONXEDIT, txt, 25);
					portionX = atof(txt);
					GetDlgItemText(Dlg,IDC_PORTIONZEDIT, txt, 25);
					portionZ = atof(txt);				
					prec = GetDlgItemInt(Dlg,IDC_PRECISIONEDIT, &succes, false);
					if (!succes) {
						MessageBox(Dlg, "A pas marché", "", MB_OK);
					}
					// Ajout aux objets de la sphere avec les spécificités rentrées
					if (rayonX > 0 && rayonZ > 0 && portionX > 0 && portionZ > 0 && prec > 0) {
				
						
					//	objets->AjouterGeo((Geometrie * )(sph));
					}
					EndDialog(Dlg,0);
					return TRUE;
				case IDCANCEL:
					EndDialog(Dlg,Select);
					return TRUE;
			}
		default:
			return FALSE;
	}
}

//Gestion des messages de la fenetre principal
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{

	HWND hDlg = NULL;
	HMENU MenuObj = NULL;
	HWND editTmp = NULL;
	char * txt = new char[3];
	Figure * pFigTmp_l = NULL;
	int coul, taille;

    switch(uMessage)
    {

		case WM_CREATE:

            CLIENTCREATESTRUCT ccs;
          
            //ccs.hWindowMenu = NULL;			
            ccs.hWindowMenu = (HMENU)GetSubMenu(GetMenu(hWnd), 4); // if you use less than 10 windows then use this line
            ccs.idFirstChild = ID_MDI_PREMIER_FENETRE;

            _HwndFenFils = CreateWindowEx(WS_EX_CLIENTEDGE, "MDICLIENT", NULL,
                                          WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,                    //WS_VSCROLL and WS_HSCROLL do not work properly yet
                                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                          hWnd, (HMENU)ID_MDI_CLIENT, _Instance, (LPVOID)&ccs);

            ShowWindow(_HwndFenFils, SW_SHOW);

    		//On recupere le sous menu de la barre des titre et on le coche le cube
			MenuObj = (HMENU)GetSubMenu(GetMenu(hWnd), PosMenuPrimitive);
			CheckMenuItem(  MenuObj,ID_CUBE, MF_CHECKED);

            return 0;
       
		break;
            
		case WM_CLOSE:
            DestroyWindow(hWnd);
        break;

		case WM_DESTROY:
            PostQuitMessage(0);
        break;

		case WM_COMMAND:
            switch(LOWORD(wParam))
            {
				case IDOPEN:
					
					objets->ChargerSauvegarde("SaveMe.sav");/*
					OPENFILENAME ofn;       // common dialog box structure
					char szFile[260];       // buffer for file name
				
					HANDLE hf;              // file handle

					// Initialize OPENFILENAME
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFile = szFile;
					//
					// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
					// use the contents of szFile to initialize itself.
					//
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					// Display the Open dialog box. 

					if (GetOpenFileName(&ofn)==TRUE){
						hf = CreateFile(ofn.lpstrFile, GENERIC_READ,0, (LPSECURITY_ATTRIBUTES) NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,(HANDLE) NULL);
						objets->ChargerSauvegarde("SaveMe.sav");
					}*/						

				break;
				
				case IDSAVE:
					objets->EcrireSauvegarde("SaveMe.sav");
				break;

                case IDCLOSE:
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;

                case IDVUE3D:
					CreerFenFils(VUE_3D);
					 PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_HORIZONTAL, 0);
				break; 

                case IDFACE:
					CreerFenFils(VUE_FACE);
					 PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_HORIZONTAL, 0);
				break; 

				case IDCOTE:
					CreerFenFils(VUE_COTE);
					PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_HORIZONTAL, 0);
				break; 

				case IDDESSUS:
					CreerFenFils(VUE_DESSUS);
					 PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_HORIZONTAL, 0);
				break;

				case ID_CUBE:
					ActionActu = creer_f;
					FigureActu = pave_f;

					//On recupere le sous menu de la barre des titre et on le coche cube
					MenuObj = (HMENU)GetSubMenu(GetMenu(hWnd), PosMenuPrimitive);
					CheckMenuItem(  MenuObj,ID_CUBE, MF_CHECKED);
					CheckMenuItem(  MenuObj,g_lastChecked, MF_UNCHECKED);
					g_lastChecked = ID_CUBE;

				break;

				case ID_CONE:
					ActionActu = creer_f;
					FigureActu = cone_f;
					
					//On recupere le sous menu de la barre des titre et on le coche cone
					MenuObj = (HMENU)GetSubMenu(GetMenu(hWnd), PosMenuPrimitive);
					CheckMenuItem(  MenuObj,ID_CONE, MF_CHECKED);
					CheckMenuItem(  MenuObj,g_lastChecked, MF_UNCHECKED);
					g_lastChecked = ID_CONE;

				break;

				case ID_CYLINDRE:
					ActionActu = creer_f;
					FigureActu = cylindre_f;
				
					//On recupere le sous menu de la barre des titre et on le coche cylindre
					MenuObj = (HMENU)GetSubMenu(GetMenu(hWnd), PosMenuPrimitive);
					CheckMenuItem(  MenuObj,ID_CYLINDRE, MF_CHECKED);
					CheckMenuItem(  MenuObj,g_lastChecked, MF_UNCHECKED);
					g_lastChecked = ID_CYLINDRE;
				break;

				case ID_SPHERE:
					ActionActu = creer_f;	
					FigureActu = sphere_f;

					
					//On recupere le sous menu de la barre des titre et on la coche sphere 
					//et on décoche l'ancien coché
					MenuObj = (HMENU)GetSubMenu(GetMenu(hWnd), PosMenuPrimitive);
					CheckMenuItem(  MenuObj,ID_SPHERE, MF_CHECKED);
					CheckMenuItem(  MenuObj,g_lastChecked, MF_UNCHECKED);
					g_lastChecked = ID_SPHERE;
					
					//hDlg=CreateDialog(_Instance,(LPCTSTR)IDD_SPHERE,NULL,(DLGPROC)DlgSphrProc);
					//ShowWindow(hDlg,SW_SHOW);
					//ShowModal(IDD_SPHERE);					
				break;

				case ID_PLAN:
					ActionActu = creer_f;	
					FigureActu = plan_f;

					MenuObj = (HMENU)GetSubMenu(GetMenu(hWnd), PosMenuPrimitive);
					CheckMenuItem(  MenuObj,ID_SPHERE, MF_CHECKED);
					CheckMenuItem(  MenuObj,g_lastChecked, MF_UNCHECKED);
					g_lastChecked = ID_PLAN;
					break;


				case ID_ICOSAEDRE:
					ActionActu = creer_f;	
					FigureActu = icosaedre_f;
					
					//On recupere le sous menu de la barre des titre et on la coche sphere 
					//et on décoche l'ancien coché
					MenuObj = (HMENU)GetSubMenu(GetMenu(hWnd), PosMenuPrimitive);
					CheckMenuItem(  MenuObj,ID_ICOSAEDRE, MF_CHECKED);
					CheckMenuItem(  MenuObj,g_lastChecked, MF_UNCHECKED);
					g_lastChecked = ID_ICOSAEDRE;
									
				break;

				case ID_TERRAIN:
					ActionActu = creer_f;	
					FigureActu = terrain_f;
					
					//On recupere le sous menu de la barre des titre et on la coche sphere 
					//et on décoche l'ancien coché
					MenuObj = (HMENU)GetSubMenu(GetMenu(hWnd), PosMenuPrimitive);
					CheckMenuItem(  MenuObj,ID_TERRAIN, MF_CHECKED);
					CheckMenuItem(  MenuObj,g_lastChecked, MF_UNCHECKED);
					g_lastChecked = ID_TERRAIN;
									
				break;
				

 
				case ID_SELECTIONNER_GEOMETRIES:
					ActionActu = selectionner_f;
					SelectionActu = objet_f;				
				break;

				case ID_SELECTIONNER_POINTS:
					ActionActu = selectionner_f;
					SelectionActu = vertex_f;				
				break;

				case ID_SELECTION_AUCUNE:
					ActionActu = selectionner_f;
					SelectionActu = aucune_f;
					objets->ViderSelection();
				break;


				case ID_SELECTION_TOUS:
					ActionActu = selectionner_f;
					SelectionActu = tous_f;
					objets->TousSelectionner();
				break;

				case ID_SELECTION_DUPLIQUER:
					ActionActu = selectionner_f;
					SelectionActu = dupliquer_f;
					objets->DupliquerSelection();
				break;

				case ID_ACTIONS_TRANSLATION:
					ActionActu = modifier_f;
					ModificationActu = translation_f;
				break;

				case ID_ACTIONS_ROTATION:
					ActionActu = modifier_f;
					ModificationActu = rotation_f;
				break;

				case ID_ACTIONS_COULEUR:
					hDlg=CreateDialog(_Instance,(LPCTSTR)IDD_COLORS,NULL,(DLGPROC)DlgCouleursProc); 
					taille = objets->GetTailleFigEnModif();
					if (taille == 1 || (taille > 0 && objets->MemeCouleur())) {
						pFigTmp_l = objets->GetGeo(0);

						coul = (int)(pFigTmp_l->colorMaterial[0] * 255);
						txt = itoa(coul, txt, 10);
						SetDlgItemText(hDlg, IDC_MREDEDIT, txt);

						coul = (int)(pFigTmp_l->colorMaterial[1] * 255);
						txt = itoa(coul, txt, 10);
						SetDlgItemText(hDlg, IDC_MGREENEDIT, txt);

						coul = (int)(pFigTmp_l->colorMaterial[2] * 255);
						txt = itoa(coul, txt, 10);
						SetDlgItemText(hDlg, IDC_MBLUEEDIT, txt);

						coul = (int)(pFigTmp_l->colorReflexion[0] * 255);
						txt = itoa(coul, txt, 10);
						SetDlgItemText(hDlg, IDC_RREDEDIT, txt);

						coul = (int)(pFigTmp_l->colorReflexion[1] * 255);
						txt = itoa(coul, txt, 10);
						SetDlgItemText(hDlg, IDC_RGREENEDIT, txt);

						coul = (int)(pFigTmp_l->colorReflexion[2] * 255);
						txt = itoa(coul, txt, 10);
						SetDlgItemText(hDlg, IDC_RBLUEEDIT, txt);

						coul = (int)(pFigTmp_l->shininess[0]);
						txt = itoa(coul, txt, 10);
						SetDlgItemText(hDlg, IDC_EDITSHININESS, txt);
					}
					ShowWindow(hDlg,SW_SHOW);					
				break;

				case IDAPROPOS:
					hDlg=CreateDialog(_Instance,(LPCTSTR)IDD_DIALOG_A_PROPOS,NULL,(DLGPROC)DlgAProposProc);
					ShowWindow(hDlg,SW_SHOW);					
				break;

				case IDHORIZONTAL:
                    PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_HORIZONTAL, 0);
                break;

                case IDVERTICAL:
                    PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_VERTICAL, 0);
                break;

                case IDCASCADE:
                    PostMessage(_HwndFenFils, WM_MDICASCADE, 0, 0);
                break;


				default:
                {
                    if(LOWORD(wParam) >= ID_MDI_PREMIER_FENETRE)
                    {
                        DefFrameProc(hWnd, _HwndFenFils, uMessage, wParam, lParam);
                    }
                    else
                    {
                        HWND hChild;
                        hChild = (HWND)SendMessage(_HwndFenFils, WM_MDIGETACTIVE, 0, 0);
                        if(hChild)
                        {
                            SendMessage(hChild, WM_COMMAND, wParam, lParam);
                        }
                    }
                }

			}
		break;

		
		case WM_SIZE:
            PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_HORIZONTAL, 0);
        


		default:
			return DefFrameProc(hWnd, _HwndFenFils, uMessage, wParam, lParam);
	}


	return 0;

}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

//	objets->ChargerSauvegarde("SaveMe.sav");
	//objets->CreerListe(facesTriangle);

	MSG Msg;
	WNDCLASSEX wc;
	_Instance = hInstance;

	//parametrage d'une fenetre fils
	wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)WndProcFils;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DSHADOW+1);  
    wc.lpszMenuName = NULL;
    wc.lpszClassName = NomClassFils;
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL,"Impossible d'enregistrer la fenetre enfant","Erreur", MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    // parametrage de la fentre principale
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DSHADOW+1);
    wc.lpszMenuName = "IDR_MENU";
    wc.lpszClassName = NomClassPrincipal;
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    if(!RegisterClassEx(&wc)) {
        MessageBox(0, "Impossible d'enregistrer la fenetre principal", "Erreur", MB_ICONERROR | MB_OK);
        return -1;
    }

	//On cree la fenetre principal
    _HwndFen = CreateWindowEx(WS_EX_LEFT, NomClassPrincipal, "CastorShape",  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,  NULL, NULL, hInstance, NULL);

	//On regarde si la fenetre a bien était créé
    if(_HwndFen == NULL){
        MessageBox(0, "Impossible de creer la fenetre.", "Erreur", MB_ICONERROR | MB_OK);
        return -1;
    }

	//On affiche la fenetre principal
	ShowWindow(_HwndFen, nCmdShow);
    UpdateWindow(_HwndFen);


	CreerFenFils(VUE_DESSUS);
	CreerFenFils(VUE_COTE);
	CreerFenFils(VUE_FACE);
	CreerFenFils(VUE_3D);

    PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_HORIZONTAL, 0);

	while(GetMessage(&Msg, NULL, 0, 0))
    {
		if(!TranslateMDISysAccel(_HwndFenFils, &Msg))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }


		for(int iLoop = 0; iLoop < nbreFils; iLoop++)   
        {
		
                if(!IsIconic(tabFils[iLoop].hWnd))   // If child isn't iconic (minimized)
                {
                    wglMakeCurrent(tabFils[iLoop].hDC, tabFils[iLoop].hRC);  // Make them the current DC and RC
                    GetClientRect(tabFils[iLoop].hWnd, &_rectFils);
                    if(_rectFils.right > 0 && _rectFils.bottom > 0)
                        SetProjection(_rectFils.right, _rectFils.bottom,tabFils[iLoop].type);        // Set the childs projection matrix
                    DrawGLScene(tabFils[iLoop].type);
                    glFlush();
                    SwapBuffers(tabFils[iLoop].hDC);
                }
			
        }
	
	}
//	objets->EcrireSauvegarde("SaveMe.sav");

	return Msg.wParam;
	


return 0;
}