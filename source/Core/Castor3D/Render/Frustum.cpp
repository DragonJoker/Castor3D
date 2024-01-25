#include "Castor3D/Render/Frustum.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Viewport.hpp"

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( castor3d, Frustum )

namespace castor3d
{
	namespace rendfrust
	{
		static bool constexpr C3D_DisableFrustumCulling = false;

		static void updatePoints( castor::Matrix4x4f const & viewProj
			, castor::Array< InterleavedVertex, 8u > & points )
		{
			castor::Array< castor::Point3f, 8u > corners;
			auto constexpr neg1 = -1.0f;
			auto constexpr pos1 = +1.0f;
			corners[size_t( FrustumCorner::eFarLeftBottom )] = { neg1, neg1, pos1 };
			corners[size_t( FrustumCorner::eFarLeftTop )] = { neg1, pos1, pos1 };
			corners[size_t( FrustumCorner::eFarRightTop )] = { pos1, pos1, pos1 };
			corners[size_t( FrustumCorner::eFarRightBottom )] = { pos1, neg1, pos1 };
			corners[size_t( FrustumCorner::eNearLeftBottom )] = { neg1, neg1, 0.0f };
			corners[size_t( FrustumCorner::eNearLeftTop )] = { neg1, pos1, 0.0f };
			corners[size_t( FrustumCorner::eNearRightTop )] = { pos1, pos1, 0.0f };
			corners[size_t( FrustumCorner::eNearRightBottom )] = { pos1, neg1, 0.0f };
			auto invViewProj = viewProj.getInverse();

			for ( auto & corner : corners )
			{
				corner = invViewProj * corner;
			}

			for ( size_t i = 0u; i < 8u; ++i )
			{
				points[i].pos = corners[i];
			}
		}
	}

	Frustum::Frustum( Viewport & viewport )
		: m_viewport{ &viewport }
	{
	}

	void Frustum::update( castor::Matrix4x4f const & view )
	{
		update( m_viewport->getProjection(), view );
	}

	void Frustum::update( castor::Matrix4x4f const & projection
		, castor::Matrix4x4f const & view )
	{
		if constexpr ( !rendfrust::C3D_DisableFrustumCulling )
		{
			auto const vp = projection * view;
			castor::Array< castor::Point4f, size_t( FrustumPlane::eCount ) > points;

			const castor::Point4f x{ vp[0][0], vp[1][0], vp[2][0], vp[3][0] };
			const castor::Point4f y{ vp[0][1], vp[1][1], vp[2][1], vp[3][1] };
			const castor::Point4f z{ vp[0][2], vp[1][2], vp[2][2], vp[3][2] };
			const castor::Point4f w{ vp[0][3], vp[1][3], vp[2][3], vp[3][3] };
			points[size_t( FrustumPlane::eNear )] = z;
			points[size_t( FrustumPlane::eFar )] = w - z;
			points[size_t( FrustumPlane::eLeft )] = w + x;
			points[size_t( FrustumPlane::eRight )] = w - x;
			points[size_t( FrustumPlane::eTop )] = w - y;
			points[size_t( FrustumPlane::eBottom )] = w + y;
			castor::Point3f min{ points[0] };
			castor::Point3f max{ points[0] };

			for ( auto & point : points )
			{
				min->x = std::min( point->x, min->x );
				min->y = std::min( point->y, min->y );
				min->z = std::min( point->z, min->z );

				max->x = std::max( point->x, max->x );
				max->y = std::max( point->y, max->y );
				max->z = std::max( point->z, max->z );

				auto len = sqrt( point[0] * point[0] + point[1] * point[1] + point[2] * point[2] );

				if ( len != 0.0f )
				{
					auto invLen = float( 1.0f / len );
					point[0] *= invLen;
					point[1] *= invLen;
					point[2] *= invLen;
					point[3] *= invLen;
				}
				else
				{
					point[0] = 0.0f;
					point[1] = 0.0f;
					point[2] = 0.0f;
					point[3] = 0.0f;
				}
			}

			m_boundingBox.load( min, max );
			m_planes[size_t( FrustumPlane::eNear )].set( castor::Point3f{ points[size_t( FrustumPlane::eNear )] }, points[size_t( FrustumPlane::eNear )][3] );
			m_planes[size_t( FrustumPlane::eFar )].set( castor::Point3f{ points[size_t( FrustumPlane::eFar )] }, points[size_t( FrustumPlane::eFar )][3] );
			m_planes[size_t( FrustumPlane::eLeft )].set( castor::Point3f{ points[size_t( FrustumPlane::eLeft )] }, points[size_t( FrustumPlane::eLeft )][3] );
			m_planes[size_t( FrustumPlane::eRight )].set( castor::Point3f{ points[size_t( FrustumPlane::eRight )] }, points[size_t( FrustumPlane::eRight )][3] );
			m_planes[size_t( FrustumPlane::eTop )].set( castor::Point3f{ points[size_t( FrustumPlane::eTop )] }, points[size_t( FrustumPlane::eTop )][3] );
			m_planes[size_t( FrustumPlane::eBottom )].set( castor::Point3f{ points[size_t( FrustumPlane::eBottom )] }, points[size_t( FrustumPlane::eBottom )][3] );

			rendfrust::updatePoints( vp, m_points );
		}
	}

	bool Frustum::isVisible( castor::BoundingBox const & box
		, castor::Matrix4x4f const & transformations )const
	{
		if constexpr ( rendfrust::C3D_DisableFrustumCulling )
		{
			return true;
		}
		else
		{
			//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
			auto aabb = box.getAxisAligned( transformations );
			return std::all_of( m_planes.begin()
				, m_planes.end()
				, [&aabb]( castor::PlaneEquation const & plane )
				{
						return plane.distance( aabb.getPositiveVertex( plane.getNormal() ) ) >= 0;
				} );
		}
	}

	bool Frustum::isVisible( castor::BoundingSphere const & sphere
		, castor::Matrix4x4f const & transformations
		, castor::Point3f const & scale)const
	{
		if constexpr ( rendfrust::C3D_DisableFrustumCulling )
		{
			return true;
		}
		else
		{
			//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
			auto maxScale = std::max( scale[0], std::max( scale[1], scale[2] ) );
			castor::Point3f center = transformations * sphere.getCenter();
			auto radius = sphere.getRadius() * maxScale;
			return std::all_of( m_planes.begin()
				, m_planes.end()
				, [&center, &radius]( castor::PlaneEquation const & plane )
				{
						return plane.distance( center ) >= -radius;
				} );
		}
	}

	bool Frustum::isVisible( castor::Point3f const & point )const
	{
		if constexpr ( rendfrust::C3D_DisableFrustumCulling )
		{
			return true;
		}
		else
		{
			//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
			return std::all_of( m_planes.begin()
				, m_planes.end()
				, [&point]( castor::PlaneEquation const & plane )
				{
						return plane.distance( point ) >= 0;
				} );
		}
	}
}
