//---------------------------------------------------------------//
// CMatrice4.h
//
// 
//
// 
//
//---------------------------------------------------------------//

#ifndef CMatrice_h
#define CMatrice_h



//---------------------------------------------------------------//
// Include
//---------------------------------------------------------------//

#include "Math.h"
#include "Vertex.h"


//---------------------------------------------------------------//
// CMatrice4
//
// 
//---------------------------------------------------------------//
class CMatrice4
{
	// Attributs ------------------------------------------------//

	public:
		double ** Matrice;
		
		


	// Construction / Destruction -------------------------------//

	public:

		CMatrice4(void);
		~CMatrice4(void);

	// Méthodes -------------------------------------------------//

	public:

		void Initialiser(void);
		void Identite(void);
		void MatriceRotX(double Angle);
		void MatriceRotY(double Angle);
		void MatriceRotZ(double Angle);

		void Multiplication(Vertex* &vertex );

};



//---------------------------------------------------------------//
// Fonctions
//---------------------------------------------------------------//

Vertex Multiplication(CMatrice4 Matrice, Vertex vertex );


#endif