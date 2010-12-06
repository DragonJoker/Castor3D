#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/CylindricMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Pattern.h"
#include "geometry/basic/Vertex.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"



using namespace Castor3D;

CylindricMesh :: CylindricMesh( real height_p, real radius_p, unsigned int nbFaces_p,
								const String & p_name)
	:	Mesh( p_name),
		m_height( height_p),
		m_radius( radius_p),
		m_nbFaces( nbFaces_p)
{
	m_meshType = eCylinder;
	GeneratePoints();
}

CylindricMesh :: ~CylindricMesh()
{
}

void CylindricMesh :: GeneratePoints()
{
	if (m_nbFaces < 2)
	{
		return;
	}

	SubmeshPtr l_submesh = CreateSubmesh( 3);

	//CALCUL DE LA POSITION DES POINTS
	real angleRotation = Angle::PiMult2 / m_nbFaces;
	unsigned int i = 0;

	Point3rPattern l_bottomArc;
	Point3rPattern l_topArc;

	Point3r l_vTmp, l_vTmp2;

	for (real dAlphaI = 0 ; dAlphaI < Angle::PiMult2 && i < m_nbFaces ; dAlphaI += angleRotation) 
	{
		l_vTmp = l_submesh->AddVertex( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		l_bottomArc.AddPoint( l_vTmp, -1);
	
		Point3r l_vTmp2( l_vTmp);
		l_vTmp2[1] += m_height;
		l_submesh->AddVertex( l_vTmp2);
		l_topArc.AddPoint( l_vTmp2, -1);
		i++;
	}

	Point3r & l_pTopCenter = l_submesh->AddVertex( 0.0, 0.0, 0.0);
	size_t l_uiTopCenterIndex = l_submesh->GetVertices().size() - 1;
	l_pTopCenter[1] += m_height;
	Point3r & l_pBottomCenter = l_submesh->AddVertex( 0.0, 0.0, 0.0);
	size_t l_uiBottomCenterIndex = l_submesh->GetVertices().size() - 1;

	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();

	//RECONSTITION DES FACES
	if (m_height < 0)
	{
		m_height = -m_height;
	}

	Point3r l_v1, l_v2, l_v3, l_v4;
	//Composition des extrémités
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		//Composition du bas
		l_v1 = l_bottomArc[i];
		l_v2 = l_bottomArc[i + 1];
		l_v3 = l_pBottomCenter;
		Face & l_faceA = l_submesh->AddFace( i + 1, i, l_uiBottomCenterIndex, 0);
		l_faceA.SetVertexTexCoords( 0, (1.0f + l_v2[0] / m_radius) / 2.0f, (1.0f + l_v2[2] / m_radius) / 2.0f);
		l_faceA.SetVertexTexCoords( 1, (1.0f + l_v1[0] / m_radius) / 2.0f, (1.0f + l_v1[2] / m_radius) / 2.0f);
		l_faceA.SetVertexTexCoords( 2, 0.5f, 0.5f);
		//Composition du dessus
		l_v1 = l_topArc[i];
		l_v2 = l_pTopCenter;
		l_v3 = l_topArc[i + 1];
		Face & l_faceB = l_submesh->AddFace( l_uiTopCenterIndex, m_nbFaces + i, m_nbFaces + i + 1, 2);
		l_faceB.SetVertexTexCoords( 0, 0.5, 0.5);
		l_faceB.SetVertexTexCoords( 1, 1.0f - (1.0f + l_v1[0] / m_radius) / 2.0f, (1.0f + l_v1[2] / m_radius) / 2.0f);
		l_faceB.SetVertexTexCoords( 2, 1.0f - (1.0f + l_v3[0] / m_radius) / 2.0f, (1.0f + l_v3[2] / m_radius) / 2.0f);
	}

	//Composition du bas
	l_v1 = l_bottomArc[m_nbFaces - 1];
	l_v2 = l_bottomArc[0];
	l_v3 = l_pBottomCenter;
	Face & l_faceA = l_submesh->AddFace( 0, m_nbFaces - 1, l_uiBottomCenterIndex, 0);
	l_faceA.SetVertexTexCoords( 0, (1.0f + l_v2[0] / m_radius) / 2.0f, (1.0f + l_v2[2] / m_radius) / 2.0f);
	l_faceA.SetVertexTexCoords( 1, (1.0f + l_v1[0] / m_radius) / 2.0f, (1.0f + l_v1[2] / m_radius) / 2.0f);
	l_faceA.SetVertexTexCoords( 2, 0.5, 0.5);
	//Composition du dessus
	l_v1 = l_topArc[m_nbFaces - 1];
	l_v2 = l_pTopCenter;
	l_v3 = l_topArc[0];
	Face & l_faceB = l_submesh->AddFace( l_uiTopCenterIndex, m_nbFaces + m_nbFaces - 1, m_nbFaces, 2);
	l_faceB.SetVertexTexCoords( 0, 0.5, 0.5);
	l_faceB.SetVertexTexCoords( 1, 1.0f - (1.0f + l_v1[0] / m_radius) / 2.0f, (1.0f + l_v1[2] / m_radius) / 2.0f);
	l_faceB.SetVertexTexCoords( 2, 1.0f - (1.0f + l_v3[0] / m_radius) / 2.0f, (1.0f + l_v3[2] / m_radius) / 2.0f);

	//Composition des côtés
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_v1 = l_bottomArc[i];
		l_v2 = l_bottomArc[i + 1];
		l_v3 = l_topArc[i + 1];
		l_v4 = l_topArc[i];
		l_submesh->AddQuadFace( i, i + 1, m_nbFaces + i + 1, m_nbFaces + i, 1, Point3r( 1.0f - real( i) / real( m_nbFaces), 0.0, 0.0), Point3r( 1.0f - real( i + 1) / real( m_nbFaces), 1.0, 0.0));
	}

	l_v1 = l_bottomArc[i];
	l_v2 = l_bottomArc[0];
	l_v3 = l_topArc[0];
	l_v4 = l_topArc[i];
	l_submesh->AddQuadFace( i, 0, m_nbFaces, m_nbFaces + i, 1, Point3r( 1.0f - real( i) / real( m_nbFaces), 0.0, 0.0), Point3r( 0.0, 1.0, 0.0));

	l_submesh->GenerateBuffers();

//	delete l_bottomArc;
//	delete l_topArc;
	Logger::LogMessage( CU_T( "Cylindre - %s - NbVertex : %d"), m_name.char_str(), l_submesh->GetVertices().size());
}

void CylindricMesh :: Modify( real p_radius, real p_height, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	m_height = p_height;
	Cleanup();
	GeneratePoints();
}