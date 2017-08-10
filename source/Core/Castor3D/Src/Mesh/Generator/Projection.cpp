#include "Projection.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"

using namespace castor3d;
using namespace castor;

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

MeshGeneratorSPtr Projection::create()
{
	return std::make_shared< Projection >();
}

void Projection::setPoints( Point3rPatternSPtr p_pPattern, Point3r const & p_vAxis, bool p_bClosed )
{
	m_pPattern = p_pPattern;
	m_bClosed = p_bClosed;
	m_vAxis = p_vAxis;
	point::normalise( m_vAxis );
	m_vAxis = m_vAxis * m_fDepth;
}

void Projection::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	//m_uiNbFaces = p_faces[0];
	//m_fDepth = p_dimensions[0];
	//uint32_t uiNbElem = m_pPattern->getSize();
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
	//	if (m_bClosed && m_pPattern->getElement( 0 ).getPoint() != m_pPattern->getElement( uiNbElem - 1 ).getPoint() )
	//	{
	//		m_pPattern->addElement( m_pPattern->getElement( uiNbElem - 1), 0);
	//		uiNbElem++;
	//	}

	//	for (uint32_t i = 1; i < uiNbElem; i++)
	//	{
	//		ptDiff = m_pPattern->getElement( i ).getPoint() - m_pPattern->getElement( i - 1 ).getPoint();
	//		fTotalDistance += real( point::distanceSquared( ptDiff ) );
	//	}

	//	for( uint32_t i = 0; i < uiNbElem; i++ )
	//	{
	//		getMesh()->createSubmesh();
	//	}

	//	if (m_bClosed)
	//	{
	//		getMesh()->createSubmesh();
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
	//			pPattern->addElement( *submesh->addPoint( pPreviousPattern->getElement( i ).getPoint() + m_vAxis ), i);
	//		}

	//		for (uint32_t i = 1; i < uiNbElem; i++)
	//		{
	//			IdPoint3r const & ptV0 = pPreviousPattern->getElement( i - 1);
	//			IdPoint3r const & ptV1 = pPreviousPattern->getElement( i);
	//			IdPoint3r const & ptV2 = pPattern->getElement( i);
	//			IdPoint3r const & ptV3 = pPattern->getElement( i - 1);

	//			ptDiff = ptV1.getPoint() - ptV0.getPoint();
	//			fDistanceToOrigin += real( point::distanceSquared( ptDiff ) );
	//			ptCurrentUV[1] = fDistanceToOrigin / fTotalDistance;

	//			submesh->addQuadFace( ptV0.getIndex(), ptV1.getIndex(), ptV2.getIndex(), ptV3.getIndex(), uiNbElem, ptPreviousUV, ptCurrentUV);

	//			ptPreviousUV[1] = ptCurrentUV[1];
	//		}

	//		pPreviousPattern = pPattern;
	//		arrayPatterns.push_back( pPattern);
	//		ptPreviousUV[0] = ptCurrentUV[0];
	//	}
	//}

	//p_mesh.computeContainers();
}
