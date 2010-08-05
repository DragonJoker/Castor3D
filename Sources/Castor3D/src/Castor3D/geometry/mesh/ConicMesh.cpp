#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/ConicMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"

#include "Log.h"

using namespace Castor3D;


ConicMesh :: ConicMesh( float height_p, float radius_p, unsigned int nbFaces_p,
						const String & p_name)
	:	Mesh( p_name),
		m_nbFaces( nbFaces_p),
		m_height( height_p),
		m_radius( radius_p)
{
	m_meshType = eCone;
	GeneratePoints();
}



ConicMesh :: ~ConicMesh()
{
}

// TODO => Vérifier la génération des coordonnées de texture

void ConicMesh :: GeneratePoints()
{
	float angleRotation = (M_PI_MULT_2) / m_nbFaces;

	Submesh * l_submesh = CreateSubmesh( 2);

	unsigned int cpt = 0;
	//etape 1 : On calcule les coordonn,es des points
	Vector3f * vTmp_l;
	for (float dAlphaI = 0 ; dAlphaI < M_PI_MULT_2 && cpt < m_nbFaces ; dAlphaI += angleRotation)
	{
		vTmp_l = l_submesh->AddVertex( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		cpt++;
	}
	Vector3f * pBottomCenter_l = l_submesh->AddVertex( 0.0, 0.0, 0.0);
	Vector3f * pTopCenter_l = l_submesh->AddVertex( 0.0f, m_height, 0.0f);

	//etape 2 : on reconstitue les faces
	Face * l_pFace;
	unsigned int i;
	size_t l_nbFaces = 0;
	float l_currentTexIndex = 1.0;
	float l_previousTexIndex = 1.0;

	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();

	if (m_height < 0)
	{
		m_height = -m_height;
	}
	Vector3fPtrArray l_vertex = l_submesh->m_vertex;
	//Composition des faces du bas
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_pFace = l_submesh->AddFace( l_vertex[i+1], l_vertex[i], pBottomCenter_l, 0);
		SetTexCoordV2( l_pFace, (1.0f + l_vertex[i]->x / m_radius) / 2.0f, (1.0f + l_vertex[i]->z / m_radius) / 2.0f);
		SetTexCoordV1( l_pFace, (1.0f + l_vertex[i+1]->x / m_radius) / 2.0f, (1.0f + l_vertex[i+1]->z / m_radius) / 2.0f);
		SetTexCoordV3( l_pFace, 0.5f, 0.5f);

		l_nbFaces += 1;
	}
	l_pFace = l_submesh->AddFace( l_vertex[0], l_vertex[m_nbFaces-1], pBottomCenter_l, 0);
	SetTexCoordV2( l_pFace, (1.0f + l_vertex[m_nbFaces-1]->x / m_radius) / 2.0f, (1.0f + l_vertex[m_nbFaces-1]->z / m_radius) / 2.0f);
	SetTexCoordV1( l_pFace, (1.0f + l_vertex[0]->x / m_radius) / 2.0f, (1.0f + l_vertex[0]->z / m_radius) / 2.0f);
	SetTexCoordV3( l_pFace, 0.5, 0.5);
	l_nbFaces += 1;

	//Composition des faces des côt,s
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_currentTexIndex -= 1.0f / ((float)m_nbFaces);
		l_pFace = l_submesh->AddFace( l_vertex[i], l_vertex[i+1], pTopCenter_l, 1);
		SetTexCoordV2( l_pFace, l_currentTexIndex, 0.0f);
		SetTexCoordV1( l_pFace, l_previousTexIndex, 0.0f);
		SetTexCoordV3( l_pFace, l_currentTexIndex, 1.0f);

		l_previousTexIndex = l_currentTexIndex;
		l_nbFaces += 1;
	}
	l_pFace = l_submesh->AddFace( l_vertex[i], l_vertex[0], pTopCenter_l, 1);
	SetTexCoordV2( l_pFace, 0.0, 0.0);
	SetTexCoordV1( l_pFace, l_currentTexIndex, 0.0);
	SetTexCoordV3( l_pFace, 0.0, 1.0);

	l_nbFaces += 1;

	l_submesh->GenerateBuffers();

	Log::LogMessage( C3D_T( "Cone - %s - Nb Vertex : %d, Nb Faces : %d"), m_name.c_str(), l_vertex.size(), l_nbFaces);
}



void ConicMesh :: Modify( float p_radius, float p_height, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	m_height = p_height;
	Cleanup();
	GeneratePoints();
}