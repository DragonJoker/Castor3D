/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBaseIO_H___
#define ___C3D_GlslBaseIO_H___

#include "SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

namespace castor3d::shader
{
	template< typename ColTypeT, sdw::var::Flag FlagT >
	using ColourStructT = sdw::IOStructInstanceHelperT< FlagT
		, "C3D_Colour"
		, sdw::IOStructFieldT< ColTypeT, "colour", 0u > >;
	template< typename PosTypeT, sdw::var::Flag FlagT >
	using PositionStructT = sdw::IOStructInstanceHelperT< FlagT
		, "C3D_Position"
		, sdw::IOStructFieldT< PosTypeT, "position", 0u > >;
	template< typename PosTypeT, sdw::var::Flag FlagT >
	using PosUvStructT = sdw::IOStructInstanceHelperT< FlagT
		, "C3D_PosUv"
		, sdw::IOStructFieldT< PosTypeT, "position", 0u >
		, sdw::IOStructFieldT< sdw::Vec2, "uv", 1u > >;
	template< typename UvTypeT, sdw::var::Flag FlagT >
	using UvStructT = sdw::IOStructInstanceHelperT< FlagT
		, "C3D_Uv"
		, sdw::IOStructFieldT< UvTypeT, "uv", 0u > >;

	template< typename ColTypeT, sdw::var::Flag FlagT >
	struct ColourT
		: public ColourStructT< ColTypeT, FlagT >
	{
		ColourT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: ColourStructT< ColTypeT, FlagT >{ writer, std::move( expr ), enabled }
		{
		}

		auto colour()const { return this->template getMember< "colour" >(); }
	};

	template< typename PosTypeT, sdw::var::Flag FlagT >
	struct PositionT
		: public PositionStructT< PosTypeT, FlagT >
	{
		PositionT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: PositionStructT< PosTypeT, FlagT >{ writer, std::move( expr ), enabled }
		{
		}

		auto position()const { return this->template getMember< "position" >(); }
	};

	template< typename PosTypeT, sdw::var::Flag FlagT >
	struct PosUvT
		: public PosUvStructT< PosTypeT, FlagT >
	{
		PosUvT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: PosUvStructT< PosTypeT, FlagT >{ writer, std::move( expr ), enabled }
		{
		}

		auto position()const { return this->template getMember< "position" >(); }
		auto uv()const { return this->template getMember< "uv" >(); }
	};

	template< typename UvTypeT, sdw::var::Flag FlagT >
	struct UvT
		: public UvStructT< UvTypeT, FlagT >
	{
		UvT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: UvStructT< UvTypeT, FlagT >{ writer, std::move( expr ), enabled }
		{
		}

		auto uv()const { return this->template getMember< "uv" >(); }
	};
}

#endif
