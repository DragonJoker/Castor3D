#include "Castor3D/Render/Ray.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace c3d
{
	Ray::Ray( Position const & point
		, Camera const & camera
		, Size const & renderSize )
	{
		auto const & projection = camera.getProjection( {}, false );
		auto const & view = camera.getView();
		auto invProjectionView = ( projection * view ).getInverse();
		Point4f screen
		{
			( 2.0f * float( point[0] ) / float( renderSize.getWidth() ) ) - 1.0f,
			1.0f - ( 2.0f * float( point[1] ) / float( renderSize.getHeight() ) ),
			-1.0f,
			1.0f
		};
		Point4f world = invProjectionView * screen;
		world /= world[3];
		m_origin[0] = world[0];
		m_origin[1] = world[1];
		m_origin[2] = world[2];

		screen[2] = 1.0f;
		world = invProjectionView * screen;
		world /= world[3];
		m_direction[0] = world[0];
		m_direction[1] = world[1];
		m_direction[2] = world[2];
		m_direction -= m_origin;
		point::normalise( m_direction );
	}

	Ray::Ray( int x, int y
		, Camera const & camera
		, Size const & renderSize )
		: Ray{ Position{ x, y }, camera, renderSize }
	{
	}

	Ray::Ray( Point3f const & origin
		, Point3f const & direction )
		: m_origin{ origin }
		, m_direction{ direction }
	{
		point::normalise( m_direction );
	}

	Intersection Ray::intersects( Point3f const & pt1
		, Point3f const & pt2
		, Point3f const & pt3
		, float & distance )const
	{
		// see http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
		auto result = Intersection::eOut;
		Point3f e1{ pt2 - pt1 };
		Point3f e2{ pt1 - pt3 };
		Point3f h{ point::cross( m_direction, e2 ) };

		if ( float a = point::dot( e1, h );
			std::abs( a ) > 0.00001f )
		{
			auto f = 1.0f / a;
			Point3f s( m_origin - pt1 );
			float u = f * point::dot( s, h );

			if ( u >= 0.0 && u <= 1.0 )
			{
				Point3f q{ point::cross( s, e1 ) };
				float v = f * point::dot( m_direction, q );

				if ( v >= 0.0 && u + v <= 1.0 )
				{
					distance = f * point::dot( e2, q );

					if ( distance > 0.00001f )
					{
						result = Intersection::eIn;
					}
				}
			}
		}

		return result;
	}

	Intersection Ray::intersects( Face const & /*face*/
		, Matrix4x4f const & /*transform*/
		, Submesh const & /*submesh*/
		, float & /*distance*/ )const
	{
		//! Point3f pt1, pt2, pt3;
		//! auto stride = submesh.getVertexBuffer().getDeclaration().stride();
		//! return intersects( transform * Vertex::getPosition( &submesh.getVertexBuffer().getData()[face[0] * stride], pt1 )
		//! 	, transform * Vertex::getPosition( &submesh.getVertexBuffer().getData()[face[1] * stride], pt2 )
		//! 	, transform * Vertex::getPosition( &submesh.getVertexBuffer().getData()[face[2] * stride], pt3 )
		//! 	, distance );
		return Intersection::eOut;
	}

	Intersection Ray::intersects( Point3f const & vertex
		, float & distance )const
	{
		Intersection result = Intersection::eOut;
		Point3f u( m_origin - vertex );

		if ( Point3f puv;
			projectVertex( u, puv ) && point::lengthSquared( puv ) < 0.000001 )
		{
			distance = float( point::length( u ) );
			result = Intersection::eIn;
		}

		return result;
	}

	Intersection Ray::intersects( BoundingBox const & box
		, float & distance )const
	{
		Point3f v1( box.getMin().constPtr() );
		Point3f v8( box.getMax().constPtr() );
		Point3f v2( v8[0], v1[1], v1[2] );
		Point3f v3( v1[0], v8[1], v1[2] );
		Point3f v4( v8[0], v8[1], v1[2] );
		Point3f v5( v8[0], v1[1], v8[2] );
		Point3f v6( v1[0], v8[1], v8[2] );
		Point3f v7( v8[0], v8[1], v8[2] );
		auto result = Intersection::eOut;
		float dist = 0.0f;
		float min = std::numeric_limits< float >::max();

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

	Intersection Ray::intersects( BoundingSphere const & sphere
		, float & distance )const
	{
		// see http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
		auto result = Intersection::eOut;
		Point3f v( sphere.getCenter() - m_origin );

		if ( Point3f puv;
			projectVertex( v, puv ) )
		{
			// Sphere's center projects on the ray.
			distance = float( point::length( puv - v ) );

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
					distance = float( point::length( puv ) + sqrt( sphere.getRadius() * sphere.getRadius() - distance * distance ) );
				}
				else
				{
					// The ray origin is outside the sphere
					distance = float( point::length( puv ) - sqrt( sphere.getRadius() * sphere.getRadius() - distance * distance ) );
				}
			}
		}
		else
		{
			distance = float( point::length( v ) );

			if ( distance == sphere.getRadius() )
			{
				// Single intersection point.
				result = Intersection::eIntersect;
			}
			else if ( point::length( v ) < sphere.getRadius() )
			{
				// The sphere's center is behind the ray, and the rays origin is inside the sphere.
				distance = float( point::length( puv - v ) );
				distance = float( sqrt( sphere.getRadius() * sphere.getRadius() - distance * distance ) - point::length( puv ) );
				result = Intersection::eIn;
			}
			else
			{
				// No intersection.
			}
		}

		return result;
	}

	Intersection Ray::intersects( GeometryRPtr geometry
		, Face & nearestFace
		, SubmeshRPtr & nearestSubmesh
		, float & distance )const
	{
		auto mesh = geometry->getMesh();
		Point3f center{ geometry->getParent()->getDerivedPosition() };
		BoundingSphere sphere{ center, mesh->getBoundingSphere().getRadius() };
		//! Matrix4x4f const & transform{ geometry->getGlobalTransform() };
		auto result = Intersection::eOut;

		if ( intersects( sphere, distance ) != Intersection::eOut )
		{
			for ( auto & submesh : *mesh )
			{
				sphere.load( center, submesh->getBoundingSphere().getRadius() );

				if ( intersects( sphere, distance ) != Intersection::eOut )
				{
					result = Intersection::eIn;

					//! if ( auto indices = submesh->getIndexBuffer().lock() )
					//! {
					//! 	float faceDist = std::numeric_limits< float >::max();
					//! 
					//! 	for ( uint32_t k = 0u; k < submesh->getFaceCount(); k++ )
					//! 	{
					//! 		Face face
					//! 		{
					//! 			indices[k * 3 + 0],
					//! 			indices[k * 3 + 1],
					//! 			indices[k * 3 + 2],
					//! 		};
					//! 		float curfaceDist = 0.0f;
					//! 
					//! 		if ( intersects( face, transform, *submesh, curfaceDist ) != Intersection::eOut && curfaceDist < faceDist )
					//! 		{
					//! 			result = Intersection::eIn;
					//! 			nearestFace = face;
					//! 			nearestSubmesh = submesh;
					//! 			distance = curfaceDist;
					//! 			faceDist = curfaceDist;
					//! 		}
					//! 	}
					//! }
					//! 
					//! submesh->getIndexBuffer().unlock();
				}
			}
		}

		return result;
	}

	bool Ray::projectVertex( Point3f const & point, Point3f & result )const
	{
		bool isProjected = false;
		result = ( m_direction * point::dot( m_direction, point ) );

		if ( point::dot( m_direction, point ) >= 0.0 )
		{
			isProjected = true;
		}

		return isProjected;
	}
}
