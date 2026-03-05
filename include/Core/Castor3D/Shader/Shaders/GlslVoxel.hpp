/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslVoxel_H___
#define ___C3D_GlslVoxel_H___

#include "SdwModule.hpp"

#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace c3d::shader
{
	struct Voxel
		: public sdw::StructInstanceHelperT< "C3D_Voxel"
			, sdw::type::MemoryLayout::eStd430
			, sdw::UIntField< "colorMask" >
			, sdw::UIntField< "normalMask" > >
	{
		SDW_DeclStructInstance( C3D_INL_API, Voxel );

		Voxel( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		auto colorMask()const { return getMember< "colorMask" >(); }
		auto normalMask()const { return getMember< "normalMask" >(); }

		static uint32_t constexpr DataSize = 8u;

	private:
		using StructInstanceHelperT::getMember;
		using StructInstanceHelperT::getMemberArray;
	};

	Writer_Parameter( Voxel );
}

#endif
