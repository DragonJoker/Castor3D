#include "Sphere.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"

#include "Miscellaneous/Parameter.hpp"

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

void Sphere::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	String param;

	if ( p_parameters.get( cuT( "subdiv" ), param ) )
	{
		m_nbFaces = string::toUInt( param );
	}

	if ( p_parameters.get( cuT( "radius" ), param ) )
	{
		m_radius = string::toFloat( param );
	}

	if ( m_nbFaces >= 3 )
	{
		Submesh & submesh = *p_mesh.createSubmesh();
		real rAngle = real( Angle::PiMult2 ) / m_nbFaces;
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
					BufferElementGroupSPtr vertex = submesh.addPoint( ptT[0] * rCos, ptT[1], ptT[0] * rSin );
					Vertex::setTexCoord( vertex, real( i ) / m_nbFaces, real( 1.0 + ptT[1] / m_radius ) / 2 );
					Vertex::setNormal( vertex, point::getNormalised( Vertex::getPosition( vertex, ptPos ) ) );
					//l_vertex->setTangent( point::getNormalised( Point3r( real( cos( dAlphaI + Angle::PiDiv2 ) ), real( 0.0 ), real( sin( dAlphaI + Angle::PiDiv2 ) ) ) ) );
					iCur++;
				}
			}

			// Calcul de la position des points
			rAlphaI = 0;

			for ( uint32_t i = 0; i <= m_nbFaces; rAlphaI += rAngle, i++ )
			{
				real rCos = cos( rAlphaI );
				real rSin = sin( rAlphaI );
				BufferElementGroupSPtr vertex = submesh.addPoint( ptB[0] * rCos, ptB[1], ptB[0] * rSin );
				Vertex::setTexCoord( vertex, real( i ) / m_nbFaces, real( 1.0 + ptB[1] / m_radius ) / 2 );
				Vertex::setNormal( vertex, point::getNormalised( Vertex::getPosition( vertex, ptPos ) ) );
				// vertex->setTangent( point::getNormalised( Point3r( real( cos( dAlphaI + Angle::PiDiv2 ) ), real( 0.0 ), real( sin( dAlphaI + Angle::PiDiv2 ) ) ) ) );
			}

			// Reconstition des faces
			for ( uint32_t i = 0; i < m_nbFaces; i++ )
			{
				submesh.addFace( iPrv + 0, iCur + 0, iPrv + 1 );
				submesh.addFace( iCur + 0, iCur + 1, iPrv + 1 );
				iPrv++;
				iCur++;
			}

			iPrv++;
			iCur++;
		}

		submesh.computeTangentsFromNormals();
	}

	p_mesh.computeContainers();
}
