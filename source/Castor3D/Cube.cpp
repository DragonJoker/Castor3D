#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Cube.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"



using namespace Castor3D;

Cube :: Cube()
	:	MeshCategory( eMESH_TYPE_CUBE)
	,	m_width	( 0)
	,	m_height( 0)
	,	m_depth	( 0)
{
}

Cube :: ~Cube()
{
}

MeshCategoryPtr Cube :: Clone()
{
	return MeshCategoryPtr( new Cube);
}

void Cube :: GeneratePoints()
{
	int CptNegatif = 0;
	if (m_width < 0)
	{
		CptNegatif++;
	}
	if (m_height < 0)
	{
		CptNegatif++;
	}
	if (m_depth < 0)
	{
		CptNegatif++;
	}

	SubmeshPtr l_pSubmesh = GetMesh()->CreateSubmesh( 6);

    //Calcul des coordonnées des 8 sommets du pav,
	l_pSubmesh->AddPoint( -m_width / 2,	-m_height / 2, -m_depth / 2);
	l_pSubmesh->AddPoint(  m_width / 2,	-m_height / 2, -m_depth / 2);
	l_pSubmesh->AddPoint(  m_width / 2,	-m_height / 2,	m_depth / 2);
	l_pSubmesh->AddPoint( -m_width / 2,	-m_height / 2,	m_depth / 2);
	l_pSubmesh->AddPoint( -m_width / 2,	 m_height / 2,	m_depth / 2);
	l_pSubmesh->AddPoint( -m_width / 2,	 m_height / 2, -m_depth / 2);
	l_pSubmesh->AddPoint(  m_width / 2,	 m_height / 2, -m_depth / 2);
	l_pSubmesh->AddPoint(  m_width / 2,	 m_height / 2,	m_depth / 2);

	unsigned int l_nbTrianglesCoords = 12 * 3 * 3;
	unsigned int l_nbTrianglesTexCoords = 12 * 3 * 2;
	unsigned int l_nbLinesCoords = 12 * 6 * 3;
	unsigned int l_nbLinesTexCoords = 12 * 6 * 2;

	//CONSTRUCTION FACES /!\ Pour OpenGL le Z est inversé

	if (CptNegatif == 1 || CptNegatif == 3)
	{
		// Faces du bas
		l_pSubmesh->AddQuadFace( 0, 1, 2, 3, 0);

		// Face du haut
		l_pSubmesh->AddQuadFace( 4, 7, 6, 5, 1);

		// Face de derrière
		l_pSubmesh->AddQuadFace( 5, 6, 1, 0, 2);

		// Face de devant
		l_pSubmesh->AddQuadFace( 3, 2, 7, 4, 3);

		// Faces de droite
		l_pSubmesh->AddQuadFace( 6, 7, 2, 1, 4);

		// Face de gauche
		l_pSubmesh->AddQuadFace( 0, 3, 4, 5, 5);
	}
	else
	{
		// Faces du bas
		l_pSubmesh->AddQuadFace( 3, 2, 1, 0, 0);

		// Face du haut
		l_pSubmesh->AddQuadFace( 5, 6, 7, 4, 1);

		// Face de derrière
		l_pSubmesh->AddQuadFace( 0, 1, 6, 5, 2);

		// Face de devant
		l_pSubmesh->AddQuadFace( 4, 7, 2, 3, 3);

		// Faces de droite
		l_pSubmesh->AddQuadFace( 1, 2, 7, 6, 4);

		// Face de gauche
		l_pSubmesh->AddQuadFace( 5, 4, 3, 0, 5);
	}

	l_pSubmesh->GenerateBuffers();

	Logger::LogMessage( cuT( "Cube - NbVertex : %d"), l_pSubmesh->GetNbPoints());
}

void Cube :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
	m_width = p_arrayDimensions[0];
	m_height = p_arrayDimensions[1];
	m_depth = p_arrayDimensions[2];
	GetMesh()->Cleanup();
	GeneratePoints();
}