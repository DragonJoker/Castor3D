#include "Castor3D/Model/Mesh/Generator/Cone.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

CU_ImplementSmartPtr( castor3d, Cone )

namespace castor3d
{
	namespace cone
	{
		static void swapComponents( uint32_t lhs
			, uint32_t rhs
			, InterleavedVertexArray & vertices )
		{
			for ( auto & vtx : vertices )
			{
				castor::swap( vtx.pos[lhs], vtx.pos[rhs] );
			}
		}
	}

	Cone::Cone()
		: MeshGenerator( cuT( "cone" ) )
	{
	}

	MeshGeneratorUPtr Cone::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Cone >();
	}

	void Cone::doGenerate( Mesh & mesh
		, Parameters const & parameters )
	{
		castor::String param;
		uint32_t nbFaces{};
		float height{};
		float radius{};
		uint32_t axis = 1u;

		if ( parameters.get( cuT( "faces" ), param ) )
		{
			nbFaces = castor::string::toUInt( param );
		}

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			radius = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "height" ), param ) )
		{
			height = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "axis" ), param ) )
		{
			if ( param == cuT( "x" ) )
			{
				axis = 0u;
			}
			else if ( param == cuT( "z" ) )
			{
				axis = 2u;
			}
		}

		if ( nbFaces >= 2
			&& height > std::numeric_limits< float >::epsilon()
			&& radius > std::numeric_limits< float >::epsilon() )
		{
			Submesh & submeshBase = *mesh.createDefaultSubmesh();
			Submesh & submeshSide = *mesh.createDefaultSubmesh();
			//CALCUL DE LA POSITION DES POINTS
			float const dalpha = castor::PiMult2< float > / float( nbFaces );
			uint32_t i = 0;
			InterleavedVertexArray baseVertex;
			InterleavedVertexArray sideVertex;
			baseVertex.reserve( size_t( nbFaces ) + 1u );
			sideVertex.reserve( ( size_t( nbFaces ) + 1u ) * 2u );

			for ( float alpha = 0; i <= nbFaces; alpha += dalpha )
			{
				auto rCos = cos( alpha );
				auto rSin = sin( alpha );

				if ( i < nbFaces )
				{
					baseVertex.emplace_back()
						.position( castor::Point3f{ radius * rCos, 0.0, radius * rSin } )
						.texcoord( castor::Point2f{ ( 1 + rCos ) / 2, ( 1 + rSin ) / 2 } );
				}

				sideVertex.emplace_back()
					.position( castor::Point3f{ radius * rCos, 0.0, radius * rSin } )
					.texcoord( castor::Point2f{ float( i ) / float( nbFaces ), float( 1.0 ) } );
				sideVertex.emplace_back()
					.position( castor::Point3f{ float( 0 ), height, float( 0 ) } )
					.texcoord( castor::Point2f{ float( i ) / float( nbFaces ), float( 0.0 ) } );
				i++;
			}

			if ( axis != 1u )
			{
				cone::swapComponents( axis, 1u, baseVertex );
				cone::swapComponents( axis, 1u, sideVertex );
			}
			
			baseVertex.emplace_back()
				.position( castor::Point3f{ 0.0, 0.0, 0.0 } )
				.texcoord( castor::Point2f{ 0.5, 0.5 } );
			auto bottomCenterIndex = uint32_t( baseVertex.size() - 1u );
			submeshBase.addPoints( baseVertex );
			submeshSide.addPoints( sideVertex );

			//RECONSTITION DES FACES
			auto indexMappingBase = submeshBase.createComponent< TriFaceMapping >();
			auto & indexMappingBaseData = indexMappingBase->getData();
			auto indexMappingSide = submeshSide.createComponent< TriFaceMapping >();
			auto & indexMappingSideData = indexMappingSide->getData();

			//Composition des extrémités
			for ( i = 0; i < nbFaces - 1; i++ )
			{
				//Composition du bas
				indexMappingBaseData.addFace( i + 1, i, bottomCenterIndex );
			}

			//Composition du bas
			indexMappingBaseData.addFace( 0, nbFaces - 1, bottomCenterIndex );

			//Composition des côtés
			for ( i = 0; i < 2 * nbFaces; i += 2 )
			{
				indexMappingSideData.addFace( i + 1, i + 0, i + 2 );
			}

			indexMappingBase->computeNormals();
			indexMappingBase->computeTangents();
			indexMappingSide->computeNormals();
			indexMappingSide->computeTangents();

			// Join the first and last edge of the cone, tangent space wise.
			auto & sideNormals = submeshSide.getComponent< NormalsComponent >()->getData();
			auto & sideTangents = submeshSide.getComponent< TangentsComponent >()->getData();
			auto normal0Top = sideNormals.getData()[0];
			auto normal0Base = sideNormals.getData()[1];
			auto tangent0Top = sideTangents.getData()[0];
			auto tangent0Base = sideTangents.getData()[1];
			normal0Top += sideNormals.getData()[submeshSide.getPointsCount() - 2];
			normal0Base += sideNormals.getData()[submeshSide.getPointsCount() - 1];
			tangent0Top += sideTangents.getData()[submeshSide.getPointsCount() - 2];
			tangent0Base += sideTangents.getData()[submeshSide.getPointsCount() - 1];
			castor::point::normalise( normal0Top );
			castor::point::normalise( normal0Base );
			castor::point::normalise( tangent0Top );
			castor::point::normalise( tangent0Base );
			sideNormals.getData()[submeshSide.getPointsCount() - 2] = normal0Top;
			sideNormals.getData()[submeshSide.getPointsCount() - 1] = normal0Base;
			sideTangents.getData()[submeshSide.getPointsCount() - 2] = tangent0Top;
			sideTangents.getData()[submeshSide.getPointsCount() - 1] = tangent0Base;
		}
	}
}
