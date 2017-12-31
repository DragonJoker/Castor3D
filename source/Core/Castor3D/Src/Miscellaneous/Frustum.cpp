#include "Frustum.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Render/Viewport.hpp"

#include <Graphics/BoundingBox.hpp>
#include <Graphics/BoundingSphere.hpp>

using namespace castor;

#define DEBUG_FRUSTUM 0

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
		bool results[size_t( FrustumPlane::eCount )] =
		{
			m_planes[size_t( FrustumPlane::eNear )].distance( aabb.getPositiveVertex( m_planes[size_t( FrustumPlane::eNear )].getNormal() ) ) >= 0,
			m_planes[size_t( FrustumPlane::eFar )].distance( aabb.getPositiveVertex( m_planes[size_t( FrustumPlane::eFar )].getNormal() ) ) >= 0,
			m_planes[size_t( FrustumPlane::eLeft )].distance( aabb.getPositiveVertex( m_planes[size_t( FrustumPlane::eLeft )].getNormal() ) ) >= 0,
			m_planes[size_t( FrustumPlane::eRight )].distance( aabb.getPositiveVertex( m_planes[size_t( FrustumPlane::eRight )].getNormal() ) ) >= 0,
			m_planes[size_t( FrustumPlane::eTop )].distance( aabb.getPositiveVertex( m_planes[size_t( FrustumPlane::eTop )].getNormal() ) ) >= 0,
			m_planes[size_t( FrustumPlane::eBottom )].distance( aabb.getPositiveVertex( m_planes[size_t( FrustumPlane::eBottom )].getNormal() ) ) >= 0,
		};
		auto result = results[size_t( FrustumPlane::eNear )]
			&& results[size_t( FrustumPlane::eFar )]
			&& results[size_t( FrustumPlane::eLeft )]
			&& results[size_t( FrustumPlane::eRight )]
			&& results[size_t( FrustumPlane::eTop )]
			&& results[size_t( FrustumPlane::eBottom )];

#if DEBUG_FRUSTUM

		if ( !result )
		{
			auto index = 0u;

			while ( results[index] )
			{
				++index;
			}

			static String const names[]
			{
				cuT( "  Near" ),
				cuT( "   Far" ),
				cuT( "  Left" ),
				cuT( " Right" ),
				cuT( "   Top" ),
				cuT( "Bottom" )
			};

			auto plane = FrustumPlane( index );
			auto vp = aabb.getPositiveVertex( m_planes[index].getNormal() );
			auto dist = m_planes[index].distance( vp );
			auto center = aabb.getCenter();
			auto dimension = aabb.getDimensions();
			std::clog << cuT( "BoundingBox" )
				<< cuT( " - P: " ) << names[index]
				<< cuT( " - D: " ) << std::setw( 7 ) << std::setprecision( 4 ) << dist
				<< cuT( " - V: " ) << std::setw( 7 ) << std::setprecision( 4 ) << vp[0]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << vp[1]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << vp[2]
				<< cuT( " - C: " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[0]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[1]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[2]
				<< cuT( " - S: " ) << std::setw( 7 ) << std::setprecision( 4 ) << dimension[0]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << dimension[1]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << dimension[2] << std::endl;
		}

#endif

		return result;
	}

	bool Frustum::isVisible( BoundingSphere const & sphere
		, Matrix4x4r const & transformations
		, Point3r const & scale)const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto maxScale = std::max( scale[0], std::max( scale[1], scale[2] ) );
		Point3r center = transformations * sphere.getCenter();
		auto radius = sphere.getRadius() * maxScale;
		bool results[size_t( FrustumPlane::eCount )] = 
		{
			m_planes[size_t( FrustumPlane::eNear )].distance( center ) >= -radius,
			m_planes[size_t( FrustumPlane::eFar )].distance( center ) >= -radius,
			m_planes[size_t( FrustumPlane::eLeft )].distance( center ) >= -radius,
			m_planes[size_t( FrustumPlane::eRight )].distance( center ) >= -radius,
			m_planes[size_t( FrustumPlane::eTop )].distance( center ) >= -radius,
			m_planes[size_t( FrustumPlane::eBottom )].distance( center ) >= -radius,
		};
		auto result = results[size_t( FrustumPlane::eNear )]
			&& results[size_t( FrustumPlane::eFar )]
			&& results[size_t( FrustumPlane::eLeft )]
			&& results[size_t( FrustumPlane::eRight )]
			&& results[size_t( FrustumPlane::eTop )]
			&& results[size_t( FrustumPlane::eBottom )];

#if DEBUG_FRUSTUM

		if ( !result )
		{
			auto index = 0u;

			while ( results[index] )
			{
				++index;
			}

			static String const names[]
			{
				cuT( "  Near" ),
				cuT( "   Far" ),
				cuT( "  Left" ),
				cuT( " Right" ),
				cuT( "   Top" ),
				cuT( "Bottom" )
			};

			auto plane = FrustumPlane( index );
			auto dist = m_planes[index].distance( center );
			std::clog << cuT( "BoundingSphere" )
				<< cuT( " - P: " ) << names[index]
				<< cuT( " - D: " ) << std::setw( 7 ) << std::setprecision( 4 ) << dist
				<< cuT( " - C: " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[0]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[1]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[2]
				<< cuT( " - R: " ) << std::setw( 7 ) << std::setprecision( 4 ) << radius << std::endl;
		}

#endif

		return result;
	}

	bool Frustum::isVisible( Point3r const & point )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		bool results[size_t( FrustumPlane::eCount )] =
		{
			m_planes[size_t( FrustumPlane::eNear )].distance( point ) >= 0,
			m_planes[size_t( FrustumPlane::eFar )].distance( point ) >= 0,
			m_planes[size_t( FrustumPlane::eLeft )].distance( point ) >= 0,
			m_planes[size_t( FrustumPlane::eRight )].distance( point ) >= 0,
			m_planes[size_t( FrustumPlane::eTop )].distance( point ) >= 0,
			m_planes[size_t( FrustumPlane::eBottom )].distance( point ) >= 0,
		};
		auto result = results[size_t( FrustumPlane::eNear )]
			&& results[size_t( FrustumPlane::eFar )]
			&& results[size_t( FrustumPlane::eLeft )]
			&& results[size_t( FrustumPlane::eRight )]
			&& results[size_t( FrustumPlane::eTop )]
			&& results[size_t( FrustumPlane::eBottom )];

#if DEBUG_FRUSTUM

		if ( !result )
		{
			auto index = 0u;

			while ( results[index] )
			{
				++index;
			}

			static String const names[]
			{
				cuT( "  Near" ),
				cuT( "   Far" ),
				cuT( "  Left" ),
				cuT( " Right" ),
				cuT( "   Top" ),
				cuT( "Bottom" )
			};

			auto plane = FrustumPlane( index );
			auto dist = m_planes[index].distance( point );
			std::clog << cuT( "Point" )
				<< cuT( " - P: " ) << names[index]
				<< cuT( " - D: " ) << std::setw( 7 ) << std::setprecision( 4 ) << dist
				<< cuT( " - C: " ) << std::setw( 7 ) << std::setprecision( 4 ) << point[0]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << point[1]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << point[2] << std::endl;
		}

#endif

		return result;
	}
}
