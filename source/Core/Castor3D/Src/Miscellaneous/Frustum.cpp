#include "Frustum.hpp"

#include "Render/Viewport.hpp"

#include <Graphics/CubeBox.hpp>
#include <Graphics/SphereBox.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		Point3r getVertexP( Point3r const & min
			, Point3r const & max
			, Point3r const & normal )
		{
			Point3r result{ min };

			if ( normal[0] >= 0 )
			{
				result[0] = max[0];
			}

			if ( normal[1] >= 0 )
			{
				result[1] = max[1];
			}

			if ( normal[2] >= 0 )
			{
				result[2] = max[2];
			}

			return result;
		}

		Point3r getVertexN( Point3r const & min
			, Point3r const & max
			, Point3r const & normal )
		{
			Point3r result{ max };

			if ( normal[0] >= 0 )
			{
				result[0] = min[0];
			}

			if ( normal[1] >= 0 )
			{
				result[1] = min[1];
			}

			if ( normal[2] >= 0 )
			{
				result[2] = min[2];
			}

			return result;
		}
	}

	//*************************************************************************************************

	Frustum::Frustum( Viewport & viewport )
		: m_viewport{ viewport }
	{
	}

	void Frustum::update( Matrix4x4r const & projection
		, Matrix4x4r const & view )
	{
		auto const viewProjection = projection * view;

		m_planes[size_t( FrustumPlane::eNear )].set( Point3r{ viewProjection[2][0]
				, viewProjection[2][1]
				, viewProjection[2][2] }
			, viewProjection[2][3] );
		m_planes[size_t( FrustumPlane::eFar )].set( Point3r{ viewProjection[3][0] - viewProjection[2][0]
				, viewProjection[3][1] - viewProjection[2][1]
				, viewProjection[3][2] - viewProjection[2][2] }
			, viewProjection[3][3] - viewProjection[2][3] );
		m_planes[size_t( FrustumPlane::eLeft )].set( Point3r{ viewProjection[3][0] + viewProjection[0][0]
				, viewProjection[3][1] + viewProjection[0][1]
				, viewProjection[3][2] + viewProjection[0][2] }
			, viewProjection[3][3] + viewProjection[0][3] );
		m_planes[size_t( FrustumPlane::eRight )].set( Point3r{ viewProjection[3][0] - viewProjection[0][0]
				, viewProjection[3][1] - viewProjection[0][1]
				, viewProjection[3][2] - viewProjection[0][2] }
			, viewProjection[3][3] - viewProjection[0][3] );
		m_planes[size_t( FrustumPlane::eTop )].set( Point3r{ viewProjection[3][0] - viewProjection[1][0]
				, viewProjection[3][1] - viewProjection[1][1]
				, viewProjection[3][2] - viewProjection[1][2] }
			, viewProjection[3][3] - viewProjection[1][3] );
		m_planes[size_t( FrustumPlane::eBottom )].set( Point3r{ viewProjection[3][0] + viewProjection[1][0]
				, viewProjection[3][1] + viewProjection[1][1]
				, viewProjection[3][2] + viewProjection[1][2] }
			, viewProjection[3][3] + viewProjection[1][3] );
	}

	void Frustum::update( Point3r const & position
		, Point3r const & x
		, Point3r const & y
		, Point3r const & z )
	{
		// Retrieve near and far planes' dimensions
		real farHeight{ 0.0_r };
		real farWidth{ 0.0_r };
		real nearHeight{ 0.0_r };
		real nearWidth{ 0.0_r };

		if ( m_viewport.getType() == ViewportType::eOrtho )
		{
			nearHeight = ( m_viewport.getBottom() - m_viewport.getTop() );
			nearWidth = ( m_viewport.getRight() - m_viewport.getLeft() );
			farHeight = nearHeight;
			farWidth = nearWidth;
		}
		else
		{
			real tan = real( m_viewport.getFovY().tan() );
			nearHeight = 2 * tan * m_viewport.getNear();
			nearWidth = nearHeight * m_viewport.getRatio();
			farHeight = 2 * tan * m_viewport.getFar();
			farWidth = farHeight * m_viewport.getRatio();
		}

		// Compute planes' points
		// N => Near
		// F => Far
		// C => Center
		// T => Top
		// L => Left
		// R => Right
		// B => Bottom
		Point3r rn{ x * nearWidth / 2 };
		Point3r ln{ -rn };
		Point3r rf{ x * farWidth / 2 };
		Point3r lf{ -rf };
		Point3r tn{ y * nearHeight / 2 };
		Point3r bn{ -tn };
		Point3r tf{ y * farHeight / 2 };
		Point3r bf{ -tf };
		Point3r nc{ position + z * m_viewport.getNear() };
		Point3r ntl{ nc + tn + ln };
		Point3r ntr{ nc + tn + rn };
		Point3r nbl{ nc + bn + ln };
		Point3r nbr{ nc + bn + rn };
		Point3r fc{ position + z * m_viewport.getFar() };
		Point3r ftl{ fc + tf + lf };
		Point3r ftr{ fc + tf + rf };
		Point3r fbl{ fc + bf + lf };
		Point3r fbr{ fc + bf + rf };

		// Fill planes
		m_planes[size_t( FrustumPlane::eNear )].set( ntl, ntr, nbr );
		m_planes[size_t( FrustumPlane::eFar )].set( ftr, ftl, fbl );
		m_planes[size_t( FrustumPlane::eLeft )].set( ntl, nbl, fbl );
		m_planes[size_t( FrustumPlane::eRight )].set( nbr, ntr, fbr );
		m_planes[size_t( FrustumPlane::eTop )].set( ntr, ntl, ftl );
		m_planes[size_t( FrustumPlane::eBottom )].set( nbl, nbr, fbr );
	}

	bool Frustum::isVisible( CubeBox const & box
		, Matrix4x4r const & transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Point3r corners[8];
		corners[0] = box.getMin();
		corners[1] = box.getMax();

		// Express object box in world coordinates
		corners[2] = transformations * Point3r{ corners[0][0], corners[1][1], corners[0][2] };
		corners[3] = transformations * Point3r{ corners[1][0], corners[1][1], corners[0][2] };
		corners[4] = transformations * Point3r{ corners[1][0], corners[0][1], corners[0][2] };
		corners[5] = transformations * Point3r{ corners[0][0], corners[1][1], corners[1][2] };
		corners[6] = transformations * Point3r{ corners[0][0], corners[0][1], corners[1][2] };
		corners[7] = transformations * Point3r{ corners[1][0], corners[0][1], corners[1][2] };
		corners[0] = transformations * corners[0];
		corners[1] = transformations * corners[1];

		// Retrieve axis aligned box boundaries
		Point3r min( corners[0] );
		Point3r max( corners[1] );

		for ( int j = 0; j < 8; ++j )
		{
			min[0] = std::min( corners[j][0], min[0] );
			min[1] = std::min( corners[j][1], min[1] );
			min[2] = std::min( corners[j][2], min[2] );

			max[0] = std::max( corners[j][0], max[0] );
			max[1] = std::max( corners[j][1], max[1] );
			max[2] = std::max( corners[j][2], max[2] );
		}

		Intersection result{ Intersection::eIn };
		size_t i{ 0u };

		while ( i < size_t( FrustumPlane::eCount ) && result != Intersection::eOut )
		{
			auto & plane = m_planes[i];

			if ( plane.distance( getVertexP( min, max, m_planes[i].getNormal() ) ) < 0 )
			{
				// The positive vertex outside?
				result = Intersection::eOut;
			}
			else if ( plane.distance( getVertexN( min, max, m_planes[i].getNormal() ) ) < 0 )
			{
				// The negative vertex outside?
				result = Intersection::eIntersect;
			}

			++i;
		}

		return result != Intersection::eOut;
	}

	bool Frustum::isVisible( castor::SphereBox const & box
		, castor::Point3r const & position )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Point3r center = box.getCenter() + position;
		auto radius = box.getRadius() * 20;

		auto it = std::find_if( m_planes.begin()
			, m_planes.end()
			, [&center, &radius]( auto const & plane )
			{
				return plane.distance( center ) < -radius;
			} );

		bool result = it == m_planes.end();

#if !defined( NDEBUG )

		if ( !result )
		{
			auto dist = ( *it ).distance( center );
			std::clog << "dist: " << dist << ", radius: " << radius / 2.0f << std::endl;
		}

#endif

		//result = true;
		return result;
	}

	bool Frustum::isVisible( Point3r const & point )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto it = std::find_if( m_planes.begin()
			, m_planes.end()
			, [&point]( auto const & p_plane )
		{
			return p_plane.distance( point ) < 0;
		} );

		bool result = it == m_planes.end();

#if !defined( NDEBUG )

		if ( !result )
		{
			auto dist = ( *it ).distance( point );
			std::clog << dist << std::endl;
		}

#endif

		//result = true;
		return result;
	}
}
