#include "Ray.hpp"

#include "Mesh/SubmeshComponent/Face.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"

using namespace castor;

namespace castor3d
{
	Ray::Ray( Point2i const & point, Camera const & camera )
	{
		auto const & projection = camera.getViewport().getProjection();
		auto const & view = camera.getView();
		auto invProjectionView = ( projection * view ).getInverse();
		Point4r screen
		{
			( 2.0_r * real( point[0] ) / camera.getWidth() ) - 1.0_r,
			1.0_r - ( 2.0_r * real( point[1] ) / camera.getHeight() ),
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

	Ray::Ray( int x, int y, Camera const & camera )
		: Ray{ Point2i{ x, y }, camera }
	{
	}

	Ray::Ray( Point3r const & origin, Point3r const & direction )
		: m_origin{ origin }
		, m_direction{ direction }
	{
		point::normalise( m_direction );
	}

	Intersection Ray::intersects( Point3r const & pt1, Point3r const & pt2, Point3r const & pt3, real & distance )const
	{
		// see http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
		Intersection result = Intersection::eOut;
		Point3r e1{ pt2 - pt1 };
		Point3r e2{ pt1 - pt3 };
		Point3r h{ point::cross( m_direction, e2 ) };
		real a = point::dot( e1, h );

		if ( std::abs( a ) > 0.00001_r )
		{
			real f = 1.0f / a;
			Point3r s( m_origin - pt1 );
			real u = f * point::dot( s, h );

			if ( u >= 0.0 && u <= 1.0 )
			{
				Point3r q{ point::cross( s, e1 ) };
				real v = f * point::dot( m_direction, q );

				if ( v >= 0.0 && u + v <= 1.0 )
				{
					distance = f * point::dot( e2, q );

					if ( distance > 0.00001_r )
					{
						result = Intersection::eIn;
					}
				}
			}
		}

		return result;
	}

	Intersection Ray::intersects( Face const & face, castor::Matrix4x4r const & transform, Submesh const & submesh, real & distance )const
	{
		//Point3r pt1, pt2, pt3;
		//auto stride = submesh.getVertexBuffer().getDeclaration().stride();
		//return intersects( transform * Vertex::getPosition( &submesh.getVertexBuffer().getData()[face[0] * stride], pt1 )
		//	, transform * Vertex::getPosition( &submesh.getVertexBuffer().getData()[face[1] * stride], pt2 )
		//	, transform * Vertex::getPosition( &submesh.getVertexBuffer().getData()[face[2] * stride], pt3 )
		//	, distance );
		return Intersection::eOut;
	}

	Intersection Ray::intersects( Point3r const & vertex, real & distance )const
	{
		Intersection result = Intersection::eOut;
		Point3r u( m_origin - vertex );
		Point3r puv;

		if ( projectVertex( u, puv ) && point::lengthSquared( puv ) < 0.000001 )
		{
			distance = real( point::length( u ) );
			result = Intersection::eIn;
		}

		return result;
	}

	Intersection Ray::intersects( BoundingBox const & box, real & distance )const
	{
		Point3r v1( box.getMin().constPtr() );
		Point3r v8( box.getMax().constPtr() );
		Point3r v2( v8[0], v1[1], v1[2] );
		Point3r v3( v1[0], v8[1], v1[2] );
		Point3r v4( v8[0], v8[1], v1[2] );
		Point3r v5( v8[0], v1[1], v8[2] );
		Point3r v6( v1[0], v8[1], v8[2] );
		Point3r v7( v8[0], v8[1], v8[2] );
		auto result = Intersection::eOut;
		real dist = 0.0_r;
		real min = std::numeric_limits< real >::max();

		if ( ( intersects( v1, v2, v3, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v2, v4, v3, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v2, v6, v4, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v6, v8, v4, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v6, v5, v8, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v5, v7, v8, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v1, v7, v5, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v1, v3, v7, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v2, v1, v5, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v2, v5, v6, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v3, v4, v7, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( ( intersects( v4, v7, v8, dist ) ) != Intersection::eOut )
		{
			result = Intersection::eIn;
			min = std::min( min, dist );
		}

		if ( result != Intersection::eOut )
		{
			distance = min;
		}

		return result;
	}

	Intersection Ray::intersects( BoundingSphere const & sphere, real & distance )const
	{
		// see http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
		auto result = Intersection::eOut;
		Point3r v( sphere.getCenter() - m_origin );
		Point3r puv;

		if ( projectVertex( v, puv ) )
		{
			// Sphere's center projects on the ray.
			distance = real( point::length( puv - v ) );

			if ( distance == sphere.getRadius() )
			{
				// Single intersection point.
				result = Intersection::eIntersect;
			}
			else if ( distance < sphere.getRadius() )
			{
				// Two intersection points, we look for the nearest one.
				result = Intersection::eIn;

				if ( point::length( v ) < sphere.getRadius() )
				{
					// The ray origin is inside the sphere.
					distance = real( point::length( puv ) + sqrt( sphere.getRadius() * sphere.getRadius() - distance * distance ) );
				}
				else
				{
					// The ray origin is outside the sphere
					distance = real( point::length( puv ) - sqrt( sphere.getRadius() * sphere.getRadius() - distance * distance ) );
				}
			}
		}
		else
		{
			distance = real( point::length( v ) );

			if ( distance == sphere.getRadius() )
			{
				// Single intersection point.
				result = Intersection::eIntersect;
			}
			else if ( point::length( v ) < sphere.getRadius() )
			{
				// The sphere's center is behind the ray, and the rays origin is inside the sphere.
				distance = real( point::length( puv - v ) );
				distance = real( sqrt( sphere.getRadius() * sphere.getRadius() - distance * distance ) - point::length( puv ) );
				result = Intersection::eIn;
			}
			else
			{
				// No intersection.
			}
		}

		return result;
	}

	Intersection Ray::intersects( GeometrySPtr geometry, Face & nearestFace, SubmeshSPtr & nearestSubmesh, real & distance )const
	{
		MeshSPtr mesh = geometry->getMesh();
		Point3r center{ geometry->getParent()->getDerivedPosition() };
		BoundingSphere sphere{ center, mesh->getBoundingSphere().getRadius() };
		Matrix4x4r const & transform{ geometry->getParent()->getDerivedTransformationMatrix() };
		auto result = Intersection::eOut;
		real faceDist = std::numeric_limits< real >::max();

		if ( intersects( sphere, distance ) != Intersection::eOut )
		{
			for ( auto submesh : *mesh )
			{
				sphere.load( center, submesh->getBoundingSphere().getRadius() );

				if ( intersects( sphere, distance ) != Intersection::eOut )
				{
					for ( uint32_t k = 0u; k < submesh->getFaceCount(); k++ )
					{
						Face face
						{
							0,//submesh->getIndexBuffer().getData()[k * 3 + 0],
							0,//submesh->getIndexBuffer().getData()[k * 3 + 1],
							0,//submesh->getIndexBuffer().getData()[k * 3 + 2],
						};
						real curfaceDist = 0.0_r;

						if ( intersects( face, transform, *submesh, curfaceDist ) != Intersection::eOut && curfaceDist < faceDist )
						{
							result = Intersection::eIn;
							nearestFace = face;
							nearestSubmesh = submesh;
							distance = curfaceDist;
							faceDist = curfaceDist;
						}
					}
				}
			}
		}

		return result;
	}

	bool Ray::projectVertex( Point3r const & point, Point3r & result )const
	{
		bool isProjected = false;
		result = ( m_direction * real( point::dot( m_direction, point ) ) );

		if ( point::dot( m_direction, point ) >= 0.0 )
		{
			isProjected = true;
		}

		return isProjected;
	}
}
