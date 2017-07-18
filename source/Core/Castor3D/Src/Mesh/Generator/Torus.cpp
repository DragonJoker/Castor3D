#include "Torus.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace Castor3D;
using namespace Castor;

Torus::Torus()
	: MeshGenerator( cuT( "torus" ) )
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
	String param;

	if ( p_parameters.Get( cuT( "inner_size" ), param ) )
	{
		m_internalRadius = string::to_float( param );
	}

	if ( p_parameters.Get( cuT( "outer_size" ), param ) )
	{
		m_externalRadius = string::to_float( param );
	}

	if ( p_parameters.Get( cuT( "inner_count" ), param ) )
	{
		m_internalNbFaces = string::to_uint( param );
	}

	if ( p_parameters.Get( cuT( "outer_count" ), param ) )
	{
		m_externalNbFaces = string::to_uint( param );
	}

	p_mesh.Cleanup();

	if ( m_internalNbFaces >= 3 && m_externalNbFaces >= 3 )
	{
		Submesh & submesh = *( p_mesh.CreateSubmesh() );
		uint32_t uiCur = 0;
		uint32_t uiPrv = 0;
		uint32_t uiPCr = 0;
		uint32_t uiPPr = 0;
		real rAngleIn = 0.0;
		real rAngleEx = 0.0;
		uint32_t uiExtMax = m_externalNbFaces;
		uint32_t uiIntMax = m_internalNbFaces;
		Point3r ptPos;
		Point3r ptNml;
		Coords3r ptTangent0;
		Coords3r ptTangent1;

		// Build the internal circle that will be rotated to build the torus
		real step = real( Angle::PiMult2 ) / m_internalNbFaces;

		for ( uint32_t j = 0; j <= uiIntMax; j++ )
		{
			BufferElementGroupSPtr vertex = submesh.AddPoint( m_internalRadius * cos( rAngleIn ) + m_externalRadius, m_internalRadius * sin( rAngleIn ), 0.0 );
			Vertex::SetTexCoord( vertex, real( 0.0 ), real( j ) / m_internalNbFaces );
			Vertex::SetNormal( vertex, point::get_normalised( Point3r( real( cos( rAngleIn ) ), real( sin( rAngleIn ) ), real( 0.0 ) ) ) );
			uiCur++;
			rAngleIn += step;
		}

		// Build the torus
		step = real( Angle::PiMult2 ) / m_externalNbFaces;

		for ( uint32_t i = 1; i <= uiExtMax; i++ )
		{
			uiPCr = uiCur;
			uiPPr = uiPrv;
			rAngleEx += step;

			for ( uint32_t j = 0; j <= uiIntMax; j++ )
			{
				BufferElementGroupSPtr vertex = submesh[j];
				Vertex::GetPosition( vertex, ptPos );
				Vertex::GetNormal( vertex, ptNml );
				vertex = submesh.AddPoint( ptPos[0] * cos( rAngleEx ), ptPos[1], ptPos[0] * sin( rAngleEx ) );
				Vertex::SetTexCoord( vertex, real( i ) / m_externalNbFaces, real( j ) / m_internalNbFaces );
				Vertex::SetNormal( vertex, point::get_normalised( Point3r( real( ptNml[0] * cos( rAngleEx ) ), real( ptNml[1] ), real( ptNml[0] * sin( rAngleEx ) ) ) ) );
			}

			for ( uint32_t j = 0; j <= uiIntMax - 1; j++ )
			{
				submesh.AddFace( uiCur + 1, uiPrv + 0, uiPrv + 1 );
				submesh.AddFace( uiCur + 0, uiPrv + 0, uiCur + 1 );
				uiPrv++;
				uiCur++;
			}

			submesh.AddFace( uiPCr + 0, uiPrv + 0, uiPPr + 0 );
			submesh.AddFace( uiCur + 0, uiPrv + 0, uiPCr + 0 );
			uiPrv++;
			uiCur++;
		}

		uiCur = 0;
		uiPCr = uiCur;
		uiPPr = uiPrv;

		for ( uint32_t j = 0; j <= uiIntMax - 1; j++ )
		{
			submesh.AddFace( uiCur + 1, uiPrv + 0, uiPrv + 1 );
			submesh.AddFace( uiCur + 0, uiPrv + 0, uiCur + 1 );
			uiPrv++;
			uiCur++;
		}

		submesh.AddFace( uiPCr + 0, uiPrv + 0, uiPPr + 0 );
		submesh.AddFace( uiCur + 0, uiPrv + 0, uiPCr + 0 );
		uiPrv++;
		uiCur++;
		submesh.ComputeTangentsFromNormals();
	}

	p_mesh.ComputeContainers();
}
