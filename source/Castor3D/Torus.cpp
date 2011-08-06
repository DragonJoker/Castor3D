#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Torus.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Pattern.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"

using namespace Castor3D;

Torus :: Torus()
	:	MeshCategory( eMESH_TYPE_TORUS)
	,	m_internalRadius( 0)
	,	m_externalRadius( 0)
	,	m_internalNbFaces( 0)
	,	m_externalNbFaces( 0)
{
}

Torus :: ~Torus()
{
}

MeshCategoryPtr Torus :: Clone()
{
	return MeshCategoryPtr( new Torus);
}

void Torus :: GeneratePoints()
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

	SubmeshPtr l_pSubmesh = GetMesh()->CreateSubmesh( 1);

	real l_internalRotationAngle = (Angle::PiMult2) / m_internalNbFaces;
	real l_externalRotationAngle = (Angle::PiMult2) / m_externalNbFaces;
	real l_alphaInternal = 0.0;
	real l_alphaExternal = 0.0;
	IdPoint3rPattern * l_arcs = new IdPoint3rPattern[m_externalNbFaces];

	//CALCUL DE LA POSITION DES POINTS

	for (unsigned int i = 0 ; i < m_internalNbFaces ; i++)
	{
		l_alphaInternal += l_internalRotationAngle;
		IdPoint3rPtr l_vertex = l_pSubmesh->AddPoint( (m_internalRadius * cos( l_alphaInternal ) + m_externalRadius), (m_internalRadius * sin( l_alphaInternal )), 0.0);
		l_arcs[0].AddElement( IdPoint3r( * l_vertex), -1);
	}

	size_t l_arcSize = l_arcs[0].GetSize();

	for (unsigned int i = 0 ; i < m_externalNbFaces-1 ; i++)
	{
		l_alphaExternal += l_externalRotationAngle;

		for (unsigned int j = 0 ; j < l_arcSize ; j++)
		{
			IdPoint3r l_vertex = l_arcs[0][j];
			l_vertex[2] = l_vertex[0] * sin( l_alphaExternal);
			l_vertex[0] = l_vertex[0] * cos( l_alphaExternal);
			l_arcs[i+1].AddElement( l_vertex, -1);
			l_pSubmesh->AddPoint( l_vertex);
		}
	}

	//GENERATION DES FACES
	for (size_t j = 0 ; j < l_arcSize - 1 ; j++)
	{
		for (unsigned int i = 0 ; i < m_externalNbFaces - 1 ; i++)
		{
			l_pSubmesh->AddQuadFace( i * l_arcSize + j, (i + 1) * l_arcSize + j, (i + 1) * l_arcSize + j + 1, i * l_arcSize + j + 1,
									0, Point3r( real( i) / real( m_externalNbFaces), real( j) / real( l_arcSize), 0.0f), Point3r( real( i + 1) / real( m_externalNbFaces), real( j + 1) / real( l_arcSize), 0.0f));
		}

		l_pSubmesh->AddQuadFace( (m_externalNbFaces-1) * l_arcSize + j, j, j + 1, (m_externalNbFaces-1) * l_arcSize + j + 1,
								0, Point3r( real( m_externalNbFaces - 1) / real( m_externalNbFaces), real( j) / real( l_arcSize), 0.0f), Point3r( 1.0f, real( j + 1) / real( l_arcSize), 0.0f));
	}

	for (unsigned int i = 0 ; i < m_externalNbFaces - 1 ; i++)
	{
		l_pSubmesh->AddQuadFace( i * l_arcSize + l_arcSize - 1, (i + 1) * l_arcSize + l_arcSize - 1, (i + 1) * l_arcSize, i * l_arcSize,
								0, Point3r( real( i) / real( m_externalNbFaces), real( l_arcSize - 1) / real( l_arcSize), 0.0f), Point3r( real( i + 1) / real( m_externalNbFaces), 1.0f, 0.0f));
	}

	l_pSubmesh->AddQuadFace( (m_externalNbFaces - 1) * l_arcSize + l_arcSize - 1, l_arcSize - 1, 0, (m_externalNbFaces - 1) * l_arcSize,
							0, Point3r( real( m_externalNbFaces - 1) / real( m_externalNbFaces), real( l_arcSize - 1) / real( l_arcSize), 0.0f), Point3r( 1.0, 1.0, 0.0));

	l_pSubmesh->GenerateBuffers();

	delete [] l_arcs;
	Logger::LogMessage( cuT( "Torus - NbFaces : %d - NbVertex : %d"), l_pSubmesh->GetNbFaces(), l_pSubmesh->GetNbPoints());

}

void Torus :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
	m_internalRadius = p_arrayDimensions[0];
	m_externalRadius = p_arrayDimensions[1];
	m_internalNbFaces = p_arrayFaces[0];
	m_externalNbFaces = p_arrayFaces[1];
	GetMesh()->Cleanup();
	GeneratePoints();
}