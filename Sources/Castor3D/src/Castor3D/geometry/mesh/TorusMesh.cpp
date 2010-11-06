#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/TorusMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Pattern.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"



using namespace Castor3D;


TorusMesh :: TorusMesh( real internalRadius_p, real externalRadius_p,
						unsigned int internalNbFaces_p, unsigned int externalNbFaces_p,
						const String & p_name)
	:	Mesh( p_name),
		m_internalRadius( internalRadius_p),
		m_externalRadius( externalRadius_p),
		m_internalNbFaces( internalNbFaces_p),
		m_externalNbFaces( externalNbFaces_p)
{
	m_meshType = Mesh::eTorus;
	GeneratePoints();
}

TorusMesh :: ~TorusMesh()
{
}

void TorusMesh :: GeneratePoints()
{
	if (m_internalRadius < 0)
	{
		m_internalRadius = -m_internalRadius;
	}
	if (m_externalRadius < 0)
	{
		m_externalRadius = -m_externalRadius;
	}
	if (m_internalNbFaces < 3 || m_externalNbFaces < 3)
	{
		return;
	}

	SubmeshPtr l_submesh = CreateSubmesh( 1);

	real internalRotationAngle_l = (Angle::PiMult2) / m_internalNbFaces;
	real externalRotationAngle_l = (Angle::PiMult2) / m_externalNbFaces;
	real alphaInternal_l = 0.0;
	real alphaExternal_l = 0.0;
	PatternPtr * l_arcs = new PatternPtr[m_externalNbFaces];

	//CALCUL DE LA POSITION DES POINTS
	VertexPtr l_vertex;
	l_arcs[0] = new Pattern;

	for (unsigned int i = 0 ; i < m_internalNbFaces ; i++)
	{
		alphaInternal_l += internalRotationAngle_l;
		l_vertex = l_submesh->AddVertex( (m_internalRadius * cos( alphaInternal_l ) + m_externalRadius),
										 (m_internalRadius * sin( alphaInternal_l )),
										 0.0);
		l_arcs[0]->AddVertex( l_vertex, -1);
	}

	size_t arcSize = l_arcs[0]->GetNumVertex();

	for (unsigned int i = 0 ; i < m_externalNbFaces-1 ; i++)
	{
		l_arcs[i+1] = new Pattern;
		alphaExternal_l += externalRotationAngle_l;

		for (unsigned int j = 0 ; j < m_internalNbFaces ; j++)
		{
			l_vertex = new Vertex( * l_arcs[0]->GetVertex( j));
			l_vertex->m_coords[2] = l_vertex->m_coords[0] * sin( alphaExternal_l);
			l_vertex->m_coords[0] = l_vertex->m_coords[0] * cos( alphaExternal_l);
			l_vertex->m_index = l_submesh->m_vertex.size();
			l_arcs[i+1]->AddVertex( l_vertex, -1);
			l_submesh->AddVertex( l_vertex);
		}
	}

	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();

	//GENERATION DES FACES
	for (size_t j = 0 ; j < arcSize - 1 ; j++)
	{
		for (unsigned int i = 0 ; i < m_externalNbFaces - 1 ; i++)
		{
			l_submesh->AddQuadFace( l_arcs[i]->GetVertex( j), l_arcs[i + 1]->GetVertex( j), l_arcs[i + 1]->GetVertex( j + 1), l_arcs[i]->GetVertex( j + 1),
									0, Point3r( real( i) / real( m_externalNbFaces), real( j) / real( arcSize), 0.0), Point3r( real( i + 1) / real( m_externalNbFaces), real( j + 1) / real( arcSize), 0.0));
		}

		l_submesh->AddQuadFace( l_arcs[m_externalNbFaces-1]->GetVertex( j), l_arcs[0]->GetVertex( j), l_arcs[0]->GetVertex( j + 1), l_arcs[m_externalNbFaces-1]->GetVertex( j + 1),
								0, Point3r( real( m_externalNbFaces - 1) / real( m_externalNbFaces), real( j) / real( arcSize), 0.0), Point3r( 1.0, real( j + 1) / real( arcSize), 0.0));
	}
	for (unsigned int i = 0 ; i < m_externalNbFaces - 1 ; i++)
	{
		l_submesh->AddQuadFace( l_arcs[i]->GetVertex( arcSize - 1), l_arcs[i + 1]->GetVertex( arcSize - 1), l_arcs[i + 1]->GetVertex( 0), l_arcs[i]->GetVertex( 0),
								0, Point3r( real( i) / real( m_externalNbFaces), real( arcSize - 1) / real( arcSize), 0.0), Point3r( real( i + 1) / real( m_externalNbFaces), 1.0, 0.0));
	}

	l_submesh->AddQuadFace( l_arcs[m_externalNbFaces - 1]->GetVertex( arcSize - 1), l_arcs[0]->GetVertex( arcSize - 1), l_arcs[0]->GetVertex( 0), l_arcs[m_externalNbFaces - 1]->GetVertex( 0),
							0, Point3r( real( m_externalNbFaces - 1) / real( m_externalNbFaces), real( arcSize - 1) / real( arcSize), 0.0), Point3r( 1.0, 1.0, 0.0));

	l_submesh->GenerateBuffers();

	for (unsigned int i = 0 ; i < m_externalNbFaces ; i++)
	{
//		delete l_arcs[i];
	}
	delete [] l_arcs;
	Log::LogMessage( CU_T( "TorusMesh - %s - NbFaces : %d - NbVertex : %d"), m_name.c_str(),
					 l_submesh->GetNbFaces(), l_submesh->m_vertex.size());

}

void TorusMesh :: Modify( real p_internalRadius, real p_externalRadius,
						  unsigned int p_internalNbFaces, unsigned int p_externalNbFaces)
{
	m_internalRadius = p_internalRadius;
	m_externalRadius = p_externalRadius;
	m_internalNbFaces = p_internalNbFaces;
	m_externalNbFaces = p_externalNbFaces;
	Cleanup();
	GeneratePoints();
}