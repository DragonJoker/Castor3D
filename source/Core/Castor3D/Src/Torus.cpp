#include "Torus.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

#include <Angle.hpp>

using namespace Castor3D;
using namespace Castor;

Torus::Torus()
	:	MeshCategory( eMESH_TYPE_TORUS )
	,	m_rInternalRadius( 0 )
	,	m_rExternalRadius( 0 )
	,	m_uiInternalNbFaces( 0 )
	,	m_uiExternalNbFaces( 0 )
{
}

Torus::~Torus()
{
}

MeshCategorySPtr Torus::Create()
{
	return std::make_shared< Torus >();
}

void Torus::Generate()
{
	m_rInternalRadius = std::abs( m_rInternalRadius );
	m_rExternalRadius = std::abs( m_rExternalRadius );

	if ( m_uiInternalNbFaces >= 3 && m_uiExternalNbFaces >= 3 )
	{
		Submesh & l_submesh = *( GetMesh()->CreateSubmesh() );
		BufferElementGroupSPtr l_pVertex;
		uint32_t l_uiCur = 0;
		uint32_t l_uiPrv = 0;
		uint32_t l_uiPCr = 0;
		uint32_t l_uiPPr = 0;
		real l_rStepAngleIn = real( Angle::PiMult2 ) / m_uiInternalNbFaces;
		real l_rStepAngleEx = real( Angle::PiMult2 ) / m_uiExternalNbFaces;
		real l_rAngleIn = 0.0;
		real l_rAngleEx = 0.0;
		uint32_t l_uiExtMax = m_uiExternalNbFaces;
		uint32_t l_uiIntMax = m_uiInternalNbFaces;
		Point3r l_ptPos;
		Point3r l_ptNml;
		Coords3r l_ptTangent0;
		Coords3r l_ptTangent1;

		for ( uint32_t j = 0; j < l_uiIntMax; j++ )
		{
			l_pVertex = l_submesh.AddPoint( m_rInternalRadius * cos( l_rAngleIn ) + m_rExternalRadius, m_rInternalRadius * sin( l_rAngleIn ), 0.0 );
			Vertex::SetTexCoord(	l_pVertex, real( 0.0 ), real( j ) / m_uiInternalNbFaces );
			Vertex::SetNormal(	l_pVertex, point::get_normalised( Point3r( real( cos( l_rAngleIn ) ), real( sin( l_rAngleIn ) ), real( 0.0 ) ) ) );
			l_uiCur++;
			l_rAngleIn += l_rStepAngleIn;
		}

		for ( uint32_t i = 1; i < l_uiExtMax; i++ )
		{
			l_uiPCr = l_uiCur;
			l_uiPPr = l_uiPrv;
			l_rAngleEx += l_rStepAngleEx;

			for ( uint32_t j = 0; j < l_uiIntMax; j++ )
			{
				l_pVertex = l_submesh[j];
				Vertex::GetPosition( l_pVertex, l_ptPos );
				Vertex::GetNormal( l_pVertex, l_ptNml );
				l_pVertex = l_submesh.AddPoint( l_ptPos[0] * cos( l_rAngleEx ), l_ptPos[1], l_ptPos[0] * sin( l_rAngleEx ) );
				Vertex::SetTexCoord(	l_pVertex, real( i ) / m_uiExternalNbFaces, real( j ) / m_uiInternalNbFaces );
				Vertex::SetNormal(	l_pVertex, point::get_normalised( Point3r( real( l_ptNml[0] * cos( l_rAngleEx ) ), real( l_ptNml[1] ), real( l_ptNml[0] * sin( l_rAngleEx ) ) ) ) );
			}

			for ( uint32_t j = 0; j < l_uiIntMax - 1; j++ )
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

		for ( uint32_t j = 0; j < l_uiIntMax - 1; j++ )
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
		/*
				for( uint32_t i = 0; i < m_uiInternalNbFaces; i++ )
				{
					l_submesh[i]->GetTangent( l_ptTangent0 );
					l_ptTangent0	+= l_submesh[l_uiPrv + i]->GetTangent( l_ptTangent1);
					point::normalise( l_ptTangent0 );
					l_submesh[l_uiPrv + i]->SetTangent(	l_ptTangent0	);
				}


				for( uint32_t i = 0; i <= l_uiMax; i++ )
				{
					l_submesh[(m_uiInternalNbFaces + 1) * i]->GetTangent( l_ptTangent0 );
					l_ptTangent0	+= l_submesh[(m_uiInternalNbFaces + 1) * (i + 1) - 1]->GetTangent( l_ptTangent1 );
					point::normalise( l_ptTangent0	);
					l_submesh[(m_uiInternalNbFaces + 1) * (i + 1) - 1]->SetTangent(	l_ptTangent0	);
				}
		*/
	}

	GetMesh()->ComputeContainers();
}

void Torus::Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions )
{
	m_rInternalRadius = p_arrayDimensions[0];
	m_rExternalRadius = p_arrayDimensions[1];
	m_uiInternalNbFaces = p_arrayFaces[0];
	m_uiExternalNbFaces = p_arrayFaces[1];
	GetMesh()->Cleanup();
	Generate();
}
