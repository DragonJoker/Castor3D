#include "Sphere.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"

#include "Miscellaneous/Parameter.hpp"

using namespace Castor3D;
using namespace Castor;

Sphere::Sphere()
	: MeshGenerator( MeshType::eSphere )
	, m_radius( 0 )
	, m_nbFaces( 0 )
{
}

Sphere::~Sphere()
{
}

MeshGeneratorSPtr Sphere::Create()
{
	return std::make_shared< Sphere >();
}

void Sphere::DoGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	String l_param;

	if ( p_parameters.Get( cuT( "subdiv" ), l_param ) )
	{
		m_nbFaces = string::to_uint( l_param );
	}

	if ( p_parameters.Get( cuT( "radius" ), l_param ) )
	{
		m_radius = string::to_float( l_param );
	}

	if ( m_nbFaces >= 3 )
	{
		Submesh & l_submesh = *p_mesh.CreateSubmesh();
		real l_rAngle = real( Angle::PiMult2 ) / m_nbFaces;
		std::vector< Point2r > l_arc( m_nbFaces + 1 );
		real l_rAlpha = 0;
		uint32_t l_iCur = 0;
		uint32_t l_iPrv = 0;
		real l_rAlphaI = 0;
		Point3r l_ptPos;

		for ( uint32_t i = 0; i <= m_nbFaces; i++ )
		{
			real l_x =  m_radius * sin( l_rAlpha );
			real l_y = -m_radius * cos( l_rAlpha );
			l_arc[i][0] = l_x;
			l_arc[i][1] = l_y;
			l_rAlpha += l_rAngle / 2;
		}

		for ( uint32_t k = 0; k < m_nbFaces; k++ )
		{
			Point2r l_ptT = l_arc[k + 0];
			Point2r l_ptB = l_arc[k + 1];

			if ( k == 0 )
			{
				// Calcul de la position des points du haut
				for ( uint32_t i = 0; i <= m_nbFaces; l_rAlphaI += l_rAngle, i++ )
				{
					real l_rCos = cos( l_rAlphaI );
					real l_rSin = sin( l_rAlphaI );
					BufferElementGroupSPtr l_vertex = l_submesh.AddPoint( l_ptT[0] * l_rCos, l_ptT[1], l_ptT[0] * l_rSin );
					Vertex::SetTexCoord( l_vertex, real( i ) / m_nbFaces, real( 1.0 + l_ptT[1] / m_radius ) / 2 );
					Vertex::SetNormal( l_vertex, point::get_normalised( Vertex::GetPosition( l_vertex, l_ptPos ) ) );
					//l_vertex->SetTangent( point::get_normalised( Point3r( real( cos( dAlphaI + Angle::PiDiv2 ) ), real( 0.0 ), real( sin( dAlphaI + Angle::PiDiv2 ) ) ) ) );
					l_iCur++;
				}
			}

			// Calcul de la position des points
			l_rAlphaI = 0;

			for ( uint32_t i = 0; i <= m_nbFaces; l_rAlphaI += l_rAngle, i++ )
			{
				real l_rCos = cos( l_rAlphaI );
				real l_rSin = sin( l_rAlphaI );
				BufferElementGroupSPtr l_vertex = l_submesh.AddPoint( l_ptB[0] * l_rCos, l_ptB[1], l_ptB[0] * l_rSin );
				Vertex::SetTexCoord( l_vertex, real( i ) / m_nbFaces, real( 1.0 + l_ptB[1] / m_radius ) / 2 );
				Vertex::SetNormal( l_vertex, point::get_normalised( Vertex::GetPosition( l_vertex, l_ptPos ) ) );
				// l_vertex->SetTangent( point::get_normalised( Point3r( real( cos( dAlphaI + Angle::PiDiv2 ) ), real( 0.0 ), real( sin( dAlphaI + Angle::PiDiv2 ) ) ) ) );
			}

			// Reconstition des faces
			for ( uint32_t i = 0; i < m_nbFaces; i++ )
			{
				l_submesh.AddFace( l_iPrv + 0, l_iCur + 0, l_iPrv + 1 );
				l_submesh.AddFace( l_iCur + 0, l_iCur + 1, l_iPrv + 1 );
				l_iPrv++;
				l_iCur++;
			}

			l_iPrv++;
			l_iCur++;
		}

		l_submesh.ComputeTangentsFromNormals();
	}

	p_mesh.ComputeContainers();
}
