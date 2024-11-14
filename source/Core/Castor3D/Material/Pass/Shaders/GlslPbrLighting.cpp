#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"

#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslClearcoatBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	PbrLightingModel::PbrLightingModel( LightingModelID lightingModelId
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
		return castor::makeUniqueDerived< LightingModel, PbrLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, ( diffuseBrdf.create
				? diffuseBrdf.create( writer, brdfHelpers )
				: PbrPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
			, ( specularBrdf.create
				? specularBrdf.create( writer, brdfHelpers )
				: PbrPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
			, ( sheenBrdf.create
				? sheenBrdf.create( writer, brdfHelpers )
				: PbrPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
			, ( clearcoatBrdf.create
				? clearcoatBrdf.create( writer, brdfHelpers )
				: PbrPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
			, shadowModel
			, lights
			, enableVolumetric );
	}

	sdw::Vec3 PbrLightingModel::doGetDiffuseResult( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedDiffuse )
	{
		return components.baseColour
			* ( lighting.diffuse
				+ ambientOcclusion * ( indirect.diffuseColour + ( reflectedDiffuse * lighting.ambient ) ) );
	}

	sdw::Vec3 PbrLightingModel::doGetSpecularResult( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedSpecular )
	{
		return lighting.dielectric
			+ ambientOcclusion * ( indirect.specular + ( reflectedSpecular * lighting.ambient ) );
	}

	//***********************************************************************************************
}
