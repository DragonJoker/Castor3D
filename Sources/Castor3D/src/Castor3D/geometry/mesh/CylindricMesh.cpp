#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/CylindricMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Pattern.h"
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

	PatternPtr l_bottomArc = new Pattern;
	PatternPtr l_topArc = new Pattern;

	VertexPtr vTmp_l, vTmp2_l;

	for (real dAlphaI = 0 ; dAlphaI < Angle::PiMult2 && i < m_nbFaces ; dAlphaI += angleRotation) 
	{
		vTmp_l = l_submesh->AddVertex( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		l_bottomArc->AddVertex( vTmp_l, -1);
	
		vTmp2_l = new Vertex( * vTmp_l);
		vTmp2_l->m_coords[1] += m_height;
		vTmp2_l->m_index = l_submesh->m_vertex.size();
		l_submesh->AddVertex( vTmp2_l);
		l_topArc->AddVertex( vTmp2_l, -1);
		i++;
	}
//	pTopCenter_l = new Point3r( m_pCenter);
	VertexPtr pTopCenter_l = l_submesh->AddVertex( 0.0, 0.0, 0.0);
	pTopCenter_l->m_coords[1] += m_height;
//
	VertexPtr pBottomCenter_l = l_submesh->AddVertex( 0.0, 0.0, 0.0);

	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();

	FacePtr l_pFace;
	//RECONSTITION DES FACES
	if (m_height < 0)
	{
		m_height = -m_height;
	}

	VertexPtr l_v1, l_v2, l_v3, l_v4;
	//Composition des extrémités
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		//Composition du bas
		l_v1 = l_bottomArc->GetVertex( i);
		l_v2 = l_bottomArc->GetVertex( i + 1);
		l_v3 = pBottomCenter_l;
		l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 0);
		l_pFace->SetTexCoordV1( (1.0f + l_v2->m_coords[0] / m_radius) / 2.0f, (1.0f + l_v2->m_coords[2] / m_radius) / 2.0f);
		l_pFace->SetTexCoordV2( (1.0f + l_v1->m_coords[0] / m_radius) / 2.0f, (1.0f + l_v1->m_coords[2] / m_radius) / 2.0f);
		l_pFace->SetTexCoordV3( 0.5f, 0.5f);
		//Composition du dessus
		l_v1 = l_topArc->GetVertex( i);
		l_v2 = pTopCenter_l;
		l_v3 = l_topArc->GetVertex( i + 1);
		l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 2);
		l_pFace->SetTexCoordV1( 0.5, 0.5);
		l_pFace->SetTexCoordV2( 1.0f - (1.0f + l_v1->m_coords[0] / m_radius) / 2.0f, (1.0f + l_v1->m_coords[2] / m_radius) / 2.0f);
		l_pFace->SetTexCoordV3( 1.0f - (1.0f + l_v3->m_coords[0] / m_radius) / 2.0f, (1.0f + l_v3->m_coords[2] / m_radius) / 2.0f);
	}
	//Composition du bas
	l_v1 = l_bottomArc->GetVertex( m_nbFaces - 1);
	l_v2 = l_bottomArc->GetVertex( 0);
	l_v3 = pBottomCenter_l;
	l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 0);
	l_pFace->SetTexCoordV1( (1.0f + l_v2->m_coords[0] / m_radius) / 2.0f, (1.0f + l_v2->m_coords[2] / m_radius) / 2.0f);
	l_pFace->SetTexCoordV2( (1.0f + l_v1->m_coords[0] / m_radius) / 2.0f, (1.0f + l_v1->m_coords[2] / m_radius) / 2.0f);
	l_pFace->SetTexCoordV3( 0.5, 0.5);
	//Composition du dessus
	l_v1 = l_topArc->GetVertex( m_nbFaces - 1);
	l_v2 = pTopCenter_l;
	l_v3 = l_topArc->GetVertex( 0);
	l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 2);
	l_pFace->SetTexCoordV1( 0.5, 0.5);
	l_pFace->SetTexCoordV2( 1.0f - (1.0f + l_v1->m_coords[0] / m_radius) / 2.0f, (1.0f + l_v1->m_coords[2] / m_radius) / 2.0f);
	l_pFace->SetTexCoordV3( 1.0f - (1.0f + l_v3->m_coords[0] / m_radius) / 2.0f, (1.0f + l_v3->m_coords[2] / m_radius) / 2.0f);

	//Composition des côtés
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_v1 = l_bottomArc->GetVertex( i);
		l_v2 = l_bottomArc->GetVertex( i + 1);
		l_v3 = l_topArc->GetVertex( i + 1);
		l_v4 = l_topArc->GetVertex( i);
		l_submesh->AddQuadFace( l_v1, l_v2, l_v3, l_v4, 1, Point3r( 1.0f - real( i) / real( m_nbFaces), 0.0, 0.0), Point3r( 1.0f - real( i + 1) / real( m_nbFaces), 1.0, 0.0));
	}

	l_v1 = l_bottomArc->GetVertex( i);
	l_v2 = l_bottomArc->GetVertex( 0);
	l_v3 = l_topArc->GetVertex( 0);
	l_v4 = l_topArc->GetVertex( i);
	l_submesh->AddQuadFace( l_v1, l_v2, l_v3, l_v4, 1, Point3r( 1.0f - real( i) / real( m_nbFaces), 0.0, 0.0), Point3r( 0.0, 1.0, 0.0));

	l_submesh->GenerateBuffers();

//	delete l_bottomArc;
//	delete l_topArc;
	Log::LogMessage( CU_T( "Cylindre - %s - NbVertex : %d"), m_name.c_str(), l_submesh->m_vertex.size());
}

void CylindricMesh :: Modify( real p_radius, real p_height, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	m_height = p_height;
	Cleanup();
	GeneratePoints();
}