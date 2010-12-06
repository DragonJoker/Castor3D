#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/SphericMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Vertex.h"
#include "geometry/basic/Pattern.h"
#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"



using namespace Castor3D;

SphericMesh :: SphericMesh( real radius_p, unsigned int nbFaces_p,
							const String & p_name)
	:	Mesh( p_name),
		m_radius( radius_p),
		m_nbFaces( nbFaces_p)
{
//	m_preferredSubdivMode = smQuad;
	m_meshType = Mesh::eSphere;
	GeneratePoints();
}

SphericMesh :: ~SphericMesh()
{
}

void SphericMesh :: GeneratePoints()
{
	if (m_radius < 0)
	{
		m_radius = -m_radius;
	}
	if (m_nbFaces < 3)
	{
		return;
	}

	SubmeshPtr l_submesh = CreateSubmesh( 1);
	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();

	real l_angle = (Angle::PiMult2) / m_nbFaces;
	Point3rPattern l_arc;
	Point3r l_vertex;

	//CALCUL DE LA POSITION DES POINTS
	real l_alpha = 0 ;
	Point3rPattern * l_arcs = new Point3rPattern[m_nbFaces];

	for (size_t i = 0 ; i < (size_t)((m_nbFaces/2)-1) ; i++)
	{
		l_alpha += l_angle;
		l_arc.AddPoint( Point3r( m_radius * cos( l_alpha), 0.0, m_radius * sin( l_alpha)), -1);
	}

	Point3r l_pTopCenter( m_radius, 0.0, 0.0);
	Point3r l_pBottomCenter( -m_radius, 0.0, 0.0);
	unsigned int l_count = 0;
	size_t l_size = l_arc.GetSize();

	for (l_alpha = 0 ; l_count < m_nbFaces ; l_alpha += l_angle)
	{
		for (size_t j = 0 ; j < l_size ; j++)
		{
			l_vertex = l_submesh->AddVertex( l_arc[j][0],
											 l_arc[j][2] * sin( l_alpha),
											 l_arc[j][2] * cos( l_alpha));
			l_arcs[l_count].AddPoint( l_vertex, -1);
		}

		l_count++;
	}

	l_submesh->AddVertex( l_pTopCenter);
	size_t l_uiTopCenterIndex = 0;
	l_submesh->AddVertex( l_pBottomCenter);
	size_t l_uiBottomCenterIndex = 1;

	//RECONSTITION DES FACES
		// Sommet et Base
	for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		Face & l_faceA = l_submesh->AddFace( i * m_nbFaces, (i + 1) * m_nbFaces, l_uiTopCenterIndex, 0);
		l_faceA.SetVertexTexCoords( 0, ((real) i) / ((real) m_nbFaces), 1.0f / l_size);
		l_faceA.SetVertexTexCoords( 1, ((real) i+1) / ((real) m_nbFaces), 1.0f / l_size);
		l_faceA.SetVertexTexCoords( 2, ((real) i+1) / ((real) m_nbFaces), 0.0);

		Face & l_faceB = l_submesh->AddFace( i * m_nbFaces + l_size - 1, l_uiBottomCenterIndex, (i + 1) * m_nbFaces + l_size - 1, 0);
		l_faceB.SetVertexTexCoords( 0, ((real) i) / ((real) m_nbFaces), (l_size - 1.0f) / l_size);
		l_faceB.SetVertexTexCoords( 1, ((real) i+1) / ((real) m_nbFaces), 1.0);
		l_faceB.SetVertexTexCoords( 2, ((real) i+1) / ((real) m_nbFaces), (l_size - 1.0f) / l_size);
	}

	Face & l_faceA = l_submesh->AddFace( 0, l_uiTopCenterIndex, (m_nbFaces - 1) * m_nbFaces, 0);
	l_faceA.SetVertexTexCoords( 0, 1.0, 1.0f / l_size / 2.0f);
	l_faceA.SetVertexTexCoords( 1, 1.0, 0.0);
	l_faceA.SetVertexTexCoords( 2, ((real) m_nbFaces-1) / ((real) m_nbFaces), 1.0f / l_size);

	Face & l_faceB = l_submesh->AddFace( l_size - 1, (m_nbFaces - 1) * m_nbFaces + l_size - 1, l_uiBottomCenterIndex, 0);
	l_faceB.SetVertexTexCoords( 0, 1.0, (l_size - 1.0f) / l_size);
	l_faceB.SetVertexTexCoords( 1, ((real) m_nbFaces-1) / ((real) m_nbFaces), (l_size - 1.0f) / l_size);
	l_faceB.SetVertexTexCoords( 2, 1.0, 1.0);

		// Autres faces
	for (size_t j = 0 ; j < l_size - 1 ; j++)
	{
		for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
		{
			l_submesh->AddQuadFace( (i + 1) * m_nbFaces + j, i * m_nbFaces + j, i * m_nbFaces + j + 1, (i + 1) * m_nbFaces + j + 1,
									0, Point3r( real( i + 1) / real( m_nbFaces), real( j) / real( l_size), 0.0), Point3r( 2, real( i) / real( m_nbFaces), real( j + 1) / real( l_size), 0.0));
		}

		l_submesh->AddQuadFace( j, (m_nbFaces - 1) * m_nbFaces + j, (m_nbFaces - 1) * m_nbFaces + j + 1, j + 1,
								0, Point3r( 2, 1.0, real( j) / real( l_size), 0.0), Point3r( real( m_nbFaces - 1) / real( m_nbFaces), real( j + 1) / real( l_size), 0.0));
	}

	delete [] l_arcs;

	l_submesh->GenerateBuffers();

	Logger::LogMessage( CU_T( "SphericMesh - %s - NbFaces : %d - NbVertex : %d"), m_name.char_str(), l_submesh->GetNbFaces(), l_submesh->GetVertices().size());
}

void SphericMesh :: Modify( real p_radius, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
    m_radius = p_radius;
	Cleanup();
	GeneratePoints();
}