/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslOutputs_H___
#define ___C3D_GlslOutputs_H___

#include "SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d::shader
{
	template< sdw::var::Flag FlagT >
	struct PrepassOutputT
		: public sdw::StructInstance
	{
		PrepassOutputT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , PrepassOutputT );

		static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, PipelineFlags const & flags );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

		sdw::Vec4 depthObj;
		sdw::UVec2 visibility;
		sdw::Vec2 velocity;
		sdw::Vec4 nmlOcc;
	};

	template< sdw::var::Flag FlagT >
	struct ShadowsOutputT
		: public sdw::StructInstance
	{
		ShadowsOutputT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , ShadowsOutputT );

		static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, bool needsVsm
			, bool needsRsm );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

		sdw::Float linear;
		sdw::Vec2 variance;
		sdw::Vec4 normal;
		sdw::Vec4 position;
		sdw::Vec4 flux;
	};
}

#include "Castor3D/Shader/Shaders/GlslOutputs.inl"

#endif
