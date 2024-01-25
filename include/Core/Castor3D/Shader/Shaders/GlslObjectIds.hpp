/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectIds_H___
#define ___C3D_ObjectIds_H___

#include "SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>

namespace castor3d::shader
{
	struct ObjectIds
		: public sdw::StructInstanceHelperT< "C3D_ObjectIdsData"
		, sdw::type::MemoryLayout::eStd430
		, sdw::UIntField< "nodeId" >
		, sdw::UIntField< "morphingId" >
		, sdw::UIntField< "skinningId" >
		, sdw::UIntField< "pad" > >
	{
		ObjectIds( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}
		
		auto nodeId()const { return getMember< "nodeId" >(); }
		auto morphingId()const { return getMember< "morphingId" >(); }
		auto skinningId()const { return getMember< "skinningId" >(); }
	};
}

#endif
