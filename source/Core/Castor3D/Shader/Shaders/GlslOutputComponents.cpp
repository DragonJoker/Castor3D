#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	sdw::expr::ExprList DirectLighting::makeZero()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec2( 0.0_f ) ) );
		return result;
	}

	//*********************************************************************************************

	sdw::expr::ExprList IndirectLighting::makeZero()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec4( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( 1.0_f ) );
		return result;
	}

	//*********************************************************************************************
}
