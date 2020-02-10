#include "Castor3D/Mesh/Generator/Projection.hpp"

#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/Vertex.hpp"

namespace castor3d
{
	Projection::Projection()
		: MeshGenerator( cuT( "projection" ) )
		, m_depth( 0 )
		, m_closed( false )
		, m_nbFaces( 0 )
	{
	}

	Projection::~Projection()
	{
		//m_pPattern.reset();
	}

	MeshGeneratorSPtr Projection::create()
	{
		return std::make_shared< Projection >();
	}

	void Projection::setPoints( Pattern< castor::Point3f > const & pattern
		, castor::Point3f const & axis
		, bool closed )
	{
		m_pattern = pattern;
		m_closed = closed;
		m_axis = axis;
		castor::point::normalise( m_axis );
		m_axis = m_axis * m_depth;
	}

	void Projection::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
	//	m_nbFaces = faces[0];
	//	m_depth = dimensions[0];
	//	uint32_t nbElem = m_pattern.getSize();
	//	real totalDistance = 0.0;
	//	SubmeshSPtr submesh;
	//	real distanceToOrigin = 0.0;
	//	Point3r currentUV;
	//	Point3r previousUV;
	//	auto previousPattern = m_pattern;

	//	if ( nbElem > 0 )
	//	{
	//		if ( m_closed && m_pattern.getElement( 0 ) != m_pattern.getElement( nbElem - 1 ) )
	//		{
	//			m_pattern.addElement( m_pattern.getElement( nbElem - 1), 0);
	//			nbElem++;
	//		}

	//		for (uint32_t i = 1; i < nbElem; i++)
	//		{
	//			auto diff = m_pattern.getElement( i ) - m_pattern.getElement( i - 1 );
	//			totalDistance += real( point::lengthSquared( diff ) );
	//		}

	//		for( uint32_t i = 0; i < nbElem; i++ )
	//		{
	//			mesh.createSubmesh();
	//		}

	//		if (m_closed)
	//		{
	//			mesh.createSubmesh();
	//		}

	//		// Construction des faces
	//		for (uint32_t j = 0; j < m_nbFaces; j++)
	//		{
	//			Pattern< castor::Point3f > pattern;

	//			currentUV[1] = 0;
	//			previousUV[1] = 0;
	//			currentUV[0] = real( j + 1 ) / real( m_nbFaces );

	//			for (uint32_t i = 0; i < nbElem; i++)
	//			{
	//				pattern.addElement( submesh->addPoint( previousPattern.getElement( i ) + m_axis ).pos, i);
	//			}

	//			for (uint32_t i = 1; i < nbElem; i++)
	//			{
	//				auto const & ptV0 = previousPattern.getElement( i - 1);
	//				auto const & ptV1 = previousPattern.getElement( i);
	//				auto const & ptV2 = pattern.getElement( i);
	//				auto const & ptV3 = pattern.getElement( i - 1);

	//				auto diff = ptV1 - ptV0;
	//				distanceToOrigin += real( point::lengthSquared( diff ) );
	//				currentUV[1] = distanceToOrigin / totalDistance;

	//				submesh->addQuadFace( ptV0.getIndex(), ptV1.getIndex(), ptV2.getIndex(), ptV3.getIndex(), nbElem, ptPreviousUV, ptCurrentUV );

	//				previousUV[1] = currentUV[1];
	//			}

	//			previousPattern = pattern;
	//			previousUV[0] = currentUV[0];
	//		}
	//	}

	//	mesh.computeContainers();
	}
}
