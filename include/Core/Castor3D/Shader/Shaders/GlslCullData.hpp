/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslCullData_H___
#define ___C3D_GlslCullData_H___

#include "SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace c3d::shader
{
	struct CullData
		: public sdw::StructInstanceHelperT< "C3D_CullData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec4Field< "sphere" >
			, sdw::Vec4Field< "cone" > >
	{
		SDW_DeclStructInstance( C3D_INL_API, CullData );

		CullData( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, sphere{ StructInstanceHelperT::getMember< "sphere" >() }
			, cone{ StructInstanceHelperT::getMember< "cone" >() }
		{
		}

		sdw::Vec4 sphere;
		sdw::Vec4 cone;
	};

	Writer_Parameter( CullData );
}

#endif
