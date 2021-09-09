#include "Castor3D/Model/Mesh/Generator/Sphere.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

namespace castor3d
{
	Sphere::Sphere()
		: MeshGenerator( cuT( "sphere" ) )
		, m_radius( 0 )
		, m_nbFaces( 0 )
	{
	}

	Sphere::~Sphere()
	{
	}

	MeshGeneratorSPtr Sphere::create()
	{
		return std::make_shared< Sphere >();
	}

	void Sphere::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;

		if ( parameters.get( cuT( "subdiv" ), param ) )
		{
			m_nbFaces = castor::string::toUInt( param );
		}

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			m_radius = castor::string::toFloat( param );
		}

		if ( m_nbFaces >= 3 )
		{
			Submesh & submesh = *mesh.createSubmesh();
			float rAngle = castor::PiMult2< float > / float( m_nbFaces );
			std::vector< castor::Point2f > arc( m_nbFaces + 1u );
			float rAlpha = 0;
			uint32_t iCur = 0;
			uint32_t iPrv = 0;
			float rAlphaI = 0;
			castor::Point3f ptPos;

			for ( uint32_t i = 0; i <= m_nbFaces; i++ )
			{
				float x = m_radius * sin( rAlpha );
				float y = -m_radius * cos( rAlpha );
				arc[i][0] = x;
				arc[i][1] = y;
				rAlpha += rAngle / 2;
			}

			auto indexMapping = std::make_shared< TriFaceMapping >( submesh );

			for ( uint32_t k = 0; k < m_nbFaces; k++ )
			{
				castor::Point2f ptT = arc[k + 0];
				castor::Point2f ptB = arc[k + 1];

				if ( k == 0 )
				{
					// Calcul de la position des points du haut
					for ( uint32_t i = 0; i <= m_nbFaces; rAlphaI += rAngle, i++ )
					{
						float rCos = cos( rAlphaI );
						float rSin = sin( rAlphaI );
						auto pos = castor::Point3f{ ptT[0] * rCos, ptT[1], ptT[0] * rSin };
						submesh.addPoint( InterleavedVertex{}
							.position( pos )
							.normal( castor::point::getNormalised( pos ) )
							.texcoord( castor::Point2f{ float( i ) / float( m_nbFaces ), float( 1.0 + ptT[1] / m_radius ) / 2 } ) );
						iCur++;
					}
				}

				// Calcul de la position des points
				rAlphaI = 0;

				for ( uint32_t i = 0; i <= m_nbFaces; rAlphaI += rAngle, i++ )
				{
					float rCos = cos( rAlphaI );
					float rSin = sin( rAlphaI );
					auto pos = castor::Point3f{ ptB[0] * rCos, ptB[1], ptB[0] * rSin };
					submesh.addPoint( InterleavedVertex{}
						.position( pos )
						.normal( castor::point::getNormalised( pos ) )
						.texcoord( castor::Point2f{ float( i ) / float( m_nbFaces ), float( 1.0 + ptB[1] / m_radius ) / 2 } ) );
				}

				// Reconstition des faces
				for ( uint32_t i = 0; i < m_nbFaces; i++ )
				{
					indexMapping->addFace( iCur + 0, iPrv + 0, iPrv + 1 );
					indexMapping->addFace( iCur + 1, iCur + 0, iPrv + 1 );
					iPrv++;
					iCur++;
				}

				iPrv++;
				iCur++;
			}

			indexMapping->computeTangentsFromNormals();
			submesh.setIndexMapping( indexMapping );
		}

		mesh.computeContainers();
	}
}
