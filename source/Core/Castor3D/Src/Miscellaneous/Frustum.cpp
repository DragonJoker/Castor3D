#include "Frustum.hpp"

#include "Render/Viewport.hpp"

#include <Graphics/CubeBox.hpp>
#include <Graphics/SphereBox.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		Point3r GetVertexP( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
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

		Point3r GetVertexN( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
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

	void Frustum::Update( Point3r const & p_position, Point3r const & p_x, Point3r const & p_y, Point3r const & p_z )
	{
		// Retrieve near and far planes' dimensions
		real farHeight{ 0.0_r };
		real farWidth{ 0.0_r };
		real nearHeight{ 0.0_r };
		real nearWidth{ 0.0_r };

		if ( m_viewport.GetType() == ViewportType::eOrtho )
		{
			nearHeight = ( m_viewport.GetBottom() - m_viewport.GetTop() );
			nearWidth = ( m_viewport.GetRight() - m_viewport.GetLeft() );
			farHeight = nearHeight;
			farWidth = nearWidth;
		}
		else
		{
			real tan = real( m_viewport.GetFovY().tan() );
			nearHeight = 2 * tan * m_viewport.GetNear();
			nearWidth = nearHeight * m_viewport.GetRatio();
			farHeight = 2 * tan * m_viewport.GetFar();
			farWidth = farHeight * m_viewport.GetRatio();
		}

		// Compute planes' points
		// N => Near, F => Far, C => Center, T => Top, L => Left, R => Right, B => Bottom
		Point3r rn{ p_x * nearWidth / 2 };
		Point3r rf{ p_x * farWidth / 2 };
		Point3r tn{ p_y * nearHeight / 2 };
		Point3r tf{ p_y * farHeight / 2 };
		Point3r nc{ p_position + p_z * m_viewport.GetNear() };
		Point3r ntl{ nc + tn - rn };
		Point3r ntr{ nc + tn + rn };
		Point3r nbl{ nc - tn - rn };
		Point3r nbr{ nc - tn + rn };
		Point3r fc{ p_position + p_z * m_viewport.GetFar() };
		Point3r ftl{ fc + tf - rf };
		Point3r ftr{ fc + tf + rf };
		Point3r fbl{ fc - tf - rf };
		Point3r fbr{ fc - tf + rf };

		// Fill planes
		m_planes[size_t( FrustumPlane::eNear )].Set( ntl, ntr, nbr );
		m_planes[size_t( FrustumPlane::eFar )].Set( ftr, ftl, fbl );
		m_planes[size_t( FrustumPlane::eLeft )].Set( ntl, nbl, fbl );
		m_planes[size_t( FrustumPlane::eRight )].Set( nbr, ntr, fbr );
		m_planes[size_t( FrustumPlane::eTop )].Set( ntr, ntl, ftl );
		m_planes[size_t( FrustumPlane::eBottom )].Set( nbl, nbr, fbr );
	}

	bool Frustum::IsVisible( CubeBox const & p_box, Matrix4x4r const & p_transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Point3r corners[8];
		corners[0] = p_box.GetMin();
		corners[1] = p_box.GetMax();

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

			if ( plane.Distance( GetVertexP( min, max, m_planes[i].GetNormal() ) ) < 0 )
			{
				// The positive vertex outside?
				result = Intersection::eOut;
			}
			else if ( plane.Distance( GetVertexN( min, max, m_planes[i].GetNormal() ) ) < 0 )
			{
				// The negative vertex outside?
				result = Intersection::eIntersect;
			}

			++i;
		}

		return result != Intersection::eOut;
	}

	bool Frustum::IsVisible( Castor::SphereBox const & p_box, Castor::Matrix4x4r const & m_transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Intersection result{ Intersection::eIn };
		Point3r center = p_box.GetCenter() + Point3r{ m_transformations[3][0], m_transformations[3][1], m_transformations[3][2] };
		size_t i{ 0u };

		while ( i < size_t( FrustumPlane::eCount ) && result != Intersection::eOut )
		{
			float distance = m_planes[i].Distance( center );

			if ( distance < -p_box.GetRadius() )
			{
				result = Intersection::eOut;
			}
			else if ( distance < p_box.GetRadius() )
			{
				result = Intersection::eIntersect;
			}

			++i;
		}

		return result != Intersection::eOut;
	}

	bool Frustum::IsVisible( Point3r const & p_point )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto it = std::find_if( m_planes.begin()
			, m_planes.end()
			, [&p_point]( auto const & p_plane )
		{
			return p_plane.Distance( p_point ) < 0;
		} );

		return it == m_planes.end();
	}
}
