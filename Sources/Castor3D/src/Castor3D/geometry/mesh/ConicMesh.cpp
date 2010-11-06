#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/ConicMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"



using namespace Castor3D;

ConicMesh :: ConicMesh( real height_p, real radius_p, unsigned int nbFaces_p,
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

void ConicMesh :: GeneratePoints()
{
	real angleRotation = real( Angle::PiMult2) / m_nbFaces;

	SubmeshPtr l_submesh = CreateSubmesh( 2);

	unsigned int cpt = 0;
	//etape 1 : On calcule les coordonn,es des points
	VertexPtr vTmp_l;
	for (real dAlphaI = 0 ; dAlphaI < Angle::PiMult2 && cpt < m_nbFaces ; dAlphaI += angleRotation)
	{
		vTmp_l = l_submesh->AddVertex( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		cpt++;
	}

	VertexPtr l_pBottomCenter = l_submesh->AddVertex( 0.0, 0.0, 0.0);
	VertexPtr l_pTopCenter = l_submesh->AddVertex( 0.0f, m_height, 0.0f);

	//etape 2 : on reconstitue les faces
	FacePtr l_pFace;
	unsigned int i;
	size_t l_nbFaces = 0;
	real l_currentTexIndex = 1.0;
	real l_previousTexIndex = 1.0;

	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();

	if (m_height < 0)
	{
		m_height = -m_height;
	}
	VertexPtrArray l_vertex = l_submesh->m_vertex;
	//Composition des faces du bas
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_pFace = l_submesh->AddFace( l_vertex[i+1], l_vertex[i], l_pBottomCenter, 0);
		l_pFace->SetTexCoordV2( (1.0f + l_vertex[i]->m_coords[0] / m_radius) / 2.0f, (1.0f + l_vertex[i]->m_coords[2] / m_radius) / 2.0f);
		l_pFace->SetTexCoordV1( (1.0f + l_vertex[i+1]->m_coords[0] / m_radius) / 2.0f, (1.0f + l_vertex[i+1]->m_coords[1] / m_radius) / 2.0f);
		l_pFace->SetTexCoordV3( 0.5f, 0.5f);

		l_nbFaces += 1;
	}
	l_pFace = l_submesh->AddFace( l_vertex[0], l_vertex[m_nbFaces-1], l_pBottomCenter, 0);
	l_pFace->SetTexCoordV2( (1.0f + l_vertex[m_nbFaces-1]->m_coords[0] / m_radius) / 2.0f, (1.0f + l_vertex[m_nbFaces-1]->m_coords[2] / m_radius) / 2.0f);
	l_pFace->SetTexCoordV1( (1.0f + l_vertex[0]->m_coords[0] / m_radius) / 2.0f, (1.0f + l_vertex[0]->m_coords[2] / m_radius) / 2.0f);
	l_pFace->SetTexCoordV3( 0.5, 0.5);
	l_nbFaces += 1;

	//Composition des faces des côt,s
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_currentTexIndex -= 1.0f / ((real)m_nbFaces);
		l_pFace = l_submesh->AddFace( l_vertex[i], l_vertex[i+1], l_pTopCenter, 1);
		l_pFace->SetTexCoordV2( l_currentTexIndex, 0.0f);
		l_pFace->SetTexCoordV1( l_previousTexIndex, 0.0f);
		l_pFace->SetTexCoordV3( l_currentTexIndex, 1.0f);

		l_previousTexIndex = l_currentTexIndex;
		l_nbFaces += 1;
	}
	l_pFace = l_submesh->AddFace( l_vertex[i], l_vertex[0], l_pTopCenter, 1);
	l_pFace->SetTexCoordV2( 0.0, 0.0);
	l_pFace->SetTexCoordV1( l_currentTexIndex, 0.0);
	l_pFace->SetTexCoordV3( 0.0, 1.0);

	l_nbFaces += 1;

	l_submesh->GenerateBuffers();

	Log::LogMessage( CU_T( "Cone - %s - Nb Vertex : %d, Nb Faces : %d"), m_name.c_str(), l_vertex.size(), l_nbFaces);
}

void ConicMesh :: Modify( real p_radius, real p_height, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	m_height = p_height;
	Cleanup();
	GeneratePoints();
}