#include "WaterMaterial/Shaders/GlslWaterLighting.hpp"

#include "WaterMaterial/Shaders/GlslWaterProfile.hpp"
#include "WaterMaterial/WaterNoiseMapComponent.hpp"
#include "WaterMaterial/WaterNormal1MapComponent.hpp"
#include "WaterMaterial/WaterNormal2MapComponent.hpp"

#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslPassShaders.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>

namespace water::shader
{
	//*********************************************************************************************

	WaterLightingModel::WaterLightingModel( sdw::ShaderWriter & writer )
	{
	}

	sdw::Vec2 WaterLightingModel::getTexture0XY( c3d::BlendComponents const & components )const
	{
		auto texType = static_cast< sdw::type::BaseStruct const & >( *components.getType() ).getMember( "texture0" ).type;

		switch ( texType->getKind() )
		{
		case sdw::type::Kind::eVec3F:
			return components.getMember< sdw::Vec3 >( "texture0" ).xy();
		default:
			return components.getMember< c3d::DerivTex >( "texture0" ).uv();
		}
	}

	void WaterLightingModel::doFinish( c3d::PassShaders const & passShaders
		, c3d::RasterizerSurfaceBase const & surface
		, c3d::BlendComponents & components )
	{
		if ( components.hasMember( "texture0" ) )
		{
		}
	}

	sdw::Float WaterLightingModel::doGetFinalTransmission( c3d::BlendComponents const & components
		, sdw::Vec3 const & incident )
	{
		return 1.0_f;
	}

	sdw::Vec3 WaterLightingModel::doAdjustDirectSpecular( c3d::BlendComponents const & components
		, sdw::Vec3 const & specular )const
	{
		if ( components.hasMember( "waterNoise" ) )
		{
			auto waterNoise = components.getMember< sdw::Float >( "waterNoise" );
			return specular * waterNoise;
		}

		return specular;
	}

	void WaterLightingModel::doComputeReflRefr( c3d::ReflectionModel & reflections
		, c3d::BlendComponents & components
		, c3d::LightSurface const & lightSurface
		, c3d::BackgroundModel & backgroundModel
		, c3d::CameraData const & camera
		, c3d::OutputComponents const & lighting
		, sdw::Vec3 const & indirectAmbient
		, sdw::Vec3 const & indirectDiffuse
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
		, c3d::DebugOutput & debugOutput )
	{
		auto & writer = *components.getWriter();

		if ( !writer.hasGlobalVariable( "c3d_mapDepthObj" ) )
		{
			return;
		}

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
		reflections.computeReflections( components
			, lightSurface
			, backgroundModel
			, envMapIndex
			, components.hasReflection
			, reflectedDiffuse
			, reflectedSpecular
			, debugOutput );
		auto hdrCoords = writer.declLocale( "hdrCoords"
			, sceneUv / vec2( mapColours.getSize( 0_i ) ) );
		auto backgroundReflection = writer.declLocale( "backgroundReflection"
			, reflectedDiffuse + reflectedSpecular );
		debugOutput.registerOutput( "Water", "Background Reflection", backgroundReflection );
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
				, debugOutput ) );
		auto reflectionResult = writer.declLocale( "reflectionResult"
			, mix( backgroundReflection, ssrResult.xyz(), ssrResult.www() ) );
		debugOutput.registerOutput( "Water", "Reflection Result", reflectionResult );

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
		debugOutput.registerOutput( "Water", "Refraction", refractionResult );
		//  Retrieve non distorted scene colour.
		auto sceneDepth = writer.declLocale( "sceneDepth"
			, mapDepthObj.lod( hdrCoords, 0.0_f ).r() );
		auto scenePosition = writer.declLocale( "scenePosition"
			, camera.curProjToWorld( utils, hdrCoords, sceneDepth ) );
		// Depth softening, to fade the alpha of the water where it meets the scene geometry by some predetermined distance. 
		auto depthSoftenedAlpha = writer.declLocale( "depthSoftenedAlpha"
			, clamp( distance( scenePosition, lightSurface.worldPosition().xyz() ) / depthSofteningDistance, 0.0_f, 1.0_f ) );
		debugOutput.registerOutput( "Water", "Depth Softened Alpha", depthSoftenedAlpha );
		auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
			, writer.ternary( distortedPosition.y() < lightSurface.worldPosition().y(), distortedPosition, scenePosition ) );
		auto waterTransmission = writer.declLocale( "waterTransmission"
			, components.colour * ( indirectAmbient + indirectDiffuse ) * lighting.diffuse );
		auto heightFactor = writer.declLocale( "heightFactor"
			, refractionHeightFactor * ( camera.farPlane() - camera.nearPlane() ) );
		auto heightDistance = writer.declLocale( "heightDistance"
			, lightSurface.worldPosition().y() - waterSurfacePosition.y() );
		refractionResult = mix( refractionResult
			, waterTransmission
			, vec3( clamp( ( lightSurface.worldPosition().y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
		debugOutput.registerOutput( "Water", "Height Mixed Refraction", refractionResult );
		auto distanceFactor = writer.declLocale( "distanceFactor"
			, refractionDistanceFactor * ( camera.farPlane() - camera.nearPlane() ) );
		refractionResult = mix( refractionResult
			, waterTransmission
			, utils.saturate( vec3( utils.saturate( length( lightSurface.viewPosition() ) / distanceFactor ) ) ) );
		debugOutput.registerOutput( "Water", "Distance Mixed Refraction", refractionResult );

		if ( components.hasMember( "waterNoise" ) )
		{
			auto waterNoise = components.getMember< sdw::Float >( "waterNoise" );
			debugOutput.registerOutput( "Water", "Specular Noise", waterNoise );
			reflectionResult *= waterNoise;
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
			debugOutput.registerOutput( "Water", "Foam Noise", waterFoamNoise );
			auto waterFoam = components.getMember< sdw::Vec3 >( "waterFoam" );
			debugOutput.registerOutput( "Water", "Foam Colour", waterFoam );
			// TODO: Check if use of worldPosition.w is correct here...
			auto foamAmount = writer.declLocale( "foamAmount"
				, utils.saturate( ( lightSurface.worldPosition().w() - foamHeightStart ) / foamFadeDistance ) * pow( utils.saturate( dot( components.normal, vec3( 0.0_f, 1.0_f, 0.0_f ) ) ), foamAngleExponent ) * waterFoamNoise );
			debugOutput.registerOutput( "Water", "Raw Foam Amount", foamAmount );
			foamAmount += pow( ( 1.0_f - depthSoftenedAlpha ), 3.0_f );
			debugOutput.registerOutput( "Water", "Depth Softened Foam Amount", foamAmount );
			auto foamResult = writer.declLocale( "foamResult"
				, lighting.diffuse * mix( vec3( 0.0_f )
					, waterFoam * foamBrightness
					, vec3( utils.saturate( foamAmount ) * depthSoftenedAlpha ) ) );
			debugOutput.registerOutput( "Water", "Foam Result", foamResult );
			refractionResult += foamResult;
			reflectionResult += foamResult;
		}

		components.opacity = depthSoftenedAlpha;
		reflectedSpecular = reflectionResult;
		reflectedDiffuse = vec3( 0.0_f );
		coatReflected = vec3( 0.0_f );
		sheenReflected = vec3( 0.0_f );
	}

	//*********************************************************************************************

	WaterPhongLightingModel::WaterPhongLightingModel( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
		: c3d::PhongLightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, enableVolumetric }
		, WaterLightingModel{ writer }
	{
		m_prefix = "water_phong_";
	}

	const castor::String WaterPhongLightingModel::getName()
	{
		return cuT( "water.phong" );
	}

	c3d::LightingModelUPtr WaterPhongLightingModel::create( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< c3d::LightingModel, WaterPhongLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, enableVolumetric );
	}

	sdw::Float WaterPhongLightingModel::getFinalTransmission( c3d::BlendComponents const & components
		, sdw::Vec3 const & incident )
	{
		return doGetFinalTransmission( components, incident );
	}

	sdw::Vec3 WaterPhongLightingModel::adjustDirectSpecular( c3d::BlendComponents const & components
		, sdw::Vec3 const & directSpecular )const
	{
		return doAdjustDirectSpecular( components
			, c3d::PhongLightingModel::adjustDirectSpecular( components, directSpecular ) );
	}

	void WaterPhongLightingModel::doFinish( c3d::PassShaders const & passShaders
		, c3d::RasterizerSurfaceBase const & surface
		, c3d::BlendComponents & components )
	{
		c3d::PhongLightingModel::doFinish( passShaders, surface, components );
		WaterLightingModel::doFinish( passShaders, surface, components );
	}

	void WaterPhongLightingModel::computeReflRefr( c3d::ReflectionModel & reflections
		, c3d::BlendComponents & components
		, c3d::LightSurface const & lightSurface
		, sdw::Vec4 const & position
		, c3d::BackgroundModel & backgroundModel
		, sdw::CombinedImage2DRgba32 const & mippedScene
		, c3d::CameraData const & camera
		, c3d::OutputComponents const & lighting
		, sdw::Vec3 const & indirectAmbient
		, sdw::Vec3 const & indirectDiffuse
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
		, c3d::DebugOutput & debugOutput )
	{
		doComputeReflRefr( reflections
			, components
			, lightSurface
			, backgroundModel
			, camera
			, lighting
			, indirectAmbient
			, indirectDiffuse
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

	void WaterPhongLightingModel::computeReflRefr( c3d::ReflectionModel & reflections
		, c3d::BlendComponents & components
		, c3d::LightSurface const & lightSurface
		, c3d::BackgroundModel & backgroundModel
		, c3d::CameraData const & camera
		, c3d::OutputComponents const & lighting
		, sdw::Vec3 const & indirectAmbient
		, sdw::Vec3 const & indirectDiffuse
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
		, c3d::DebugOutput & debugOutput )
	{
		doComputeReflRefr( reflections
			, components
			, lightSurface
			, backgroundModel
			, camera
			, lighting
			, indirectAmbient
			, indirectDiffuse
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

	//*********************************************************************************************

	WaterPbrLightingModel::WaterPbrLightingModel( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
		: c3d::PbrLightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, enableVolumetric }
		, WaterLightingModel{ writer }
	{
		m_prefix = "water_pbr_";
	}

	const castor::String WaterPbrLightingModel::getName()
	{
		return cuT( "water.pbr" );
	}

	c3d::LightingModelUPtr WaterPbrLightingModel::create( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< c3d::LightingModel, WaterPbrLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, enableVolumetric );
	}

	sdw::Float WaterPbrLightingModel::getFinalTransmission( c3d::BlendComponents const & components
		, sdw::Vec3 const & incident )
	{
		return doGetFinalTransmission( components, incident );
	}

	sdw::Vec3 WaterPbrLightingModel::adjustDirectSpecular( c3d::BlendComponents const & components
		, sdw::Vec3 const & directSpecular )const
	{
		return doAdjustDirectSpecular( components
			, c3d::PbrLightingModel::adjustDirectSpecular( components, directSpecular ) );
	}

	void WaterPbrLightingModel::doFinish( c3d::PassShaders const & passShaders
		, c3d::RasterizerSurfaceBase const & surface
		, c3d::BlendComponents & components )
	{
		c3d::PbrLightingModel::doFinish( passShaders, surface, components );
		WaterLightingModel::doFinish( passShaders, surface, components );
	}

	void WaterPbrLightingModel::computeReflRefr( c3d::ReflectionModel & reflections
		, c3d::BlendComponents & components
		, c3d::LightSurface const & lightSurface
		, sdw::Vec4 const & position
		, c3d::BackgroundModel & backgroundModel
		, sdw::CombinedImage2DRgba32 const & mippedScene
		, c3d::CameraData const & camera
		, c3d::OutputComponents const & lighting
		, sdw::Vec3 const & indirectAmbient
		, sdw::Vec3 const & indirectDiffuse
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
		, c3d::DebugOutput & debugOutput )
	{
		doComputeReflRefr( reflections
			, components
			, lightSurface
			, backgroundModel
			, camera
			, lighting
			, indirectAmbient
			, indirectDiffuse
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

	void WaterPbrLightingModel::computeReflRefr( c3d::ReflectionModel & reflections
		, c3d::BlendComponents & components
		, c3d::LightSurface const & lightSurface
		, c3d::BackgroundModel & backgroundModel
		, c3d::CameraData const & camera
		, c3d::OutputComponents const & lighting
		, sdw::Vec3 const & indirectAmbient
		, sdw::Vec3 const & indirectDiffuse
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
		, c3d::DebugOutput & debugOutput )
	{
		doComputeReflRefr( reflections
			, components
			, lightSurface
			, backgroundModel
			, camera
			, lighting
			, indirectAmbient
			, indirectDiffuse
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

	//*********************************************************************************************
}
