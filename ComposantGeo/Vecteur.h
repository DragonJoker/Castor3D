/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D
 Fichier: Vecteur.h - Vecteur.cpp
 Desc:   Classe gérant les vecteurs
*******************************************************************************/

#ifndef Vecteur_H
#define Vecteur_H    
//______________________________________________________________________________
#include <iostream>  
#include <math.h>
#include "Vertex.h"
//______________________________________________________________________________
using namespace std;
//______________________________________________________________________________
class Vecteur
{
    public :
        double x,y,z;

        //**********************************************************************
        //Constructeurs et Destructeurs
        //**********************************************************************

        Vecteur();

        Vecteur(Vecteur * v);

		Vecteur(Vertex *v);

		Vecteur(Vertex *v1, Vertex *v2);

        Vecteur(double x, double y, double z);

        ~Vecteur();
		
		//**********************************************************************
        // Opérateurs
        //**********************************************************************

		Vecteur & operator + (const Vecteur &pVecteur);

		Vecteur * operator + (Vecteur *pVecteur);
        
        Vecteur & operator - (const Vecteur &pVecteur);  
		
		Vecteur * operator - (Vecteur *pVecteur);

        Vecteur & operator ^ (const Vecteur &pVecteur);

		Vecteur * operator ^ (Vecteur *pVecteur);

		Vecteur * operator * (int Valeur);		

		Vecteur * operator * (float Valeur);	

		Vecteur * operator * (double Valeur);

		Vecteur * operator / (int Valeur);		

		Vecteur * operator / (float Valeur);	

		Vecteur * operator / (double Valeur);
		        
		bool operator != (const Vecteur &pVecteur);

		bool operator != (Vecteur *pVecteur);
        
        bool operator == (const Vecteur &pVecteur);

		bool operator == (Vecteur *pVecteur);

		//**********************************************************************
        // Autres
        //**********************************************************************

		float GetLongueur();

		void Normaliser();

		void Inverser();

		Vecteur* GetNormalise();

		Vecteur* GetNormal(Vecteur* Vecteur);

		void TranslaterVertex(Vertex *&rVertex);



};
//______________________________________________________________________________
#endif