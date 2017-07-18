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
	//uint32_t uiNbElem = m_pPattern->GetSize();
	//real fTotalDistance = 0.0;
	//Point3r ptDiff;
	//SubmeshSPtr submesh;
	//real fDistanceToOrigin = 0.0;
	//Point3r ptCurrentUV;
	//Point3r ptPreviousUV;
	//IdPoint3rPattern::PointerArray arrayPatterns;
	//IdPoint3rPatternPtr pPreviousPattern = m_pPattern;
	//IdPoint3rPatternPtr pPattern;

	//if (uiNbElem > 0)
	//{
	//	if (m_bClosed && m_pPattern->GetElement( 0 ).GetPoint() != m_pPattern->GetElement( uiNbElem - 1 ).GetPoint() )
	//	{
	//		m_pPattern->AddElement( m_pPattern->GetElement( uiNbElem - 1), 0);
	//		uiNbElem++;
	//	}

	//	for (uint32_t i = 1; i < uiNbElem; i++)
	//	{
	//		ptDiff = m_pPattern->GetElement( i ).GetPoint() - m_pPattern->GetElement( i - 1 ).GetPoint();
	//		fTotalDistance += real( point::distance_squared( ptDiff ) );
	//	}

	//	for( uint32_t i = 0; i < uiNbElem; i++ )
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
	//		pPattern.reset( new IdPoint3rPattern);

	//		ptCurrentUV[1] = 0;
	//		ptPreviousUV[1] = 0;
	//		ptCurrentUV[0] = real( j + 1) / real( m_uiNbFaces);

	//		for (uint32_t i = 0; i < uiNbElem; i++)
	//		{
	//			pPattern->AddElement( *submesh->AddPoint( pPreviousPattern->GetElement( i ).GetPoint() + m_vAxis ), i);
	//		}

	//		for (uint32_t i = 1; i < uiNbElem; i++)
	//		{
	//			IdPoint3r const & ptV0 = pPreviousPattern->GetElement( i - 1);
	//			IdPoint3r const & ptV1 = pPreviousPattern->GetElement( i);
	//			IdPoint3r const & ptV2 = pPattern->GetElement( i);
	//			IdPoint3r const & ptV3 = pPattern->GetElement( i - 1);

	//			ptDiff = ptV1.GetPoint() - ptV0.GetPoint();
	//			fDistanceToOrigin += real( point::distance_squared( ptDiff ) );
	//			ptCurrentUV[1] = fDistanceToOrigin / fTotalDistance;

	//			submesh->AddQuadFace( ptV0.GetIndex(), ptV1.GetIndex(), ptV2.GetIndex(), ptV3.GetIndex(), uiNbElem, ptPreviousUV, ptCurrentUV);

	//			ptPreviousUV[1] = ptCurrentUV[1];
	//		}

	//		pPreviousPattern = pPattern;
	//		arrayPatterns.push_back( pPattern);
	//		ptPreviousUV[0] = ptCurrentUV[0];
	//	}
	//}

	//p_mesh.ComputeContainers();
}
