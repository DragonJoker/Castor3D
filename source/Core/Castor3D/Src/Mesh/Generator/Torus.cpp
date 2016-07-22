#include "Torus.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"

#include <Math/Angle.hpp>

using namespace Castor3D;
using namespace Castor;

Torus::Torus()
	: MeshGenerator( eMESH_TYPE_TORUS )
	, m_rInternalRadius( 0 )
	, m_rExternalRadius( 0 )
	, m_uiInternalNbFaces( 0 )
	, m_uiExternalNbFaces( 0 )
{
}

Torus::~Torus()
{
}

MeshGeneratorSPtr Torus::Create()
{
	return std::make_shared< Torus >();
}

void Torus::DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions )
{
	m_rInternalRadius = std::abs( p_dimensions[0] );
	m_rExternalRadius = std::abs( p_dimensions[1] );
	m_uiInternalNbFaces = p_faces[0];
	m_uiExternalNbFaces = p_faces[1];
	p_mesh.Cleanup();

	if ( m_uiInternalNbFaces >= 3 && m_uiExternalNbFaces >= 3 )
	{
		Submesh & l_submesh = *( p_mesh.CreateSubmesh() );
		uint32_t l_uiCur = 0;
		uint32_t l_uiPrv = 0;
		uint32_t l_uiPCr = 0;
		uint32_t l_uiPPr = 0;
		real l_rAngleIn = 0.0;
		real l_rAngleEx = 0.0;
		uint32_t l_uiExtMax = m_uiExternalNbFaces;
		uint32_t l_uiIntMax = m_uiInternalNbFaces;
		Point3r l_ptPos;
		Point3r l_ptNml;
		Coords3r l_ptTangent0;
		Coords3r l_ptTangent1;

		// Build the internal circle that will be rotated to build the torus
		real l_step = real( Angle::PiMult2 ) / m_uiInternalNbFaces;

		for ( uint32_t j = 0; j <= l_uiIntMax; j++ )
		{
			BufferElementGroupSPtr l_vertex = l_submesh.AddPoint( m_rInternalRadius * cos( l_rAngleIn ) + m_rExternalRadius, m_rInternalRadius * sin( l_rAngleIn ), 0.0 );
			Vertex::SetTexCoord( l_vertex, real( 0.0 ), real( j ) / m_uiInternalNbFaces );
			Vertex::SetNormal( l_vertex, point::get_normalised( Point3r( real( cos( l_rAngleIn ) ), real( sin( l_rAngleIn ) ), real( 0.0 ) ) ) );
			l_uiCur++;
			l_rAngleIn += l_step;
		}

		// Build the torus
		l_step = real( Angle::PiMult2 ) / m_uiExternalNbFaces;

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
				Vertex::SetTexCoord( l_vertex, real( i ) / m_uiExternalNbFaces, real( j ) / m_uiInternalNbFaces );
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
