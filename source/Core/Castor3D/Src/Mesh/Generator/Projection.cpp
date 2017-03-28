#include "Projection.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"

using namespace Castor3D;
using namespace Castor;

Projection::Projection()
	: MeshGenerator( cuT( "projection" ) )
	, m_fDepth( 0 )
	, m_bClosed( false )
	, m_uiNbFaces( 0 )
{
}

Projection::~Projection()
{
	m_pPattern.reset();
}

MeshGeneratorSPtr Projection::Create()
{
	return std::make_shared< Projection >();
}

void Projection::SetPoints( Point3rPatternSPtr p_pPattern, Point3r const & p_vAxis, bool p_bClosed )
{
	m_pPattern = p_pPattern;
	m_bClosed = p_bClosed;
	m_vAxis = p_vAxis;
	point::normalise( m_vAxis );
	m_vAxis = m_vAxis * m_fDepth;
}

void Projection::DoGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	//m_uiNbFaces = p_faces[0];
	//m_fDepth = p_dimensions[0];
	//uint32_t l_uiNbElem = m_pPattern->GetSize();
	//real l_fTotalDistance = 0.0;
	//Point3r l_ptDiff;
	//SubmeshSPtr l_submesh;
	//real l_fDistanceToOrigin = 0.0;
	//Point3r l_ptCurrentUV;
	//Point3r l_ptPreviousUV;
	//IdPoint3rPattern::PointerArray l_arrayPatterns;
	//IdPoint3rPatternPtr l_pPreviousPattern = m_pPattern;
	//IdPoint3rPatternPtr l_pPattern;

	//if (l_uiNbElem > 0)
	//{
	//	if (m_bClosed && m_pPattern->GetElement( 0 ).GetPoint() != m_pPattern->GetElement( l_uiNbElem - 1 ).GetPoint() )
	//	{
	//		m_pPattern->AddElement( m_pPattern->GetElement( l_uiNbElem - 1), 0);
	//		l_uiNbElem++;
	//	}

	//	for (uint32_t i = 1; i < l_uiNbElem; i++)
	//	{
	//		l_ptDiff = m_pPattern->GetElement( i ).GetPoint() - m_pPattern->GetElement( i - 1 ).GetPoint();
	//		l_fTotalDistance += real( point::distance_squared( l_ptDiff ) );
	//	}

	//	for( uint32_t i = 0; i < l_uiNbElem; i++ )
	//	{
	//		GetMesh()->CreateSubmesh();
	//	}

	//	if (m_bClosed)
	//	{
	//		GetMesh()->CreateSubmesh();
	//	}

	//	// Construction des faces
	//	for (uint32_t j = 0; j < m_uiNbFaces; j++)
	//	{
	//		l_pPattern.reset( new IdPoint3rPattern);

	//		l_ptCurrentUV[1] = 0;
	//		l_ptPreviousUV[1] = 0;
	//		l_ptCurrentUV[0] = real( j + 1) / real( m_uiNbFaces);

	//		for (uint32_t i = 0; i < l_uiNbElem; i++)
	//		{
	//			l_pPattern->AddElement( *l_submesh->AddPoint( l_pPreviousPattern->GetElement( i ).GetPoint() + m_vAxis ), i);
	//		}

	//		for (uint32_t i = 1; i < l_uiNbElem; i++)
	//		{
	//			IdPoint3r const & l_ptV0 = l_pPreviousPattern->GetElement( i - 1);
	//			IdPoint3r const & l_ptV1 = l_pPreviousPattern->GetElement( i);
	//			IdPoint3r const & l_ptV2 = l_pPattern->GetElement( i);
	//			IdPoint3r const & l_ptV3 = l_pPattern->GetElement( i - 1);

	//			l_ptDiff = l_ptV1.GetPoint() - l_ptV0.GetPoint();
	//			l_fDistanceToOrigin += real( point::distance_squared( l_ptDiff ) );
	//			l_ptCurrentUV[1] = l_fDistanceToOrigin / l_fTotalDistance;

	//			l_submesh->AddQuadFace( l_ptV0.GetIndex(), l_ptV1.GetIndex(), l_ptV2.GetIndex(), l_ptV3.GetIndex(), l_uiNbElem, l_ptPreviousUV, l_ptCurrentUV);

	//			l_ptPreviousUV[1] = l_ptCurrentUV[1];
	//		}

	//		l_pPreviousPattern = l_pPattern;
	//		l_arrayPatterns.push_back( l_pPattern);
	//		l_ptPreviousUV[0] = l_ptCurrentUV[0];
	//	}
	//}

	//p_mesh.ComputeContainers();
}
