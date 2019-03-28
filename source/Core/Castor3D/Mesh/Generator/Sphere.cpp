#include "Castor3D/Mesh/Generator/Sphere.hpp"

#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/Vertex.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

using namespace castor3d;
using namespace castor;

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
	String param;

	if ( parameters.get( cuT( "subdiv" ), param ) )
	{
		m_nbFaces = string::toUInt( param );
	}

	if ( parameters.get( cuT( "radius" ), param ) )
	{
		m_radius = string::toFloat( param );
	}

	if ( m_nbFaces >= 3 )
	{
		Submesh & submesh = *mesh.createSubmesh();
		real rAngle = PiMult2< real > / m_nbFaces;
		std::vector< Point2r > arc( m_nbFaces + 1 );
		real rAlpha = 0;
		uint32_t iCur = 0;
		uint32_t iPrv = 0;
		real rAlphaI = 0;
		Point3r ptPos;

		for ( uint32_t i = 0; i <= m_nbFaces; i++ )
		{
			real x =  m_radius * sin( rAlpha );
			real y = -m_radius * cos( rAlpha );
			arc[i][0] = x;
			arc[i][1] = y;
			rAlpha += rAngle / 2;
		}

		auto indexMapping = std::make_shared< TriFaceMapping >( submesh );

		for ( uint32_t k = 0; k < m_nbFaces; k++ )
		{
			Point2r ptT = arc[k + 0];
			Point2r ptB = arc[k + 1];

			if ( k == 0 )
			{
				// Calcul de la position des points du haut
				for ( uint32_t i = 0; i <= m_nbFaces; rAlphaI += rAngle, i++ )
				{
					real rCos = cos( rAlphaI );
					real rSin = sin( rAlphaI );
					auto pos = Point3f{ ptT[0] * rCos, ptT[1], ptT[0] * rSin };
					submesh.addPoint( InterleavedVertex::createPNT( pos
						, Point3f{ point::getNormalised( pos ) }
						, Point2f{ real( i ) / m_nbFaces, real( 1.0 + ptT[1] / m_radius ) / 2 } ) );
					iCur++;
				}
			}

			// Calcul de la position des points
			rAlphaI = 0;

			for ( uint32_t i = 0; i <= m_nbFaces; rAlphaI += rAngle, i++ )
			{
				real rCos = cos( rAlphaI );
				real rSin = sin( rAlphaI );
				auto pos = Point3f{ ptB[0] * rCos, ptB[1], ptB[0] * rSin };
				submesh.addPoint( InterleavedVertex::createPNT( pos
					, Point3f{ point::getNormalised( pos ) }
					, Point2f{ real( i ) / m_nbFaces, real( 1.0 + ptB[1] / m_radius ) / 2 } ) );
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
