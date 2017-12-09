#include "Frustum.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Render/Viewport.hpp"

#include <Graphics/BoundingBox.hpp>
#include <Graphics/BoundingSphere.hpp>

using namespace castor;

namespace castor3d
{
	Frustum::Frustum( Viewport & viewport )
		: m_viewport{ viewport }
	{
	}

	void Frustum::update( Matrix4x4r const & projection
		, Matrix4x4r const & view )
	{
		auto const viewProjection = projection * view;
		auto near = viewProjection[3] + viewProjection[2];
		auto far = viewProjection[3] - viewProjection[2];
		auto left = viewProjection[3] + viewProjection[0];
		auto right = viewProjection[3] - viewProjection[0];
		auto bottom = viewProjection[3] + viewProjection[1];
		auto top = viewProjection[3] - viewProjection[1];
		m_planes[size_t( FrustumPlane::eNear )].set( Point3r{ near[0], near[1], near[2] }, near[3] );
		m_planes[size_t( FrustumPlane::eFar )].set( Point3r{ far[0], far[1], far[2] }, far[3] );
		m_planes[size_t( FrustumPlane::eLeft )].set( Point3r{ left[0], left[1], left[2] }, left[3] );
		m_planes[size_t( FrustumPlane::eRight )].set( Point3r{ right[0], right[1], right[2] }, right[3] );
		m_planes[size_t( FrustumPlane::eBottom )].set( Point3r{ bottom[0], bottom[1], bottom[2] }, bottom[3] );
		m_planes[size_t( FrustumPlane::eTop )].set( Point3r{ top[0], top[1], top[2] }, top[3] );
	}

	void Frustum::update( Point3r const & position
		, Point3r const & right
		, Point3r const & up
		, Point3r const & front )
	{
		// Retrieve near and far planes' dimensions
		real farH{ 0.0_r };
		real farW{ 0.0_r };
		real nearH{ 0.0_r };
		real nearW{ 0.0_r };
		real const near{ m_viewport.getNear() };
		real const far{ m_viewport.getFar() };

		if ( m_viewport.getType() == ViewportType::eOrtho )
		{
			nearH = ( m_viewport.getBottom() - m_viewport.getTop() );
			nearW = ( m_viewport.getRight() - m_viewport.getLeft() );
			farH = nearH;
			farW = nearW;
		}
		else
		{
			auto ratio = m_viewport.getRatio();
			real const tan = real( m_viewport.getFovY().tan() );
			nearH = near * tan;
			nearW = nearH * ratio;
			farH = far * tan;
			farW = farH * ratio;
		}

		// Compute planes' corners
		Point3r nc{ position + front * near };
		Point3r fc{ position + front * far };
		m_corners[size_t( FrustumCorner::eFarLeftBottom )] = fc - up * farH - right * farW;
		m_corners[size_t( FrustumCorner::eFarLeftTop )] = fc + up * farH - right * farW;
		m_corners[size_t( FrustumCorner::eFarRightTop )] = fc + up * farH + right * farW;
		m_corners[size_t( FrustumCorner::eFarRightBottom )] = fc - up * farH + right * farW;
		m_corners[size_t( FrustumCorner::eNearLeftBottom )] = nc - up * nearH - right * nearW;
		m_corners[size_t( FrustumCorner::eNearLeftTop )] = nc + up * nearH - right * nearW;
		m_corners[size_t( FrustumCorner::eNearRightTop )] = nc + up * nearH + right * nearW;
		m_corners[size_t( FrustumCorner::eNearRightBottom )] = nc - up * nearH + right * nearW;

		// Fill planes
		m_planes[size_t( FrustumPlane::eBottom )].set( m_corners[size_t( FrustumCorner::eNearLeftBottom )]
			, m_corners[size_t( FrustumCorner::eNearRightBottom )]
			, m_corners[size_t( FrustumCorner::eFarRightBottom )] );
		m_planes[size_t( FrustumPlane::eFar )].set( m_corners[size_t( FrustumCorner::eFarRightTop )]
			, m_corners[size_t( FrustumCorner::eFarLeftTop )]
			, m_corners[size_t( FrustumCorner::eFarLeftBottom )] );
		m_planes[size_t( FrustumPlane::eLeft )].set( m_corners[size_t( FrustumCorner::eNearLeftTop )]
			, m_corners[size_t( FrustumCorner::eNearLeftBottom )]
			, m_corners[size_t( FrustumCorner::eFarLeftBottom )] );
		m_planes[size_t( FrustumPlane::eNear )].set( m_corners[size_t( FrustumCorner::eNearLeftTop )]
			, m_corners[size_t( FrustumCorner::eNearRightTop )]
			, m_corners[size_t( FrustumCorner::eNearRightBottom )] );
		m_planes[size_t( FrustumPlane::eRight )].set( m_corners[size_t( FrustumCorner::eNearRightBottom )]
			, m_corners[size_t( FrustumCorner::eNearRightTop )]
			, m_corners[size_t( FrustumCorner::eFarRightBottom )] );
		m_planes[size_t( FrustumPlane::eTop )].set( m_corners[size_t( FrustumCorner::eNearRightTop )]
			, m_corners[size_t( FrustumCorner::eNearLeftTop )]
			, m_corners[size_t( FrustumCorner::eFarLeftTop )] );
	}

	void Frustum::update( Point3r const & eye
		, Point3r const & target
		, Point3r const & up )
	{
		auto f = point::getNormalised( target - eye );
		auto u = point::getNormalised( up );
		auto r = point::cross( f, u );
		update( eye, r, u, f );
	}

	bool Frustum::isVisible( BoundingBox const & box
		, Matrix4x4r const & transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto aabb = box.getAxisAligned( transformations );
		auto it = std::find_if( m_planes.begin()
			, m_planes.end()
			, [&aabb]( PlaneEquation const & plane )
			{
				return plane.distance( aabb.getPositiveVertex( plane.getNormal() ) ) < 0;
			} );
		bool result = it == m_planes.end();

#if !defined( NDEBUG )

		if ( !result )
		{
			auto dist = it->distance( aabb.getPositiveVertex( it->getNormal() ) ) < 0;
			std::clog << dist << std::endl;
		}

#endif

		return result;

		return it == m_planes.end();
	}

	bool Frustum::isVisible( BoundingSphere const & sphere
		, Point3r const & position
		, Point3r const & scale )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto maxScale = std::max( scale[0], std::max( scale[1], scale[2] ) );
		Point3r center = ( sphere.getCenter() * scale ) + position;
		auto radius = sphere.getRadius() * maxScale;
		auto it = std::find_if( m_planes.begin()
			, m_planes.end()
			, [&center, &radius]( PlaneEquation const & plane )
			{
				return plane.distance( center ) < -radius;
			} );
		bool result = it == m_planes.end();

#if !defined( NDEBUG )

		if ( !result )
		{
			auto dist = it->distance( center );
			std::clog << "dist: " << dist << ", radius: " << radius << ", scale: " << point::length( scale ) << std::endl;
		}

#endif

		return result;
	}

	bool Frustum::isVisible( Point3r const & point )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto it = std::find_if( m_planes.begin()
			, m_planes.end()
			, [&point]( PlaneEquation const & p_plane )
		{
			return p_plane.distance( point ) < 0;
		} );
		bool result = it == m_planes.end();

#if !defined( NDEBUG )

		if ( !result )
		{
			auto dist = it->distance( point );
			std::clog << dist << std::endl;
		}

#endif

		return result;
	}
}
