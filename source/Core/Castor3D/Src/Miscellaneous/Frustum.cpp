#include "Frustum.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Render/Viewport.hpp"

#include <Graphics/BoundingBox.hpp>
#include <Graphics/BoundingSphere.hpp>

using namespace castor;

#define C3D_DisableFrustumCulling 0

#if C3D_DebugFrustum
bool C3D_DebugFrustumDisplay = false;
#endif

namespace castor3d
{
	Frustum::Frustum( Viewport & viewport )
		: m_viewport{ viewport }
	{
	}

	Frustum::Planes Frustum::update( Matrix4x4r const & projection
		, Matrix4x4r const & view )
	{
#if !C3D_DisableFrustumCulling
		auto const viewProjection = projection * view;
		std::array< Point4f, size_t( FrustumPlane::eCount ) > points;

		const Point4f w{ viewProjection[0][3]
			, viewProjection[1][3]
			, viewProjection[2][3]
			, viewProjection[3][3] };
		const Point4f z{ viewProjection[0][2]
			, viewProjection[1][2]
			, viewProjection[2][2]
			, viewProjection[3][2] };
		points[size_t( FrustumPlane::eNear )] = w + z;
		points[size_t( FrustumPlane::eFar )] = w - z;

		const Point4f x{ viewProjection[0][0]
			, viewProjection[1][0]
			, viewProjection[2][0]
			, viewProjection[3][0] };
		points[size_t( FrustumPlane::eLeft )] = w - x;
		points[size_t( FrustumPlane::eRight )] = w + x;

		const Point4f y{ viewProjection[0][1]
			, viewProjection[1][1]
			, viewProjection[2][1]
			, viewProjection[3][1] };
		points[size_t( FrustumPlane::eTop )] = w - y;
		points[size_t( FrustumPlane::eBottom )] = w + y;

		for ( auto & point : points )
		{
			auto invLen = float( 1.0f / sqrt( point[0] * point[0] + point[1] * point[1] + point[2] * point[2] ) );
			point[0] *= invLen;
			point[1] *= invLen;
			point[2] *= invLen;
			point[3] *= invLen;
		}

		m_planes[size_t( FrustumPlane::eNear )].set( Point3r{ points[size_t( FrustumPlane::eNear )] }, points[size_t( FrustumPlane::eNear )][3] );
		m_planes[size_t( FrustumPlane::eFar )].set( Point3r{ points[size_t( FrustumPlane::eFar )] }, points[size_t( FrustumPlane::eFar )][3] );
		m_planes[size_t( FrustumPlane::eLeft )].set( Point3r{ points[size_t( FrustumPlane::eLeft )] }, points[size_t( FrustumPlane::eLeft )][3] );
		m_planes[size_t( FrustumPlane::eRight )].set( Point3r{ points[size_t( FrustumPlane::eRight )] }, points[size_t( FrustumPlane::eRight )][3] );
		m_planes[size_t( FrustumPlane::eTop )].set( Point3r{ points[size_t( FrustumPlane::eTop )] }, points[size_t( FrustumPlane::eTop )][3] );
		m_planes[size_t( FrustumPlane::eBottom )].set( Point3r{ points[size_t( FrustumPlane::eBottom )] }, points[size_t( FrustumPlane::eBottom )][3] );
#endif
#if C3D_DebugFrustum
		std::clog << cuT( "Frustum" ) << std::endl
			<< cuT( "  - Bottom: " ) << m_planes[size_t( FrustumPlane::eBottom )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eBottom )].getDistance() << std::endl
			<< cuT( "  - Far:    " ) << m_planes[size_t( FrustumPlane::eFar )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eFar )].getDistance() << std::endl
			<< cuT( "  - Left:   " ) << m_planes[size_t( FrustumPlane::eLeft )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eLeft )].getDistance() << std::endl
			<< cuT( "  - Near:   " ) << m_planes[size_t( FrustumPlane::eNear )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eNear )].getDistance() << std::endl
			<< cuT( "  - Right:  " ) << m_planes[size_t( FrustumPlane::eRight )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eRight )].getDistance() << std::endl
			<< cuT( "  - Top:    " ) << m_planes[size_t( FrustumPlane::eTop )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eTop )].getDistance() << std::endl;
#endif

		return m_planes;
	}

	Frustum::Planes Frustum::update( Point3r const & position
		, Point3r const & right
		, Point3r const & up
		, Point3r const & front )
	{
#if !C3D_DisableFrustumCulling
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
		Point3r nearC{ position + front * near };
		Point3r farC{ position + front * far };
		std::array< castor::Point3r, size_t( FrustumCorner::eCount ) > corners;
		corners[size_t( FrustumCorner::eFarLeftBottom )] = farC - up * farH - right * farW;
		corners[size_t( FrustumCorner::eFarLeftTop )] = farC + up * farH - right * farW;
		corners[size_t( FrustumCorner::eFarRightTop )] = farC + up * farH + right * farW;
		corners[size_t( FrustumCorner::eFarRightBottom )] = farC - up * farH + right * farW;
		corners[size_t( FrustumCorner::eNearLeftBottom )] = nearC - up * nearH - right * nearW;
		corners[size_t( FrustumCorner::eNearLeftTop )] = nearC + up * nearH - right * nearW;
		corners[size_t( FrustumCorner::eNearRightTop )] = nearC + up * nearH + right * nearW;
		corners[size_t( FrustumCorner::eNearRightBottom )] = nearC - up * nearH + right * nearW;

		// Fill planes
		m_planes[size_t( FrustumPlane::eBottom )].set( corners[size_t( FrustumCorner::eNearLeftBottom )]
			, corners[size_t( FrustumCorner::eNearRightBottom )]
			, corners[size_t( FrustumCorner::eFarRightBottom )] );
		m_planes[size_t( FrustumPlane::eFar )].set( corners[size_t( FrustumCorner::eFarRightTop )]
			, corners[size_t( FrustumCorner::eFarLeftTop )]
			, corners[size_t( FrustumCorner::eFarLeftBottom )] );
		m_planes[size_t( FrustumPlane::eLeft )].set( corners[size_t( FrustumCorner::eNearLeftTop )]
			, corners[size_t( FrustumCorner::eNearLeftBottom )]
			, corners[size_t( FrustumCorner::eFarLeftBottom )] );
		m_planes[size_t( FrustumPlane::eNear )].set( corners[size_t( FrustumCorner::eNearLeftTop )]
			, corners[size_t( FrustumCorner::eNearRightTop )]
			, corners[size_t( FrustumCorner::eNearRightBottom )] );
		m_planes[size_t( FrustumPlane::eRight )].set( corners[size_t( FrustumCorner::eNearRightBottom )]
			, corners[size_t( FrustumCorner::eNearRightTop )]
			, corners[size_t( FrustumCorner::eFarRightBottom )] );
		m_planes[size_t( FrustumPlane::eTop )].set( corners[size_t( FrustumCorner::eNearRightTop )]
			, corners[size_t( FrustumCorner::eNearLeftTop )]
			, corners[size_t( FrustumCorner::eFarLeftTop )] );
#endif
#if C3D_DebugFrustum
		std::clog << cuT( "Frustum" ) << std::endl
			<< cuT( "  - Bottom: " ) << m_planes[size_t( FrustumPlane::eBottom )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eBottom )].getDistance() << std::endl
			<< cuT( "  - Far:    " ) << m_planes[size_t( FrustumPlane::eFar )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eFar )].getDistance() << std::endl
			<< cuT( "  - Left:   " ) << m_planes[size_t( FrustumPlane::eLeft )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eLeft )].getDistance() << std::endl
			<< cuT( "  - Near:   " ) << m_planes[size_t( FrustumPlane::eNear )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eNear )].getDistance() << std::endl
			<< cuT( "  - Right:  " ) << m_planes[size_t( FrustumPlane::eRight )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eRight )].getDistance() << std::endl
			<< cuT( "  - Top:    " ) << m_planes[size_t( FrustumPlane::eTop )].getNormal() << " - " << m_planes[size_t( FrustumPlane::eTop )].getDistance() << std::endl;
#endif

		return m_planes;
	}

	Frustum::Planes Frustum::update( Point3r const & eye
		, Point3r const & target
		, Point3r const & up )
	{
#if !C3D_DisableFrustumCulling
		auto f = point::getNormalised( target - eye );
		auto u = point::getNormalised( up );
		auto r = point::cross( f, u );
		update( eye, r, u, f );
#endif

		return m_planes;
	}

	bool Frustum::isVisible( BoundingBox const & box
		, Matrix4x4r const & transformations )const
	{
#if C3D_DisableFrustumCulling
		return true;
#else
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

#	if C3D_DebugFrustum

		if ( C3D_DebugFrustumDisplay )
		{
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
				std::clog << cuT( " - BoundingBox" )
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
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << dimension[2];
			}
			else
			{
				std::clog << cuT( " - BoundingBox - OK" );
			}
		}

#	endif

		return result;
#endif
	}

	bool Frustum::isVisible( BoundingSphere const & sphere
		, Matrix4x4r const & transformations
		, Point3r const & scale)const
	{
#if C3D_DisableFrustumCulling
		return true;
#else
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

#	if C3D_DebugFrustum

		if ( C3D_DebugFrustumDisplay )
		{
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
				std::clog << cuT( " - BoundingSphere" )
					<< cuT( " - P: " ) << names[index]
					<< cuT( " - D: " ) << std::setw( 7 ) << std::setprecision( 4 ) << dist
					<< cuT( " - C: " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[0]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[1]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << center[2]
					<< cuT( " - R: " ) << std::setw( 7 ) << std::setprecision( 4 ) << radius;
			}
			else
			{
				std::clog << cuT( " - BoundingSphere - OK" );
			}
		}

#	endif

		return result;
#endif
	}

	bool Frustum::isVisible( Point3r const & point )const
	{
#if C3D_DisableFrustumCulling
		return true;
#else
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

#	if C3D_DebugFrustum

		if ( C3D_DebugFrustumDisplay )
		{
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
				std::clog << cuT( " - Point" )
					<< cuT( " - P: " ) << names[index]
					<< cuT( " - D: " ) << std::setw( 7 ) << std::setprecision( 4 ) << dist
					<< cuT( " - C: " ) << std::setw( 7 ) << std::setprecision( 4 ) << point[0]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << point[1]
					<< cuT( ", " ) << std::setw( 7 ) << std::setprecision( 4 ) << point[2];
			}
			else
			{
				std::clog << cuT( " - Point - OK" );
			}
		}

#	endif

		return result;
#endif
	}
}
