#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	DirectLighting::DirectLighting( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		, ambient{ getMember< "ambient" >() }
		, diffuse{ getMember< "diffuse" >() }
		, dielectric{ getMember< "dielectric" >() }
		, metal{ getMember< "metal" >() }
		, scattering{ getMember< "scattering" >() }
		, coating{ getMember< "coating" >() }
		, sheen{ getMember< "sheen" >() }
	{
	}

	DirectLighting::DirectLighting( sdw::ShaderWriter & writer )
		: DirectLighting{ writer
		, sdw::makeAggrInit( DirectLighting::makeType( sdw::getTypesCache( writer ) ), makeInit() )
		, true }
	{
	}

	void DirectLighting::attenuate( sdw::Float const attenuation
		, bool withScattering
		, bool withDiffuse )
	{
		if ( withDiffuse )
		{
			diffuse *= attenuation;
		}

		if ( withScattering )
		{
			scattering *= attenuation;
		}

		dielectric *= attenuation;
		metal *= attenuation;
		coating *= attenuation;
		sheen.rgb() *= attenuation;
	}

	void DirectLighting::registerDebug( DebugOutput & debugOutput
		, castor::String const & category )const
	{
		debugOutput.registerOutput( category, cuT( "Ambient" ), ambient );
		debugOutput.registerOutput( category, cuT( "Diffuse" ), diffuse );
		debugOutput.registerOutput( category, cuT( "Dielectric BRDF" ), dielectric );
		debugOutput.registerOutput( category, cuT( "Metal BRDF" ), metal );
		debugOutput.registerOutput( category, cuT( "Scattering" ), scattering );
		debugOutput.registerOutput( category, cuT( "Coating" ), coating );
		debugOutput.registerOutput( category, cuT( "Sheen" ), sheen.xyz() );
		debugOutput.registerOutput( category, cuT( "Sheen Scale" ), sheen.w() );
	}

	DirectLighting & DirectLighting::operator+=( DirectLighting const & rhs )
	{
		diffuse += max( vec3( 0.0_f ), rhs.diffuse );
		dielectric += max( vec3( 0.0_f ), rhs.dielectric );
		metal += max( vec3( 0.0_f ), rhs.metal );
		scattering += max( vec3( 0.0_f ), rhs.scattering );
		coating += max( vec3( 0.0_f ), rhs.coating );
		sheen += max( vec4( 0.0_f ), rhs.sheen );

		return *this;
	}

	DirectLighting & DirectLighting::operator*=( sdw::Float const & rhs )
	{
		diffuse *= rhs;
		dielectric *= rhs;
		metal *= rhs;
		scattering *= rhs;
		coating *= rhs;
		sheen.rgb() *= rhs;

		return *this;
	}

	sdw::expr::ExprList DirectLighting::makeInit()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec4( 0.0_f ) ) );
		return result;
	}

	//*********************************************************************************************

	IndirectLighting::IndirectLighting( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		, ambient{ getMember< "ambient" >() }
		, rawDiffuse{ getMember< "diffuse" >() }
		, specular{ getMember< "specular" >() }
		, occlusion{ getMember< "occlusion" >() }
		, diffuseColour{ rawDiffuse.rgb() }
		, diffuseBlend{ rawDiffuse.a() }
	{
	}

	IndirectLighting::IndirectLighting( sdw::ShaderWriter & writer )
		: IndirectLighting{ writer
			, sdw::makeAggrInit( IndirectLighting::makeType( sdw::getTypesCache( writer ) ), makeInit() )
			, true }
	{
	}

	void IndirectLighting::registerDebug( DebugOutput & debugOutput
		, castor::String const & category )const
	{
		debugOutput.registerOutput( category, cuT( "Raw Diffuse" ), rawDiffuse );
		debugOutput.registerOutput( category, cuT( "Diffuse Colour" ), diffuseColour );
		debugOutput.registerOutput( category, cuT( "Diffuse Blend" ), diffuseBlend );
		debugOutput.registerOutput( category, cuT( "Specular" ), specular );
		debugOutput.registerOutput( category, cuT( "Occlusion" ), occlusion );
		debugOutput.registerOutput( category, cuT( "Ambient" ), ambient );
	}

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

	ReflectionRefraction::ReflectionRefraction( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		, diffuse{ getMember< "diffuse" >() }
		, dielectric{ getMember< "dielectric" >() }
		, metal{ getMember< "metal" >() }
		, coating{ getMember< "coating" >() }
		, sheen{ getMember< "sheen" >() }
	{
	}

	ReflectionRefraction::ReflectionRefraction( sdw::ShaderWriter & writer )
		: ReflectionRefraction{ writer
			, sdw::makeAggrInit( ReflectionRefraction::makeType( sdw::getTypesCache( writer ) ), makeInit() )
			, true }
	{
	}

	void ReflectionRefraction::registerDebug( DebugOutput & debugOutput
		, castor::String const & category )const
	{
		debugOutput.registerOutput( category, cuT( "Diffuse" ), diffuse );
		debugOutput.registerOutput( category, cuT( "Dielectric BRDF" ), dielectric );
		debugOutput.registerOutput( category, cuT( "Metal BRDF" ), metal );
		debugOutput.registerOutput( category, cuT( "Coating BRDF" ), coating );
		debugOutput.registerOutput( category, cuT( "Refl. Sheen" ), sheen.xyz() );
		debugOutput.registerOutput( category, cuT( "Refl. Sheen Scale" ), sheen.w() );
	}

	sdw::expr::ExprList ReflectionRefraction::makeInit()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec4( 0.0_f ) ) );
		return result;
	}

	//*********************************************************************************************
}
