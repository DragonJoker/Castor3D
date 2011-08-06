#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Plane.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/Plane.hpp"



using namespace Castor3D;

Plane :: Plane()
	:	MeshCategory( eMESH_TYPE_PLANE)
	,	m_depth( 0)
	,	m_width( 0)
	,	m_subDivisionsW( 0)
	,	m_subDivisionsD( 0)
{
}

Plane :: ~Plane()
{
}

MeshCategoryPtr Plane :: Clone()
{
	return MeshCategoryPtr( new Plane);
}

void Plane :: GeneratePoints()
{
	m_width = (m_width > 0.0 ? m_width : -m_width);
	m_depth = (m_depth > 0.0 ? m_depth : -m_depth);

	if (m_subDivisionsW < 0 || m_subDivisionsD < 0)
	{
		return;
	}

	real l_gapW = m_width / (m_subDivisionsW + 1);
	real l_gapD = m_depth / (m_subDivisionsD + 1);
	unsigned int l_nbVertexW = m_subDivisionsW + 2;
	unsigned int l_nbVertexD = m_subDivisionsD + 2;
	Point3r l_ptCurrentUV;
	Point3r l_ptPreviousUV;

	SubmeshPtr l_pSubmesh = GetMesh()->CreateSubmesh( 1);

	for (unsigned int i = 0 ; i < l_nbVertexW ; i++)
	{
		for (unsigned int j = 0 ; j < l_nbVertexD ; j++)
		{
			l_pSubmesh->AddPoint( (i * l_gapW), 0.0, (j * l_gapD));
		}
	}

	IdPoint3rPtr l_v1, l_v2, l_v3, l_v4;

	for (unsigned int i = 0 ; i < m_subDivisionsW + 1 ; i++)
	{
		l_ptCurrentUV[0] += 1.0f / real( m_subDivisionsW + 1);

		for (unsigned int j = i * (m_subDivisionsD + 1) ; j < (i + 1) * (m_subDivisionsD + 1) ; j++)
		{
			l_v1 = l_pSubmesh->GetPoint( j + i);
			l_v2 = l_pSubmesh->GetPoint( j + m_subDivisionsW + 2 + i);
			l_v3 = l_pSubmesh->GetPoint( j + m_subDivisionsW + 3 + i);
			l_v4 = l_pSubmesh->GetPoint( j + i + 1);

			l_ptCurrentUV[1] += 1.0f / real( m_subDivisionsD + 1);

			l_pSubmesh->AddQuadFace( l_v1->GetIndex(), l_v2->GetIndex(), l_v3->GetIndex(), l_v4->GetIndex(), 0, l_ptPreviousUV, l_ptCurrentUV);

			l_ptPreviousUV[1] = l_ptCurrentUV[1];
		}

		l_ptPreviousUV[0] = l_ptCurrentUV[0];
	}

	l_pSubmesh->GenerateBuffers();

	Logger::LogMessage( cuT( "Plane - NbVertex : %d"), l_pSubmesh->GetNbPoints());
}

void Plane :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
	m_subDivisionsW = p_arrayFaces[0];
	m_subDivisionsD = p_arrayFaces[1];
	m_width = p_arrayDimensions[0];
	m_depth = p_arrayDimensions[1];
	GetMesh()->Cleanup();
	GeneratePoints();
}