#include "Frustum.hpp"

#include "Render/Viewport.hpp"

#include <Graphics/CubeBox.hpp>
#include <Graphics/SphereBox.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		Point3r getVertexP( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r result{ p_min };

			if ( p_normal[0] >= 0 )
			{
				result[0] = p_max[0];
			}

			if ( p_normal[1] >= 0 )
			{
				result[1] = p_max[1];
			}

			if ( p_normal[2] >= 0 )
			{
				result[2] = p_max[2];
			}

			return result;
		}

		Point3r getVertexN( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r result{ p_max };

			if ( p_normal[0] >= 0 )
			{
				result[0] = p_min[0];
			}

			if ( p_normal[1] >= 0 )
			{
				result[1] = p_min[1];
			}

			if ( p_normal[2] >= 0 )
			{
				result[2] = p_min[2];
			}

			return result;
		}
	}

	//*************************************************************************************************

	Frustum::Frustum( Viewport & p_viewport )
		: m_viewport{ p_viewport }
	{
	}

	void Frustum::update( Point3r const & p_position, Point3r const & p_x, Point3r const & p_y, Point3r const & p_z )
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
		// N => Near, F => Far, C => Center, T => Top, L => Left, R => Right, B => Bottom
		Point3r rn{ p_x * nearWidth / 2 };
		Point3r rf{ p_x * farWidth / 2 };
		Point3r tn{ p_y * nearHeight / 2 };
		Point3r tf{ p_y * farHeight / 2 };
		Point3r nc{ p_position + p_z * m_viewport.getNear() };
		Point3r ntl{ nc + tn - rn };
		Point3r ntr{ nc + tn + rn };
		Point3r nbl{ nc - tn - rn };
		Point3r nbr{ nc - tn + rn };
		Point3r fc{ p_position + p_z * m_viewport.getFar() };
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

	bool Frustum::isVisible( CubeBox const & p_box, Matrix4x4r const & p_transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Point3r corners[8];
		corners[0] = p_box.getMin();
		corners[1] = p_box.getMax();

		// Express object box in world coordinates
		corners[2] = p_transformations * Point3r{ corners[0][0], corners[1][1], corners[0][2] };
		corners[3] = p_transformations * Point3r{ corners[1][0], corners[1][1], corners[0][2] };
		corners[4] = p_transformations * Point3r{ corners[1][0], corners[0][1], corners[0][2] };
		corners[5] = p_transformations * Point3r{ corners[0][0], corners[1][1], corners[1][2] };
		corners[6] = p_transformations * Point3r{ corners[0][0], corners[0][1], corners[1][2] };
		corners[7] = p_transformations * Point3r{ corners[1][0], corners[0][1], corners[1][2] };
		corners[0] = p_transformations * corners[0];
		corners[1] = p_transformations * corners[1];

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

	bool Frustum::isVisible( castor::SphereBox const & p_box, castor::Matrix4x4r const & m_transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Intersection result{ Intersection::eIn };
		Point3r center = p_box.getCenter() + Point3r{ m_transformations[3][0], m_transformations[3][1], m_transformations[3][2] };

		for ( auto & plane : m_planes )
		{
			float distance = plane.distance( center );

			if ( distance < -( p_box.getRadius() ) )
			{
				return false;
			}

			if ( distance < p_box.getRadius() )
			{
				result = Intersection::eIntersect;
			}
		}

		return result != Intersection::eOut;
	}

	bool Frustum::isVisible( Point3r const & p_point )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto it = std::find_if( m_planes.begin()
			, m_planes.end()
			, [&p_point]( auto const & p_plane )
		{
			return p_plane.distance( p_point ) < 0;
		} );

		return it == m_planes.end();
	}
}
