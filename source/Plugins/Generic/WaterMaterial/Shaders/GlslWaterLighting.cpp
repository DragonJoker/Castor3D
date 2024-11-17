#include "WaterMaterial/Shaders/GlslWaterLighting.hpp"

#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

namespace water::shader
{
	//*********************************************************************************************

	WaterLightingModel::WaterLightingModel( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdfHelpers
		, c3d::LightingModelSpec spec
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
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
			, castor::String{ cuT( "c3d_water_" ) } }
	{
	}

	castor::StringView WaterLightingModel::getName()
	{
		return cuT( "c3d.water" );
	}

	c3d::LightingModelPtr WaterLightingModel::create( castor3d::LightingModelID lightingModelId
		, c3d::LightingModelDesc const & desc
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdfHelpers
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, WaterLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, c3d::LightingModelSpec{ ( desc.diffuse.create
					? desc.diffuse.create( writer, brdfHelpers )
					: castor3d::PbrPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
				, ( desc.specular.create
					? desc.specular.create( writer, brdfHelpers )
					: castor3d::PbrPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
				, ( desc.sheen.create
					? desc.sheen.create( writer, brdfHelpers )
					: castor3d::PbrPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
				, ( desc.clearcoat.create
					? desc.clearcoat.create( writer, brdfHelpers )
					: castor3d::PbrPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
				, ( desc.scattering.create
					? desc.scattering.create( writer )
					: castor3d::PbrPass::DefaultScatteringModel.create( writer ) ) }
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void WaterLightingModel::doCombine( c3d::DebugOutputCategory const & debugOutput
		, c3d::ReflectionModel & reflections
		, sdw::CombinedImage2DRgba32 const & brdf
		, c3d::BlendComponents const & components
		, c3d::LightSurface const & lightSurface
		, sdw::Vec3 const & incident
		, sdw::Float const & ambientOcclusion
		, c3d::DirectLighting const & directLighting
		, c3d::IndirectLighting const & indirectLighting
		, c3d::ReflectionRefraction const & reflRefr
		, sdw::Vec3 & combineResult )
	{
		auto fresnelFactor = m_writer.hasVariable( "fresnelFactor", true )
			? m_writer.getVariable< sdw::Float >( "fresnelFactor"
				, ( components.hasMember( "specularFactor" )
					|| components.hasMember( "transmissionFactor" ) ) )
			: m_writer.declLocale( "fresnelFactor"
				, m_utils.fresnelMix( incident
					, components.getRawNormal()
					, components.ior )
				, ( components.hasMember( "specularFactor" )
					|| components.hasMember( "transmissionFactor" ) ) );
		if ( fresnelFactor.isEnabled() )
		{
			debugOutput.registerOutput( cuT( "Fresnel Factor" ), fresnelFactor );
		}

		auto clearcoatFresnel = components.getMember( "clearcoatFresnel", vec3( 0.0_f ) );

		auto backgroundResult = m_writer.declLocale( "backgroundResult", vec3( 0.0_f ) );
		processBackground( debugOutput
			, components, reflRefr, fresnelFactor, clearcoatFresnel
			, backgroundResult );

		auto directLightingResult = m_writer.declLocale( "directLightingResult", vec3( 0.0_f ) );
		processDirectLighting( debugOutput
			, components, directLighting, fresnelFactor
			, directLightingResult );

		auto indirectLightingResult = m_writer.declLocale( "indirectLightingResult", vec3( 0.0_f ) );
		processIndirectLighting( debugOutput
			, components, indirectLighting, fresnelFactor
			, indirectLightingResult );

		auto emissiveResult = m_writer.declLocale( "emissiveResult"
			, ( components.emissiveColour * components.emissiveFactor
				* ( 1.0_f - components.clearcoatFactor * clearcoatFresnel ) ) );
		debugOutput.registerOutput( "Emissive Result", emissiveResult );

		combineResult += ( backgroundResult * ambientOcclusion )
			+ ( indirectLightingResult * ambientOcclusion )
			+ directLightingResult
			+ emissiveResult;
	}

	void WaterLightingModel::processBackground( c3d::DebugOutputCategory const & debugOutput
		, c3d::BlendComponents const & components
		, c3d::ReflectionRefraction reflRefr
		, sdw::Float const & fresnelFactor
		, sdw::Vec3 const & clearcoatFresnel
		, sdw::Vec3 & backgroundResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Background" ) );
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

		backgroundResult = bgDiffuse + bgSpecular;
		debugOutputBlock.registerOutput( "Result", backgroundResult );
		doComputeBackgroundLayers( debugOutputBlock
			, components, reflRefr, clearcoatFresnel
			, backgroundResult );
	}

	void WaterLightingModel::processDirectLighting( c3d::DebugOutputCategory const & debugOutput
		, c3d::BlendComponents const & components
		, c3d::DirectLighting const & directLighting
		, sdw::Float const & fresnelFactor
		, sdw::Vec3 & directLightingResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Direct" ) );

		auto dlSpecularResult = m_writer.declLocale( "dlSpecularResult"
			, directLighting.specular );
		debugOutputBlock.registerOutput( "Specular Result", dlSpecularResult );

		if ( components.hasMember( "specularFactor" ) )
		{
			auto specularFactor = m_writer.declLocale( "c3d_specularFactor"
				, clamp( components.getMember< sdw::Float >( "specularFactor" ), 0.0_f, 1.0_f ) );
			dlSpecularResult *= specularFactor * fresnelFactor;
			debugOutputBlock.registerOutput( cuT( "Specular Factor" ), specularFactor );
		}
		else
		{
			debugOutputBlock.registerOutput( cuT( "Specular Factor" ), 0.0_f );
		}

		directLightingResult = dlSpecularResult;
		debugOutputBlock.registerOutput( cuT( "Result" ), directLightingResult );
	}

	void WaterLightingModel::processIndirectLighting( c3d::DebugOutputCategory const & debugOutput
		, c3d::BlendComponents const & components
		, c3d::IndirectLighting const & indirectLighting
		, sdw::Float const & fresnelFactor
		, sdw::Vec3 & indirectLightingResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Indirect" ) );

		auto ilSpecularResult = m_writer.declLocale( "ilSpecularResult"
			, indirectLighting.specular );
		debugOutputBlock.registerOutput( "Specular Result", ilSpecularResult );

		if ( components.hasMember( "specularFactor" ) )
		{
			auto specularFactor = m_writer.getVariable< sdw::Float >( "c3d_specularFactor" );
			ilSpecularResult *= specularFactor * fresnelFactor;
		}

		indirectLightingResult = ilSpecularResult;
		debugOutputBlock.registerOutput( cuT( "Result" ), indirectLightingResult );
	}

	//*********************************************************************************************
}
