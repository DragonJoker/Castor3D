#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/SphericalVertex.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
using namespace Castor::Utils;
#endif

using namespace Castor::Math;

SphericalVertex :: SphericalVertex( real p_radius, real p_phi, real p_theta)
	:	m_radius	( p_radius),
		m_phi		( p_phi),
		m_theta		( p_theta)
{
}

SphericalVertex :: SphericalVertex( SphericalVertex const & p_pSphericalVertex3f)
{
	m_radius = p_pSphericalVertex3f.m_radius;
	m_phi    = p_pSphericalVertex3f.m_phi;
	m_theta  = p_pSphericalVertex3f.m_theta;
}

SphericalVertex :: SphericalVertex( Point3r const & p_vertex)
{
	m_radius = real( p_vertex.length());
	m_phi = acos( p_vertex[2] / m_radius);
	real l_dTmp = m_radius * sin( m_phi);

	if (l_dTmp == 0.0 || p_vertex[0] - l_dTmp < 0.001)
	{
		m_theta = asin( p_vertex[1] / l_dTmp);
	}
	else
	{
		m_theta = acos( p_vertex[0] / l_dTmp);
	}
}

SphericalVertex :: ~SphericalVertex()
{
}