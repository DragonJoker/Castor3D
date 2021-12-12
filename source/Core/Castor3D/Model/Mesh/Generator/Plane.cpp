#include "Castor3D/Model/Mesh/Generator/Plane.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SecondaryUVComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

namespace castor3d
{
	Plane::Plane()
		: MeshGenerator{ cuT( "plane" ) }
	{
	}

	MeshGeneratorSPtr Plane::create()
	{
		return std::make_shared< Plane >();
	}

	void Plane::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;
		parameters.get( cuT( "tile_uv" ), m_tileUV );

		if ( parameters.get( cuT( "width_subdiv" ), param ) )
		{
			m_subDivisionsW = castor::string::toUInt( param );
		}

		if ( parameters.get( cuT( "depth_subdiv" ), param ) )
		{
			m_subDivisionsD = castor::string::toUInt( param );
		}

		if ( parameters.get( cuT( "width" ), param ) )
		{
			m_width = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "depth" ), param ) )
		{
			m_depth = castor::string::toFloat( param );
		}

		uint32_t nbVertexW = m_subDivisionsW + 2;
		uint32_t nbVertexH = m_subDivisionsD + 2;
		float offsetW = -m_width / 2;
		float offsetH = -m_depth / 2;
		float gapW = m_width / float( m_subDivisionsW + 1 );
		float gapH = m_depth / float( m_subDivisionsD + 1 );
		castor::Point3f ptCurrentUV;
		castor::Point3f ptPreviousUV;
		castor::Point3f ptNormal( 0.0, 1.0, 0.0 );
		castor::Point3f ptTangent;
		castor::Point2f ptUv;
		SubmeshSPtr submesh = mesh.createSubmesh();

		for ( uint32_t i = 0; i < nbVertexW; i++ )
		{
			for ( uint32_t j = 0; j < nbVertexH; j++ )
			{
				submesh->addPoint( InterleavedVertex{}
					.position( castor::Point3f{ offsetW + ( float( i ) * gapW ), offsetH + ( float( j ) * gapH ), 0.0 } )
					.normal( castor::Point3f{ 0.0, 0.0, 1.0 } )
					.texcoord( castor::Point2f{ float( i ) * gapW / m_width, float( j ) * gapH / m_depth } ) );
			}
		}

		if ( m_tileUV )
		{
			std::vector< castor::Point3f > tiledUV;

			for ( uint32_t i = 0; i < nbVertexW; i++ )
			{
				for ( uint32_t j = 0; j < nbVertexH; j++ )
				{
					tiledUV.push_back( castor::Point3f{ float( ( m_subDivisionsW + 1u )* i ) * gapW / m_width
						, float( ( m_subDivisionsD + 1u ) * j ) * gapH / m_depth
						, 0.0f } );
				}
			}

			auto secondaryUV = std::make_shared< SecondaryUVComponent >( *submesh );
			secondaryUV->addTexcoords( tiledUV );
			submesh->addComponent( secondaryUV );
		}

		auto indexMapping = std::make_shared< TriFaceMapping >( *submesh );

		for ( uint32_t i = 0; i < m_subDivisionsW + 1; i++ )
		{
			for ( uint32_t j = i * ( m_subDivisionsD + 1 ); j < ( i + 1 ) * ( m_subDivisionsD + 1 ); j++ )
			{
				indexMapping->addFace( j + m_subDivisionsW + 2 + i, j + i, j + m_subDivisionsW + 3 + i );
				indexMapping->addFace( j + i + 1, j + m_subDivisionsW + 3 + i, j + i );
			}
		}

		indexMapping->computeTangentsFromNormals();
		submesh->setIndexMapping( indexMapping );
		mesh.computeContainers();
	}
}
