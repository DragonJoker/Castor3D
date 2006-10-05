/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Arcs.h - Arcs.cpp

 Desc:   Classe gérant les arcs ; une liste de points

*******************************************************************************/
#ifndef Arcs_H
#define Arcs_H        
//______________________________________________________________________________
#include "Constantes.h" 
#include "../../Math/Vecteur.h"   
#include <iostream>
#include <vector>

//______________________________________________________________________________
using namespace std;
//______________________________________________________________________________
class Arcs{

    public :  
		vector < Vertex * > vSuivants;
		Vertex* _CoordMin; 
    	Vertex* _CoordMax;

        
		//**********************************************************************
        //Constructeurs et Destructeurs
        //**********************************************************************
        Arcs();

        ~Arcs();


		//**********************************************************************
        //Autres fonctions
        //**********************************************************************

        virtual Type_Geometrie GetTypeGeom() {return arcs_g;}

        void Dessiner(Type_Affichage typeAff_p);
        
        Arcs * Inverse ();  

		bool IsClosed();

        void AddVertex(Vertex * pPoint_p, int ind_p);        

		Vertex* GetCoordMin();

		Vertex* GetCoordMax();

		void CalculerExtremes();

		void MajExtremes(Vertex * pPoint_p);

		Vertex* GetVertex(int i);

		int Size();



};       
//______________________________________________________________________________
#endif