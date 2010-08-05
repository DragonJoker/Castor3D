#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/CylindricMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Arc.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"

#include "Log.h"

using namespace Castor3D;


CylindricMesh :: CylindricMesh( float height_p, float radius_p, unsigned int nbFaces_p,
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

	Submesh * l_submesh = CreateSubmesh( 2);

	//CALCUL DE LA POSITION DES POINTS
	float angleRotation = M_PI_MULT_2 / m_nbFaces;
	unsigned int i = 0;

	Arc * l_bottomArc = new Arc;
	Arc * l_topArc = new Arc;

	Vector3f * vTmp_l, * vTmp2_l;
	for (float dAlphaI = 0 ; dAlphaI < M_PI_MULT_2 && i < m_nbFaces ; dAlphaI += angleRotation) 
	{
		vTmp_l = l_submesh->AddVertex( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		l_bottomArc->AddVertex( vTmp_l, -1);
	
		vTmp2_l = new Vector3f( *vTmp_l);
		vTmp2_l->y += m_height;
		vTmp2_l->m_index = l_submesh->m_vertex.size();
		l_submesh->AddVertex( vTmp2_l);
		l_topArc->AddVertex( vTmp2_l, -1);
		i++;
	}
//	pTopCenter_l = new Vector3f( m_pCenter);
	Vector3f * pTopCenter_l = l_submesh->AddVertex( 0.0, 0.0, 0.0);
	pTopCenter_l->y += m_height;
//
	Vector3f * pBottomCenter_l = l_submesh->AddVertex( 0.0, 0.0, 0.0);

	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();

	Face * l_pFace;
	//RECONSTITION DES FACES
	if (m_height < 0)
	{
		m_height = -m_height;
	}

	Vector3f * l_v1, * l_v2, * l_v3;
	//Composition des extrémités
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		//Composition du bas
		l_v1 = l_bottomArc->GetVertex( i);
		l_v2 = l_bottomArc->GetVertex( i + 1);
		l_v3 = pBottomCenter_l;
		l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 0);
		SetTexCoordV1( l_pFace, (1.0f + l_v2->x / m_radius) / 2.0f, (1.0f + l_v2->z / m_radius) / 2.0f);
		SetTexCoordV2( l_pFace, (1.0f + l_v1->x / m_radius) / 2.0f, (1.0f + l_v1->z / m_radius) / 2.0f);
		SetTexCoordV3( l_pFace, 0.5f, 0.5f);
		//Composition du dessus
		l_v1 = l_topArc->GetVertex( i);
		l_v2 = pTopCenter_l;
		l_v3 = l_topArc->GetVertex( i + 1);
		l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 0);
		SetTexCoordV1( l_pFace, 0.5, 0.5);
		SetTexCoordV2( l_pFace, 1.0f - (1.0f + l_v1->x / m_radius) / 2.0f, (1.0f + l_v1->z / m_radius) / 2.0f);
		SetTexCoordV3( l_pFace, 1.0f - (1.0f + l_v3->x / m_radius) / 2.0f, (1.0f + l_v3->z / m_radius) / 2.0f);
	}
	//Composition du bas
	l_v1 = l_bottomArc->GetVertex( m_nbFaces - 1);
	l_v2 = l_bottomArc->GetVertex( 0);
	l_v3 = pBottomCenter_l;
	l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 0);
	SetTexCoordV1( l_pFace, (1.0f + l_v2->x / m_radius) / 2.0f, (1.0f + l_v2->z / m_radius) / 2.0f);
	SetTexCoordV2( l_pFace, (1.0f + l_v1->x / m_radius) / 2.0f, (1.0f + l_v1->z / m_radius) / 2.0f);
	SetTexCoordV3( l_pFace, 0.5, 0.5);
	//Composition du dessus
	l_v1 = l_topArc->GetVertex( m_nbFaces - 1);
	l_v2 = pTopCenter_l;
	l_v3 = l_topArc->GetVertex( 0);
	l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 0);
	SetTexCoordV1( l_pFace, 0.5, 0.5);
	SetTexCoordV2( l_pFace, 1.0f - (1.0f + l_v1->x / m_radius) / 2.0f, (1.0f + l_v1->z / m_radius) / 2.0f);
	SetTexCoordV3( l_pFace, 1.0f - (1.0f + l_v3->x / m_radius) / 2.0f, (1.0f + l_v3->z / m_radius) / 2.0f);

	//Composition des côtés
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		//Composition du cote1
		l_v1 = l_bottomArc->GetVertex( i);
		l_v2 = l_topArc->GetVertex( i);
		l_v3 = l_bottomArc->GetVertex( i + 1);
		l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 1);
		SetTexCoordV1( l_pFace, 1.0f - ((float)i) / ((float)m_nbFaces), 1.0);
		SetTexCoordV2( l_pFace, 1.0f - ((float)i) / ((float)m_nbFaces), 0.0);
		SetTexCoordV3( l_pFace, 1.0f - ((float)i+1) / ((float)m_nbFaces), 0.0);
		//Composition du cote2
		l_v1 = l_topArc->GetVertex( i);
		l_v2 = l_topArc->GetVertex( i + 1);
		l_v3 = l_bottomArc->GetVertex( i + 1);
		l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 1);
		SetTexCoordV1( l_pFace, 1.0f - ((float)i+1) / ((float)m_nbFaces), 1.0);
		SetTexCoordV2( l_pFace, 1.0f - ((float)i) / ((float)m_nbFaces), 1.0);
		SetTexCoordV3( l_pFace, 1.0f - ((float)i+1) / ((float)m_nbFaces), 0.0);
	}
	//Composition du cote1
	l_v1 = l_bottomArc->GetVertex( i);
	l_v2 = l_topArc->GetVertex( i);
	l_v3 = l_bottomArc->GetVertex( 0);
	l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 1);
	SetTexCoordV1( l_pFace, 1.0f - ((float)i) / ((float)m_nbFaces), 1.0);
	SetTexCoordV2( l_pFace, 1.0f - ((float)i) / ((float)m_nbFaces), 0.0);
	SetTexCoordV3( l_pFace, 0.0, 0.0);
	//Composition du cote2
	l_v1 = l_topArc->GetVertex( i);
	l_v2 = l_topArc->GetVertex( 0);
	l_v3 = l_bottomArc->GetVertex( 0);
	l_pFace = l_submesh->AddFace( l_v2, l_v1, l_v3, 1);
	SetTexCoordV1( l_pFace, 0.0, 1.0);
	SetTexCoordV2( l_pFace, 1.0f - ((float)i) / ((float)m_nbFaces), 1.0);
	SetTexCoordV3( l_pFace, 0.0, 0.0);

	l_submesh->GenerateBuffers();

	delete l_bottomArc;
	delete l_topArc;
	Log::LogMessage( C3D_T( "Cylindre - %s - NbVertex : %d"), m_name.c_str(), l_submesh->m_vertex.size());
}



void CylindricMesh :: Modify( float p_radius, float p_height, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	m_height = p_height;
	Cleanup();
	GeneratePoints();
}