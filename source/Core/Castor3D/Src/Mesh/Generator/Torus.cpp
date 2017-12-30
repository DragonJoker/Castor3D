﻿#include "Torus.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace castor3d;
using namespace castor;

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

MeshGeneratorSPtr Torus::create()
{
	return std::make_shared< Torus >();
}

void Torus::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	String param;

	if ( p_parameters.get( cuT( "inner_size" ), param ) )
	{
		m_internalRadius = string::toFloat( param );
	}

	if ( p_parameters.get( cuT( "outer_size" ), param ) )
	{
		m_externalRadius = string::toFloat( param );
	}

	if ( p_parameters.get( cuT( "inner_count" ), param ) )
	{
		m_internalNbFaces = string::toUInt( param );
	}

	if ( p_parameters.get( cuT( "outer_count" ), param ) )
	{
		m_externalNbFaces = string::toUInt( param );
	}

	p_mesh.cleanup();

	if ( m_internalNbFaces >= 3 && m_externalNbFaces >= 3 )
	{
		Submesh & submesh = *( p_mesh.createSubmesh() );
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
			BufferElementGroupSPtr vertex = submesh.addPoint( m_internalRadius * cos( rAngleIn ) + m_externalRadius, m_internalRadius * sin( rAngleIn ), 0.0 );
			Vertex::setTexCoord( vertex, real( 0.0 ), real( j ) / m_internalNbFaces );
			Vertex::setNormal( vertex, point::getNormalised( Point3r( real( cos( rAngleIn ) ), real( sin( rAngleIn ) ), real( 0.0 ) ) ) );
			uiCur++;
			rAngleIn += step;
		}

		// Build the torus
		step = real( Angle::PiMult2 ) / m_externalNbFaces;
		auto indexMapping = std::make_shared< TriFaceMapping >( submesh );

		for ( uint32_t i = 1; i <= uiExtMax; i++ )
		{
			uiPCr = uiCur;
			uiPPr = uiPrv;
			rAngleEx += step;

			for ( uint32_t j = 0; j <= uiIntMax; j++ )
			{
				BufferElementGroupSPtr vertex = submesh[j];
				Vertex::getPosition( vertex, ptPos );
				Vertex::getNormal( vertex, ptNml );
				vertex = submesh.addPoint( ptPos[0] * cos( rAngleEx ), ptPos[1], ptPos[0] * sin( rAngleEx ) );
				Vertex::setTexCoord( vertex, real( i ) / m_externalNbFaces, real( j ) / m_internalNbFaces );
				Vertex::setNormal( vertex, point::getNormalised( Point3r( real( ptNml[0] * cos( rAngleEx ) ), real( ptNml[1] ), real( ptNml[0] * sin( rAngleEx ) ) ) ) );
			}

			for ( uint32_t j = 0; j <= uiIntMax - 1; j++ )
			{
				indexMapping->addFace( uiCur + 1, uiPrv + 0, uiPrv + 1 );
				indexMapping->addFace( uiCur + 0, uiPrv + 0, uiCur + 1 );
				uiPrv++;
				uiCur++;
			}

			indexMapping->addFace( uiPCr + 0, uiPrv + 0, uiPPr + 0 );
			indexMapping->addFace( uiCur + 0, uiPrv + 0, uiPCr + 0 );
			uiPrv++;
			uiCur++;
		}

		uiCur = 0;
		uiPCr = uiCur;
		uiPPr = uiPrv;

		for ( uint32_t j = 0; j <= uiIntMax - 1; j++ )
		{
			indexMapping->addFace( uiCur + 1, uiPrv + 0, uiPrv + 1 );
			indexMapping->addFace( uiCur + 0, uiPrv + 0, uiCur + 1 );
			uiPrv++;
			uiCur++;
		}

		indexMapping->addFace( uiPCr + 0, uiPrv + 0, uiPPr + 0 );
		indexMapping->addFace( uiCur + 0, uiPrv + 0, uiPCr + 0 );

		indexMapping->computeTangentsFromNormals();
		submesh.setIndexMapping( indexMapping );
	}

	p_mesh.computeContainers();
}
