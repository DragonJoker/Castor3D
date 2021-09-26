#include "CastorUtils/Math/SphericalVertex.hpp"

namespace castor
{
	SphericalVertex::SphericalVertex( float radius, float phi, float theta )
		: m_radius( radius )
		, m_phi( phi )
		, m_theta( theta )
	{
	}

	SphericalVertex::SphericalVertex( Point3f const & vertex )
	{
		m_radius = float( point::length( vertex ) );
		m_phi = float( acos( vertex[2] / m_radius ) );
		auto tmp = m_radius * sin( m_phi );

		if ( tmp == 0.0 || vertex[0] - tmp < 0.001 )
		{
			m_theta = float( asin( vertex[1] / tmp ) );
		}
		else
		{
			m_theta = float( acos( vertex[0] / tmp ) );
		}
	}

	SphericalVertex::~SphericalVertex()
	{
	}
}
