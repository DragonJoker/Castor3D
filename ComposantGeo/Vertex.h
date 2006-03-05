/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loïc DASSONVILLE
 But:    Moteur 3D
 Fichier: Vertex.h - Vertex.cpp
 Desc:   Classe gérant les sommets des géométries 
*******************************************************************************/

#ifndef Vertex_H
#define Vertex_H    
//______________________________________________________________________________
#include <iostream>  
#include <math.h>
//______________________________________________________________________________
using namespace std;
//______________________________________________________________________________
class Vertex
{
    public :
        double x,y,z;
        

        //**********************************************************************
        //Constructeurs et Destructeurs
        //**********************************************************************

        Vertex();

        Vertex(Vertex * v);

        Vertex(double x, double y, double z);

        ~Vertex();


        //**********************************************************************
        // Opérateurs
        //**********************************************************************


		Vertex & operator + (const Vertex &pVertex);

		Vertex * operator + (Vertex *pVertex);

		void operator += (Vertex *pVertex);
        
        Vertex & operator - (const Vertex &pVertex);
		
		Vertex * operator - (Vertex *pVertex);
		
		void operator -= (Vertex *pVertex);

        Vertex & operator * (const Vertex &pVertex);

		Vertex * operator * (Vertex *pVertex);

		Vertex * operator / (int Valeur);

		void operator /= (int Valeur);

        bool operator != (const Vertex &pVertex);

		bool operator != (Vertex *pVertex);
        
        bool operator == (const Vertex &pVertex);

		bool operator == (Vertex *Vertex);

		

		



};
//______________________________________________________________________________
#endif