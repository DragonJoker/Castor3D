#include "Castor3D/Model/Mesh/Generator/SphereSection.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#pragma GCC diagnostic ignored "-Wuseless-cast"

namespace castor3d
{
	SphereSection::SphereSection()
		: MeshGenerator( cuT( "sphere_section" ) )
		, m_radius( 0 )
		, m_angle( 0.0_degrees )
		, m_nbFaces( 0 )
	{
	}

	MeshGeneratorSPtr SphereSection::create()
	{
		return std::make_shared< SphereSection >();
	}

	void SphereSection::doGenerate( Mesh & mesh, Parameters const & parameters )
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

		if ( parameters.get( cuT( "angle" ), param ) )
		{
			m_angle = castor::Angle::fromDegrees( castor::string::toFloat( param ) );
		}

		if ( m_nbFaces >= 3
			&& m_angle.degrees() > 0.0f )
		{
			auto arcAngle = m_angle / ( 2.0f * float( m_nbFaces ) );
			std::vector< castor::Point2f > arc( m_nbFaces + 1u );
			castor::Angle arcAlpha = 0.0_degrees;
			uint32_t iCur = 0;
			uint32_t iPrv = 0;
			float rAlphaI = 0;
			auto rAngle = castor::PiMult2< float > / float( m_nbFaces );

			for ( uint32_t i = 0; i <= m_nbFaces; i++ )
			{
				arc[i]->x = float( m_radius * arcAlpha.sin() );
				arc[i]->y = float( m_radius * arcAlpha.cos() );
				arcAlpha += arcAngle;
			}

			// Constitution de la base sphérique
			Submesh & baseSubmesh = *mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
			auto baseIndexMapping = baseSubmesh.createComponent< TriFaceMapping >();

			for ( uint32_t k = 0; k < m_nbFaces; k++ )
			{
				castor::Point2f ptT = arc[k + 0];
				castor::Point2f ptB = arc[k + 1];

				if ( k == 0 )
				{
					// Calcul de la position des points du haut
					for ( uint32_t i = 0; i <= m_nbFaces; rAlphaI += rAngle, i++ )
					{
						auto rCos = float( cos( rAlphaI ) );
						auto rSin = float( sin( rAlphaI ) );
						auto pos = castor::Point3f{ ptT->x * rCos, ptT->x * rSin, ptT->y };
						baseSubmesh.addPoint( InterleavedVertex{}
							.position( pos )
							.normal( castor::point::getNormalised( pos ) )
							.texcoord( castor::Point2f{ 0.5f, 0.5f } ) );
						iCur++;
					}
				}

				// Calcul de la position des points
				rAlphaI = 0;

				for ( uint32_t i = 0; i <= m_nbFaces; rAlphaI += rAngle, i++ )
				{
					auto rCos = float( cos( rAlphaI ) );
					auto rSin = float( sin( rAlphaI ) );
					auto pos = castor::Point3f{ ptB->x * rCos, ptB->x * rSin, ptB->y };
					InterleavedVertex vertex{};
					vertex.position( pos )
						.normal( castor::point::getNormalised( pos ) )
						.texcoord( castor::Point2f{ 0.5 + ( float( k ) / float( m_nbFaces ) ) * rCos / 2.0f
							, 0.5 + ( float( k ) / float( m_nbFaces ) ) * rSin / 2.0f } );
					baseSubmesh.addPoint( vertex );
				}

				// Reconstition des faces de la base sphérique
				for ( uint32_t i = 0; i < m_nbFaces; i++ )
				{
					baseIndexMapping->addFace( iCur + 0, iPrv + 0, iPrv + 1 );
					baseIndexMapping->addFace( iCur + 1, iCur + 0, iPrv + 1 );
					iPrv++;
					iCur++;
				}

				iPrv++;
				iCur++;
			}

			baseIndexMapping->computeTangents();

			// Constitution des côtés
			Submesh & sideSubmesh = *mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
			castor::Point2f ptA = arc[m_nbFaces];
			rAlphaI = 0;

			// Calcul de la position des points des côtés
			for ( uint32_t i = 0; i <= m_nbFaces; rAlphaI += rAngle, i++ )
			{
				auto rCos = float( cos( rAlphaI ) );
				auto rSin = float( sin( rAlphaI ) );
				sideSubmesh.addPoint( InterleavedVertex{}
					.position( { ptA->x * rCos, ptA->x * rSin, ptA->y } )
					.texcoord( castor::Point2f{ float( i ) / float( m_nbFaces ), float( 0.0 ) } ) );
				sideSubmesh.addPoint( InterleavedVertex{}
					.position( { 0.0f, 0.0f, 0.0f } )
					.texcoord( castor::Point2f{ float( i ) / float( m_nbFaces ), float( 1.0 ) } ) );
				iCur++;
			}

			// Reconstition des faces des côtés
			auto sideIndexMapping = sideSubmesh.createComponent< TriFaceMapping >();

			for ( uint32_t i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				sideIndexMapping->addFace( i + 1, i + 0, i + 2 );
			}

			sideIndexMapping->computeNormals();
		}
	}
}
