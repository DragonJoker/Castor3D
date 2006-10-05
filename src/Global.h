// Global.h: interface for the CGlobal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBAL_H__A4111CD2_9998_43F9_97FF_BAC640EAF952__INCLUDED_)
#define AFX_GLOBAL_H__A4111CD2_9998_43F9_97FF_BAC640EAF952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include <iostream>


#include "../Math/CMatrice.h"
#include "../Geometrie/Primitive/Pave.h"
#include "../Geometrie/Primitive/Cylindre.h"
#include "../Geometrie/Primitive/Cone.h"
#include "../Geometrie/Primitive/Sphere.h"
#include "../Geometrie/Primitive/Plan.h"
#include "../Geometrie/Primitive/Icosaedre.h"
#include "../Geometrie/Primitive/Terrain.h"

typedef struct {
	Figure * Fig;
	bool isHidden;
} GEOELE;

class CGlobal  
{
private:
	vector <GEOELE> vGeometries;
	vector <Vertex*> vVertexSelec;
	vector <Figure*> vFigEnModif;
	int sIdGeo;

	int IdListe;
	int IdListe2;

public:
	Figure * GeoActu;

public:

	CGlobal();

	virtual ~CGlobal();

	void ViderSelection();

	void ViderFigureEnModif();

	void AjouterGeo (Figure * fig);

	void SelectionnerGeoXY(float x1_p, float x2_p, float y1_p, float y2_p);

	void SelectionnerGeoXZ(float x1_p, float x2_p, float z1_p, float z2_p);

	void SelectionnerGeoZY(float z1_p, float z2_p, float y1_p, float y2_p);

	void SelectionnerVertexXY(float x1_p, float x2_p, float y1_p, float y2_p);

	void SelectionnerVertexXZ(float x1_p, float x2_p, float z1_p, float z2_p);

	void SelectionnerVertexZY(float z1_p, float z2_p, float y1_p, float y2_p);

	void SupprimerGeoSel();

	//ChercherGeo (Geometrie * geo);

	Figure * GetGeo (int i);

	int GetTailleFigEnModif() {return vFigEnModif.size();}

	void Dessiner(Type_Affichage typeAff_p);

	void TranslaterSelection(Vecteur* VectTranslation);

	void TranslaterSelectionXY(Vecteur* VectTranslation);

	void TranslaterSelectionXZ(Vecteur* VectTranslation);

	void TranslaterSelectionYZ(Vecteur* VectTranslation);

	void TousSelectionner();

	void DupliquerSelection();

	void RotationSelectionX(float angle);

	void RotationSelectionY(float angle);
	
	void RotationSelectionZ(float angle);
	
	bool ChargerSauvegarde(std::string FileName);

	bool EcrireSauvegarde(std::string FileName);

	void CreerListe(Type_Affichage typeAff_p);

	void DessinerLister();

	bool MemeCouleur ();




private:

	bool AVerifierXY(float x1_p, float x2_p, float y1_p, float y2_p, Vertex* CoordMin_p, Vertex* CoordMax_p);

	bool AVerifierXZ(float x1_p, float x2_p, float y1_p, float y2_p, Vertex* CoordMin_p, Vertex* CoordMax_p);

	bool AVerifierZY(float z1_p, float z2_p, float y1_p, float y2_p, Vertex* CoordMin_p, Vertex* CoordMax_p);

	void SelectionnerFig(Figure* MaFigure);





};

#endif // !defined(AFX_GLOBAL_H__A4111CD2_9998_43F9_97FF_BAC640EAF952__INCLUDED_)
