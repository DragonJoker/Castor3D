#include "VertexSpherical.h"

using namespace General::Math;

VertexSpherical :: VertexSpherical( float p_radius, float p_phi, float p_theta)
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

VertexSpherical :: ~VertexSpherical()
{
}