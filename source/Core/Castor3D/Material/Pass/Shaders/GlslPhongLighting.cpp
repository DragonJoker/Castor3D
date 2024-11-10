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

namespace castor3d::shader
{
	//*********************************************************************************************

	PhongLightingModel::PhongLightingModel( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdfHelpers
		, DiffuseBRDFPtr diffuse
		, SpecularBRDFPtr specular
		, SheenBRDFPtr sheen
		, ClearcoatBRDFPtr clearcoat
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
		: LightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, std::move( diffuse )
			, std::move( specular )
			, std::move( sheen )
			, std::move( clearcoat )
			, shadowModel
			, lights
			, false
			, false
			, false
			, enableVolumetric
			, castor::String{ cuT( "c3d_phong_" ) } }
	{
	}

	castor::StringView PhongLightingModel::getName()
	{
		return cuT( "c3d.phong" );
	}

	LightingModelPtr PhongLightingModel::create( LightingModelID lightingModelId
		, DiffuseBrdfDesc const & diffuseBrdf
		, SpecularBrdfDesc const & specularBrdf
		, SheenBrdfDesc const & sheenBrdf
		, ClearcoatBrdfDesc const & clearcoatBrdf
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdfHelpers
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, PhongLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, ( diffuseBrdf.create
				? diffuseBrdf.create( writer, brdfHelpers )
				: PhongPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
			, ( specularBrdf.create
				? specularBrdf.create( writer, brdfHelpers )
				: PhongPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
			, ( sheenBrdf.create
				? sheenBrdf.create( writer, brdfHelpers )
				: PhongPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
			, ( clearcoatBrdf.create
				? clearcoatBrdf.create( writer, brdfHelpers )
				: PhongPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void PhongLightingModel::doFinish( PassShaders const & passShaders
		, BlendComponents & components )
	{
		components.f0 = components.specular;
	}

	sdw::Vec3 PhongLightingModel::doGetDiffuseResult( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedDiffuse )
	{
		return components.colour
			* ( lighting.diffuse
				+ ambientOcclusion * ( reflectedDiffuse + indirect.diffuseColour + ( lighting.ambient * indirect.ambient ) ) );
	}

	sdw::Vec3 PhongLightingModel::doGetSpecularResult( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedSpecular )
	{
		return lighting.specular
			+ ambientOcclusion * ( reflectedSpecular + indirect.specular );
	}

	//*********************************************************************************************
}
