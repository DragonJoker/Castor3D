#include "Frustum.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Render/Viewport.hpp"

#include <Graphics/BoundingBox.hpp>
#include <Graphics/BoundingSphere.hpp>

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
			real const ratio{ m_viewport.getRatio() };
			real const tan = real( m_viewport.getFovY().tan() );
			nearH = 2 * tan * near;
			nearW = nearH * m_viewport.getRatio();
			farH = 2 * tan * far;
			farW = farH * m_viewport.getRatio();
		}

		// Compute planes' points
		// N => Near
		// F => Far
		// C => Center
		// T => Top
		// L => Left
		// R => Right
		// B => Bottom
		Point3r rn{ right * nearW / 2.0_r };
		Point3r rf{ right * farW / 2.0_r };
		Point3r tn{ up * nearH / 2.0_r };
		Point3r tf{ up * farH / 2.0_r };
		Point3r nc{ position + front * near };
		Point3r ntl{ nc + tn - rn };
		Point3r ntr{ nc + tn + rn };
		Point3r nbl{ nc - tn - rn };
		Point3r nbr{ nc - tn + rn };
		Point3r fc{ position + front * far };
		Point3r ftl{ fc + tf - rf };
		Point3r ftr{ fc + tf + rf };
		Point3r fbl{ fc - tf - rf };
		Point3r fbr{ fc - tf + rf };

		// Fill planes
		m_planes[size_t( FrustumPlane::eNear )].set( ntl, ntr, nbr );
		m_planes[size_t( FrustumPlane::eFar )].set( ftr, ftl, fbl );
		m_planes[size_t( FrustumPlane::eLeft )].set( ntl, nbl, fbl );
		m_planes[size_t( FrustumPlane::eRight )].set( nbr, ntr, fbr );
		m_planes[size_t( FrustumPlane::eTop )].set( ntr, ntl, ftl );
		m_planes[size_t( FrustumPlane::eBottom )].set( nbl, nbr, fbr );
	}

	bool Frustum::isVisible( BoundingBox const & box
		, Matrix4x4r const & transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto aabb = box.getAxisAligned( transformations );
		auto & min = aabb.getMin();
		auto & max = aabb.getMax();

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

	bool Frustum::isVisible( BoundingSphere const & sphere
		, Point3r const & position
		, Point3r const & scale )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto maxScale = std::max( scale[0], std::max( scale[1], scale[2] ) );
		Point3r center = ( sphere.getCenter() * scale ) + position;
		auto radius = sphere.getRadius() * point::length( Point3r{ maxScale, maxScale, maxScale } );

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

		return result;
	}
}
