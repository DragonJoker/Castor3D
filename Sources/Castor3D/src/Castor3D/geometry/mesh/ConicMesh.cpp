#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/ConicMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Vertex.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"


using namespace Castor::Math;
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
	//etape 1 : On calcule les coordonnées des points
	for (real dAlphaI = 0 ; dAlphaI < Angle::PiMult2 && cpt < m_nbFaces ; dAlphaI += angleRotation)
	{
		l_submesh->AddVertex( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		cpt++;
	}

	Point3r & l_pBottomCenter = l_submesh->AddVertex( 0.0, 0.0, 0.0);
	size_t l_uiBottomCenterIndex = l_submesh->GetVertices().size() - 1;
	Point3r & l_pTopCenter = l_submesh->AddVertex( 0.0f, m_height, 0.0f);
	size_t l_uiTopCenterIndex = l_submesh->GetVertices().size() - 1;

	//etape 2 : on reconstitue les faces
	unsigned int i;
	size_t l_nbFaces = 0;
	real l_currentTexIndex = 1.0;
	real l_previousTexIndex = 1.0;
/*
	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();
*/
	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();

	if (m_height < 0)
	{
		m_height = -m_height;
	}

	const Point3rArray & l_vertex = l_submesh->GetVertices();
	//Composition des faces du bas
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		Face & l_faceA = l_submesh->AddFace( i + 1, i, l_uiBottomCenterIndex, 0);
		l_faceA.SetVertexTexCoords( 0, (1.0f + l_vertex[i+1][0] / m_radius) / 2.0f, (1.0f + l_vertex[i+1][1] / m_radius) / 2.0f);
		l_faceA.SetVertexTexCoords( 1, (1.0f + l_vertex[i][0] / m_radius) / 2.0f, (1.0f + l_vertex[i][2] / m_radius) / 2.0f);
		l_faceA.SetVertexTexCoords( 2, 0.5f, 0.5f);

		l_nbFaces += 1;
	}

	Face & l_faceB = l_submesh->AddFace( 0, m_nbFaces - 1, l_uiBottomCenterIndex, 0);
	l_faceB.SetVertexTexCoords( 0, (1.0f + l_vertex[0][0] / m_radius) / 2.0f, (1.0f + l_vertex[0][2] / m_radius) / 2.0f);
	l_faceB.SetVertexTexCoords( 1, (1.0f + l_vertex[m_nbFaces-1][0] / m_radius) / 2.0f, (1.0f + l_vertex[m_nbFaces-1][2] / m_radius) / 2.0f);
	l_faceB.SetVertexTexCoords( 2, 0.5, 0.5);
	l_nbFaces += 1;

	//Composition des faces des côt,s
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_currentTexIndex -= 1.0f / ((real)m_nbFaces);
		Face & l_faceA = l_submesh->AddFace( i, i + 1, l_uiTopCenterIndex, 1);
		l_faceA.SetVertexTexCoords( 0, l_previousTexIndex, 0.0f);
		l_faceA.SetVertexTexCoords( 1, l_currentTexIndex, 0.0f);
		l_faceA.SetVertexTexCoords( 2, l_currentTexIndex, 1.0f);

		l_previousTexIndex = l_currentTexIndex;
		l_nbFaces += 1;
	}

	Face & l_faceC = l_submesh->AddFace( i, 0, l_uiTopCenterIndex, 1);
	l_faceC.SetVertexTexCoords( 0, l_currentTexIndex, 0.0);
	l_faceC.SetVertexTexCoords( 1, 0.0, 0.0);
	l_faceC.SetVertexTexCoords( 2, 0.0, 1.0);

	l_nbFaces += 1;

	l_submesh->GenerateBuffers();

	Logger::LogMessage( CU_T( "Cone - %s - Nb Vertex : %d, Nb Faces : %d"), m_name.char_str(), l_vertex.size(), l_nbFaces);
}

void ConicMesh :: Modify( real p_radius, real p_height, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	m_height = p_height;
	Cleanup();
	GeneratePoints();
}