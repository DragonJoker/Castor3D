#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/CubicMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"

#include "Log.h"

using namespace Castor3D;


CubicMesh :: CubicMesh( float width_p, float height_p, float depth_p,
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

	Submesh * l_submesh = new Submesh( Root::GetRenderSystem()->CreateSubmeshRenderer(), 6);
	m_submeshes.push_back( l_submesh);

    //Calcul des coordonnées des 8 sommets du pav,
	l_submesh->AddVertex( -m_width,	-m_height,	-m_depth);
	l_submesh->AddVertex( m_width,	-m_height,	-m_depth);
	l_submesh->AddVertex( m_width,	-m_height,	m_depth);
	l_submesh->AddVertex( -m_width,	-m_height,	m_depth);
	l_submesh->AddVertex( -m_width,	m_height,	m_depth);
	l_submesh->AddVertex( -m_width,	m_height,	-m_depth);
	l_submesh->AddVertex( m_width,	m_height,	-m_depth);
	l_submesh->AddVertex( m_width,	m_height,	m_depth);

	unsigned int l_nbTrianglesCoords = 12 * 3 * 3;
	unsigned int l_nbTrianglesTexCoords = 12 * 3 * 2;
	unsigned int l_nbLinesCoords = 12 * 6 * 3;
	unsigned int l_nbLinesTexCoords = 12 * 6 * 2;

	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();

	//CONSTRUCTION FACES /!\ Pour OpenGL le Z est inversé
	Face * l_face;
	Vector3fPtrArray l_vertex = l_submesh->m_vertex;
	if (CptNegatif == 1 || CptNegatif == 3)
	{
		// Faces du bas
		l_face = l_submesh->AddFace( l_vertex[1], l_vertex[3], l_vertex[0], 0);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		l_face = l_submesh->AddFace( l_vertex[2], l_vertex[3], l_vertex[1], 0);
		SetTexCoordV1( l_face, 1.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		// Face du haut
		l_face = l_submesh->AddFace( l_vertex[7], l_vertex[6], l_vertex[5], 1);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		l_face = l_submesh->AddFace( l_vertex[5], l_vertex[4], l_vertex[7], 1);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		// Face de derrišre
		l_face = l_submesh->AddFace( l_vertex[6], l_vertex[1], l_vertex[0], 2);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		l_face = l_submesh->AddFace( l_vertex[0], l_vertex[5], l_vertex[6], 2);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		// Face de devant
		l_face = l_submesh->AddFace( l_vertex[2], l_vertex[7], l_vertex[4], 3);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		l_face = l_submesh->AddFace( l_vertex[4], l_vertex[3], l_vertex[2], 3);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		// Faces de droite
		l_face = l_submesh->AddFace( l_vertex[7], l_vertex[2], l_vertex[1], 4);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		l_face = l_submesh->AddFace( l_vertex[1], l_vertex[6], l_vertex[7], 4);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		// Face de gauche
		l_face = l_submesh->AddFace( l_vertex[3], l_vertex[4], l_vertex[5], 5);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		l_face = l_submesh->AddFace( l_vertex[5], l_vertex[0], l_vertex[3], 5);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);
	}
	else
	{
		// Faces du bas !!
		l_face = l_submesh->AddFace( l_vertex[0], l_vertex[3], l_vertex[1], 0);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 0.0);

		l_face = l_submesh->AddFace( l_vertex[1], l_vertex[3], l_vertex[2], 0);
		SetTexCoordV1( l_face, 1.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		// Face du haut !!
		l_face = l_submesh->AddFace( l_vertex[5], l_vertex[6], l_vertex[7], 1);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		l_face = l_submesh->AddFace( l_vertex[7], l_vertex[4], l_vertex[5], 1);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		// Face de derrišre !!
		l_face = l_submesh->AddFace( l_vertex[0], l_vertex[1], l_vertex[6], 2);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		l_face = l_submesh->AddFace( l_vertex[6], l_vertex[5], l_vertex[0], 2);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		// Face de devant !!
		l_face = l_submesh->AddFace( l_vertex[4], l_vertex[7], l_vertex[2], 3);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		l_face = l_submesh->AddFace( l_vertex[2], l_vertex[3], l_vertex[4], 3);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		// Faces de droite !!
		l_face = l_submesh->AddFace( l_vertex[1], l_vertex[2], l_vertex[7], 4);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);

		l_face = l_submesh->AddFace( l_vertex[7], l_vertex[6], l_vertex[1], 4);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		// Face de gauche !!
		l_face = l_submesh->AddFace( l_vertex[5], l_vertex[4], l_vertex[3], 5);
		SetTexCoordV1( l_face, 1.0, 1.0);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		l_face = l_submesh->AddFace( l_vertex[3], l_vertex[0], l_vertex[5], 5);
		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 1.0);
		SetTexCoordV3( l_face, 1.0, 1.0);
	}

	l_submesh->GenerateBuffers();
	Log::LogMessage( C3D_T( "CubicMesh - %s - NbVertex : %d"), m_name.c_str(), l_submesh->m_vertex.size());
}



void CubicMesh :: Modify( float width_p, float height_p, float depth_p )
{
	m_width = width_p;
	m_height = height_p;
	m_depth = depth_p;
	Cleanup();
	GeneratePoints();
}