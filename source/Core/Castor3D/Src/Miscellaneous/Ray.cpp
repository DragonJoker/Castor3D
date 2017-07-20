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

using namespace Castor;

namespace Castor3D
{
	Ray::Ray( Point2i const & p_point, Camera const & p_camera )
	{
		auto const & projection = p_camera.GetViewport().GetProjection();
		auto const & view = p_camera.GetView();
		auto invProjectionView = ( projection * view ).get_inverse();
		Point4r screen
		{
			( 2.0_r * real( p_point[0] ) / p_camera.GetWidth() ) - 1.0_r,
			1.0_r - ( 2.0_r * real( p_point[1] ) / p_camera.GetHeight() ),
			-1.0_r,
			1.0_r
		};
		Point4r world = invProjectionView * screen;
		world /= world[3];
		m_origin[0] = world[0];
		m_origin[1] = world[1];
		m_origin[2] = world[2];

		screen[2] = 1.0_r;
		world = invProjectionView * screen;
		world /= world[3];
		m_direction[0] = world[0];
		m_direction[1] = world[1];
		m_direction[2] = world[2];
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
		Intersection result = Intersection::eOut;
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
						result = Intersection::eIn;
					}
				}
			}
		}

		return result;
	}

	Intersection Ray::Intersects( Face const & p_face, Castor::Matrix4x4r const & p_transform, Submesh const & p_submesh, real & p_distance )const
	{
		Point3r pt1, pt2, pt3;
		auto stride = p_submesh.GetVertexBuffer().GetDeclaration().stride();
		return Intersects( p_transform * Vertex::GetPosition( &p_submesh.GetVertexBuffer().GetData()[p_face[0] * stride], pt1 )
						   , p_transform * Vertex::GetPosition( &p_submesh.GetVertexBuffer().GetData()[p_face[1] * stride], pt2 )
						   , p_transform * Vertex::GetPosition( &p_submesh.GetVertexBuffer().GetData()[p_face[2] * stride], pt3 )
						   , p_distance );
	}

	Intersection Ray::Intersects( Point3r const & p_vertex, real & p_distance )const
	{
		Intersection result = Intersection::eOut;
		Point3r u( m_origin - p_vertex );
		Point3r puv;

		if ( ProjectVertex( u, puv ) && point::length_squared( puv ) < 0.000001 )
		{
			p_distance = real( point::length( u ) );
			result = Intersection::eIn;
		}

		return result;
	}

	Intersection Ray::Intersects( CubeBox const & p_box, real & p_distance )const
	{
		Point3r v1( p_box.GetMin().const_ptr() );
		Point3r v8( p_box.GetMax().const_ptr() );
		Point3r v2( v8[0], v1[1], v1[2] );
		Point3r v3( v1[0], v8[1], v1[2] );
		Point3r v4( v8[0], v8[1], v1[2] );
		Point3r v5( v8[0], v1[1], v8[2] );
		Point3r v6( v1[0], v8[1], v8[2] );
		Point3r v7( v8[0], v8[1], v8[2] );
		auto result = Intersection::eOut;
		real dist = 0.0_r;
		real min = std::numeric_limits< real >::max();

		if ( ( Intersects( v1, v2, v3, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v2, v4, v3, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v2, v6, v4, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v6, v8, v4, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v6, v5, v8, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v5, v7, v8, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v1, v7, v5, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v1, v3, v7, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v2, v1, v5, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v2, v5, v6, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v3, v4, v7, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( Intersects( v4, v7, v8, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( result != Intersection::eOut )
		{
			p_distance = min;
		}

		return result;
	}

	Intersection Ray::Intersects( SphereBox const & p_sphere, real & p_distance )const
	{
		// see http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
		auto result = Intersection::eOut;
		Point3r v( p_sphere.GetCenter() - m_origin );
		Point3r puv;

		if ( ProjectVertex( v, puv ) )
		{
			// Sphere's center projects on the ray.
			p_distance = real( point::length( puv - v ) );

			if ( p_distance == p_sphere.GetRadius() )
			{
				// Single intersection point.
				result = Intersection::eIntersect;
			}
			else if ( p_distance < p_sphere.GetRadius() )
			{
				// Two intersection points, we look for the nearest one.
				result = Intersection::eIn;

				if ( point::length( v ) < p_sphere.GetRadius() )
				{
					// The ray origin is inside the sphere.
					p_distance = real( point::length( puv ) + sqrt( p_sphere.GetRadius() * p_sphere.GetRadius() - p_distance * p_distance ) );
				}
				else
				{
					// The ray origin is outside the sphere
					p_distance = real( point::length( puv ) - sqrt( p_sphere.GetRadius() * p_sphere.GetRadius() - p_distance * p_distance ) );
				}
			}
		}
		else
		{
			p_distance = real( point::length( v ) );

			if ( p_distance == p_sphere.GetRadius() )
			{
				// Single intersection point.
				result = Intersection::eIntersect;
			}
			else if ( point::length( v ) < p_sphere.GetRadius() )
			{
				// The sphere's center is behind the ray, and the rays origin is inside the sphere.
				p_distance = real( point::length( puv - v ) );
				p_distance = real( sqrt( p_sphere.GetRadius() * p_sphere.GetRadius() - p_distance * p_distance ) - point::length( puv ) );
				result = Intersection::eIn;
			}
			else
			{
				// No intersection.
			}
		}

		return result;
	}

	Intersection Ray::Intersects( GeometrySPtr p_geometry, Face & p_nearestFace, SubmeshSPtr & p_nearestSubmesh, real & p_distance )const
	{
		MeshSPtr mesh = p_geometry->GetMesh();
		Point3r center{ p_geometry->GetParent()->GetDerivedPosition() };
		SphereBox sphere{ center, mesh->GetCollisionSphere().GetRadius() };
		Matrix4x4r const & transform{ p_geometry->GetParent()->GetDerivedTransformationMatrix() };
		auto result = Intersection::eOut;
		real faceDist = std::numeric_limits< real >::max();

		if ( Intersects( sphere, p_distance ) != Intersection::eOut )
		{
			for ( auto submesh : *mesh )
			{
				sphere.Load( center, submesh->GetCollisionSphere().GetRadius() );

				if ( Intersects( sphere, p_distance ) != Intersection::eOut )
				{
					for ( uint32_t k = 0u; k < submesh->GetFaceCount(); k++ )
					{
						Face face
						{
							submesh->GetIndexBuffer().GetData()[k * 3 + 0],
							submesh->GetIndexBuffer().GetData()[k * 3 + 1],
							submesh->GetIndexBuffer().GetData()[k * 3 + 2],
						};
						real curfaceDist = 0.0_r;

						if ( Intersects( face, transform, *submesh, curfaceDist ) != Intersection::eOut && curfaceDist < faceDist )
						{
							result = Intersection::eIn;
							p_nearestFace = face;
							p_nearestSubmesh = submesh;
							p_distance = curfaceDist;
							faceDist = curfaceDist;
						}
					}
				}
			}
		}

		return result;
	}

	bool Ray::ProjectVertex( Point3r const & p_point, Point3r & p_result )const
	{
		bool result = false;
		p_result = ( m_direction * real( point::dot( m_direction, p_point ) ) );

		if ( point::dot( m_direction, p_point ) >= 0.0 )
		{
			result = true;
		}

		return result;
	}
}
