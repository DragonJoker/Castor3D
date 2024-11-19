#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"

#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	PbrLightingModel::PbrLightingModel( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdfHelpers
		, LightingModelSpec spec
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
		: LightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, std::move( spec )
			, shadowModel
			, lights
			, true
			, true
			, true
			, enableVolumetric
			, cuT( "c3d_pbr_" ) }
	{
	}

	castor::StringView PbrLightingModel::getName()
	{
		return cuT( "c3d.pbr" );
	}

	LightingModelPtr PbrLightingModel::create( LightingModelID lightingModelId
		, LightingModelDesc const & desc
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdfHelpers
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, PbrLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, LightingModelSpec{ ( desc.diffuse.create
					? desc.diffuse.create( writer, brdfHelpers )
					: PbrPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
				, ( desc.specular.create
					? desc.specular.create( writer, brdfHelpers )
					: PbrPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
				, ( desc.sheen.create
					? desc.sheen.create( writer, brdfHelpers )
					: PbrPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
				, ( desc.clearcoat.create
					? desc.clearcoat.create( writer, brdfHelpers )
					: PbrPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
				, ( desc.scattering.create
					? desc.scattering.create( writer )
					: PbrPass::DefaultScatteringModel.create( writer ) ) }
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void PbrLightingModel::doCombine( DebugOutputCategory const & debugOutput
		, ReflectionModel & reflections
		, sdw::CombinedImage2DRgba32 const & brdf
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 const & incident
		, sdw::Float const & ambientOcclusion
		, DirectLighting const & directLighting
		, IndirectLighting const & indirectLighting
		, ReflectionRefraction const & reflRefr
		, sdw::Vec3 & combineResult )
	{
		auto clearcoatFresnel = components.getMember( "clearcoatFresnel", vec3( 0.0_f ) );
		debugOutput.registerOutput( "Clearcoat Fresnel", clearcoatFresnel );

		auto metalFresnel = m_writer.declLocale( "metalFresnel"
			, computeFresnel( lightSurface.NdotV().value()
				, components.perceptualRoughness
				, components.baseColour
				, 1.0_f ) );
		debugOutput.registerOutput( "Metal Fresnel", metalFresnel );
		auto dielectricFresnel = m_writer.declLocale( "dielectricFresnel"
			, computeFresnel( lightSurface.NdotV().value()
				, components.perceptualRoughness
				, components.dielectricF0
				, components.specularWeight ) );
		debugOutput.registerOutput( "Dielectric Fresnel", dielectricFresnel );

		auto backgroundResult = m_writer.declLocale( "backgroundResult", vec3( 0.0_f ) );
		processBackground( debugOutput
			, components, lightSurface, reflRefr
			, clearcoatFresnel, metalFresnel, dielectricFresnel
			, backgroundResult );

		auto directLightingResult = m_writer.declLocale( "directLightingResult", vec3( 0.0_f ) );
		processDirectLighting( debugOutput
			, components, directLighting, clearcoatFresnel
			, directLightingResult );

		auto indirectLightingResult = m_writer.declLocale( "indirectLightingResult", vec3( 0.0_f ) );
		processIndirectLighting( debugOutput
			, components, indirectLighting
			, metalFresnel, dielectricFresnel
			, indirectLightingResult );

		auto emissiveResult = m_writer.declLocale( "emissiveResult"
			, ( components.emissiveColour
				* components.emissiveFactor
				* ( 1.0_f - components.clearcoatFactor * clearcoatFresnel ) ) );
		debugOutput.registerOutput( "Emissive Result", emissiveResult );

		combineResult += ( backgroundResult * ambientOcclusion * indirectLighting.ambient )
			+ ( indirectLightingResult * ambientOcclusion )
			+ directLightingResult
			+ emissiveResult;
	}

	sdw::RetVec3 PbrLightingModel::computeFresnel( sdw::Float const & pNdotV
		, sdw::Float const & proughness
		, sdw::Vec3 const & pF0
		, sdw::Float const & pspecularWeight )
	{
		if ( !m_computeFresnel )
		{
			m_computeFresnel = m_writer.implementFunction< sdw::Vec3 >( "c3d_bgComputeFresnel"
				, [this]( sdw::Float const & NdotV
					, sdw::Float const & roughness
					, sdw::Vec3 const & F0
					, sdw::Float const & specularWeight )
				{
					auto brdf = m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapBrdf" );
					// see https://bruop.github.io/ibl/#single_scattering_results at Single Scattering Results
					// Roughness dependent fresnel, from Fdez-Aguera
					auto f_ab = m_writer.declLocale( "f_ab"
						, BackgroundModel::getBrdf( brdf, NdotV, roughness ) );
					auto Fr = m_writer.declLocale( "Fr"
						, max( vec3( 1.0_f - roughness ), F0 ) - F0 );
					auto k_S = m_writer.declLocale( "k_S"
						, F0 + Fr * pow( 1.0_f - NdotV, 5.0_f ) );
					auto FssEss = m_writer.declLocale( "FssEss"
						, specularWeight * ( k_S * f_ab.x() + f_ab.y() ) );

					// Multiple scattering, from Fdez-Aguera
					auto Ems = m_writer.declLocale( "Ems"
						, ( 1.0_f - ( f_ab.x() + f_ab.y() ) ) );
					auto F_avg = m_writer.declLocale( "F_avg"
						, specularWeight * ( F0 + ( 1.0_f - F0 ) / 21.0_f ) );
					auto FmsEms = m_writer.declLocale( "FmsEms"
						, Ems * FssEss * F_avg / ( 1.0_f - F_avg * Ems ) );

					m_writer.returnStmt( FssEss + FmsEms );
				}
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InVec3{ m_writer, "F0" }
				, sdw::InFloat{ m_writer, "specularWeight" } );
		}

		return m_computeFresnel( pNdotV, proughness, pF0, pspecularWeight );
	}

	void PbrLightingModel::computeSpecularBrdfs( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, sdw::Vec3 const & reflectedDiffuse
		, sdw::Vec3 const & reflectedSpecular
		, sdw::Vec3 const & metalFresnel
		, sdw::Vec3 const & dielectricFresnel
		, sdw::Vec3 & metal
		, sdw::Vec3 & dielectric )
	{
		metal = reflectedSpecular * metalFresnel;
		debugOutput.registerOutput( "Raw Metal BRDF", metal );
		dielectric = mix( reflectedDiffuse, reflectedSpecular, dielectricFresnel );
		debugOutput.registerOutput( "Raw Dielectric BRDF", dielectric );

		if ( components.hasMember( "iridescenceFactor" ) )
		{
			metal = mix( metal
				, reflectedSpecular * components.getMember< sdw::Vec3 >( "iridescenceMetallicFresnel" )
				, vec3( components.iridescenceFactor ) );
			debugOutput.registerOutput( "Iridescent Metal BRDF", metal );
			dielectric = mix( dielectric
				, Utils::rgbMix( reflectedDiffuse, reflectedSpecular, components.getMember< sdw::Vec3 >( "iridescenceDielectricFresnel" ) )
				, vec3( components.iridescenceFactor ) );
			debugOutput.registerOutput( "Iridescent Dielectric BRDF", dielectric );
		}
		else
		{
			debugOutput.registerOutput( "Iridescent Metal BRDF", metal );
			debugOutput.registerOutput( "Iridescent Dielectric BRDF", dielectric );
		}
	}

	void PbrLightingModel::processBackground( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, ReflectionRefraction reflRefr
		, sdw::Vec3 const & clearcoatFresnel
		, sdw::Vec3 const & metalFresnel
		, sdw::Vec3 const & dielectricFresnel
		, sdw::Vec3 & backgroundResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Background" ) );
		reflRefr.registerDebug( debugOutputBlock );
		auto bgDiffuse = m_writer.declLocale( "bgDiffuse", reflRefr.diffuseReflection );
		auto bgSpecular = m_writer.declLocale( "bgSpecular", reflRefr.specularReflection );

		if ( components.hasMember( "transmissionFactor" ) )
		{
			bgDiffuse = mix( bgDiffuse, reflRefr.specularTransmission, vec3( components.transmissionFactor ) );
			debugOutputBlock.registerOutput( "Diffuse Mixed With Specular Transmission", bgDiffuse );
		}
		else
		{
			debugOutputBlock.registerOutput( "Diffuse Mixed With Specular Transmission", bgDiffuse );
		}

		if ( components.hasMember( "diffuseTransmissionFactor" ) )
		{
			bgDiffuse = mix( bgDiffuse
				, reflRefr.diffuseTransmission
				, vec3( components.diffuseTransmissionFactor ) );
			debugOutputBlock.registerOutput( "Diffuse Mixed With Diffuse Transmission", bgDiffuse );
		}
		else
		{
			debugOutputBlock.registerOutput( "Diffuse Mixed With Diffuse Transmission", bgDiffuse );
		}

		debugOutputBlock.registerOutput( "Diffuse", bgDiffuse );
		debugOutputBlock.registerOutput( "Specular", bgSpecular );
		auto bgDielectric = m_writer.declLocale( "bgDielectric", vec3( 0.0_f ) );
		auto bgMetallic = m_writer.declLocale( "bgMetallic", vec3( 0.0_f ) );
		computeSpecularBrdfs( debugOutputBlock, components
			, bgDiffuse, bgSpecular
			, metalFresnel, dielectricFresnel
			, bgMetallic, bgDielectric );
		backgroundResult = mix( bgDielectric, bgMetallic, vec3( components.metalness ) );
		debugOutputBlock.registerOutput( "Result", backgroundResult );
		doComputeBackgroundLayers( debugOutputBlock
			, components, reflRefr, clearcoatFresnel
			, backgroundResult );
	}

	void PbrLightingModel::processDirectLighting( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, DirectLighting const & directLighting
		, sdw::Vec3 const & clearcoatFresnel
		, sdw::Vec3 & directLightingResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Direct" ) );
		directLighting.registerDebug( debugOutputBlock );
		directLightingResult = mix( directLighting.dielectric, directLighting.metal, vec3( components.metalness ) );
		debugOutputBlock.registerOutput( "Result", directLightingResult );

		if ( components.sheenColour )
		{
			auto ltSheen = m_writer.declLocale( "ltSheen"
				, directLighting.sheen.rgb() );
			debugOutputBlock.registerOutput( "Sheen", ltSheen );
			auto ltAlbedoSheenScaling = m_writer.declLocale( "ltAlbedoSheenScaling"
				, 1.0_f - directLighting.sheen.w() * Utils::max3( components.sheenColour ) );
			debugOutputBlock.registerOutput( "Albedo Sheen Scaling", ltAlbedoSheenScaling );
			directLightingResult = ltSheen + directLightingResult * ltAlbedoSheenScaling;
			debugOutputBlock.registerOutput( "With Sheen", directLightingResult );
		}
		else
		{
			debugOutputBlock.registerOutput( "Sheen", 0.0_f );
			debugOutputBlock.registerOutput( "Albedo Sheen Scaling", 0.0_f );
			debugOutputBlock.registerOutput( "With Sheen", directLightingResult );
		}

		if ( components.clearcoatFactor )
		{
			directLightingResult = mix( directLightingResult, directLighting.coating, components.clearcoatFactor * clearcoatFresnel );
			debugOutputBlock.registerOutput( "With Clearcoat", directLightingResult );
		}
		else
		{
			debugOutputBlock.registerOutput( "With Clearcoat", directLightingResult );
		}
	}

	void PbrLightingModel::processIndirectLighting( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, IndirectLighting const & indirectLighting
		, sdw::Vec3 const & metalFresnel
		, sdw::Vec3 const & dielectricFresnel
		, sdw::Vec3 & indirectLightingResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Indirect" ) );
		indirectLighting.registerDebug( debugOutputBlock );
		auto indirectMetal = m_writer.declLocale( "indirectMetal", vec3( 0.0_f ) );
		auto indirectDielectric = m_writer.declLocale( "indirectDielectric", vec3( 0.0_f ) );
		computeSpecularBrdfs( debugOutputBlock, components
			, indirectLighting.diffuseColour, indirectLighting.specular
			, metalFresnel, dielectricFresnel
			, indirectMetal, indirectDielectric );
		debugOutputBlock.registerOutput( "Raw Dielectric", indirectDielectric );
		indirectDielectric *= components.baseColour;
		debugOutputBlock.registerOutput( "Tinted Dielectric", indirectDielectric );
		indirectLightingResult = mix( indirectDielectric, indirectMetal, vec3( components.metalness ) );
		debugOutputBlock.registerOutput( "Result", indirectLightingResult );
	}
}
