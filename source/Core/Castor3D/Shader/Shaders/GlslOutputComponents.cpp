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
		, specular{ getMember< "specular" >() }
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

		specular *= attenuation;
		coating *= attenuation;
		sheen.x() *= attenuation;
	}

	void DirectLighting::sheenAlbedoScale( BlendComponents const & components
		, bool withDiffuse )
	{
		auto & writer = *getWriter();
		auto maxSheenColour = writer.declLocale( "maxSheenColour"
			, max( components.sheenColour.r(), max( components.sheenColour.g(), components.sheenColour.b() ) ) );

		IF( writer, maxSheenColour != 0.0_f )
		{
			auto albedoSheenScaling = writer.declLocale( "albedoSheenScaling"
				, ( 1.0_f - sheen.w() * maxSheenColour ) );
			diffuse *= albedoSheenScaling;
			specular *= albedoSheenScaling;
		}
		FI
	}

	void DirectLighting::registerDebug( DebugOutput & debugOutput
		, castor::String const & category )const
	{
		debugOutput.registerOutput( category, cuT( "Diffuse" ), diffuse );
		debugOutput.registerOutput( category, cuT( "Specular" ), specular );
		debugOutput.registerOutput( category, cuT( "Scattering" ), scattering );
		debugOutput.registerOutput( category, cuT( "Coating" ), coating );
		debugOutput.registerOutput( category, cuT( "Sheen" ), sheen.xyz() );
		debugOutput.registerOutput( category, cuT( "Sheen Scale" ), sheen.w() );
	}

	DirectLighting & DirectLighting::operator+=( DirectLighting const & rhs )
	{
		diffuse += max( vec3( 0.0_f ), rhs.diffuse );
		specular += max( vec3( 0.0_f ), rhs.specular );
		scattering += max( vec3( 0.0_f ), rhs.scattering );
		coating += max( vec3( 0.0_f ), rhs.coating );
		sheen += max( vec4( 0.0_f ), rhs.sheen );

		return *this;
	}

	DirectLighting & DirectLighting::operator*=( sdw::Float const & rhs )
	{
		diffuse *= rhs;
		specular *= rhs;
		scattering *= rhs;
		coating *= rhs;
		sheen.x() = rhs;

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
		, reflDiffuse { getMember< "reflDiffuse" >() }
		, reflSpecular { getMember< "reflSpecular" >() }
		, reflCoating { getMember< "reflCoating" >() }
		, reflSheen { getMember< "reflSheen" >() }
		, refrDiffuse{ getMember< "refrDiffuse" >() }
		, refrSpecular{ getMember< "refrSpecular" >() }
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
		debugOutput.registerOutput( category, cuT( "Refl. Diffuse" ), reflDiffuse );
		debugOutput.registerOutput( category, cuT( "Refl. Specular" ), reflSpecular );
		debugOutput.registerOutput( category, cuT( "Refl. Coating" ), reflCoating );
		debugOutput.registerOutput( category, cuT( "Refl. Sheen" ), reflSheen.xyz() );
		debugOutput.registerOutput( category, cuT( "Refl. Sheen Scale" ), reflSheen.w() );
		debugOutput.registerOutput( category, cuT( "Refr. Diffuse" ), refrDiffuse );
		debugOutput.registerOutput( category, cuT( "Refr. Specular" ), refrSpecular );
	}

	sdw::expr::ExprList ReflectionRefraction::makeInit()
	{
		sdw::expr::ExprList result;
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec4( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		result.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		return result;
	}

	//*********************************************************************************************
}
