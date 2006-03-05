// Global.cpp: implementation of the CGlobal class.
//
//////////////////////////////////////////////////////////////////////

#include "Global.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//______________________________________________________________________________
CGlobal::CGlobal()
{
	sIdGeo = 0;
	GeoActu=NULL;
}
//______________________________________________________________________________
CGlobal::~CGlobal()
{
	vGeometries.erase(vGeometries.begin(), vGeometries.end());
	vVertexSelec.erase(vVertexSelec.begin(), vVertexSelec.end());
}
//______________________________________________________________________________
void CGlobal::AjouterGeo (Figure * fig)
{
	GEOELE geom;
	geom.Fig = fig;
	geom.isHidden = false;
	vGeometries.push_back(geom);
}
//______________________________________________________________________________
void CGlobal::Dessiner(Type_Affichage typeAff_p)
{
	int i;

	//On dessine en rouge les vertex selectionnés pour les vues 2D	
	if(typeAff_p != facesTriangle){
		
		glColor3f(1.0f, 0.0f, 0.0f);
		glPointSize (3.0); 
	
		glBegin(GL_POINTS);
		for (i = 0 ; i < vVertexSelec.size(); i++) {
			glVertex3f(vVertexSelec[i]->x, vVertexSelec[i]->y, vVertexSelec[i]->z ); 
		}
		glEnd();
	}

	//On dessine la figure en cours de création
	if(GeoActu!=NULL){
		GeoActu->Dessiner(typeAff_p);
	}

	//On dessine les objets
	for (i = 0 ; i < vGeometries.size(); i++) {
		GEOELE geotmp = vGeometries[i];
		if (!geotmp.isHidden) {
			//(geotmp.Fig)->DessinerKillBox();
			(geotmp.Fig)->Dessiner(typeAff_p);
		}
	}



}
//______________________________________________________________________________
void CGlobal::ViderSelection()
{
	ViderFigureEnModif();
	vVertexSelec.erase(vVertexSelec.begin(), vVertexSelec.end());

}

//______________________________________________________________________________
void CGlobal::ViderFigureEnModif()
{
	vFigEnModif.erase(vFigEnModif.begin(), vFigEnModif.end());
}

//______________________________________________________________________________
void CGlobal::SelectionnerGeoXY(float x1_p, float x2_p, float y1_p, float y2_p)
{
	int i;
	Vertex* CoordMin = NULL;
	Vertex* CoordMax = NULL;

	ViderSelection();
	ViderFigureEnModif();

	for (i = 0 ; i < vGeometries.size(); i++) {
		
		GEOELE geotmp = vGeometries[i];	

		if (!geotmp.isHidden) {

			Figure* MaFigure =(Figure*)geotmp.Fig;
			MaFigure->GetExtremes(CoordMin, CoordMax);
			
			if(AVerifierXY(x1_p, x2_p, y1_p, y2_p, CoordMin, CoordMax))
			{
				SelectionnerFig(geotmp.Fig);
			}
		}

	}	
}
//______________________________________________________________________________
void CGlobal::SelectionnerGeoXZ(float x1_p, float x2_p, float z1_p, float z2_p)
{
	int i;
	Vertex* CoordMin = NULL;
	Vertex* CoordMax = NULL;

	ViderSelection();

	for (i = 0 ; i < vGeometries.size(); i++) {
		
		GEOELE geotmp = vGeometries[i];	

		if (!geotmp.isHidden) {

			Figure* MaFigure =(Figure*)geotmp.Fig;
			MaFigure->GetExtremes(CoordMin, CoordMax);
			
			if(AVerifierXZ(x1_p, x2_p, z1_p, z2_p, CoordMin, CoordMax))
			{
				SelectionnerFig(geotmp.Fig);
			}
		}

	}
}
//______________________________________________________________________________
void CGlobal::SelectionnerGeoZY(float z1_p, float z2_p, float y1_p, float y2_p)
{
	int i;
	Vertex* CoordMin = NULL;
	Vertex* CoordMax = NULL;

	ViderSelection();

	for (i = 0 ; i < vGeometries.size(); i++) {
		
		GEOELE geotmp = vGeometries[i];	

		if (!geotmp.isHidden) {

			Figure* MaFigure =(Figure*)geotmp.Fig;
			MaFigure->GetExtremes(CoordMin, CoordMax);
			
			if(AVerifierZY(z1_p, z2_p, y1_p, y2_p, CoordMin, CoordMax))
			{
				SelectionnerFig(geotmp.Fig);
			}
		}

	}
}
//______________________________________________________________________________


void CGlobal::SelectionnerVertexXY(float x1_p, float x2_p, float y1_p, float y2_p)
{
	int i;
	int j;
	Vertex* CoordMin = NULL;
	Vertex* CoordMax = NULL;

	ViderSelection();

	int NbGeometries = vGeometries.size();

	for (i = 0 ; i < NbGeometries; i++) {
		
		GEOELE geotmp = vGeometries[i];	

		if (!geotmp.isHidden) {

			Figure* MaFigure =(Figure*)geotmp.Fig;
			MaFigure->GetExtremes(CoordMin, CoordMax);
			
			//Si la figure vaut la peine d'etre controlé
			if(AVerifierXY(x1_p, x2_p, y1_p, y2_p, CoordMin, CoordMax))
			{
				int NbVertex = MaFigure->vMesVertex.size();
				vFigEnModif.push_back(MaFigure);

				for(j=0; j<NbVertex;j++)
				{
					float xVertexActu = MaFigure->vMesVertex[j]->x;
					float yVertexActu = MaFigure->vMesVertex[j]->y;

					if(x1_p <= xVertexActu && xVertexActu <= x2_p && y1_p <= yVertexActu && yVertexActu <= y2_p)
					{
						vVertexSelec.push_back(MaFigure->vMesVertex[j]);
					}
				}
			}
		}
	}
}

//______________________________________________________________________________


void CGlobal::SelectionnerVertexXZ(float x1_p, float x2_p, float z1_p, float z2_p)
{
	int i;
	int j;
	Vertex* CoordMin = NULL;
	Vertex* CoordMax = NULL;

	ViderSelection();

	int NbGeometries = vGeometries.size();

	for (i = 0 ; i < NbGeometries; i++) {
		
		GEOELE geotmp = vGeometries[i];	

		if (!geotmp.isHidden) {

			Figure* MaFigure =(Figure*)geotmp.Fig;
			MaFigure->GetExtremes(CoordMin, CoordMax);
			
			if(AVerifierXZ(x1_p, x2_p, z1_p, z2_p, CoordMin, CoordMax))
			{
				int NbVertex = MaFigure->vMesVertex.size();
				vFigEnModif.push_back(MaFigure);
				
				for(j=0; j<NbVertex;j++)
				{
					float xVertexActu = MaFigure->vMesVertex[j]->x;
					float zVertexActu = MaFigure->vMesVertex[j]->z;

					if(x1_p <= xVertexActu && xVertexActu <= x2_p && z1_p <= zVertexActu && zVertexActu <= z2_p)
					{
						vVertexSelec.push_back(MaFigure->vMesVertex[j]);
					}
				}
			}
		}
	}
}

//______________________________________________________________________________


void CGlobal::SelectionnerVertexZY(float z1_p, float z2_p, float y1_p, float y2_p)
{
	int i;
	int j;
	Vertex* CoordMin = NULL;
	Vertex* CoordMax = NULL;

	ViderSelection();

	int NbGeometries = vGeometries.size();


	for (i = 0 ; i < NbGeometries; i++) {
		
		GEOELE geotmp = vGeometries[i];	

		if (!geotmp.isHidden) {

			Figure* MaFigure =(Figure*)geotmp.Fig;
			MaFigure->GetExtremes(CoordMin, CoordMax);
			
			if(AVerifierZY(z1_p, z2_p, y1_p, y2_p, CoordMin, CoordMax))
			{
				int NbVertex = MaFigure->vMesVertex.size();
				vFigEnModif.push_back(MaFigure);

				for(j=0; j<NbVertex;j++)
				{
					float zVertexActu = MaFigure->vMesVertex[j]->z;
					float yVertexActu = MaFigure->vMesVertex[j]->y;
					
					if(y1_p <= yVertexActu && yVertexActu <= y2_p && z1_p <= zVertexActu && zVertexActu <= z2_p)
					{
						vVertexSelec.push_back(MaFigure->vMesVertex[j]);
					}
				}
			}
		}
	}
}

//______________________________________________________________________________

bool CGlobal::AVerifierXY(float x1_p, float x2_p, float y1_p, float y2_p, Vertex* CoordMin_p, Vertex* CoordMax_p)
{

	//On regarde si l'un des points de la killbox est dans la selection
	if(CoordMin_p == NULL || CoordMax_p == NULL){
		return false;
	}

	bool Test1 = (x1_p <= CoordMin_p->x && CoordMin_p->x <= x2_p);
	bool Test2 = (y1_p <= CoordMin_p->y && CoordMin_p->y <= y2_p);
	bool Test3 = (x1_p <= CoordMax_p->x && CoordMax_p->x <= x2_p);
	bool Test4 = (y1_p <= CoordMax_p->y && CoordMax_p->y <= y2_p);

	if(Test1 && Test2){
		return true;
	}
	if(Test1 && Test4){
		return true;
	}
	if(Test3 && Test2){
		return true;
	}
	if(Test3 && Test4){
		return true;
	}

	//On regarde si l'un des points de la selection est dans la killbox
	Test1 = (CoordMin_p->x <= x1_p && x1_p <= CoordMax_p->x);
	Test2 = (CoordMin_p->y <= y1_p && y1_p <= CoordMax_p->y);
	Test3 = (CoordMin_p->x <= x2_p && x2_p <= CoordMax_p->x);
	Test4 = (CoordMin_p->y <= y2_p && y2_p <= CoordMax_p->y);

	if(Test1 && Test2){
		return true;
	}
	if(Test1 && Test4){
		return true;
	}
	if(Test3 && Test2){
		return true;
	}
	if(Test3 && Test4){
		return true;
	}

	return false;
}
//______________________________________________________________________________

bool CGlobal::AVerifierXZ(float x1_p, float x2_p, float z1_p, float z2_p, Vertex* CoordMin_p, Vertex* CoordMax_p)
{
	//On regarde si l'un des points de la killbox est dans la selection
	if(CoordMin_p == NULL || CoordMax_p == NULL){
		return false;
	}

	bool Test1 = (x1_p <= CoordMin_p->x && CoordMin_p->x <= x2_p);
	bool Test2 = (z1_p <= CoordMin_p->z && CoordMin_p->z <= z2_p);
	bool Test3 = (x1_p <= CoordMax_p->x && CoordMax_p->x <= x2_p);
	bool Test4 = (z1_p <= CoordMax_p->z && CoordMax_p->z <= z2_p);

	if(Test1 && Test2){
		return true;
	}
	if(Test1 && Test4){
		return true;
	}
	if(Test3 && Test2){
		return true;
	}
	if(Test3 && Test4){
		return true;
	}

	//On regarde si l'un des points de la selection est dans la killbox
	Test1 = (CoordMin_p->x <= x1_p && x1_p <= CoordMax_p->x);
	Test2 = (CoordMin_p->z <= z1_p && z1_p <= CoordMax_p->z);
	Test3 = (CoordMin_p->x <= x2_p && x2_p <= CoordMax_p->x);
	Test4 = (CoordMin_p->z <= z2_p && z2_p <= CoordMax_p->z);

	if(Test1 && Test2){
		return true;
	}
	if(Test1 && Test4){
		return true;
	}
	if(Test3 && Test2){
		return true;
	}
	if(Test3 && Test4){
		return true;
	}


	return false;

}

//______________________________________________________________________________

bool CGlobal::AVerifierZY(float z1_p, float z2_p, float y1_p, float y2_p, Vertex* CoordMin_p, Vertex* CoordMax_p)
{
	//On regarde si l'un des points de la killbox est dans la selection
	if(CoordMin_p == NULL || CoordMax_p == NULL){
		return false;
	}

	bool Test1 = (z1_p <= CoordMin_p->z && CoordMin_p->z <= z2_p);
	bool Test2 = (y1_p <= CoordMin_p->y && CoordMin_p->y <= y2_p);
	bool Test3 = (z1_p <= CoordMax_p->z && CoordMax_p->z <= z2_p);
	bool Test4 = (y1_p <= CoordMax_p->y && CoordMax_p->y <= y2_p);

	if(Test1 && Test2){
		return true;
	}
	if(Test1 && Test4){
		return true;
	}
	if(Test3 && Test2){
		return true;
	}
	if(Test3 && Test4){
		return true;
	}


	//On regarde si l'un des points de la selection est dans la killboz
	Test1 = (CoordMin_p->z <= z1_p && z1_p <= CoordMax_p->z);
	Test2 = (CoordMin_p->y <= y1_p && y1_p <= CoordMax_p->y);
	Test3 = (CoordMin_p->z <= z2_p && z2_p <= CoordMax_p->z);
	Test4 = (CoordMin_p->y <= y2_p && y2_p <= CoordMax_p->y);

	if(Test1 && Test2){
		return true;
	}
	if(Test1 && Test4){
		return true;
	}
	if(Test3 && Test2){
		return true;
	}
	if(Test3 && Test4){
		return true;
	}

	return false;
}

//______________________________________________________________________________

void CGlobal::TranslaterSelection(Vecteur* VectTranslation)
{
	int nbVertex = vVertexSelec.size();
	int nbFigureModif = vFigEnModif.size();
	int i = 0;


	for (i = 0 ; i < nbVertex; i++) {
		VectTranslation->TranslaterVertex(vVertexSelec[i]);		
	}

	for (i = 0 ; i < nbFigureModif; i++) {
		vFigEnModif[i]->CalculerExtremes();		
	}	

	
}

//______________________________________________________________________________

void CGlobal::SelectionnerFig(Figure* MaFigure)
{
	int Size = MaFigure->vMesVertex.size();
	vFigEnModif.push_back(MaFigure);

	for(int i=0; i<Size; i++)
	{
		vVertexSelec.push_back(MaFigure->vMesVertex[i]);			
	}

}

//______________________________________________________________________________
void CGlobal::SupprimerGeoSel()
{

	int l_NbGeoASupp = vFigEnModif.size();
	int l_NbGeoTotal = vGeometries.size();	
	vector <GEOELE>::iterator Iter;
	bool trouve;	

	for(int i=0; i< l_NbGeoASupp; i++)
	{
		trouve = false;

		for( Iter = vGeometries.begin( ); !trouve && Iter != vGeometries.end( ) ; Iter++ ){
			
			if((*Iter).Fig == vFigEnModif[i]){
				
				trouve=true;
				delete((*Iter).Fig);
		   		(*Iter).Fig = NULL;
				vGeometries.erase(Iter);			
			}		
		}
	}

	ViderSelection();
}

//______________________________________________________________________________
