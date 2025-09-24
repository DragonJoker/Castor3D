/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectIds_H___
#define ___C3D_ObjectIds_H___

#include "SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>

namespace c3d::shader
{
	struct ObjectIds
		: public sdw::StructInstanceHelperT< "C3D_ObjectIdsData"
		, sdw::type::MemoryLayout::eStd430
		, sdw::UIntField< "nodeId" >
		, sdw::UIntField< "morphingId" >
		, sdw::UIntField< "skinningId" >
		, sdw::UIntField< "pad" > >
	{
		SDW_DeclStructInstance( C3D_API, ObjectIds );

		ObjectIds( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		auto nodeId()const { return getMember< "nodeId" >(); }
		auto morphingId()const { return getMember< "morphingId" >(); }
		auto skinningId()const { return getMember< "skinningId" >(); }
	};
}

#endif
