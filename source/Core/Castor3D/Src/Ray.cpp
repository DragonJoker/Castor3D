#include "Ray.hpp"
#include "Viewport.hpp"
#include "Camera.hpp"
#include "SceneNode.hpp"
#include "Face.hpp"
#include "Geometry.hpp"
#include "Mesh.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

using namespace Castor3D;
using namespace Castor;

Ray::Ray( Point2i const & p_point, Camera const & p_camera )
{
	Viewport const & l_viewport = p_camera.GetViewport();
	m_ptOrigin = p_camera.GetParent()->GetPosition();
	m_ptOrigin[2] += l_viewport.GetNear();
	Quaternion l_camOrient = p_camera.GetParent()->GetOrientation();
	m_ptOrigin *= l_camOrient;
	//l_viewport.GetDirection( p_point, m_ptDirection );
	point::normalise( m_ptDirection );
}

Ray::Ray( int p_x, int p_y, Camera const & p_camera )
{
	Viewport const & l_viewport = p_camera.GetViewport();
	m_ptOrigin = p_camera.GetParent()->GetPosition();
	m_ptOrigin[2] += l_viewport.GetNear();
	Quaternion l_camOrient = p_camera.GetParent()->GetOrientation();
	m_ptOrigin *= l_camOrient;
	Point2i l_point( p_x, p_y );
	//l_viewport.GetDirection( l_point, m_ptDirection );
	point::normalise( m_ptDirection );
}

Ray::Ray( Point3r const & p_ptOrigin, Point3r const & p_ptDirection )
{
	m_ptOrigin = p_ptOrigin;
	m_ptDirection = p_ptDirection;
}

Ray::Ray( Ray const & p_ray )
	: m_ptOrigin( p_ray.m_ptOrigin )
	, m_ptDirection( p_ray.m_ptDirection )
{
	point::normalise( m_ptDirection );
}

Ray::Ray( Ray && p_ray )
	: m_ptOrigin( std::move( p_ray.m_ptOrigin ) )
	, m_ptDirection( std::move( p_ray.m_ptDirection ) )
{
	p_ray.m_ptOrigin	= Point3r();
	p_ray.m_ptDirection	= Point3r();
}

Ray::~Ray()
{
}

real Ray::Intersects( Point3r const & p_pt1, Point3r const & p_pt2, Point3r const & p_pt3 )
{
	real l_fReturn = -1.0f;
	Point3r e1( p_pt2 - p_pt1 );
	Point3r e2( p_pt1 - p_pt3 );
	Point3r e3( p_pt3 - p_pt2 );
	Point3r h( m_ptDirection ^ e2 );
	real a = point::dot( e1, h );

	if ( ! policy::is_null( a ) )
	{
		real f = 1.0f / a;
		Point3r s( m_ptOrigin - p_pt1 );
		real u = f * point::dot( s, h );

		if ( u >= 0.0 && u <= 1.0 )
		{
			Point3r q( s ^ e1 );
			real v = f * point::dot( m_ptDirection, q );

			if ( v >= 0.0 && u + v <= 1.0 )
			{
				real t = f * point::dot( e2, q );

				if ( ! policy::is_null( t ) )
				{
					l_fReturn = t;
				}
			}
		}
	}

	return l_fReturn;
}

real Ray::Intersects( Face const & p_face, Submesh const & p_submesh )
{
	Point3r l_pt1, l_pt2, l_pt3;
	return Intersects( Vertex::GetPosition( p_submesh[p_face.GetVertexIndex( 0 )], l_pt1 ), Vertex::GetPosition( p_submesh[p_face.GetVertexIndex( 1 )], l_pt2 ), Vertex::GetPosition( p_submesh[p_face.GetVertexIndex( 2 )], l_pt3 ) );
}

real Ray::Intersects( Point3r const & p_vertex )
{
	Point3r u( m_ptOrigin - p_vertex );
	Point3r puv;

	if ( ProjectVertex( u, puv ) && point::distance_squared( puv ) < 0.000001 )
	{
		return real( point::distance( puv ) );
	}

	return -1;
}

real Ray::Intersects( CubeBox const & p_box )
{
	Point3r l_v1( p_box.GetMin().const_ptr() );
	Point3r l_v8( p_box.GetMax().const_ptr() );
	Point3r l_v2( l_v8[0], l_v1[1], l_v1[2] );
	Point3r l_v3( l_v1[0], l_v8[1], l_v1[2] );
	Point3r l_v4( l_v8[0], l_v8[1], l_v1[2] );
	Point3r l_v5( l_v8[0], l_v1[1], l_v8[2] );
	Point3r l_v6( l_v1[0], l_v8[1], l_v8[2] );
	Point3r l_v7( l_v8[0], l_v8[1], l_v8[2] );
	bool l_foundOne = false;
	real l_dist, l_min = 1e6f;

	if ( ( l_dist = Intersects( l_v1, l_v2, l_v3 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = l_dist;
	}

	if ( ( l_dist = Intersects( l_v2, l_v4, l_v3 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v2, l_v6, l_v4 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v6, l_v8, l_v4 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v6, l_v5, l_v8 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v5, l_v7, l_v8 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v1, l_v7, l_v5 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v1, l_v3, l_v7 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v2, l_v1, l_v5 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v2, l_v5, l_v6 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v3, l_v4, l_v7 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( ( l_dist = Intersects( l_v4, l_v7, l_v8 ) ) >= 0 )
	{
		l_foundOne = true;
		l_min = std::min<real>( l_min, l_dist );
	}

	if ( l_foundOne )
	{
		return l_min;
	}

	return -1.0f;
}

real Ray::Intersects( SphereBox const & p_sphere )
{
	real l_fReturn = -1.0f;
	// intersection rayon/sphere
	Point3r l_vDist( p_sphere.GetCenter() - m_ptOrigin );
	real B = point::dot( m_ptDirection, l_vDist );
	real D = B * B - point::dot( l_vDist, l_vDist ) + p_sphere.GetRadius() * p_sphere.GetRadius();

	if ( D >= 0.0f )
	{
		real t0 = B - std::sqrt( D );
		real t1 = B + std::sqrt( D );

		if ( t0 > 0.1f )
		{
			l_fReturn = t0;
		}

		if ( t1 > 0.1f && t1 < l_fReturn )
		{
			l_fReturn = t1;
		}
	}

	return l_fReturn;
}

real Ray::Intersects( GeometrySPtr p_pGeometry, FaceSPtr * CU_PARAM_UNUSED( p_nearestFace ), SubmeshSPtr * p_nearestSubmesh )
{
	Point3r l_vCenter( p_pGeometry->GetParent()->GetPosition() );
	MeshSPtr l_mesh = p_pGeometry->GetMesh();
	SphereBox l_sphere( l_vCenter, l_mesh->GetCollisionSphere().GetRadius() );
	real l_distance = Intersects( l_sphere );
	//real l_faceDist = 10e6, l_vertexDist = 10e6;
	//real l_curfaceDist, l_curvertexDist;

	if ( l_distance >= 0.0f )
	{
		l_distance = -1.0f;

		for ( auto && l_submesh : *l_mesh )
		{
			Point3r l_submeshCenter = l_vCenter + l_submesh->GetSphere().GetCenter();
			l_sphere.Load( l_submeshCenter, l_submesh->GetSphere().GetRadius() );

			if ( p_nearestSubmesh )
			{
				*p_nearestSubmesh = l_submesh;
			}

			//if (Intersects( l_sphere) >= 0.0f)
			//{
			//	for (uint32_t k = 0; k < l_submesh->GetFaceCount(); k++)
			//	{
			//		FaceSPtr l_pFace = l_submesh->GetFace( k );

			//		if ((l_curfaceDist = Intersects( * l_pFace)) >= 0.0 && l_curfaceDist < l_faceDist)
			//		{
			//			if (p_nearestFace)
			//			{
			//				*p_nearestFace = l_pFace;
			//			}

			//			if (p_nearestSubmesh)
			//			{
			//				*p_nearestSubmesh = l_submesh;
			//			}

			//			l_faceDist = l_curfaceDist;
			//			l_distance = l_curfaceDist;

			//			if ((l_curvertexDist = Intersects( * l_pFace->m_vertex1)) >= 0.0 && l_curvertexDist < l_distance)
			//			{
			//				l_distance = l_curvertexDist;
			//			}

			//			if ((l_curvertexDist = Intersects( * l_pFace->m_vertex2)) >= 0.0 && l_curvertexDist < l_distance)
			//			{
			//				l_distance = l_curvertexDist;
			//			}

			//			if ((l_curvertexDist = Intersects( * l_pFace->m_vertex2)) >= 0.0 && l_curvertexDist < l_distance)
			//			{
			//				l_distance = l_curvertexDist;
			//			}
			//		}
			//	}
			//}
		}
	}

	return l_distance;
}

bool Ray::ProjectVertex( Point3r const & p_point, Point3r & p_result )
{
	bool l_return = false;

	if ( point::dot( m_ptDirection, p_point ) >= 0.0 )
	{
		p_result = m_ptDirection * real( point::dot( m_ptDirection, p_point ) / point::distance( m_ptDirection ) );
		l_return = true;
	}

	return l_return;
}
