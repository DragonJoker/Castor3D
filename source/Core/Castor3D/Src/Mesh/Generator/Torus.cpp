#include "Torus.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace Castor3D;
using namespace Castor;

Torus::Torus()
	: MeshGenerator( MeshType::eTorus )
	, m_internalRadius( 0 )
	, m_externalRadius( 0 )
	, m_internalNbFaces( 0 )
	, m_externalNbFaces( 0 )
{
}

Torus::~Torus()
{
}

MeshGeneratorSPtr Torus::Create()
{
	return std::make_shared< Torus >();
}

void Torus::DoGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	String l_param;

	if ( p_parameters.Get( cuT( "inner_size" ), l_param ) )
	{
		m_internalRadius = string::to_float( l_param );
	}

	if ( p_parameters.Get( cuT( "outer_size" ), l_param ) )
	{
		m_externalRadius = string::to_float( l_param );
	}

	if ( p_parameters.Get( cuT( "inner_count" ), l_param ) )
	{
		m_internalNbFaces = string::to_uint( l_param );
	}

	if ( p_parameters.Get( cuT( "outer_count" ), l_param ) )
	{
		m_externalNbFaces = string::to_uint( l_param );
	}

	p_mesh.Cleanup();

	if ( m_internalNbFaces >= 3 && m_externalNbFaces >= 3 )
	{
		Submesh & l_submesh = *( p_mesh.CreateSubmesh() );
		uint32_t l_uiCur = 0;
		uint32_t l_uiPrv = 0;
		uint32_t l_uiPCr = 0;
		uint32_t l_uiPPr = 0;
		real l_rAngleIn = 0.0;
		real l_rAngleEx = 0.0;
		uint32_t l_uiExtMax = m_externalNbFaces;
		uint32_t l_uiIntMax = m_internalNbFaces;
		Point3r l_ptPos;
		Point3r l_ptNml;
		Coords3r l_ptTangent0;
		Coords3r l_ptTangent1;

		// Build the internal circle that will be rotated to build the torus
		real l_step = real( Angle::PiMult2 ) / m_internalNbFaces;

		for ( uint32_t j = 0; j <= l_uiIntMax; j++ )
		{
			BufferElementGroupSPtr l_vertex = l_submesh.AddPoint( m_internalRadius * cos( l_rAngleIn ) + m_externalRadius, m_internalRadius * sin( l_rAngleIn ), 0.0 );
			Vertex::SetTexCoord( l_vertex, real( 0.0 ), real( j ) / m_internalNbFaces );
			Vertex::SetNormal( l_vertex, point::get_normalised( Point3r( real( cos( l_rAngleIn ) ), real( sin( l_rAngleIn ) ), real( 0.0 ) ) ) );
			l_uiCur++;
			l_rAngleIn += l_step;
		}

		// Build the torus
		l_step = real( Angle::PiMult2 ) / m_externalNbFaces;

		for ( uint32_t i = 1; i <= l_uiExtMax; i++ )
		{
			l_uiPCr = l_uiCur;
			l_uiPPr = l_uiPrv;
			l_rAngleEx += l_step;

			for ( uint32_t j = 0; j <= l_uiIntMax; j++ )
			{
				BufferElementGroupSPtr l_vertex = l_submesh[j];
				Vertex::GetPosition( l_vertex, l_ptPos );
				Vertex::GetNormal( l_vertex, l_ptNml );
				l_vertex = l_submesh.AddPoint( l_ptPos[0] * cos( l_rAngleEx ), l_ptPos[1], l_ptPos[0] * sin( l_rAngleEx ) );
				Vertex::SetTexCoord( l_vertex, real( i ) / m_externalNbFaces, real( j ) / m_internalNbFaces );
				Vertex::SetNormal( l_vertex, point::get_normalised( Point3r( real( l_ptNml[0] * cos( l_rAngleEx ) ), real( l_ptNml[1] ), real( l_ptNml[0] * sin( l_rAngleEx ) ) ) ) );
			}

			for ( uint32_t j = 0; j <= l_uiIntMax - 1; j++ )
			{
				l_submesh.AddFace( l_uiCur + 1, l_uiPrv + 0, l_uiPrv + 1 );
				l_submesh.AddFace( l_uiCur + 0, l_uiPrv + 0, l_uiCur + 1 );
				l_uiPrv++;
				l_uiCur++;
			}

			l_submesh.AddFace( l_uiPCr + 0, l_uiPrv + 0, l_uiPPr + 0 );
			l_submesh.AddFace( l_uiCur + 0, l_uiPrv + 0, l_uiPCr + 0 );
			l_uiPrv++;
			l_uiCur++;
		}

		l_uiCur = 0;
		l_uiPCr = l_uiCur;
		l_uiPPr = l_uiPrv;

		for ( uint32_t j = 0; j <= l_uiIntMax - 1; j++ )
		{
			l_submesh.AddFace( l_uiCur + 1, l_uiPrv + 0, l_uiPrv + 1 );
			l_submesh.AddFace( l_uiCur + 0, l_uiPrv + 0, l_uiCur + 1 );
			l_uiPrv++;
			l_uiCur++;
		}

		l_submesh.AddFace( l_uiPCr + 0, l_uiPrv + 0, l_uiPPr + 0 );
		l_submesh.AddFace( l_uiCur + 0, l_uiPrv + 0, l_uiPCr + 0 );
		l_uiPrv++;
		l_uiCur++;
		l_submesh.ComputeTangentsFromNormals();
	}

	p_mesh.ComputeContainers();
}
