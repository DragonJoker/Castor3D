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
		, diffuse{ getMember< "diffuse" >() }
		, specular{ getMember< "specular" >() }
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

		specular *= specular;
		dielectric *= attenuation;
		metal *= attenuation;
		coating *= attenuation;
		sheen.rgb() *= attenuation;
	}

	void DirectLighting::registerDebug( DebugOutputCategory const & debugOutput )const
	{
		debugOutput.registerOutput( cuT( "Diffuse" ), diffuse );
		debugOutput.registerOutput( cuT( "Specular" ), specular );
		debugOutput.registerOutput( cuT( "Dielectric BRDF" ), dielectric );
		debugOutput.registerOutput( cuT( "Metal BRDF" ), metal );
		debugOutput.registerOutput( cuT( "Scattering" ), scattering );
		debugOutput.registerOutput( cuT( "Coating" ), coating );
		debugOutput.registerOutput( cuT( "Sheen" ), sheen.xyz() );
		debugOutput.registerOutput( cuT( "Sheen Scale" ), sheen.w() );
	}

	DirectLighting & DirectLighting::operator+=( DirectLighting const & rhs )
	{
		diffuse += max( vec3( 0.0_f ), rhs.diffuse );
		specular += max( vec3( 0.0_f ), rhs.specular );
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
		specular *= rhs;
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

	void IndirectLighting::registerDebug( DebugOutputCategory const & debugOutput )const
	{
		debugOutput.registerOutput( cuT( "Raw Diffuse" ), rawDiffuse );
		debugOutput.registerOutput( cuT( "Diffuse Colour" ), diffuseColour );
		debugOutput.registerOutput( cuT( "Diffuse Blend" ), diffuseBlend );
		debugOutput.registerOutput( cuT( "Specular" ), specular );
		debugOutput.registerOutput( cuT( "Occlusion" ), occlusion );
		debugOutput.registerOutput( cuT( "Ambient" ), ambient );
	}

	sdw::expr::ExprList IndirectLighting::makeInit()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
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
		, diffuseReflection{ getMember< "diffuseReflection" >() }
		, specularReflection{ getMember< "specularReflection" >() }
		, diffuseTransmission{ getMember< "diffuseTransmission" >() }
		, specularTransmission{ getMember< "specularTransmission" >() }
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

	void ReflectionRefraction::registerDebug( DebugOutputCategory const & debugOutput )const
	{
		debugOutput.registerOutput( cuT( "Background Diffuse Reflection" ), diffuseReflection );
		debugOutput.registerOutput( cuT( "Background Specular Reflection" ), specularReflection );
		debugOutput.registerOutput( cuT( "Background Diffuse Transmission" ), diffuseTransmission );
		debugOutput.registerOutput( cuT( "Background Specular Transmission" ), specularTransmission );
		debugOutput.registerOutput( cuT( "Background Coating BRDF" ), coating );
		debugOutput.registerOutput( cuT( "Background Sheen" ), sheen.xyz() );
		debugOutput.registerOutput( cuT( "Background Sheen Scale" ), sheen.w() );
	}

	sdw::expr::ExprList ReflectionRefraction::makeInit()
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
}
