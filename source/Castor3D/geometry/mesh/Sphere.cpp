#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/mesh/Sphere.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/basic/Pattern.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/render_system/Buffer.h"



using namespace Castor3D;

Sphere :: Sphere( MeshManager * p_pManager, real radius_p, unsigned int nbFaces_p,
							const String & p_name)
	:	Mesh( p_pManager, p_name)
	,	m_radius( radius_p)
	,	m_nbFaces( nbFaces_p)
{
//	m_preferredSubdivMode = smQuad;
	m_meshType = Mesh::eSphere;
	GeneratePoints();
}

Sphere :: ~Sphere()
{
}

void Sphere :: GeneratePoints()
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

	real l_angle = (Angle::PiMult2) / m_nbFaces;
	Point3rPattern l_arc;
	IdPoint3rPtr l_vertex;

	//CALCUL DE LA POSITION DES POINTS
	real l_alpha = 0 ;
	IdPoint3rPattern * l_arcs = new IdPoint3rPattern[m_nbFaces];

	for (size_t i = 0 ; i < (size_t)((m_nbFaces/2)-1) ; i++)
	{
		l_alpha += l_angle;
		l_arc.AddElement( Point3r( m_radius * cos( l_alpha), 0.0, m_radius * sin( l_alpha)), -1);
	}

	unsigned int l_count = 0;
	size_t l_size = l_arc.GetSize();

	for (l_alpha = 0 ; l_count < m_nbFaces ; l_alpha += l_angle)
	{
		for (size_t j = 0 ; j < l_size ; j++)
		{
			l_vertex = l_submesh->AddPoint( l_arc[j][0],
											l_arc[j][2] * sin( l_alpha),
											l_arc[j][2] * cos( l_alpha));
			l_arcs[l_count].AddElement( IdPoint3r( * l_vertex), -1);
		}

		l_count++;
	}

	IdPoint3rPtr l_pTopCenter = l_submesh->AddPoint( m_radius, 0.0, 0.0);
	IdPoint3rPtr l_pBottomCenter = l_submesh->AddPoint( -m_radius, 0.0, 0.0);

	//RECONSTITION DES FACES
		// Sommet et Base
	for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		FacePtr l_faceA = l_submesh->AddFace( l_arcs[i][0].GetIndex(), l_arcs[i+1][0].GetIndex(), l_pTopCenter->GetIndex(), 0);
		l_faceA->SetVertexTexCoords( 0, real( i) / real( m_nbFaces),			real( 1.0) / real( l_size));
		l_faceA->SetVertexTexCoords( 1, real( i+1) / real( m_nbFaces),		real( 1.0) / real( l_size));
		l_faceA->SetVertexTexCoords( 2, real( i+i+1) / real( 2 * m_nbFaces),	real( 0.0));

		FacePtr l_faceB = l_submesh->AddFace( l_arcs[i][l_size - 1].GetIndex(), l_pBottomCenter->GetIndex(), l_arcs[i+1][l_size - 1].GetIndex(), 0);
		l_faceB->SetVertexTexCoords( 0, real( i) / real( m_nbFaces),			real( l_size - 1) / real( l_size));
		l_faceB->SetVertexTexCoords( 1, real( i+i+1) / real( 2*m_nbFaces),	real( 1.0));
		l_faceB->SetVertexTexCoords( 2, real( i+1) / real( m_nbFaces),		real( l_size - 1) / real( l_size));
	}

	FacePtr l_faceA = l_submesh->AddFace( l_arcs[0][0].GetIndex(), l_pTopCenter->GetIndex(), l_arcs[m_nbFaces-1][0].GetIndex(), 0);
	l_faceA->SetVertexTexCoords( 0, real( 1.0),											real( 1.0) / real( l_size));
	l_faceA->SetVertexTexCoords( 1, real( m_nbFaces+m_nbFaces-1) / real( 2*m_nbFaces),	real( 0.0));
	l_faceA->SetVertexTexCoords( 2, real( m_nbFaces-1) / real( m_nbFaces),				real( 1.0) / real( l_size));

	FacePtr l_faceB = l_submesh->AddFace( l_arcs[0][l_size - 1].GetIndex(), l_arcs[m_nbFaces-1][l_size - 1].GetIndex(), l_pBottomCenter->GetIndex(), 0);
	l_faceB->SetVertexTexCoords( 0, real( 1.0),											real( l_size - 1) / real( l_size));
	l_faceB->SetVertexTexCoords( 1, real( m_nbFaces-1) / real( m_nbFaces),				real( l_size - 1) / real( l_size));
	l_faceB->SetVertexTexCoords( 2, real( m_nbFaces+m_nbFaces-1) / real( 2*m_nbFaces),	real( 1.0));

		// Autres faces
	for (size_t j = 0 ; j < l_size - 1 ; j++)
	{
		for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
		{
			l_submesh->AddQuadFace( l_arcs[i + 1][j].GetIndex(), l_arcs[i][j].GetIndex(), l_arcs[i][j + 1].GetIndex(), l_arcs[i + 1][j + 1].GetIndex(),
									0,	Point3r( real( i + 1) / real( m_nbFaces),	real( j + 0) / real( l_size),	0.0),
										Point3r( real( i + 0) / real( m_nbFaces),	real( j + 1) / real( l_size),	0.0));
		}

		l_submesh->AddQuadFace( l_arcs[0][j].GetIndex(), l_arcs[m_nbFaces - 1][j].GetIndex(), l_arcs[m_nbFaces - 1][j + 1].GetIndex(), l_arcs[0][j + 1].GetIndex(),
								0,	Point3r( 1.0,										real( j + 0) / real( l_size),	0.0),
									Point3r( real( m_nbFaces - 1) / real( m_nbFaces),	real( j + 1) / real( l_size),	0.0));
	}

	delete [] l_arcs;

	l_submesh->GenerateBuffers();

	Logger::LogMessage( CU_T( "Sphere - %s - NbFaces : %d - NbVertex : %d"), GetName().char_str(), l_submesh->GetNbFaces(), l_submesh->GetNbPoints());
}

void Sphere :: Modify( real p_radius, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
    m_radius = p_radius;
	Cleanup();
	GeneratePoints();
}