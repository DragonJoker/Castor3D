#include "Ray.hpp"

#include "Mesh/Face.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/IndexBuffer.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/SceneNode.hpp"

using namespace Castor;

namespace Castor3D
{
	Ray::Ray( Point2i const & p_point, Camera const & p_camera )
	{
		auto const & l_projection = p_camera.GetViewport().GetProjection();
		auto const & l_view = p_camera.GetView();
		auto l_invProjectionView = ( l_projection * l_view ).get_inverse();
		Point4r l_screen
		{
			( 2.0_r * real( p_point[0] ) / p_camera.GetWidth() ) - 1.0_r,
			1.0_r - ( 2.0_r * real( p_point[1] ) / p_camera.GetHeight() ),
			-1.0_r,
			1.0_r
		};
		Point4r l_world = l_invProjectionView * l_screen;
		l_world /= l_world[3];
		m_origin[0] = l_world[0];
		m_origin[1] = l_world[1];
		m_origin[2] = l_world[2];

		l_screen[2] = 1.0_r;
		l_world = l_invProjectionView * l_screen;
		l_world /= l_world[3];
		m_direction[0] = l_world[0];
		m_direction[1] = l_world[1];
		m_direction[2] = l_world[2];
		m_direction -= m_origin;
		point::normalise( m_direction );
	}

	Ray::Ray( int p_x, int p_y, Camera const & p_camera )
		: Ray{ Point2i{ p_x, p_y }, p_camera }
	{
	}

	Ray::Ray( Point3r const & p_origin, Point3r const & p_direction )
		: m_origin{ p_origin }
		, m_direction{ p_direction }
	{
		point::normalise( m_direction );
	}

	Intersection Ray::Intersects( Point3r const & p_pt1, Point3r const & p_pt2, Point3r const & p_pt3, real & p_distance )const
	{
		// see http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
		Intersection l_return = Intersection::eOut;
		Point3r e1{ p_pt2 - p_pt1 };
		Point3r e2{ p_pt1 - p_pt3 };
		Point3r h( m_direction ^ e2 );
		real a = point::dot( e1, h );

		if ( std::abs( a ) > 0.00001_r )
		{
			real f = 1.0f / a;
			Point3r s( m_origin - p_pt1 );
			real u = f * point::dot( s, h );

			if ( u >= 0.0 && u <= 1.0 )
			{
				Point3r q( s ^ e1 );
				real v = f * point::dot( m_direction, q );

				if ( v >= 0.0 && u + v <= 1.0 )
				{
					p_distance = f * point::dot( e2, q );

					if ( p_distance > 0.00001_r )
					{
						l_return = Intersection::eIn;
					}
				}
			}
		}

		return l_return;
	}

	Intersection Ray::Intersects( Face const & p_face, Castor::Matrix4x4r const & p_transform, Submesh const & p_submesh, real & p_distance )const
	{
		Point3r l_pt1, l_pt2, l_pt3;
		auto l_stride = p_submesh.GetVertexBuffer().GetDeclaration().stride();
		return Intersects( p_transform * Vertex::GetPosition( &p_submesh.GetVertexBuffer().data()[p_face[0] * l_stride], l_pt1 )
						   , p_transform * Vertex::GetPosition( &p_submesh.GetVertexBuffer().data()[p_face[1] * l_stride], l_pt2 )
						   , p_transform * Vertex::GetPosition( &p_submesh.GetVertexBuffer().data()[p_face[2] * l_stride], l_pt3 )
						   , p_distance );
	}

	Intersection Ray::Intersects( Point3r const & p_vertex, real & p_distance )const
	{
		Intersection l_return = Intersection::eOut;
		Point3r u( m_origin - p_vertex );
		Point3r puv;

		if ( ProjectVertex( u, puv ) && point::length_squared( puv ) < 0.000001 )
		{
			p_distance = real( point::length( u ) );
			l_return = Intersection::eIn;
		}

		return l_return;
	}

	Intersection Ray::Intersects( CubeBox const & p_box, real & p_distance )const
	{
		Point3r l_v1( p_box.GetMin().const_ptr() );
		Point3r l_v8( p_box.GetMax().const_ptr() );
		Point3r l_v2( l_v8[0], l_v1[1], l_v1[2] );
		Point3r l_v3( l_v1[0], l_v8[1], l_v1[2] );
		Point3r l_v4( l_v8[0], l_v8[1], l_v1[2] );
		Point3r l_v5( l_v8[0], l_v1[1], l_v8[2] );
		Point3r l_v6( l_v1[0], l_v8[1], l_v8[2] );
		Point3r l_v7( l_v8[0], l_v8[1], l_v8[2] );
		auto l_return = Intersection::eOut;
		real l_dist = 0.0_r;
		real l_min = std::numeric_limits< real >::max();

		if ( ( Intersects( l_v1, l_v2, l_v3, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v2, l_v4, l_v3, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v2, l_v6, l_v4, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v6, l_v8, l_v4, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v6, l_v5, l_v8, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v5, l_v7, l_v8, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v1, l_v7, l_v5, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v1, l_v3, l_v7, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v2, l_v1, l_v5, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v2, l_v5, l_v6, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v3, l_v4, l_v7, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( ( Intersects( l_v4, l_v7, l_v8, l_dist ) ) != Intersection::eOut )
		{
			l_return = Intersection::eIn;
			l_min = std::min( l_min, l_dist );
		}

		if ( l_return != Intersection::eOut )
		{
			p_distance = l_min;
		}

		return l_return;
	}

	Intersection Ray::Intersects( SphereBox const & p_sphere, real & p_distance )const
	{
		// see http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
		auto l_return = Intersection::eOut;
		Point3r l_v( p_sphere.GetCenter() - m_origin );
		Point3r l_puv;

		if ( ProjectVertex( l_v, l_puv ) )
		{
			// Sphere's center projects on the ray.
			p_distance = real( point::length( l_puv - l_v ) );

			if ( p_distance == p_sphere.GetRadius() )
			{
				// Single intersection point.
				l_return = Intersection::eIntersect;
			}
			else if ( p_distance < p_sphere.GetRadius() )
			{
				// Two intersection points, we look for the nearest one.
				l_return = Intersection::eIn;

				if ( point::length( l_v ) < p_sphere.GetRadius() )
				{
					// The ray origin is inside the sphere.
					p_distance = real( point::length( l_puv ) + sqrt( p_sphere.GetRadius() * p_sphere.GetRadius() - p_distance * p_distance ) );
				}
				else
				{
					// The ray origin is outside the sphere
					p_distance = real( point::length( l_puv ) - sqrt( p_sphere.GetRadius() * p_sphere.GetRadius() - p_distance * p_distance ) );
				}
			}
		}
		else
		{
			p_distance = real( point::length( l_v ) );

			if ( p_distance == p_sphere.GetRadius() )
			{
				// Single intersection point.
				l_return = Intersection::eIntersect;
			}
			else if ( point::length( l_v ) < p_sphere.GetRadius() )
			{
				// The sphere's center is behind the ray, and the rays origin is inside the sphere.
				p_distance = real( point::length( l_puv - l_v ) );
				p_distance = real( sqrt( p_sphere.GetRadius() * p_sphere.GetRadius() - p_distance * p_distance ) - point::length( l_puv ) );
				l_return = Intersection::eIn;
			}
			else
			{
				// No intersection.
			}
		}

		return l_return;
	}

	Intersection Ray::Intersects( GeometrySPtr p_geometry, Face & p_nearestFace, SubmeshSPtr & p_nearestSubmesh, real & p_distance )const
	{
		MeshSPtr l_mesh = p_geometry->GetMesh();
		Point3r l_center{ p_geometry->GetParent()->GetDerivedPosition() };
		SphereBox l_sphere{ l_center, l_mesh->GetCollisionSphere().GetRadius() };
		Matrix4x4r const & l_transform{ p_geometry->GetParent()->GetDerivedTransformationMatrix() };
		auto l_return = Intersection::eOut;
		real l_faceDist = std::numeric_limits< real >::max();

		if ( Intersects( l_sphere, p_distance ) != Intersection::eOut )
		{
			for ( auto l_submesh : *l_mesh )
			{
				l_sphere.Load( l_center, l_submesh->GetCollisionSphere().GetRadius() );

				if ( Intersects( l_sphere, p_distance ) != Intersection::eOut )
				{
					for ( uint32_t k = 0u; k < l_submesh->GetFaceCount(); k++ )
					{
						Face l_face
						{
							l_submesh->GetIndexBuffer().data()[k * 3 + 0],
							l_submesh->GetIndexBuffer().data()[k * 3 + 1],
							l_submesh->GetIndexBuffer().data()[k * 3 + 2],
						};
						real l_curfaceDist = 0.0_r;

						if ( Intersects( l_face, l_transform, *l_submesh, l_curfaceDist ) != Intersection::eOut && l_curfaceDist < l_faceDist )
						{
							l_return = Intersection::eIn;
							p_nearestFace = l_face;
							p_nearestSubmesh = l_submesh;
							p_distance = l_curfaceDist;
							l_faceDist = l_curfaceDist;
						}
					}
				}
			}
		}

		return l_return;
	}

	bool Ray::ProjectVertex( Point3r const & p_point, Point3r & p_result )const
	{
		bool l_return = false;
		p_result = ( m_direction * real( point::dot( m_direction, p_point ) ) );

		if ( point::dot( m_direction, p_point ) >= 0.0 )
		{
			l_return = true;
		}

		return l_return;
	}
}
