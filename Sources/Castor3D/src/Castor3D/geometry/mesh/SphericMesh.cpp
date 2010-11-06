#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/SphericMesh.h"
#include "geometry/mesh/Submesh.h"
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

	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();

	real l_angle = (Angle::PiMult2) / m_nbFaces;
	PatternPtr l_arc;
	VertexPtr l_vertex;

	if (m_nbFaces % 2 == 0)
	{
		//CALCUL DE LA POSITION DES POINTS
		real l_alpha = 0 ;
		PatternPtr * l_arcs = new PatternPtr[m_nbFaces];
		l_arc = new Pattern;

		for (size_t i = 0 ; i < (size_t)((m_nbFaces/2)-1) ; i++)
		{
			l_alpha += l_angle;
			l_vertex = l_submesh->AddVertex( m_radius * cos( l_alpha), 0.0, m_radius * sin( l_alpha));
			l_arc->AddVertex( l_vertex, -1);
		}

		VertexPtr l_pTopCenter = new Vertex( m_radius, 0.0, 0.0);
		VertexPtr l_pBottomCenter = new Vertex( - m_radius, 0.0, 0.0);
		unsigned int l_count = 0;
		size_t l_size = l_arc->GetNumVertex();

		for (l_alpha = 0 ; l_count < m_nbFaces ; l_alpha += l_angle)
		{
			l_arcs[l_count] = new Pattern;

			for (size_t j = 0 ; j < l_size ; j++)
			{
				l_vertex = l_submesh->AddVertex( l_arc->GetVertex( j)->m_coords[0],
												 l_arc->GetVertex( j)->m_coords[2] * sin( l_alpha),
												 l_arc->GetVertex( j)->m_coords[2] * cos( l_alpha));
				l_arcs[l_count]->AddVertex( l_vertex, -1);
			}

			l_count++;
		}

		l_submesh->AddVertex( l_pTopCenter);
		l_submesh->AddVertex( l_pBottomCenter);

		//RECONSTITION DES FACES
			// Sommet et Base
		FacePtr l_face;

		for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
		{
			l_face = l_submesh->AddFace( l_arcs[i]->GetVertex( 0), l_arcs[i+1]->GetVertex( 0), l_pTopCenter, 0);
			l_face->SetTexCoordV1( ((real) i) / ((real) m_nbFaces), 1.0f / l_size);
			l_face->SetTexCoordV2( ((real) i+1) / ((real) m_nbFaces), 1.0f / l_size);
			l_face->SetTexCoordV3( ((real) i+1) / ((real) m_nbFaces), 0.0);

			l_face = l_submesh->AddFace( l_arcs[i]->GetVertex( l_size - 1), l_pBottomCenter, l_arcs[i+1]->GetVertex( l_size - 1), 0);
			l_face->SetTexCoordV1( ((real) i) / ((real) m_nbFaces), (l_size - 1.0f) / l_size);
			l_face->SetTexCoordV2( ((real) i+1) / ((real) m_nbFaces), 1.0);
			l_face->SetTexCoordV3( ((real) i+1) / ((real) m_nbFaces), (l_size - 1.0f) / l_size);
		}

		l_face = l_submesh->AddFace( l_arcs[0]->GetVertex( 0), l_pTopCenter, l_arcs[m_nbFaces-1]->GetVertex( 0), 0);
		l_face->SetTexCoordV1( 1.0, 1.0f / l_size / 2.0f);
		l_face->SetTexCoordV2( 1.0, 0.0);
		l_face->SetTexCoordV3( ((real) m_nbFaces-1) / ((real) m_nbFaces), 1.0f / l_size);

		l_face = l_submesh->AddFace( l_arcs[0]->GetVertex( l_size - 1), l_arcs[m_nbFaces-1]->GetVertex( l_size - 1), l_pBottomCenter, 0);
		l_face->SetTexCoordV1( 1.0, (l_size - 1.0f) / l_size);
		l_face->SetTexCoordV2( ((real) m_nbFaces-1) / ((real) m_nbFaces), (l_size - 1.0f) / l_size);
		l_face->SetTexCoordV3( 1.0, 1.0);

			// Autres faces
		for (size_t j = 0 ; j < l_size - 1 ; j++)
		{
			for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
			{
				l_submesh->AddQuadFace( l_arcs[i + 1]->GetVertex( j), l_arcs[i]->GetVertex( j), l_arcs[i]->GetVertex( j + 1), l_arcs[i + 1]->GetVertex( j + 1),
										0, Point3r( real( i + 1) / real( m_nbFaces), real( j) / real( l_size), 0.0), Point3r( 2, real( i) / real( m_nbFaces), real( j + 1) / real( l_size), 0.0));
			}

			l_submesh->AddQuadFace( l_arcs[0]->GetVertex( j), l_arcs[m_nbFaces - 1]->GetVertex( j), l_arcs[m_nbFaces - 1]->GetVertex( j + 1), l_arcs[0]->GetVertex( j + 1),
									0, Point3r( 2, 1.0, real( j) / real( l_size), 0.0), Point3r( real( m_nbFaces - 1) / real( m_nbFaces), real( j + 1) / real( l_size), 0.0));
		}

		for (unsigned int i = 0 ; i < m_nbFaces ; i++)
		{
//			delete l_arcs[i];
		}

		delete [] l_arcs;
	}
	else
	{
		//CALCUL DE LA POSITION DES POINTS
		real l_alpha = 0;
		l_arc = new Pattern;

		for (size_t i = 0 ; i < (size_t)(m_nbFaces/2) ; i++)
		{
			l_alpha += l_angle;
			l_vertex = new Vertex( m_radius * cos( l_alpha), 0.0, m_radius * sin( l_alpha));
			l_arc->AddVertex( l_vertex, -1);
		}

		size_t l_size = l_arc->GetNumVertex();
		VertexPtr l_pTopCenter = new Vertex( m_radius, 0.0, 0.0);
		VertexPtr l_pBottomCenter = new Vertex( l_arc->GetVertex( l_size - 1)->m_coords[0], 0.0, 0.0);
		unsigned int l_count = 0;

		for (l_alpha = 0 ; l_count < m_nbFaces ; l_alpha += l_angle)
		{
			for (size_t j = 0; j < l_size ; j++)
			{
				l_vertex = l_submesh->AddVertex( l_arc->GetVertex( j)->m_coords[0],
												 l_arc->GetVertex( j)->m_coords[2] * sin( l_alpha),
												 l_arc->GetVertex( j)->m_coords[2] * cos( l_alpha));
			}
			l_count++;
		}

		l_submesh->AddVertex( l_pTopCenter);
		l_submesh->AddVertex( l_pBottomCenter);

		//RECONSTITION DES FACES
		FacePtr l_face;
		VertexPtrArray l_vertex = l_submesh->m_vertex;

		for (size_t i = 0 ; i < l_vertex.size() - l_size ; i += l_size)
		{
			l_face = l_submesh->AddFace( l_vertex[i+l_size], l_vertex[i], l_pTopCenter, 0);
			l_face = l_submesh->AddFace( l_pBottomCenter, l_vertex[i+l_size-1], l_vertex[i+l_size+l_size-1], 0);
		}

		l_face = l_submesh->AddFace( l_pTopCenter, l_vertex[0], l_vertex[l_vertex.size() - l_size], 0);
		l_face = l_submesh->AddFace( l_vertex[l_vertex.size() - 1], l_vertex[l_size - 1], l_pBottomCenter, 0);

		for (size_t j = 0 ; j < l_size - 1 ; j++)
		{
			for (size_t i = 0 ; i < l_vertex.size() - l_size ; i += l_size)
			{
				l_face = l_submesh->AddFace( l_vertex[j+i+1], l_vertex[j+i], l_vertex[j+i+l_size], 0);
				l_face = l_submesh->AddFace( l_vertex[j+i+l_size+1], l_vertex[i+j+1], l_vertex[j+i+l_size], 0);
			}

			l_face = l_submesh->AddFace( l_vertex[j], l_vertex[j+1], l_vertex[l_vertex.size()-l_size+1+j], 0);
			l_face = l_submesh->AddFace( l_vertex[l_vertex.size()-l_size+1+j], l_vertex[l_vertex.size()-l_size+j], l_vertex[j], 0);
		}
	}

	l_submesh->GenerateBuffers();

//	delete l_arc;
	Log::LogMessage( CU_T( "SphericMesh - %s - NbFaces : %d - NbVertex : %d"), m_name.c_str(), l_submesh->GetNbFaces(), l_submesh->m_vertex.size());
}

void SphericMesh :: Modify( real p_radius, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
    m_radius = p_radius;
	Cleanup();
	GeneratePoints();
}