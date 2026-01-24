#include "Castor3D/Model/Mesh/Generator/Projection.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

namespace c3d
{
	Projection::Projection()
		: MeshGenerator( cuT( "projection" ) )
	{
	}

	MeshGeneratorUPtr Projection::create()
	{
		return makeUniqueDerived< MeshGenerator, Projection >();
	}

	void Projection::setPoints( Pattern< Point3f > const & pattern
		, Point3f const & axis
		, bool closed )
	{
		m_pattern = pattern;
		m_closed = closed;
		m_axis = axis;
		point::normalise( m_axis );
		m_axis = m_axis * m_depth;
	}

	void Projection::doGenerate( [[maybe_unused]] Mesh & mesh
		, [[maybe_unused]] Parameters const & parameters )
	{
		String param;
		if ( parameters.get( cuT( "depth" ), param ) )
			m_depth = string::toFloat( param );
		if ( parameters.get( cuT( "faces" ), param ) )
			m_nbFaces = string::toUInt( param );

		uint32_t nbElem = m_pattern.getSize();
		auto previousPattern = m_pattern;

		if ( nbElem > 0 && m_nbFaces > 0 && m_depth > 0 )
		{
			if ( m_closed && m_pattern.getElement( 0 ) != m_pattern.getElement( nbElem - 1 ) )
			{
				m_pattern.addElement( m_pattern.getElement( nbElem - 1), 0);
				nbElem++;
			}

			float totalDistance = 0.0;
			for (uint32_t i = 1; i < nbElem; i++)
				totalDistance += float( point::lengthSquared( m_pattern.getElement( i ) - m_pattern.getElement( i - 1 ) ) );

			Vector< SubmeshRPtr > submeshes{};
			Vector< TriFaceMapping * > mappings{};
			for ( uint32_t i = 0; i < nbElem; i++ )
			{
				auto submesh = mesh.createDefaultSubmesh();
				mappings.push_back( submesh->createComponent< TriFaceMapping >() );
				submeshes.push_back( submesh );
			}

			if (m_closed)
			{
				auto submesh = mesh.createDefaultSubmesh();
				mappings.push_back( submesh->createComponent< TriFaceMapping >() );
				submeshes.push_back( submesh );
			}

			// Construction des faces
			Point3f currentUV;
			Point3f previousUV;
			float distanceToOrigin = 0.0;
			for (uint32_t j = 0; j < m_nbFaces; j++)
			{
				Pattern< Point3f > pattern;

				currentUV[1] = 0;
				previousUV[1] = 0;
				currentUV[0] = float( j + 1 ) / float( m_nbFaces );

				for ( uint32_t i = 0; i < nbElem; i++ )
					pattern.addElement( submeshes[i]->addPoint( previousPattern.getElement( i ) + m_axis ).pos, i );

				for (uint32_t i = 1; i < nbElem; i++)
				{
					auto const & ptV0 = previousPattern.getElement( i - 1 );
					auto const & ptV1 = previousPattern.getElement( i );
					auto diff = ptV1 - ptV0;
					distanceToOrigin += float( point::lengthSquared( diff ) );
					currentUV[1] = distanceToOrigin / totalDistance;

					mappings[i]->getData().addQuadFace( i - 1, i, nbElem + i, nbElem + i - 1
						, previousUV, currentUV );

					previousUV[1] = currentUV[1];
				}

				previousPattern = pattern;
				previousUV[0] = currentUV[0];
			}

			for ( auto indexMapping : mappings )
			{
				indexMapping->computeNormals();
				indexMapping->computeTangents();
			}
		}
	}
}
