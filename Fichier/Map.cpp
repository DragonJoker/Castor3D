#include "CTerrain.h"


int terrainLINE;
int terrainTRIANGLE;

CTerrain :: CTerrain(void)
{
	Gl_triangle_strip = true;
	Espacement = 0;                        //espacement entre 2 pixels voisins   
	Attenuation = 5;
	DivCommuns = NULL;
	NbDivCommuns = 0;
}


CTerrain :: CTerrain(std::string NomFichier )
{
	Gl_triangle_strip = true;
	Espacement = 0;   
	Attenuation = 1;
	DivCommuns = NULL;
	NbDivCommuns = 0;
	ChargerFichier(NomFichier);	

}

CTerrain :: ~CTerrain()
{


}

void CTerrain :: SetCouleur( int x, int y)		
{
	float r = (float)HeightMap[x][y].Red/255.0;
	float v = (float)HeightMap[x][y].Green/255.0;
	float b = (float)HeightMap[x][y].Blue/255.0;

	glColor3d(r, v, b );

}



void CTerrain::Afficher()
{

	if(Gl_triangle_strip){
		glCallList(terrainTRIANGLE);
	}else{
		glCallList(terrainLINE);
	}	
}


/*
void CTerrain ::ChargerFichier(std::string NomFichier)
{

	FILE *pFile = NULL;

	// On ouvre le fichier en lecture binaire
	pFile = fopen( NomFichier.c_str(), "rb" );

	// Si on ne trouve pas le fichier, on quitte
	if ( pFile == NULL )	
	{		
		MessageBox(NULL, "Fichier introuvable", "Erreur", MB_OK);
		return;
	}

	Longeur= LecteurLong(pFile);
	Largeur = LecteurLong(pFile); 
	CalculerDivCommuns();
	

	HeightMap = new POINTMAP*[Largeur];

	for(int i=0; i<Largeur;i++){
		HeightMap[i] = new POINTMAP[Longeur];
	}

	for(i=0; i<Largeur;i++){
		for(int j=0; j<Longeur;j++){
			
			fread( &HeightMap[i][j], 1, 4, pFile );
				
			// En cas d'erreur on afficher un message
			if (ferror( pFile ))
			{
				MessageBox(NULL, "Failed To Get Data!", "Error", MB_OK);
			}
		}
	}
	
	//On ferme le fichier
	fclose(pFile);


	//CREATION DES LISTES D'AFFICHAGE

	//Liste d'affichage du terrain en triangle

	if (glIsList(terrainTRIANGLE))
		glDeleteLists(terrainTRIANGLE,1);
	
	terrainTRIANGLE=glGenLists(1);
	glNewList(terrainTRIANGLE,GL_COMPILE);

	int num = Espacement;
	int Espacement = DivCommuns[num];

	for(i=0; i<Largeur-Espacement; i+=Espacement ){	 
			
		glBegin(GL_TRIANGLE_STRIP);			

		for(int j=0; j<Longeur; j+=Espacement){	
			
			SetCouleur( i, j );	
			glVertex3i(i, HeightMap[i][j].Height/(float)Attenuation , j);
			
			SetCouleur( i+Espacement, j);	
			int cpt = i+Espacement;
			glVertex3i(cpt, HeightMap[cpt][j].Height/(float)Attenuation , j); 			
		}
		
		glEnd();
	}
	
	glEndList();

	//Liste d'affichage du terrain en ligne

	if (glIsList(terrainLINE))
		glDeleteLists(terrainLINE,1);
	
	terrainLINE=glGenLists(1);
	glNewList(terrainLINE,GL_COMPILE);

	num = Espacement;
	Espacement = DivCommuns[num];

	for(i=0; i<Largeur-Espacement; i+=Espacement ){	
		
		glBegin(GL_LINE_STRIP); 			

		for(int j=0; j<Longeur; j+=Espacement){	
			
			SetCouleur( i, j );	
			glVertex3i(i, HeightMap[i][j].Height/(float)Attenuation , j);
			
			SetCouleur( i+Espacement, j);	
			int cpt = i+Espacement;
			glVertex3i(cpt, HeightMap[cpt][j].Height/(float)Attenuation , j); 			
		}
		
		glEnd();
	}
	
	glEndList();	


}
*/


void CTerrain ::ChargerFichier(std::string NomFichier)
{

	FILE *pFile = NULL;

	// On ouvre le fichier en lecture binaire
	pFile = fopen( NomFichier.c_str(), "rb" );

	// Si on ne trouve pas le fichier, on quitte
	if ( pFile == NULL )	
	{		
		MessageBox(NULL, "Fichier introuvable", "Erreur", MB_OK);
		return;
	}

	Longeur= LecteurLong(pFile);
	Largeur = LecteurLong(pFile); 
	CalculerDivCommuns();
	

	HeightMap = new POINTMAP*[Largeur];

	for(int i=0; i<Largeur;i++){
		HeightMap[i] = new POINTMAP[Longeur];
	}

	for(i=0; i<Largeur;i++){
		for(int j=0; j<Longeur;j++){
			
			fread( &HeightMap[i][j], 1, 4, pFile );
				
			// En cas d'erreur on afficher un message
			if (ferror( pFile ))
			{
				MessageBox(NULL, "Failed To Get Data!", "Error", MB_OK);
			}
		}
	}
	
	//On ferme le fichier
	fclose(pFile);


	//CREATION DES LISTES D'AFFICHAGE

	//Liste d'affichage du terrain en triangle
	
	if (glIsList(terrainTRIANGLE))
		glDeleteLists(terrainTRIANGLE,1);
	
	terrainTRIANGLE=glGenLists(1);
	glNewList(terrainTRIANGLE,GL_COMPILE);

	int num = Espacement;
	int Espacement = DivCommuns[num];
	glDisable(GL_LIGHTING);
	for(i=0; i<Largeur-Espacement; i+=Espacement ){	 
			
		glBegin(GL_LINE_STRIP);	//GL_TRIANGLE_STRIP		

		for(int j=0; j<Longeur; j+=Espacement){	
			
			SetCouleur( i, j );	
			glVertex3i(i, HeightMap[i][j].Height/(float)Attenuation , j);
			
			SetCouleur( i+Espacement, j);	
			int cpt = i+Espacement;
			glVertex3i(cpt, HeightMap[cpt][j].Height/(float)Attenuation , j); 			
		}
		
		glEnd();
	}
	
	glEndList();

	//Liste d'affichage du terrain en ligne

	if (glIsList(terrainLINE))
		glDeleteLists(terrainLINE,1);
	
	terrainLINE=glGenLists(1);
	glNewList(terrainLINE,GL_COMPILE);

	num = Espacement;
	Espacement = DivCommuns[num];
	glDisable(GL_LIGHTING);

	for(i=0; i<Largeur-Espacement; i+=Espacement ){	
		
				

		for(int j=0; j<Longeur-Espacement; j+=Espacement){	
			

			glColor3f(1.0f, 0.0f, 0.0f);
			CVertex* v1 = new CVertex(i, 
				                      HeightMap[i][j].Height/(float)Attenuation, 
									  j);

			CVertex* v2 = new CVertex(i+Espacement, 
									  HeightMap[i+Espacement][j].Height/(float)Attenuation, 
									  j);

			CVertex* v3 = new CVertex((i+Espacement), 
									  HeightMap[i+Espacement][j+Espacement].Height/(float)Attenuation, 
									  j+Espacement); 

			CVecteur* Vec1 = new CVecteur(v2, v1);
			CVecteur* Vec2 = new CVecteur(v2, v3);
			CVecteur* Vec3 = Vec1->operator ^(Vec2);
			CVecteur* Vec4 = Vec3->pNormaliser();

			float x_l = (v1->x + v2->x + v3->x)/3;
			float y_l = (v1->y + v2->y + v3->y)/3;
			float z_l = (v1->z + v2->z + v3->z)/3;

			glBegin(GL_LINES);
			glVertex3f(x_l,y_l,z_l);
			glVertex3f(Vec4->GetX()*3+x_l, Vec4->GetY()*3+y_l, Vec4->GetZ()*3+z_l);
/*		
			if(Vec4->GetX()!=0 || Vec4->GetY()!=1 || Vec4->GetZ()!=0){
				MessageBox(NULL,"PB","",MB_OK);
			}
*/

			glEnd();

			delete v1;
			delete v2;
			delete v3;
			delete Vec1;
			delete Vec2;
			delete Vec3;
			delete Vec4;


			glBegin(GL_TRIANGLES);
			SetCouleur( i, j );	
			glVertex3i(i, HeightMap[i][j].Height/(float)Attenuation , j);
			
			SetCouleur( i+Espacement, j);
			glVertex3i(i+Espacement, HeightMap[i+Espacement][j].Height/(float)Attenuation , j); 

			SetCouleur( i+Espacement, j+Espacement);
			glVertex3i((i+Espacement), HeightMap[i+Espacement][j+Espacement].Height/(float)Attenuation , j+Espacement); 
		
			glEnd();



			glBegin(GL_TRIANGLES); 

			SetCouleur( i, j );	
			glVertex3i(i, HeightMap[i][j].Height/(float)Attenuation , j);

			SetCouleur( i+Espacement, j+Espacement);			
			glVertex3i(i+Espacement, HeightMap[i+Espacement][j+Espacement].Height/(float)Attenuation , j+Espacement); 

			SetCouleur( i, j+Espacement);			
			glVertex3i(i, HeightMap[i][j+Espacement].Height/(float)Attenuation , j+Espacement); 
					
			glEnd();
	


		}
		
		
	}
	
	glEndList();	


}

LONG CTerrain::LecteurLong(FILE *pFile)
{

	if(pFile == NULL){
		return 0;
	}

	LONG Number;
	fread( &Number, 1, sizeof(LONG), pFile );

	if (ferror( pFile )){
		MessageBox(NULL, "Failed To Set Data!", "Error", MB_OK);
		return false;
	}
	
	return Number;	
}

void CTerrain::SetGlTriangleStrip(bool type)
{
	Gl_triangle_strip = type;
}


void CTerrain::IncEspacement()
{
	if( Espacement+1 < NbDivCommuns ){
		Espacement++;
	}
}

void CTerrain::DecEspacement()
{
	if( Espacement-1 >=0 ){
		Espacement--;
	}
}

void CTerrain:: AddDivCommun(int Nbre)
{
	if(DivCommuns==NULL){
		NbDivCommuns = 1;
		DivCommuns = new int[NbDivCommuns];	
		DivCommuns[0] = Nbre;
		return;
	}

	int *TabTmp = new int[NbDivCommuns+1];
	
	for(int i=0; i<NbDivCommuns ;i++){
		TabTmp[i] = DivCommuns[i];	
	}

	TabTmp[NbDivCommuns] = Nbre;
	NbDivCommuns++;

	delete(DivCommuns);
	DivCommuns = TabTmp;


}


void CTerrain:: CalculerDivCommuns()
{
	int Nbre1 = 0;
	int Nbre2 = 0;

	if(Longeur < Largeur){
		Nbre1 = Longeur;
		Nbre2 = Largeur;
	}else{
		Nbre1 = Largeur;
		Nbre2 = Longeur;
	}

	for(int i=1; i<=Nbre1; i++){		
		if((Nbre1%i) == 0 && (Nbre2%i) == 0 ){
			AddDivCommun(i);
		}
	}
}



CVertex* CTerrain::CalculerCentre(CVertex* v1, CVertex* v2, CVertex* v3)
{
	CVertex* v1v2Centre = new CVertex((v1->GetX()+v2->GetX())/2,
		                              (v1->GetY()+v2->GetY())/2,
		                              (v1->GetZ()+v2->GetZ())/2);

	CVertex* v1v3Centre = new CVertex((v1->GetX()+v3->GetX())/2,
		                              (v1->GetY()+v3->GetY())/2,
		                              (v1->GetZ()+v3->GetZ())/2);

	return NULL;
}