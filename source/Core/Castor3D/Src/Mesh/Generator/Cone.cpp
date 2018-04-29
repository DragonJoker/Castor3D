#include "Cone.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace castor;

namespace castor3d
{
	Cone::Cone()
		: MeshGenerator( cuT( "cone" ) )
		, m_nbFaces( 0 )
		, m_height( 0 )
		, m_radius( 0 )
	{
	}

	Cone::~Cone()
	{
	}

	MeshGeneratorSPtr Cone::create()
	{
		return std::make_shared< Cone >();
	}

	void Cone::doGenerate( Mesh & mesh
		, Parameters const & parameters )
	{
		String param;

		if ( parameters.get( cuT( "faces" ), param ) )
		{
			m_nbFaces = string::toUInt( param );
		}

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			m_radius = string::toFloat( param );
		}

		if ( parameters.get( cuT( "height" ), param ) )
		{
			m_height = string::toFloat( param );
		}

		if ( m_nbFaces >= 2 && m_height > std::numeric_limits< real >::epsilon() && m_radius > std::numeric_limits< real >::epsilon() )
		{
			Submesh & submeshBase = *mesh.createSubmesh();
			Submesh & submeshSide = *mesh.createSubmesh();
			//CALCUL DE LA POSITION DES POINTS
			real angleRotation = real( Angle::PiMult2 / m_nbFaces );
			uint32_t i = 0;
			real rCos, rSin;
			InterleavedVertexArray baseVertex;
			InterleavedVertexArray sideVertex;

			for ( real dAlphaI = 0; i <= m_nbFaces; dAlphaI += angleRotation )
			{
				rCos = cos( dAlphaI );
				rSin = sin( dAlphaI );

				if ( i < m_nbFaces )
				{
					baseVertex.push_back( InterleavedVertex::createPT( Point3f{ m_radius * rCos, 0.0, m_radius * rSin }
						, Point2f{ ( 1 + rCos ) / 2, ( 1 + rSin ) / 2 } ) );
				}

				sideVertex.push_back( InterleavedVertex::createPT( Point3f{ m_radius * rCos, 0.0, m_radius * rSin }
					, Point2f{ real( i ) / m_nbFaces, real( 1.0 ) } ) );
				sideVertex.push_back( InterleavedVertex::createPT( Point3f{ real( 0 ), m_height, real( 0 ) }
					, Point2f{ real( i ) / m_nbFaces, real( 0.0 ) } ) );
				i++;
			}
			
			baseVertex.push_back( InterleavedVertex::createPT( Point3f{ 0.0, 0.0, 0.0 }
				, Point2f{ 0.5, 0.5 } ) );
			auto bottomCenterIndex = uint32_t( baseVertex.size() - 1u );
			submeshBase.addPoints( baseVertex );
			submeshSide.addPoints( sideVertex );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			auto indexMappingBase = std::make_shared< TriFaceMapping >( submeshBase );
			auto indexMappingSide = std::make_shared< TriFaceMapping >( submeshSide );

			//Composition des extrémités
			for ( i = 0; i < m_nbFaces - 1; i++ )
			{
				//Composition du bas
				indexMappingBase->addFace( i + 1, i, bottomCenterIndex );
			}

			//Composition du bas
			indexMappingBase->addFace( 0, m_nbFaces - 1, bottomCenterIndex );

			//Composition des côtés
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				indexMappingSide->addFace( i + 1, i + 0, i + 2 );
			}

			submeshBase.setIndexMapping( indexMappingBase );
			submeshSide.setIndexMapping( indexMappingSide );

			indexMappingBase->computeNormals( true );
			indexMappingSide->computeNormals( true );

			auto normal0Top = submeshSide[0].nml;
			auto normal0Base = submeshSide[1].nml;
			auto tangent0Top = submeshSide[0].tan;
			auto tangent0Base = submeshSide[1].tan;
			normal0Top += submeshSide[submeshSide.getPointsCount() - 2].nml;
			normal0Base += submeshSide[submeshSide.getPointsCount() - 1].nml;
			tangent0Top += submeshSide[submeshSide.getPointsCount() - 2].tan;
			tangent0Base += submeshSide[submeshSide.getPointsCount() - 1].tan;
			point::normalise( normal0Top );
			point::normalise( normal0Base );
			point::normalise( tangent0Top );
			point::normalise( tangent0Base );
			submeshSide[submeshSide.getPointsCount() - 2].nml = normal0Top;
			submeshSide[submeshSide.getPointsCount() - 1].nml = normal0Base;
			submeshSide[submeshSide.getPointsCount() - 2].tan = tangent0Top;
			submeshSide[submeshSide.getPointsCount() - 1].tan = tangent0Base;

			mesh.computeContainers();
		}
	}
}
