/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslAABB_H___
#define ___C3D_GlslAABB_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d::shader
{
	struct Plane
		: public sdw::StructInstanceHelperT< "C3D_Plane"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "normal" >
			, sdw::FloatField< "distance" > >
	{
		Plane( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API Plane( sdw::Vec3 const normal
			, sdw::Float const distance
			, bool enabled = true );

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
		Cone( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API Cone( sdw::Vec3 const apex
			, sdw::Vec3 const direction
			, sdw::Float const range
			, sdw::Float const apertureCos
			, sdw::Float const apertureSin
			, sdw::Float const apertureTan
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
		AABB( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API AABB(sdw::Vec4 const min
			, sdw::Vec4 const max
			, bool enabled = true );
		C3D_API AABB( sdw::Vec3 const position
			, sdw::Float range
			, bool enabled = true );

		auto min()const { return getMember< "bmin" >(); }
		auto max()const { return getMember< "bmax" >(); }
	};
}

#endif
