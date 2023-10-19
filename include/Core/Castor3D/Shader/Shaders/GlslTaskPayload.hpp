/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslTaskPayload_H___
#define ___C3D_GlslTaskPayload_H___

#include "SdwModule.hpp"

#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

namespace castor3d
{
	namespace shader
	{
		template< sdw::var::Flag FlagT >
		using PayloadStructT = sdw::IOStructInstanceHelperT< FlagT
			, "C3D_Payload"
			, sdw::IOUIntArrayField< "meshletIndices", ast::type::Struct::InvalidLocation, 32u > >;

		template< sdw::var::Flag FlagT >
		struct PayloadT
			: public PayloadStructT< FlagT >
		{
			PayloadT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: PayloadStructT< FlagT >{ writer, std::move( expr ), enabled }
			{
			}

			auto meshletIndices()const { return this->template getMember< "meshletIndices" >(); }
		};
	}
}

#endif
