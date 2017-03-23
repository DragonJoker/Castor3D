#include "SphericalVertex.hpp"

namespace Castor
{
	SphericalVertex::SphericalVertex( real p_radius, real p_phi, real p_theta )
		:	m_rRadius( p_radius )
		,	m_rPhi( p_phi )
		,	m_rTheta( p_theta )
	{
	}

	SphericalVertex::SphericalVertex( SphericalVertex const & p_sphericalVertex )
		:	m_rRadius( p_sphericalVertex.m_rRadius )
		,	m_rPhi( p_sphericalVertex.m_rPhi )
		,	m_rTheta( p_sphericalVertex.m_rTheta )
	{
	}

	SphericalVertex::SphericalVertex( SphericalVertex && p_sphericalVertex )
		:	m_rRadius( 0 )
		,	m_rPhi( 0 )
		,	m_rTheta( 0 )
	{
		m_rRadius	= std::move( p_sphericalVertex.m_rRadius );
		m_rPhi		= std::move( p_sphericalVertex.m_rPhi );
		m_rTheta	= std::move( p_sphericalVertex.m_rTheta );
		p_sphericalVertex.m_rRadius	= 0;
		p_sphericalVertex.m_rPhi	= 0;
		p_sphericalVertex.m_rTheta	= 0;
	}

	SphericalVertex & SphericalVertex::operator =( SphericalVertex const & p_sphericalVertex )
	{
		m_rRadius	= p_sphericalVertex.m_rRadius;
		m_rPhi		= p_sphericalVertex.m_rPhi;
		m_rTheta	= p_sphericalVertex.m_rTheta;
		return * this;
	}

	SphericalVertex & SphericalVertex::operator =( SphericalVertex && p_sphericalVertex )
	{
		if ( this != & p_sphericalVertex )
		{
			m_rRadius	= std::move( p_sphericalVertex.m_rRadius );
			m_rPhi		= std::move( p_sphericalVertex.m_rPhi );
			m_rTheta	= std::move( p_sphericalVertex.m_rTheta );
			p_sphericalVertex.m_rRadius	= 0;
			p_sphericalVertex.m_rPhi	= 0;
			p_sphericalVertex.m_rTheta	= 0;
		}

		return * this;
	}

	SphericalVertex::SphericalVertex( Point3r const & p_vertex )
	{
		m_rRadius = real( point::length( p_vertex ) );
		m_rPhi = acos( p_vertex[2] / m_rRadius );
		real l_dTmp = m_rRadius * sin( m_rPhi );

		if ( l_dTmp == 0.0 || p_vertex[0] - l_dTmp < 0.001 )
		{
			m_rTheta = asin( p_vertex[1] / l_dTmp );
		}
		else
		{
			m_rTheta = acos( p_vertex[0] / l_dTmp );
		}
	}

	SphericalVertex::~SphericalVertex()
	{
	}
}
