//---------------------------------------------------------------//
// CTerrain.h
//
// 
//
// 
//
//---------------------------------------------------------------//

#ifndef CTerrain_h
#define CTerrain_h

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <stdio.h>
#include <string>           //Chaine de caracteres
#include "../Maths/CVecteur.h"


//---------------------------------------------------------------//
// Include
//---------------------------------------------------------------//



typedef struct tagPOINTMAP {
    BYTE Height;			
    BYTE Red;	
	BYTE Green;
    BYTE Blue;
} POINTMAP;



//---------------------------------------------------------------//
// CTerrain
//
// 
//---------------------------------------------------------------//
class CTerrain
{
	// Attributs ------------------------------------------------//

	public:
		int Longeur;            //Longueur du terrain
		int Largeur;            //Largeur du terrain
		double Espacement;      //Espacement entre un vertex et ses voisin
		int Attenuation;        //Facteur de reduction de la hauteur  
		bool Gl_triangle_strip;
		POINTMAP ** HeightMap;  //Tableau de hauteurs
		int * DivCommuns;
		int NbDivCommuns;
 

	// Construction / Destruction -------------------------------//

	public:

		CTerrain(void);
		CTerrain(std::string NomFichier );
		~CTerrain(void);

	// Méthodes -------------------------------------------------//

	public:
		void Afficher();
		void ChargerFichier(std::string NomFichier);
		void SetCouleur(int x, int y);	
		void SetGlTriangleStrip(bool );
		void IncEspacement();
		void DecEspacement();
		LONG LecteurLong(FILE *pFile);

	private:
		void CalculerDivCommuns();
		void AddDivCommun(int Nbre);
		CVertex* CalculerCentre(CVertex* v1, CVertex* v2, CVertex* v3);
		

};



//---------------------------------------------------------------//
// Fonctions
//---------------------------------------------------------------//




#endif