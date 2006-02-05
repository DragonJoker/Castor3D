#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <stdlib.h>   
#include "resource.h"

#define VUE_FACE 1
#define VUE_COTE 2
#define VUE_DESSUS 3
#define VUE_3D 4

HINSTANCE  _Instance;
HWND _HwndFenFils;
HWND _HwndFen;

char NomClassPrincipal[] = "ClassPrincipal";
char NomClassFils[] = "ClassFils";


float    g_fCubeRotationX , g_fCubeRotationY;      
float    g_fZoomDessus=1, g_fZoomFace=1, g_fZoomCote=1;

bool g_bMouseDrag = 0;                         // Mouse capture flag
int g_iMouseLastX;                             // Holds previous X position of mouse
int g_iMouseLastY;                             // Holds previous Y position of mouse
int g_iMouseDeltaX;                            // Holds the mouse increment in X since LastX
int g_iMouseDeltaY;                            // Holds the mouse increment in Y since LastY



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


//Initialisation de la librairie graphique
void InitGL()
{
    glCullFace(GL_BACK);
    glClearColor(0.0, 0.0, 0.0, 0.0);
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

    float LightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
    float LightPosition[] = {-3.0, 2.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);

}



void SetProjection(int iWidth, int iHeight, int Type)
{
    if(iHeight == 0)
    {
        iHeight = 1; //on evite la division par 0
    }
    glViewport(0, 0, iWidth, iHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	switch(Type){

		case VUE_3D:		
			gluPerspective(45.0,(float)iWidth/(float)iHeight,0.1,200.0);
			gluLookAt(5,0,0,0,0,0,0,1,0);
		break;
	
		case VUE_FACE:
			glOrtho(-iWidth / g_fZoomFace, iWidth / g_fZoomFace, -iHeight / g_fZoomFace, iHeight / g_fZoomFace, -2000, 2000);
		break;

		case VUE_DESSUS:
			glOrtho(-iWidth / g_fZoomDessus, iWidth / g_fZoomDessus, -iHeight / g_fZoomDessus, iHeight / g_fZoomDessus, -2000, 2000);
		break;

		case VUE_COTE:
			glOrtho(-iWidth / g_fZoomCote, iWidth / g_fZoomCote, -iHeight / g_fZoomCote, iHeight / g_fZoomCote, -2000, 2000);
		break;
	}


/*
	if(iHeight == 0)
    {
        iHeight = 1; //on evite la division par 0
    }
    glViewport(0, 0, iWidth, iHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 //   glOrtho(-iWidth * 0.5, iWidth * 0.5, -iHeight * 0.5, iHeight * 0.5, -2000, 2000);
	gluOrtho2D(45.0,(float)iWidth/(float)iHeight);
*/
}






///////////////////////////////////////////////////////////////////////////////////////////
void DrawGLScene(int Type)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);//GL_MODELVIEW);
    glLoadIdentity();



    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glPushMatrix();



	
	switch(Type){
		case VUE_FACE:
			glRotatef(90,1.0,0.0,0.0);
			glBegin(GL_LINES);
			break;
		case VUE_COTE:
			glRotatef(90,0.0,1.0,0.0);
			glBegin(GL_LINES);
			break;
		case VUE_DESSUS:
			glRotatef(90,0.0,0.0,1.0);
			glBegin(GL_LINES);
			break;
		case VUE_3D:
			glRotatef(g_fCubeRotationX,1.0,0.0,0.0);
			glRotatef(g_fCubeRotationY,0.0,1.0,1.0);
			glBegin(GL_QUADS);
			break;
	}


        // Front Face
      glColor3f(1.0,0.0,0.0);
        glVertex3f(-1.0, -1.0,  1.0);    // Bottom Left
        glColor3f(0.0,1.0,0.0);
        glVertex3f( 1.0, -1.0,  1.0);    // Bottom Right
        glColor3f(0.0,0.0,1.0);
        glVertex3f( 1.0,  1.0,  1.0);    // Top Right
        glColor3f(1.0,1.0,0.0);
        glVertex3f(-1.0,  1.0,  1.0);    // Top Left


 /* 		
		glColor3f(10.0,0.0,0.0);
        glVertex3f(-100.0, -100.0,  100.0);    // Bottom Left
        glColor3f(0.0,1.0,0.0);
        glVertex3f( 100.0, -100.0,  100.0);    // Bottom Right
        glColor3f(0.0,0.0,1.0);
        glVertex3f( 100.0,  100.0,  100.0);    // Top Right
        glColor3f(1.0,1.0,0.0);
        glVertex3f(-100.0,  100.0,  100.0);
		
*/



        // Back Face
        glColor3f(0.0,0.0,1.0);
        glVertex3f(-1.0, -1.0, -1.0);    // Bottom Right
        glColor3f(0.0,1.0,0.0);
        glVertex3f(-1.0,  1.0, -1.0);    // Top Right
        glColor3f(1.0,0.0,0.0);
        glVertex3f( 1.0,  1.0, -1.0);    // Top Left
        glColor3f(1.0,1.0,0.0);
        glVertex3f( 1.0, -1.0, -1.0);    // Bottom Left
        // Top Face
        glColor3f(0.0,1.0,0.0);
        glVertex3f(-1.0,  1.0, -1.0);    // Top Left
        glColor3f(1.0,1.0,0.0);
        glVertex3f(-1.0,  1.0,  1.0);    // Bottom Left
        glColor3f(0.0,0.0,1.0);
        glVertex3f( 1.0,  1.0,  1.0);    // Bottom Right
        glColor3f(1.0,0.0,0.0);
        glVertex3f( 1.0,  1.0, -1.0);    // Top Right
        // Bottom Face
        glColor3f(0.0,0.0,1.0);
        glVertex3f(-1.0, -1.0, -1.0);    // Top Right
        glColor3f(1.0,1.0,0.0);
        glVertex3f( 1.0, -1.0, -1.0);    // Top Left
        glColor3f(0.0,1.0,0.0);
        glVertex3f( 1.0, -1.0,  1.0);    // Bottom Left
        glColor3f(1.0,0.0,0.0);
        glVertex3f(-1.0, -1.0,  1.0);    // Bottom Right
        // Right face
        glColor3f(1.0,1.0,0.0);
        glVertex3f( 1.0, -1.0, -1.0);    // Bottom Right
        glColor3f(1.0,0.0,0.0);
        glVertex3f( 1.0,  1.0, -1.0);    // Top Right
        glColor3f(0.0,0.0,1.0);
        glVertex3f( 1.0,  1.0,  1.0);    // Top Left
        glColor3f(0.0,1.0,0.0);
        glVertex3f( 1.0, -1.0,  1.0);    // Bottom Left
        // Left Face
        glColor3f(0.0,0.0,1.0);
        glVertex3f(-1.0, -1.0, -1.0);    // Bottom Left
        glColor3f(1.0,0.0,0.0);
        glVertex3f(-1.0, -1.0,  1.0);    // Bottom Right
        glColor3f(1.0,1.0,0.0);
        glVertex3f(-1.0,  1.0,  1.0);    // Top Right
        glColor3f(0.0,1.0,0.0);
        glVertex3f(-1.0,  1.0, -1.0);    // Top Left
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

}




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
		nomFenetre="Vue de Coté";
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


//gestion des messages de fenetre client
LRESULT CALLBACK WndProcFils(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uMessage)
    {


        case WM_LBUTTONDOWN:
            g_bMouseDrag = 1;                // Set mouse flag
            g_iMouseLastX = LOWORD(lParam);  // Get mouse position
            g_iMouseLastY = HIWORD(lParam);
            SetCapture(hWnd);             // Capture the mouse
			break;

        case WM_MOUSEMOVE:
            if(g_bMouseDrag)                  // If mouse flag set
            {
				bool trouve = false;
				for(int i=0; i<nbreFils && !trouve; i++){
					if (tabFils[i].hWnd == hWnd && tabFils[i].type == VUE_3D ) {
						g_iMouseDeltaX = LOWORD(lParam) - g_iMouseLastX;   // Get mouse deltas
						g_iMouseDeltaY = HIWORD(lParam) - g_iMouseLastY;
						g_iMouseLastX = LOWORD(lParam);                 // Get new mouse position
						g_iMouseLastY = HIWORD(lParam);
						g_fCubeRotationX += g_iMouseDeltaX;
						g_fCubeRotationY += g_iMouseDeltaY;
						trouve = true;
					}
				}
            }
			break;

        case WM_LBUTTONUP:         // If left button is released
            g_bMouseDrag = 0;         // Clear mouse flag
            ReleaseCapture();      // Release the mouse capture
			break;


		case WM_KEYDOWN:

		
			// Si on appuie sur la touche haut
			if(GetKeyState(VK_UP) & 0x80){
				bool trouve = false;
				// on recherche dans quelle fenetre on est et on zoome
				for(int i=0; i<nbreFils && !trouve; i++){
					if (tabFils[i].hWnd == hWnd && tabFils[i].type == VUE_DESSUS ) {
						g_fZoomDessus+=1;
						trouve = true;
					}else if (tabFils[i].hWnd == hWnd && tabFils[i].type == VUE_COTE ){ 
						g_fZoomCote+=1;
						trouve = true;
					}else if (tabFils[i].hWnd == hWnd && tabFils[i].type == VUE_FACE) {
						g_fZoomFace+=1;
						trouve = true;
					}
				}
			// Si on appuie sur la touche bas
			}else if(GetKeyState(VK_DOWN) & 0x80){
				bool trouve = false;
				// on recherche dans quelle fenetre on est et on dézoome
				for(int i=0; i<nbreFils && !trouve; i++){
					if (tabFils[i].hWnd == hWnd && tabFils[i].type == VUE_DESSUS ) {
						g_fZoomDessus-=1;
						if (g_fZoomDessus <= 0)
							g_fZoomDessus = .01f;
						trouve = true;
					}else if (tabFils[i].hWnd == hWnd && tabFils[i].type == VUE_COTE ){ 
						g_fZoomCote-=1;
						if (g_fZoomCote <= 0)
							g_fZoomCote = .01f;
						trouve = true;
					}else if (tabFils[i].hWnd == hWnd && tabFils[i].type == VUE_FACE ) {
						g_fZoomFace-=1;
						if (g_fZoomFace <= 0)
							g_fZoomFace = .01f;
						trouve = true;
					}
				}
			}

			break;

		case WM_CLOSE:
			int i = 0;
			while (i<nbreFils && tabFils[i].hWnd!= hWnd){
				i++;
			}

			for(i; i < nbreFils-1; i++) {
				tabFils[i] = tabFils[i+1]; 		
			}

			nbreFils--;		
			break;
	}
		



//	return 0;
	return DefMDIChildProc(hWnd, uMessage, wParam, lParam);

}

//Gestion des messages de la fenetre principal
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{


    switch(uMessage)
    {

		case WM_CREATE:

            CLIENTCREATESTRUCT ccs;
          
            ccs.hWindowMenu = NULL;			
            //ccs.hWindowMenu = (HMENU)GetSubMenu(GetMenu(hWnd), 2); // if you use less than 10 windows then use this line
            ccs.idFirstChild = ID_MDI_PREMIER_FENETRE;

            _HwndFenFils = CreateWindowEx(WS_EX_CLIENTEDGE, "MDICLIENT", NULL,
                                          WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,                    //WS_VSCROLL and WS_HSCROLL do not work properly yet
                                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                          hWnd, (HMENU)ID_MDI_CLIENT, _Instance, (LPVOID)&ccs);

            ShowWindow(_HwndFenFils, SW_SHOW);
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
                case IDCLOSE:
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;

                case IDVUE3D:
					CreerFenFils(VUE_3D);
				break; 

                case IDFACE:
					CreerFenFils(VUE_FACE);
				break; 

				case IDCOTE:
					CreerFenFils(VUE_COTE);
				break; 

				case IDDESSUS:
					CreerFenFils(VUE_DESSUS);
				break; 

				case IDAPROPOS:
					MessageBox(0, "Projet : CastorShape \nRéalisation : Loïc DASSONVILLE, Sylvain DOREMUS \nAnnée : 2005-2006 ", "Informations", MB_ICONINFORMATION | MB_OK);
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
    wc.hIcon = NULL; //LoadIcon(h_Instance, MAKEINTRESOURCE(IDI_CHILDICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DSHADOW+1);  
    wc.lpszMenuName = NULL;
    wc.lpszClassName = NomClassFils;
    wc.hIconSm = NULL; //LoadIcon(h_Instance, MAKEINTRESOURCE(IDI_CHILDICON));

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
    wc.hIcon = NULL; //LoadIcon(h_Instance, MAKEINTRESOURCE(IDI_MYICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DSHADOW+1);
    wc.lpszMenuName = "IDR_MENU";
    wc.lpszClassName = NomClassPrincipal;
    wc.hIconSm = NULL; //LoadIcon(h_Instance, MAKEINTRESOURCE(IDI_MYICON));

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

	CreerFenFils(VUE_3D);
	CreerFenFils(VUE_DESSUS);
	CreerFenFils(VUE_COTE);
	CreerFenFils(VUE_FACE);
    PostMessage(_HwndFenFils, WM_MDITILE, MDITILE_HORIZONTAL, 0);

	while(GetMessage(&Msg, NULL, 0, 0))
    {
		if(!TranslateMDISysAccel(_HwndFenFils, &Msg))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }


		for(int iLoop = 0; iLoop < nbreFils; iLoop++)   // Loop through all children
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


	return Msg.wParam;
}