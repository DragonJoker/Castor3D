#include "Frustum.hpp"

#include "Render/Viewport.hpp"

#include <Graphics/CubeBox.hpp>
#include <Graphics/SphereBox.hpp>
#include <Math/TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		Point3r GetVertexP( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r l_return{ p_min };

			if ( p_normal[0] >= 0 )
			{
				l_return[0] = p_max[0];
			}

			if ( p_normal[1] >= 0 )
			{
				l_return[1] = p_max[1];
			}

			if ( p_normal[2] >= 0 )
			{
				l_return[2] = p_max[2];
			}

			return l_return;
		}

		Point3r GetVertexN( Point3r const & p_min, Point3r const & p_max, Point3r const & p_normal )
		{
			Point3r l_return{ p_max };

			if ( p_normal[0] >= 0 )
			{
				l_return[0] = p_min[0];
			}

			if ( p_normal[1] >= 0 )
			{
				l_return[1] = p_min[1];
			}

			if ( p_normal[2] >= 0 )
			{
				l_return[2] = p_min[2];
			}

			return l_return;
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
		real l_farHeight{ 0.0_r };
		real l_farWidth{ 0.0_r };
		real l_nearHeight{ 0.0_r };
		real l_nearWidth{ 0.0_r };

		if ( m_viewport.GetType() == ViewportType::eOrtho )
		{
			l_nearHeight = ( m_viewport.GetBottom() - m_viewport.GetTop() );
			l_nearWidth = ( m_viewport.GetRight() - m_viewport.GetLeft() );
			l_farHeight = l_nearHeight;
			l_farWidth = l_nearWidth;
		}
		else
		{
			real l_tan = real( tan( m_viewport.GetFovY().radians() ) );
			l_nearHeight = l_tan * m_viewport.GetNear();
			l_nearWidth = l_nearHeight * m_viewport.GetRatio();
			l_farHeight = l_tan * m_viewport.GetFar();
			l_farWidth = l_farHeight * m_viewport.GetRatio();
		}

		// Compute planes' points
		// N => Near, F => Far, C => Center, T => Top, L => Left, R => Right, B => Bottom
		Point3r l_rn{ p_x * l_nearWidth };
		Point3r l_rf{ p_x * l_farWidth };
		Point3r l_tn{ p_y * l_nearHeight };
		Point3r l_tf{ p_y * l_farHeight };
		Point3r l_nc{ p_position + p_z * m_viewport.GetNear() };
		Point3r l_ntl{ l_nc + l_tn - l_rn };
		Point3r l_ntr{ l_nc + l_tn + l_rn };
		Point3r l_nbl{ l_nc - l_tn - l_rn };
		Point3r l_nbr{ l_nc - l_tn + l_rn };
		Point3r l_fc{ p_position + p_z * m_viewport.GetFar() };
		Point3r l_ftl{ l_fc + l_tf - l_rf };
		Point3r l_ftr{ l_fc + l_tf + l_rf };
		Point3r l_fbl{ l_fc - l_tf - l_rf };
		Point3r l_fbr{ l_fc - l_tf + l_rf };

		// Fill planes
		m_planes[size_t( FrustumPlane::eNear )].Set( l_ntl, l_ntr, l_nbr );
		m_planes[size_t( FrustumPlane::eFar )].Set( l_ftr, l_ftl, l_fbl );
		m_planes[size_t( FrustumPlane::eLeft )].Set( l_ntl, l_nbl, l_fbl );
		m_planes[size_t( FrustumPlane::eRight )].Set( l_nbr, l_ntr, l_fbr );
		m_planes[size_t( FrustumPlane::eTop )].Set( l_ntr, l_ntl, l_ftl );
		m_planes[size_t( FrustumPlane::eBottom )].Set( l_nbl, l_nbr, l_fbr );
	}

	bool Frustum::IsVisible( CubeBox const & p_box, Matrix4x4r const & p_transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Point3r l_corners[8];
		l_corners[0] = p_box.GetMin();
		l_corners[1] = p_box.GetMax();

		// Express object box in world coordinates
		l_corners[2] = p_transformations * Point3r{ l_corners[0][0], l_corners[1][1], l_corners[0][2] };
		l_corners[3] = p_transformations * Point3r{ l_corners[1][0], l_corners[1][1], l_corners[0][2] };
		l_corners[4] = p_transformations * Point3r{ l_corners[1][0], l_corners[0][1], l_corners[0][2] };
		l_corners[5] = p_transformations * Point3r{ l_corners[0][0], l_corners[1][1], l_corners[1][2] };
		l_corners[6] = p_transformations * Point3r{ l_corners[0][0], l_corners[0][1], l_corners[1][2] };
		l_corners[7] = p_transformations * Point3r{ l_corners[1][0], l_corners[0][1], l_corners[1][2] };
		l_corners[0] = p_transformations * l_corners[0];
		l_corners[1] = p_transformations * l_corners[1];

		// Retrieve axis aligned box boundaries
		Point3r l_min( l_corners[0] );
		Point3r l_max( l_corners[1] );

		for ( int j = 0; j < 8; ++j )
		{
			l_min[0] = std::min( l_corners[j][0], l_min[0] );
			l_min[1] = std::min( l_corners[j][1], l_min[1] );
			l_min[2] = std::min( l_corners[j][2], l_min[2] );

			l_max[0] = std::max( l_corners[j][0], l_max[0] );
			l_max[1] = std::max( l_corners[j][1], l_max[1] );
			l_max[2] = std::max( l_corners[j][2], l_max[2] );
		}

		Intersection l_return{ Intersection::eIn };
		size_t i{ 0u };

		while ( i < size_t( FrustumPlane::eCount ) && l_return != Intersection::eOut )
		{
			auto & l_plane = m_planes[i];

			if ( l_plane.Distance( GetVertexP( l_min, l_max, m_planes[i].GetNormal() ) ) < 0 )
			{
				// The positive vertex outside?
				l_return = Intersection::eOut;
			}
			else if ( l_plane.Distance( GetVertexN( l_min, l_max, m_planes[i].GetNormal() ) ) < 0 )
			{
				// The negative vertex outside?
				l_return = Intersection::eIntersect;
			}

			++i;
		}

		return l_return != Intersection::eOut;
	}

	bool Frustum::IsVisible( Castor::SphereBox const & p_box, Castor::Matrix4x4r const & m_transformations )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		Intersection l_return{ Intersection::eIn };
		Point3r l_center = p_box.GetCenter() + Point3r{ m_transformations[3][0], m_transformations[3][1], m_transformations[3][2] };
		size_t i{ 0u };

		while ( i < size_t( FrustumPlane::eCount ) && l_return != Intersection::eOut )
		{
			float l_distance = m_planes[i].Distance( l_center );

			if ( l_distance < -p_box.GetRadius() )
			{
				l_return = Intersection::eOut;
			}
			else if ( l_distance < p_box.GetRadius() )
			{
				l_return = Intersection::eIntersect;
			}

			++i;
		}

		return l_return != Intersection::eOut;
	}

	bool Frustum::IsVisible( Point3r const & p_point )const
	{
		//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
		auto l_it = std::find_if( m_planes.begin(), m_planes.end(), [&p_point]( auto const & p_plane )
		{
			return p_plane.Distance( p_point ) < 0;
		} );

		return l_it == m_planes.end();
	}
}
