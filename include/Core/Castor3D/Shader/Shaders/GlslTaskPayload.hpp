/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslTaskPayload_H___
#define ___C3D_GlslTaskPayload_H___

#include "SdwModule.hpp"

#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		template< sdw::var::Flag FlagT >
		struct PayloadT
			: public sdw::StructInstance
		{
			PayloadT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true );

			SDW_DeclStructInstance( , PayloadT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache );
			static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

			sdw::Array< sdw::UInt > meshletIndices;
		};
	}
}

#include "GlslTaskPayload.inl"

#endif
