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
			, castor::String{ cuT( "c3d_phong_" ) } }
	{
	}

	castor::StringView PhongLightingModel::getName()
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
		return castor::makeUniqueDerived< LightingModel, PhongLightingModel >( lightingModelId
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

	sdw::Vec3 PhongLightingModel::doGetDiffuseResult( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedDiffuse )
	{
		return components.baseColour
			* ( lighting.diffuse
				+ ambientOcclusion * ( reflectedDiffuse + indirect.diffuseColour + indirect.ambient ) );
	}

	sdw::Vec3 PhongLightingModel::doGetSpecularResult( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedSpecular )
	{
		return lighting.dielectric
			+ ambientOcclusion * ( reflectedSpecular + indirect.specular );
	}

	//*********************************************************************************************
}
