#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"

#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	void DirectLighting::registerDebug( DebugOutput & debugOutput
		, castor::String const & category )const
	{
		debugOutput.registerOutput( category, cuT( "Diffuse" ), diffuse() );
		debugOutput.registerOutput( category, cuT( "Specular" ), specular() );
		debugOutput.registerOutput( category, cuT( "Scattering" ), scattering() );
		debugOutput.registerOutput( category, cuT( "Coating" ), coating() );
		debugOutput.registerOutput( category, cuT( "Sheen" ), sheen().xyz() );
		debugOutput.registerOutput( category, cuT( "Sheen Scale" ), sheen().w() );
	}

	sdw::expr::ExprList DirectLighting::makeInit()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec4( 0.0_f ) ) );
		return result;
	}

	//*********************************************************************************************

	sdw::expr::ExprList IndirectLighting::makeInit()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec4( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( 1.0_f ) );
		return result;
	}

	//*********************************************************************************************

	void ReflectionRefraction::registerDebug( DebugOutput & debugOutput
		, castor::String const & category )const
	{
		debugOutput.registerOutput( category, cuT( "Refl. Diffuse" ), reflDiffuse );
		debugOutput.registerOutput( category, cuT( "Refl. Specular" ), reflSpecular );
		debugOutput.registerOutput( category, cuT( "Refl. Coating" ), reflCoating );
		debugOutput.registerOutput( category, cuT( "Refl. Sheen" ), reflSheen.xyz() );
		debugOutput.registerOutput( category, cuT( "Refl. Sheen Scale" ), reflSheen.w() );
	}

	sdw::expr::ExprList ReflectionRefraction::makeInit()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec4( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		return result;
	}

	//*********************************************************************************************
}
