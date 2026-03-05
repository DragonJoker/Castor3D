/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslAABB_H___
#define ___C3D_GlslAABB_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace c3d::shader
{
	struct Plane
		: public sdw::StructInstanceHelperT< "C3D_Plane"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "normal" >
			, sdw::FloatField< "distance" > >
	{
		SDW_DeclStructInstance( C3D_INL_API, Plane );

		Plane( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		C3D_API Plane( sdw::Vec3 const & normal
			, sdw::Float const & distance
			, bool enabled = true );

		C3D_API void set( sdw::Vec3 const & p1
			, sdw::Vec3 const & p2
			, sdw::Vec3 const & p3 );
		C3D_API sdw::RetFloat distance( sdw::Vec3 const & p );

		auto normal()const { return getMember< "normal" >(); }
		auto distance()const { return getMember< "distance" >(); }
	};

	struct Cone
		: public sdw::StructInstanceHelperT< "C3D_Cone"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "apex" >
			, sdw::FloatField< "range" >
			, sdw::Vec3Field< "direction" >
			, sdw::FloatField< "apertureCos" >
			, sdw::FloatField< "apertureSin" >
			, sdw::FloatField< "apertureTan" > >
	{
		SDW_DeclStructInstance( C3D_INL_API, Cone );

		Cone( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		C3D_API Cone( sdw::Vec3 const & apex
			, sdw::Vec3 const & direction
			, sdw::Float const & range
			, sdw::Float const & apertureCos
			, sdw::Float const & apertureSin
			, sdw::Float const & apertureTan
			, bool enabled = true );

		auto apex()const { return getMember< "apex" >(); }
		auto range()const { return getMember< "range" >(); }
		auto direction()const { return getMember< "direction" >(); }
		auto apertureCos()const { return getMember< "apertureCos" >(); }
		auto apertureSin()const { return getMember< "apertureSin" >(); }
		auto apertureTan()const { return getMember< "apertureTan" >(); }
	};

	struct AABB
		: public sdw::StructInstanceHelperT< "C3D_AABB"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec4Field< "bmin" >
			, sdw::Vec4Field< "bmax" > >
	{
		SDW_DeclStructInstance( C3D_INL_API, AABB );

		AABB( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		C3D_API AABB(sdw::Vec4 const & min
			, sdw::Vec4 const & max
			, bool enabled = true );
		C3D_API AABB( sdw::Vec3 const & position
			, sdw::Float const & range
			, bool enabled = true );

		C3D_API void set(sdw::Vec4 const & min
			, sdw::Vec4 const & max );
		C3D_API void set( sdw::Vec3 const & position
			, sdw::Float const & range );
		C3D_API sdw::RetBoolean intersectAABB( AABB const & rhs )const;
		C3D_API sdw::RetBoolean intersectAABBCoarse( AABB const & rhs )const;
		C3D_API sdw::RetBoolean intersectSphere( sdw::Vec4 const & rhs )const;
		C3D_API sdw::RetBoolean intersectCone( Cone const & rhs )const;
		C3D_API sdw::RetVec3 getPositiveVertex( sdw::Vec3 const & normal )const;

		auto min()const { return getMember< "bmin" >(); }
		auto max()const { return getMember< "bmax" >(); }

	private:
		mutable sdw::Function< sdw::Boolean, InAABB, InAABB > m_intersectAABBCoarse;
		mutable sdw::Function< sdw::Boolean, InAABB, sdw::InVec4 > m_intersectSphere;
		mutable sdw::Function< sdw::Boolean, InAABB, InCone > m_intersectCone;
		mutable sdw::Function< sdw::Vec3, InAABB, sdw::InVec3 > m_getPositiveVertex;
	};
}

#endif
