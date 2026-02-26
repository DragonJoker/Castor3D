/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslFrustum_H___
#define ___C3D_GlslFrustum_H___

#include "Castor3D/Shader/Shaders/GlslAABB.hpp"

namespace c3d::shader
{
	struct Frustum
		: public sdw::StructInstanceHelperT< "C3D_Frustum"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec4Field< "nearTopLeft" >
			, sdw::Vec4Field< "nearTopRight" >
			, sdw::Vec4Field< "nearBottomLeft" >
			, sdw::Vec4Field< "nearBottomRight" >
			, sdw::Vec4Field< "farTopLeft" >
			, sdw::Vec4Field< "farTopRight" >
			, sdw::Vec4Field< "farBottomLeft" >
			, sdw::Vec4Field< "farBottomRight" >
			, sdw::StructFieldArrayT< Plane, "planes", 6u >
			, sdw::StructFieldT< AABB, "aabb" > >
	{
		SDW_DeclStructInstance( C3D_API, Frustum );

		Frustum( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		C3D_API void set( sdw::Vec3 const & nearTopLeft, sdw::Vec3 const & nearTopRight,
			sdw::Vec3 const & nearBottomLeft, sdw::Vec3 const & nearBottomRight,
			sdw::Vec3 const & farTopLeft, sdw::Vec3 const & farTopRight,
			sdw::Vec3 const & farBottomLeft, sdw::Vec3 const & farBottomRight );

		sdw::RetBoolean intersectAABB( AABB const & rhs )const;
		sdw::RetBoolean intersectAABBCoarse( AABB const & rhs )const;
		sdw::RetBoolean intersectAABBFine( AABB const & rhs )const;
		sdw::RetBoolean intersectSphere( sdw::Vec4 const & rhs )const;
		sdw::RetBoolean intersectSphereCoarse( sdw::Vec4 const & rhs )const;
		sdw::RetBoolean intersectSphereFine( sdw::Vec4 const & rhs )const;
		sdw::RetBoolean intersectCone( Cone const & rhs )const;

		auto nearTopLeft()const { return getMember< "nearTopLeft" >(); }
		auto nearTopRight()const { return getMember< "nearTopRight" >(); }
		auto nearBottomLeft()const { return getMember< "nearBottomLeft" >(); }
		auto nearBottomRight()const { return getMember< "nearBottomRight" >(); }
		auto farTopLeft()const { return getMember< "farTopLeft" >(); }
		auto farTopRight()const { return getMember< "farTopRight" >(); }
		auto farBottomLeft()const { return getMember< "farBottomLeft" >(); }
		auto farBottomRight()const { return getMember< "farBottomRight" >(); }
		auto aabb()const { return getMember< "aabb" >(); }
		auto planes()const { return getMember< "planes" >(); }

	private:
		mutable sdw::Function< sdw::Boolean, InFrustum, InAABB > m_intersectAABBFine;
		mutable sdw::Function< sdw::Boolean, InFrustum, sdw::InVec4 > m_intersectSphereFine;
	};
}

#endif
