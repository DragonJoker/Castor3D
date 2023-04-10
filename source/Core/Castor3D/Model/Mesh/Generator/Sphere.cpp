#include "Castor3D/Model/Mesh/Generator/Sphere.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( castor3d, Sphere )

namespace castor3d
{
	Sphere::Sphere()
		: MeshGenerator( cuT( "sphere" ) )
		, m_radius( 0 )
		, m_nbFaces( 0 )
	{
	}

	MeshGeneratorUPtr Sphere::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Sphere >();
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
			Submesh & submesh = *mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
			float rAngle = castor::PiMult2< float > / float( m_nbFaces );
			std::vector< castor::Point2f > arc( m_nbFaces + 1u );
			float rAlpha = 0;
			uint32_t iCur = 0;
			uint32_t iPrv = 0;
			float rAlphaI = 0;

			for ( uint32_t i = 0; i <= m_nbFaces; i++ )
			{
				auto x = float( m_radius * sin( rAlpha ) );
				auto y = float( -m_radius * cos( rAlpha ) );
				arc[i][0] = x;
				arc[i][1] = y;
				rAlpha += rAngle / 2;
			}

			auto indexMapping = submesh.createComponent< TriFaceMapping >();

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
					auto rCos = float( cos( rAlphaI ) );
					auto rSin = float( sin( rAlphaI ) );
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

			indexMapping->computeTangents();
		}
	}
}
