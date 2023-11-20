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
		, sdw::UInt const & hasRefraction
		, sdw::Float const & refractionRatio
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular
		, sdw::Vec3 & refracted
		, sdw::Vec3 & coatReflected
		, sdw::Vec3 & sheenReflected
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
			, components.hasRefraction
			, components.refractionRatio
			, reflectedDiffuse
			, reflectedSpecular
			, refracted
			, coatReflected
			, sheenReflected
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
		, sdw::UInt const & hasRefraction
		, sdw::Float const & refractionRatio
		, sdw::Vec3 & reflectedDiffuse
		, sdw::Vec3 & reflectedSpecular
		, sdw::Vec3 & refractionResult
		, sdw::Vec3 & coatReflected
		, sdw::Vec3 & sheenReflected
		, c3d::DebugOutput & debugOutput )const
	{
		auto & writer = *components.getWriter();

		if ( !writer.hasGlobalVariable( "c3d_mapDepthObj" ) )
		{
			return;
		}

		auto debugOutputBlock = debugOutput.pushBlock( "Water" );
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

		// Reflections
		auto bgDiffuseReflection = writer.declLocale("bgDiffuseReflection"
			, vec3( 0.0_f ) );
		auto bgSpecularReflection = writer.declLocale("bgSpecularReflection"
			, vec3( 0.0_f ) );
		reflections.computeReflections( components
			, lightSurface
			, backgroundModel
			, envMapIndex
			, components.hasReflection
			, bgDiffuseReflection
			, bgSpecularReflection
			, debugOutputBlock );
		auto hdrCoords = writer.declLocale( "hdrCoords"
			, sceneUv / vec2( mapColours.getSize( 0_i ) ) );
		auto backgroundReflection = writer.declLocale( "backgroundReflection"
			, bgDiffuseReflection + bgSpecularReflection );
		debugOutputBlock.registerOutput( "Background Reflection", backgroundReflection );
		auto ssrResult = writer.declLocale( "ssrResult"
			, reflections.computeScreenSpace( camera
				, lightSurface.viewPosition()
				, components.normal
				, hdrCoords
				, vec4( ssrStepSize
					, writer.cast< sdw::Float >( ssrForwardStepsCount )
					, writer.cast< sdw::Float >( ssrBackwardStepsCount )
					, ssrDepthMult )
				, mapDepthObj
				, mapNormals
				, mapColours
				, debugOutputBlock ) );
		auto reflectionResult = writer.declLocale( "reflectionResult"
			, mix( backgroundReflection, ssrResult.xyz(), ssrResult.www() ) );
		debugOutputBlock.registerOutput( "Reflection Result", reflectionResult );

		// Refraction
		// Wobbly refractions
		auto distortedTexCoord = writer.declLocale( "distortedTexCoord"
			, ( hdrCoords + ( ( components.normal.xz() + components.normal.xy() ) * 0.5_f ) * refractionDistortionFactor ) );
		auto distortedDepth = writer.declLocale( "distortedDepth"
			, mapDepthObj.lod( distortedTexCoord, 0.0_f ).r() );
		auto distortedPosition = writer.declLocale( "distortedPosition"
			, camera.curProjToWorld( utils, distortedTexCoord, distortedDepth ) );
		auto refractionTexCoord = writer.declLocale( "refractionTexCoord"
			, writer.ternary( distortedPosition.y() < lightSurface.worldPosition().y(), distortedTexCoord, hdrCoords ) );
		refractionResult = mapColours.lod( refractionTexCoord, 0.0_f ).rgb() * components.colour;
		debugOutputBlock.registerOutput( "Refraction", refractionResult );
		//  Retrieve non distorted scene colour.
		auto sceneDepth = writer.declLocale( "sceneDepth"
			, mapDepthObj.lod( hdrCoords, 0.0_f ).r() );
		auto scenePosition = writer.declLocale( "scenePosition"
			, camera.curProjToWorld( utils, hdrCoords, sceneDepth ) );
		// Depth softening, to fade the alpha of the water where it meets the scene geometry by some predetermined distance. 
		auto depthSoftenedAlpha = writer.declLocale( "depthSoftenedAlpha"
			, clamp( distance( scenePosition, lightSurface.worldPosition().xyz() ) / depthSofteningDistance, 0.0_f, 1.0_f ) );
		debugOutputBlock.registerOutput( "Depth Softened Alpha", depthSoftenedAlpha );
		auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
			, writer.ternary( distortedPosition.y() < lightSurface.worldPosition().y(), distortedPosition, scenePosition ) );
		auto waterTransmission = writer.declLocale( "waterTransmission"
			, components.colour * ( indirect.ambient() + indirect.diffuseColour() ) * lighting.diffuse() );
		auto heightFactor = writer.declLocale( "heightFactor"
			, refractionHeightFactor * ( camera.farPlane() - camera.nearPlane() ) );
		auto heightDistance = writer.declLocale( "heightDistance"
			, lightSurface.worldPosition().y() - waterSurfacePosition.y() );
		refractionResult = mix( refractionResult
			, waterTransmission
			, vec3( clamp( ( lightSurface.worldPosition().y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
		debugOutputBlock.registerOutput( "Height Mixed Refraction", refractionResult );
		auto distanceFactor = writer.declLocale( "distanceFactor"
			, refractionDistanceFactor * ( camera.farPlane() - camera.nearPlane() ) );
		refractionResult = mix( refractionResult
			, waterTransmission
			, utils.saturate( vec3( utils.saturate( length( lightSurface.viewPosition() ) / distanceFactor ) ) ) );
		debugOutputBlock.registerOutput( "Distance Mixed Refraction", refractionResult );

		if ( components.hasMember( "waterNoise" ) )
		{
			auto waterNoise = components.getMember< sdw::Float >( "waterNoise" );
			debugOutputBlock.registerOutput( "Specular Noise", waterNoise );
			lighting.specular() *= waterNoise;
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
			debugOutputBlock.registerOutput( "Foam Noise", waterFoamNoise );
			debugOutputBlock.registerOutput( "Foam Colour", waterFoam );
			auto foamAmount = writer.declLocale( "foamAmount"
				, utils.saturate( ( lightSurface.worldPosition().w() - foamHeightStart ) / foamFadeDistance ) * pow( utils.saturate( dot( components.normal, vec3( 0.0_f, 1.0_f, 0.0_f ) ) ), foamAngleExponent ) * waterFoamNoise );
			debugOutputBlock.registerOutput( "Raw Foam Amount", foamAmount );
			foamAmount += pow( ( 1.0_f - depthSoftenedAlpha ), 3.0_f );
			debugOutputBlock.registerOutput( "Depth Softened Foam Amount", foamAmount );
			auto foamResult = writer.declLocale( "foamResult"
				, lighting.diffuse() * mix( vec3( 0.0_f )
					, waterFoam * foamBrightness
					, vec3( utils.saturate( foamAmount ) * depthSoftenedAlpha ) ) );
			debugOutputBlock.registerOutput( "Foam Result", foamResult );
			refractionResult += foamResult;
			reflectionResult += foamResult;
		}
		
		auto fresnelFactor = writer.declLocale( "fresnelFactor"
			, utils.fresnelMix( incident
				, components.normal
				, components.refractionRatio ) );
		debugOutputBlock.registerOutput( "Fresnel Factor", fresnelFactor );
		reflectionResult *= fresnelFactor;
		debugOutputBlock.registerOutput( "Final Reflection", reflectionResult );
		refractionResult *= vec3( 1.0_f ) - fresnelFactor;
		debugOutputBlock.registerOutput( "Final Refraction", refractionResult );

		components.opacity = depthSoftenedAlpha;
		reflectedSpecular = reflectionResult;
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
