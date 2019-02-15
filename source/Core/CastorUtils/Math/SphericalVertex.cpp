#include "CastorUtils/Math/SphericalVertex.hpp"

namespace castor
{
	SphericalVertex::SphericalVertex( real p_radius, real p_phi, real p_theta )
		: m_radius( p_radius )
		, m_phi( p_phi )
		, m_theta( p_theta )
	{
	}

	SphericalVertex::SphericalVertex( Point3r const & p_vertex )
	{
		m_radius = real( point::length( p_vertex ) );
		m_phi = acos( p_vertex[2] / m_radius );
		real tmp = m_radius * sin( m_phi );

		if ( tmp == 0.0 || p_vertex[0] - tmp < 0.001 )
		{
			m_theta = asin( p_vertex[1] / tmp );
		}
		else
		{
			m_theta = acos( p_vertex[0] / tmp );
		}
	}

	SphericalVertex::~SphericalVertex()
	{
	}
}
