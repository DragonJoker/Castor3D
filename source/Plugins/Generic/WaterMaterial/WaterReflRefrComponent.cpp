#include "WaterMaterial/WaterReflRefrComponent.hpp"

#include "WaterMaterial/Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslPassShaders.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>

CU_ImplementSmartPtr( water, WaterReflRefrComponent )

namespace water
{
	using namespace castor3d;
	namespace c3d = castor3d::shader;

	//*********************************************************************************************

	void WaterReflRefrComponent::ReflRefrShader::computeReflRefr( c3d::ReflectionModel & reflections
		, c3d::BlendComponents & components
		, c3d::LightSurface const & lightSurface
		, sdw::Vec4 const & position
		, c3d::BackgroundModel & backgroundModel
		, sdw::CombinedImage2DRgba32 const & mippedScene
		, c3d::CameraData const & camera
		, c3d::DirectLighting & lighting
		, c3d::IndirectLighting & indirect
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 const & incident
		, sdw::UInt const & hasReflection
		, sdw::Float const & refractionRatio
		, c3d::ReflectionRefraction & output
		, c3d::DebugOutput & debugOutput )const
	{
		computeReflRefr( reflections
			, components
			, lightSurface
			, backgroundModel
			, camera
			, lighting
			, indirect
			, sceneUv
			, envMapIndex
			, incident
			, components.hasReflection
			, components.ior
			, output
			, debugOutput );
	}

	void WaterReflRefrComponent::ReflRefrShader::computeReflRefr( c3d::ReflectionModel & reflections
		, c3d::BlendComponents & components
		, c3d::LightSurface const & lightSurface
		, c3d::BackgroundModel & backgroundModel
		, c3d::CameraData const & camera
		, c3d::DirectLighting & lighting
		, c3d::IndirectLighting & indirect
		, sdw::Vec2 const & sceneUv
		, sdw::UInt const & envMapIndex
		, sdw::Vec3 const & incident
		, sdw::UInt const & hasReflection
		, sdw::Float const & refractionRatio
		, c3d::ReflectionRefraction & output
		, c3d::DebugOutput & debugOutput )const
	{
		auto & writer = *components.getWriter();

		if ( !writer.hasGlobalVariable( "c3d_mapDepthObj" ) )
		{
			return;
		}

		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Water" ) );
		auto & utils = reflections.getUtils();
		auto mapDepthObj = writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapDepthObj" );
		auto mapNormals = writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapNormals" );
		auto mapColours = writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapScene" );
		auto ssrStepSize = components.getMember< sdw::Float >( "ssrStepSize", true );
		auto ssrForwardStepsCount = components.getMember< sdw::UInt32 >( "ssrForwardStepsCount", true );
		auto ssrBackwardStepsCount = components.getMember< sdw::UInt32 >( "ssrBackwardStepsCount", true );
		auto ssrDepthMult = components.getMember< sdw::Float >( "ssrDepthMult", true );
		auto refractionDistortionFactor = components.getMember< sdw::Float >( "refractionDistortionFactor", true );
		auto depthSofteningDistance = components.getMember< sdw::Float >( "depthSofteningDistance", true );
		auto refractionHeightFactor = components.getMember< sdw::Float >( "refractionHeightFactor", true );
		auto refractionDistanceFactor = components.getMember< sdw::Float >( "refractionDistanceFactor", true );

		//  Retrieve non distorted scene colour.
		auto hdrCoords = writer.declLocale( "hdrCoords"
			, sceneUv / vec2( camera.renderSize() ) );
		auto sceneDepth = writer.declLocale( "sceneDepth"
			, mapDepthObj.lod( hdrCoords, 0.0_f ).r() );
		auto scenePosition = writer.declLocale( "scenePosition"
			, camera.curProjToWorld( utils, hdrCoords, sceneDepth ) );

		// Reflections
		auto bgDiffuseReflection = writer.declLocale("bgDiffuseReflection"
			, vec3( 0.0_f ) );
		auto bgSpecularReflection = writer.declLocale("bgSpecularReflection"
			, vec3( 0.0_f ) );
		reflections.computeReflection( components
			, lightSurface
			, backgroundModel
			, envMapIndex
			, components.hasReflection
			, bgDiffuseReflection
			, bgSpecularReflection
			, debugOutputBlock );
		auto backgroundReflection = writer.declLocale( "backgroundReflection"
			, bgDiffuseReflection + bgSpecularReflection );
		debugOutputBlock.registerOutput( cuT( "Background Reflection" ), backgroundReflection );
		auto ssrResult = writer.declLocale( "ssrResult"
			, reflections.computeScreenSpace( camera
				, lightSurface.viewPosition().value()
				, components.getRawNormal()
				, hdrCoords
				, vec4( ssrStepSize
					, writer.cast< sdw::Float >( ssrForwardStepsCount )
					, writer.cast< sdw::Float >( ssrBackwardStepsCount )
					, ssrDepthMult )
				, mapDepthObj
				, mapNormals
				, mapColours
				, debugOutputBlock ) );
		debugOutputBlock.registerOutput( cuT( "SSR Result" ), ssrResult.xyz() );
		debugOutputBlock.registerOutput( cuT( "SSR Factor" ), ssrResult.w() );
		auto reflectionResult = writer.declLocale( "reflectionResult"
			, mix( backgroundReflection, ssrResult.xyz(), ssrResult.www() ) );
		debugOutputBlock.registerOutput( cuT( "Reflection Result" ), reflectionResult );

		// Refraction
		// Wobbly refractions
		auto heightFactor = writer.declLocale( "heightFactor"
			, refractionHeightFactor * ( camera.farPlane() - camera.nearPlane() ) );
		auto distanceFactor = writer.declLocale( "distanceFactor"
			, refractionDistanceFactor * ( camera.farPlane() - camera.nearPlane() ) );
		auto distortedTexCoord = writer.declLocale( "distortedTexCoord"
			, fma( ( components.getRawNormal().xz() + components.getRawNormal().xy() ) * 0.5_f
					, vec2( ( ( components.hasMember( "mdlPosition" ) && components.hasMember( "waterDensity" ) )
						? refractionDistortionFactor * utils.saturate( length( scenePosition - lightSurface.worldPosition().value().xyz() ) * 0.5_f )
						: refractionDistortionFactor ) )
					, hdrCoords ) );
		auto distortedDepth = writer.declLocale( "distortedDepth"
			, mapDepthObj.lod( distortedTexCoord, 0.0_f ).r() );
		auto distortedPosition = writer.declLocale( "distortedPosition"
			, camera.curProjToWorld( utils, distortedTexCoord, distortedDepth ) );
		auto refractionTexCoord = writer.declLocale( "refractionTexCoord"
			, writer.ternary( distortedPosition.y() < lightSurface.worldPosition().value().y(), distortedTexCoord, hdrCoords ) );
		auto specularTransmission = writer.declLocale( "specularTransmission"
			, mapColours.lod( refractionTexCoord, 0.0_f ).rgb() );
		debugOutputBlock.registerOutput( cuT( "Raw Refraction" ), specularTransmission );
		auto waterTransmission = writer.declLocale( "waterTransmission"
			, components.baseColour * ( indirect.ambient + indirect.diffuseColour ) );
		debugOutputBlock.registerOutput( cuT( "Raw Transmission" ), waterTransmission );

		if ( components.hasMember( "mdlPosition" )
			&& components.hasMember( "waterDensity" ) )
		{
			auto mdlPosition = components.getMember< sdw::Vec3 >( "mdlPosition" );
			auto waterDensity = components.getMember< sdw::Float >( "waterDensity" );
			auto lightAbsorbtion = writer.declLocale( "lightAbsorbtion"
				, specularTransmission * ( 1.0_f - utils.saturate( sdw::log( mdlPosition.y() - distortedPosition.y() ) * waterDensity ) ) );
			debugOutputBlock.registerOutput( cuT( "Light Absorbtion" ), lightAbsorbtion );
			waterTransmission *= lightAbsorbtion;
			debugOutputBlock.registerOutput( cuT( "Absorbed Transmission" ), waterTransmission );
			specularTransmission *= waterTransmission;
			debugOutputBlock.registerOutput( cuT( "Transmitted Refraction" ), specularTransmission );
		}
		else
		{
			waterTransmission *= lighting.diffuse;
			debugOutputBlock.registerOutput( cuT( "Lit Transmission" ), waterTransmission );
			specularTransmission *= components.baseColour;
			debugOutputBlock.registerOutput( cuT( "Coloured Refraction" ), specularTransmission );
		}

		// Depth softening, to fade the alpha of the water where it meets the scene geometry by some predetermined distance. 
		auto depthSoftenedAlpha = writer.declLocale( "depthSoftenedAlpha"
			, clamp( distance( scenePosition, lightSurface.worldPosition().value().xyz() ) / depthSofteningDistance, 0.0_f, 1.0_f ) );
		debugOutputBlock.registerOutput( cuT( "Depth Softened Alpha" ), depthSoftenedAlpha );
		auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
			, writer.ternary( distortedPosition.y() < lightSurface.worldPosition().value().y(), distortedPosition, scenePosition ) );
		specularTransmission = mix( specularTransmission
			, waterTransmission
			, vec3( clamp( ( lightSurface.worldPosition().value().y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
		debugOutputBlock.registerOutput( cuT( "Height Mixed Refraction" ), specularTransmission );
		specularTransmission = mix( specularTransmission
			, waterTransmission
			, utils.saturate( vec3( utils.saturate( length( lightSurface.viewPosition().value() ) / distanceFactor ) ) ) );
		debugOutputBlock.registerOutput( cuT( "Distance Mixed Refraction" ), specularTransmission );

		if ( components.hasMember( "waterNoise" ) )
		{
			auto waterNoise = components.getMember< sdw::Float >( "waterNoise" );
			debugOutputBlock.registerOutput( cuT( "Specular Noise" ), waterNoise );
			lighting.dielectric *= waterNoise;
			lighting.metal *= waterNoise;
		}

		if ( components.hasMember( "waterColourMod" ) )
		{
			auto waterColourMod = components.getMember< sdw::Float >( "waterColourMod" );
			reflectionResult *= waterColourMod;
			specularTransmission *= waterColourMod;
		}

		if ( components.hasMember( "foamHeightStart" )
			&& components.hasMember( "waterFoamNoise" ) )
		{
			// Now apply some foam on top of waves
			auto foamHeightStart = components.getMember< sdw::Float >( "foamHeightStart" );
			auto foamFadeDistance = components.getMember< sdw::Float >( "foamFadeDistance" );
			auto foamAngleExponent = components.getMember< sdw::Float >( "foamAngleExponent" );
			auto foamBrightness = components.getMember< sdw::Float >( "foamBrightness" );
			auto waterFoamNoise = components.getMember< sdw::Float >( "waterFoamNoise" );
			auto waterFoam = components.getMember< sdw::Vec3 >( "waterFoam" );
			debugOutputBlock.registerOutput( cuT( "Foam Noise" ), waterFoamNoise );
			debugOutputBlock.registerOutput( cuT( "Foam Colour" ), waterFoam );
			auto foamAmount = writer.declLocale( "foamAmount"
				, utils.saturate( ( lightSurface.worldPosition().value().w() - foamHeightStart ) / foamFadeDistance ) * pow( utils.saturate( dot( components.getRawNormal(), vec3( 0.0_f, 1.0_f, 0.0_f ) ) ), foamAngleExponent ) * waterFoamNoise );
			debugOutputBlock.registerOutput( cuT( "Raw Foam Amount" ), foamAmount );
			foamAmount += pow( ( 1.0_f - depthSoftenedAlpha ), 3.0_f );
			debugOutputBlock.registerOutput( cuT( "Depth Softened Foam Amount" ), foamAmount );
			auto foamResult = writer.declLocale( "foamResult"
				, lighting.diffuse * mix( vec3( 0.0_f )
					, waterFoam * foamBrightness
					, vec3( utils.saturate( foamAmount ) * depthSoftenedAlpha ) ) );
			debugOutputBlock.registerOutput( cuT( "Foam Result" ), foamResult );
			specularTransmission += foamResult;
			reflectionResult += foamResult;
		}

		components.opacity = depthSoftenedAlpha;

		if ( components.hasMember( "transmissionFactor" ) )
		{
			output.diffuse = mix( output.diffuse, specularTransmission, vec3( components.transmissionFactor ) );
		}

		auto brdf = writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapBrdf" );
		output.metal = reflectionResult
			* reflections.computeFresnel( brdf
				, lightSurface.NdotV().value()
				, components.perceptualRoughness
				, components.baseColour
				, 1.0_f );
		output.dielectric = mix( output.diffuse
			, reflectionResult
			, reflections.computeFresnel( brdf
				, lightSurface.NdotV().value()
				, components.perceptualRoughness
				, components.dielectricF0
				, components.specularWeight ) );
	}

	//*********************************************************************************************

	castor::String const WaterReflRefrComponent::TypeName = C3D_PluginMakePassReflectionComponentName( "water", "water" );

	WaterReflRefrComponent::WaterReflRefrComponent( castor3d::Pass & pass )
		: castor3d::PassComponent{ pass, TypeName }
	{
	}

	castor3d::PassComponentUPtr WaterReflRefrComponent::doClone( castor3d::Pass & pass )const
	{
		auto result = castor::makeUnique< WaterReflRefrComponent >( pass );
		return castor::ptrRefCast< castor3d::PassComponent >( result );
	}

	//*********************************************************************************************
}
