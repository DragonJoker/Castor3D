#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/CubicMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"



using namespace Castor3D;

CubicMesh :: CubicMesh( real width_p, real height_p, real depth_p,
						const String & p_name)
	:	Mesh( p_name),
		m_height( height_p),
		m_width	( width_p),
		m_depth	( depth_p)
{
	m_meshType = eCube;
	GeneratePoints();
}

CubicMesh :: ~CubicMesh()
{
}

void CubicMesh :: GeneratePoints()
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

	SubmeshPtr l_submesh = CreateSubmesh( 6);

    //Calcul des coordonnées des 8 sommets du pav,
	l_submesh->AddVertex( -m_width / 2,	-m_height / 2, -m_depth / 2);
	l_submesh->AddVertex(  m_width / 2,	-m_height / 2, -m_depth / 2);
	l_submesh->AddVertex(  m_width / 2,	-m_height / 2,	m_depth / 2);
	l_submesh->AddVertex( -m_width / 2,	-m_height / 2,	m_depth / 2);
	l_submesh->AddVertex( -m_width / 2,	 m_height / 2,	m_depth / 2);
	l_submesh->AddVertex( -m_width / 2,	 m_height / 2, -m_depth / 2);
	l_submesh->AddVertex(  m_width / 2,	 m_height / 2, -m_depth / 2);
	l_submesh->AddVertex(  m_width / 2,	 m_height / 2,	m_depth / 2);

	unsigned int l_nbTrianglesCoords = 12 * 3 * 3;
	unsigned int l_nbTrianglesTexCoords = 12 * 3 * 2;
	unsigned int l_nbLinesCoords = 12 * 6 * 3;
	unsigned int l_nbLinesTexCoords = 12 * 6 * 2;
/*
	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();
*/
	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();

	//CONSTRUCTION FACES /!\ Pour OpenGL le Z est inversé

	if (CptNegatif == 1 || CptNegatif == 3)
	{
		// Faces du bas
		l_submesh->AddQuadFace( 0, 1, 2, 3, 0);

		// Face du haut
		l_submesh->AddQuadFace( 4, 7, 6, 5, 1);

		// Face de derrière
		l_submesh->AddQuadFace( 5, 6, 1, 0, 2);

		// Face de devant
		l_submesh->AddQuadFace( 3, 2, 7, 4, 3);

		// Faces de droite
		l_submesh->AddQuadFace( 6, 7, 2, 1, 4);

		// Face de gauche
		l_submesh->AddQuadFace( 0, 3, 4, 5, 5);
	}
	else
	{
		// Faces du bas
		l_submesh->AddQuadFace( 3, 2, 1, 0, 0);

		// Face du haut
		l_submesh->AddQuadFace( 5, 6, 7, 4, 1);

		// Face de derrière
		l_submesh->AddQuadFace( 0, 1, 6, 5, 2);

		// Face de devant
		l_submesh->AddQuadFace( 4, 7, 2, 3, 3);

		// Faces de droite
		l_submesh->AddQuadFace( 1, 2, 7, 6, 4);

		// Face de gauche
		l_submesh->AddQuadFace( 5, 4, 3, 0, 5);
	}

	l_submesh->GenerateBuffers();

	Logger::LogMessage( CU_T( "CubicMesh - %s - NbVertex : %d"), m_name.char_str(), l_submesh->GetVertices().size());
}



void CubicMesh :: Modify( real width_p, real height_p, real depth_p )
{
	m_width = width_p;
	m_height = height_p;
	m_depth = depth_p;
	Cleanup();
	GeneratePoints();
}