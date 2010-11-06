#include "PrecompiledHeader.h"

#include "VertexSpherical.h"

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::Math;

VertexSpherical :: VertexSpherical( real p_radius, real p_phi, real p_theta)
	:	m_radius	( p_radius),
		m_phi		( p_phi),
		m_theta		( p_theta)
{
}

VertexSpherical :: VertexSpherical( const VertexSpherical & p_pVertexSpherical3f)
{
	m_radius = p_pVertexSpherical3f.m_radius;
	m_phi    = p_pVertexSpherical3f.m_phi;
	m_theta  = p_pVertexSpherical3f.m_theta;
}

VertexSpherical :: VertexSpherical( const Point3r & p_vertex)
{
	m_radius = p_vertex.GetLength();
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

VertexSpherical :: ~VertexSpherical()
{
}