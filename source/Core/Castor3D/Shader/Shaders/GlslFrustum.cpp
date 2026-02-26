#include "Castor3D/Shader/Shaders/GlslFrustum.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace c3d::shader
{
	//*********************************************************************************************

	namespace frustum
	{
		static u32 constexpr PlaneNear = 0u;
		static u32 constexpr PlaneFar = 1u;
		static u32 constexpr PlaneLeft = 2u;
		static u32 constexpr PlaneRight = 3u;
		static u32 constexpr PlaneTop = 4u;
		static u32 constexpr PlaneBottom = 5u;
	}

	void Frustum::set( sdw::Vec3 const & pnearTopLeft, sdw::Vec3 const & pnearTopRight,
		sdw::Vec3 const & pnearBottomLeft, sdw::Vec3 const & pnearBottomRight,
		sdw::Vec3 const & pfarTopLeft, sdw::Vec3 const & pfarTopRight,
		sdw::Vec3 const & pfarBottomLeft, sdw::Vec3 const & pfarBottomRight )
	{
		nearTopLeft() = sdw::vec4( pnearTopLeft, 0.0f );
		nearTopRight() = sdw::vec4( pnearTopRight, 0.0f );
		nearBottomLeft() = sdw::vec4( pnearBottomLeft, 0.0f );
		nearBottomRight() = sdw::vec4( pnearBottomRight, 0.0f );
		farTopLeft() = sdw::vec4( pfarTopLeft, 0.0f );
		farTopRight() = sdw::vec4( pfarTopRight, 0.0f );
		farBottomLeft() = sdw::vec4( pfarBottomLeft, 0.0f );
		farBottomRight() = sdw::vec4( pfarBottomRight, 0.0f );
		aabb().set( min( nearTopLeft(), farBottomRight() ), max( nearTopLeft(), farBottomRight() ) );

		planes()[frustum::PlaneNear].set( pnearBottomLeft, pnearTopLeft, pnearBottomRight );
		planes()[frustum::PlaneFar].set( pfarBottomRight, pfarTopRight, pfarBottomLeft );
		planes()[frustum::PlaneLeft].set( pnearBottomLeft, pfarBottomLeft, pnearTopLeft );
		planes()[frustum::PlaneRight].set( pnearBottomRight, pnearTopRight, pfarBottomRight );
		planes()[frustum::PlaneTop].set( pnearTopRight, pnearTopLeft, pfarTopRight );
		planes()[frustum::PlaneBottom].set( pnearBottomLeft, pnearBottomRight, pfarBottomLeft );
	}

	sdw::RetBoolean Frustum::intersectAABB( AABB const & rhs )const
	{
		return intersectAABBCoarse( rhs )
			&& intersectAABBFine( rhs );
	}

	sdw::RetBoolean Frustum::intersectAABBCoarse( AABB const & rhs )const
	{
		return aabb().intersectAABBCoarse( rhs );
	}

	sdw::RetBoolean Frustum::intersectAABBFine( AABB const & rhs )const
	{
		if ( !m_intersectAABBFine )
		{
			auto & writer = sdw::findWriterMandat( *this );
			m_intersectAABBFine = writer.implementFunction< sdw::Boolean >( "c3d_intersectAABBFine"
				, [&writer]( Frustum const & frustum
					, AABB const & aabb )
				{
					//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
					auto result = writer.declLocale( "result"
						, 1_b );
					for ( u32 i = 0u; i < 6u; ++i )
						result = result && frustum.planes()[i].distance( aabb.getPositiveVertex( frustum.planes()[i].normal() ) ) >= 0.0f;

					writer.returnStmt( result );
				}
				, InFrustum{ writer, "frustum" }
				, InAABB{ writer, "aabb" } );
		}
		return m_intersectAABBFine( *this, rhs );
	}

	sdw::RetBoolean Frustum::intersectSphere( sdw::Vec4 const & rhs )const
	{
		return intersectSphereCoarse( rhs )
			&& intersectSphereFine( rhs );
	}

	sdw::RetBoolean Frustum::intersectSphereCoarse( sdw::Vec4 const & rhs )const
	{
		return aabb().intersectSphere( rhs );
	}

	sdw::RetBoolean Frustum::intersectSphereFine( sdw::Vec4 const & rhs )const
	{
		if ( !m_intersectSphereFine )
		{
			auto & writer = sdw::findWriterMandat( *this );
			m_intersectSphereFine = writer.implementFunction< sdw::Boolean >( "c3d_intersectSphereFine"
				, [&writer]( Frustum const & frustum
					, sdw::Vec4 const & sphere )
				{
					//see http://www.lighthouse3d.com/tutorials/view-frustum-culling/
					auto result = writer.declLocale( "result"
						, 1_b );
					for ( u32 i = 0u; i < 6u; ++i )
						result = result && frustum.planes()[i].distance( sphere.xyz() ) >= -sphere.w();

					writer.returnStmt( result );
				}
				, InFrustum{ writer, "frustum" }
				, sdw::InVec4{ writer, "sphere" } );
		}
		return m_intersectSphereFine( *this, rhs );
	}

	sdw::RetBoolean Frustum::intersectCone( Cone const & rhs )const
	{
		return aabb().intersectCone( rhs );
	}

	//*********************************************************************************************
}
