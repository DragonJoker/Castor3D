#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Sphere.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Pattern.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"



using namespace Castor3D;

Sphere :: Sphere()
	:	MeshCategory( eMESH_TYPE_SPHERE)
	,	m_radius( 0)
	,	m_nbFaces( 0)
{
}

Sphere :: ~Sphere()
{
}

MeshCategoryPtr Sphere :: Clone()
{
	return MeshCategoryPtr( new Sphere);
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

	SubmeshPtr l_pSubmesh = GetMesh()->CreateSubmesh( 1);

	real l_angle = (Angle::PiMult2) / m_nbFaces;
	Point3rPattern l_arc;
	IdPoint3rPtr l_vertex;

	//CALCUL DE LA POSITION DES POINTS
	real l_alpha = 0 ;
	IdPoint3rPattern * l_arcs = new IdPoint3rPattern[m_nbFaces];

	for (size_t i = 0 ; i < (size_t)((m_nbFaces/2)-1) ; i++)
	{
		l_alpha += l_angle;
		l_arc.AddElement( Point3r( real( m_radius * cos( l_alpha)), real( 0.0), real( m_radius * sin( l_alpha))), -1);
	}

	unsigned int l_count = 0;
	size_t l_size = l_arc.GetSize();

	for (l_alpha = 0 ; l_count < m_nbFaces ; l_alpha += l_angle)
	{
		for (size_t j = 0 ; j < l_size ; j++)
		{
			l_vertex = l_pSubmesh->AddPoint( l_arc[j][0],
											l_arc[j][2] * sin( l_alpha),
											l_arc[j][2] * cos( l_alpha));
			l_arcs[l_count].AddElement( IdPoint3r( * l_vertex), -1);
		}

		l_count++;
	}

	IdPoint3rPtr l_pTopCenter = l_pSubmesh->AddPoint( m_radius, 0.0, 0.0);
	IdPoint3rPtr l_pBottomCenter = l_pSubmesh->AddPoint( -m_radius, 0.0, 0.0);

	//RECONSTITION DES FACES
		// Sommet et Base
	for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		FacePtr l_faceA = l_pSubmesh->AddFace( l_arcs[i][0].GetIndex(), l_arcs[i+1][0].GetIndex(), l_pTopCenter->GetIndex(), 0);
		l_faceA->SetVertexTexCoords( 0, real( i) / real( m_nbFaces),			real( 1.0) / real( l_size));
		l_faceA->SetVertexTexCoords( 1, real( i+1) / real( m_nbFaces),		real( 1.0) / real( l_size));
		l_faceA->SetVertexTexCoords( 2, real( i+i+1) / real( 2 * m_nbFaces),	real( 0.0));

		FacePtr l_faceB = l_pSubmesh->AddFace( l_arcs[i][l_size - 1].GetIndex(), l_pBottomCenter->GetIndex(), l_arcs[i+1][l_size - 1].GetIndex(), 0);
		l_faceB->SetVertexTexCoords( 0, real( i) / real( m_nbFaces),			real( l_size - 1) / real( l_size));
		l_faceB->SetVertexTexCoords( 1, real( i+i+1) / real( 2*m_nbFaces),	real( 1.0));
		l_faceB->SetVertexTexCoords( 2, real( i+1) / real( m_nbFaces),		real( l_size - 1) / real( l_size));
	}

	FacePtr l_faceA = l_pSubmesh->AddFace( l_arcs[0][0].GetIndex(), l_pTopCenter->GetIndex(), l_arcs[m_nbFaces-1][0].GetIndex(), 0);
	l_faceA->SetVertexTexCoords( 0, real( 1.0),											real( 1.0) / real( l_size));
	l_faceA->SetVertexTexCoords( 1, real( m_nbFaces+m_nbFaces-1) / real( 2*m_nbFaces),	real( 0.0));
	l_faceA->SetVertexTexCoords( 2, real( m_nbFaces-1) / real( m_nbFaces),				real( 1.0) / real( l_size));

	FacePtr l_faceB = l_pSubmesh->AddFace( l_arcs[0][l_size - 1].GetIndex(), l_arcs[m_nbFaces-1][l_size - 1].GetIndex(), l_pBottomCenter->GetIndex(), 0);
	l_faceB->SetVertexTexCoords( 0, real( 1.0),											real( l_size - 1) / real( l_size));
	l_faceB->SetVertexTexCoords( 1, real( m_nbFaces-1) / real( m_nbFaces),				real( l_size - 1) / real( l_size));
	l_faceB->SetVertexTexCoords( 2, real( m_nbFaces+m_nbFaces-1) / real( 2*m_nbFaces),	real( 1.0));

		// Autres faces
	for (size_t j = 0 ; j < l_size - 1 ; j++)
	{
		for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
		{
			l_pSubmesh->AddQuadFace( l_arcs[i + 1][j].GetIndex(), l_arcs[i][j].GetIndex(), l_arcs[i][j + 1].GetIndex(), l_arcs[i + 1][j + 1].GetIndex(),
									0,	Point3r( real( i + 1) / real( m_nbFaces),	real( j + 0) / real( l_size),	0.0f),
										Point3r( real( i + 0) / real( m_nbFaces),	real( j + 1) / real( l_size),	0.0f));
		}

		l_pSubmesh->AddQuadFace( l_arcs[0][j].GetIndex(), l_arcs[m_nbFaces - 1][j].GetIndex(), l_arcs[m_nbFaces - 1][j + 1].GetIndex(), l_arcs[0][j + 1].GetIndex(),
								0,	Point3r( 1.0f,										real( j + 0) / real( l_size),	0.0f),
									Point3r( real( m_nbFaces - 1) / real( m_nbFaces),	real( j + 1) / real( l_size),	0.0f));
	}

	delete [] l_arcs;

	l_pSubmesh->GenerateBuffers();

	Logger::LogMessage( cuT( "Sphere - NbFaces : %d - NbVertex : %d"), l_pSubmesh->GetNbFaces(), l_pSubmesh->GetNbPoints());
}

void Sphere :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
	m_nbFaces = p_arrayFaces[0];
    m_radius = p_arrayDimensions[0];
	GetMesh()->Cleanup();
	GeneratePoints();
}
