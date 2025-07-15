#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"

#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d::shader
{
	//*********************************************************************************************

	PhongLightingModel::PhongLightingModel( LightingModelID lightingModelId
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
			, false
			, false
			, false
			, enableVolumetric
			, String{ cuT( "c3d_phong_" ) } }
	{
	}

	String PhongLightingModel::getName()
	{
		return cuT( "c3d.phong" );
	}

	LightingModelPtr PhongLightingModel::create( LightingModelID lightingModelId
		, LightingModelDesc const & desc
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdfHelpers
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
	{
		return makeUniqueDerived< LightingModel, PhongLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, LightingModelSpec{ ( desc.diffuse.create
					? desc.diffuse.create( writer, brdfHelpers )
					: PhongPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
				, ( desc.specular.create
					? desc.specular.create( writer, brdfHelpers )
					: PhongPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
				, ( desc.sheen.create
					? desc.sheen.create( writer, brdfHelpers )
					: PhongPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
				, ( desc.clearcoat.create
					? desc.clearcoat.create( writer, brdfHelpers )
					: PhongPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
				, ( desc.scattering.create
					? desc.scattering.create( writer )
					: PhongPass::DefaultScatteringModel.create( writer ) ) }
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void PhongLightingModel::doCombine( DebugOutputCategory const & debugOutput
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

	void PhongLightingModel::processBackground( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, ReflectionRefraction reflRefr
		, sdw::Float const & fresnelFactor
		, sdw::Vec3 const & clearcoatFresnel
		, sdw::Vec3 & backgroundResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Background" ) );
		reflRefr.registerDebug( debugOutputBlock );

		if ( fresnelFactor.isEnabled() )
		{
			debugOutputBlock.registerOutput( cuT( "Fresnel Factor" ), fresnelFactor );

			sdwIF( m_writer, components.ior != 0.0_f )
			{
				reflRefr.diffuseTransmission = mix( reflRefr.diffuseTransmission
					, vec3( 0.0_f )
					, vec3( fresnelFactor ) );
				reflRefr.specularTransmission = mix( reflRefr.specularTransmission
					, vec3( 0.0_f )
					, vec3( fresnelFactor ) );
				reflRefr.diffuseReflection = mix( vec3( 0.0_f )
					, reflRefr.diffuseReflection
					, vec3( fresnelFactor ) );
				reflRefr.specularReflection = mix( vec3( 0.0_f )
					, reflRefr.specularReflection
					, vec3( fresnelFactor ) );
			}
			sdwFI
		}
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

		backgroundResult = bgDiffuse + bgSpecular * components.getMember< sdw::Vec3 >( "specular", vec3( 1.0_f ) );
		debugOutputBlock.registerOutput( "Result", backgroundResult );
		doComputeBackgroundLayers( debugOutputBlock
			, components, reflRefr, clearcoatFresnel
			, backgroundResult );
	}

	void PhongLightingModel::processDirectLighting( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, DirectLighting const & directLighting
		, sdw::Float const & fresnelFactor
		, sdw::Vec3 & directLightingResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Direct" ) );
		directLighting.registerDebug( debugOutputBlock );
		auto dlDiffuseResult = m_writer.declLocale( "dlDiffuseResult"
			, directLighting.diffuse );
		debugOutputBlock.registerOutput( "Diffuse Result", dlDiffuseResult );

		auto dlSpecularResult = m_writer.declLocale( "dlSpecularResult"
			, directLighting.specular );
		debugOutputBlock.registerOutput( "Specular Result", dlSpecularResult );

		if ( components.hasMember( "specularFactor" ) )
		{
			auto specularFactor = m_writer.declLocale( "c3d_specularFactor"
				, clamp( components.getMember< sdw::Float >( "specularFactor" ), 0.0_f, 1.0_f ) );
			dlSpecularResult *= specularFactor * fresnelFactor;
			dlDiffuseResult *= 1.0_f - specularFactor * fresnelFactor;
			debugOutputBlock.registerOutput( cuT( "Specular Factor" ), specularFactor );
		}
		else
		{
			debugOutputBlock.registerOutput( cuT( "Specular Factor" ), 0.0_f );
		}

		directLightingResult = dlDiffuseResult + dlSpecularResult * components.getMember< sdw::Vec3 >( "specular", vec3( 1.0_f ) );
		debugOutputBlock.registerOutput( "Result", directLightingResult );
	}

	void PhongLightingModel::processIndirectLighting( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, IndirectLighting const & indirectLighting
		, sdw::Float const & fresnelFactor
		, sdw::Vec3 & indirectLightingResult )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Indirect" ) );
		indirectLighting.registerDebug( debugOutputBlock );
		auto ilDiffuseResult = m_writer.declLocale( "ilDiffuseResult"
			, components.baseColour * ( indirectLighting.diffuseColour + indirectLighting.ambient ) );
		debugOutputBlock.registerOutput( "Diffuse Result", ilDiffuseResult );

		auto ilSpecularResult = m_writer.declLocale( "ilSpecularResult"
			, indirectLighting.specular );
		debugOutputBlock.registerOutput( "Specular Result", ilSpecularResult );

		if ( components.hasMember( "specularFactor" ) )
		{
			auto specularFactor = m_writer.getVariable< sdw::Float >( "c3d_specularFactor" );
			ilSpecularResult *= specularFactor * fresnelFactor;
			ilDiffuseResult *= 1.0_f - specularFactor * fresnelFactor;
		}

		indirectLightingResult = ilDiffuseResult + ilSpecularResult;
		debugOutputBlock.registerOutput( "Result", indirectLightingResult );
	}

	//*********************************************************************************************
}
