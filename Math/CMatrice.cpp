#include "CMatrice.h"


CMatrice4::CMatrice4(void)
{
	Matrice = new double * [ 4 ];
	
	for( int i = 0; i < 4; i++){
		Matrice[ i ] = new double[4];
    }

	Initialiser();
}

CMatrice4:: ~CMatrice4(void)
{
	for(int i=0; i<4; i++)
	{
		delete Matrice[i];
	}

	delete Matrice;

}


void CMatrice4::Initialiser(void)
{
	for( int i = 0; i < 4; i++){
		for( int j = 0; j < 4; j++){
			Matrice[i][j] = .0f;
		}
    }
}


void CMatrice4::Identite(void)
{
	Initialiser();

	for(int i=0; i<4; i++){
		Matrice[i][i] = 1;
	}

}

void CMatrice4::MatriceRotX(double Angle)
{
	//Ligne 1
	Matrice[0][0] = 1;
	Matrice[0][1] = 0;
	Matrice[0][2] = 0;
	Matrice[0][3] = 0;

	//Ligne 2
	Matrice[1][0] = 0;
	Matrice[1][1] = cos(Angle);
	Matrice[1][2] = -sin(Angle);
	Matrice[1][3] = 0;

	//Ligne 3
	Matrice[2][0] = 0;
	Matrice[2][1] = -Matrice[1][2];
	Matrice[2][2] = Matrice[1][1];
	Matrice[2][3] = 0;

	//Ligne 4
	Matrice[3][0] = 0;
	Matrice[3][1] = 0;
	Matrice[3][2] = 0;
	Matrice[3][3] = 1;
}
		
void CMatrice4::MatriceRotY(double Angle)
{
	//Ligne 1
	Matrice[0][0] = cos(Angle);
	Matrice[0][1] = 0;
	Matrice[0][2] = -sin(Angle);
	Matrice[0][3] = 0;

	//Ligne 2
	Matrice[1][0] = 0;
	Matrice[1][1] = 1;
	Matrice[1][2] = 0;
	Matrice[1][3] = 0;

	//Ligne 3
	Matrice[2][0] = -Matrice[0][2];
	Matrice[2][1] = 0;
	Matrice[2][2] = Matrice[0][0];
	Matrice[2][3] = 0;

	//Ligne 4
	Matrice[3][0] = 0;
	Matrice[3][1] = 0;
	Matrice[3][2] = 0;
	Matrice[3][3] = 1;

}
		
void CMatrice4::MatriceRotZ(double Angle)
{
	//Ligne 1
	Matrice[0][0] = cos(Angle);
	Matrice[0][1] = -sin(Angle);
	Matrice[0][2] = 0;
	Matrice[0][3] = 0;

	//Ligne 2
	Matrice[1][0] = -Matrice[0][1];
	Matrice[1][1] = Matrice[0][0];
	Matrice[1][2] = 0;
	Matrice[1][3] = 0;

	//Ligne 3
	Matrice[2][0] = 0;
	Matrice[2][1] = 0;
	Matrice[2][2] = 1;
	Matrice[2][3] = 0;

	//Ligne 4
	Matrice[3][0] = 0;
	Matrice[3][1] = 0;
	Matrice[3][2] = 0;
	Matrice[3][3] = 1;
}

void CMatrice4::Multiplication(Vertex* &vertex )
{
	float x = (float)(Matrice[0][0]*vertex->x + Matrice[0][1]*vertex->y + Matrice[0][2]*vertex->z + Matrice[0][3]*1);
	float y = (float)(Matrice[1][0]*vertex->x + Matrice[1][1]*vertex->y + Matrice[1][2]*vertex->z + Matrice[1][3]*1);
	float z = (float)(Matrice[2][0]*vertex->x + Matrice[2][1]*vertex->y + Matrice[2][2]*vertex->z + Matrice[2][3]*1);

	vertex->x =x;
	vertex->y =y;
	vertex->z =z;
}


Vertex Multiplication(CMatrice4 Matrice, Vertex vertex )
{
	Vertex VertexRetour;

	float x = (float)(Matrice.Matrice[0][0]*vertex.x + Matrice.Matrice[0][1]*vertex.y + Matrice.Matrice[0][2]*vertex.z + Matrice.Matrice[0][3]*1);
	float y = (float)(Matrice.Matrice[1][0]*vertex.x + Matrice.Matrice[1][1]*vertex.y + Matrice.Matrice[1][2]*vertex.z + Matrice.Matrice[1][3]*1);
	float z = (float)(Matrice.Matrice[2][0]*vertex.x + Matrice.Matrice[2][1]*vertex.y + Matrice.Matrice[2][2]*vertex.z + Matrice.Matrice[2][3]*1);

	VertexRetour.x =x;
	VertexRetour.y =y;
	VertexRetour.z =z;

	return VertexRetour; 

}
