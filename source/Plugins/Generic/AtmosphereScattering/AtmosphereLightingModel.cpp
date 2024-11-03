#include "AtmosphereScattering/AtmosphereLightingModel.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp>
#include <Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslOrenNayarBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslShadow.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureAnimation.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	void AtmosphereLightingModel::initBackground( c3d::BackgroundModel & background
		, c3d::Shadow & shadowModel )
	{
		if ( !atmosphereBackground )
		{
			atmosphereBackground = &static_cast< AtmosphereBackgroundModel & >( background );
			atmosphereBackground->atmosphere.settings.shadowMapEnabled = true;
			atmosphereBackground->atmosphere.shadows = &shadowModel;
		}
	}

	sdw::Vec3 AtmosphereLightingModel::compRadiance( sdw::Vec3 const & lightDirection )const
	{
		return atmosphereBackground->getSunRadiance( lightDirection );
	}

	void AtmosphereLightingModel::compScatteringTerm( c3d::LightSurface const & lightSurface
		, sdw::Vec3 & output )
	{
		auto & writer = sdw::findWriterMandat( lightSurface, output );
		auto targetSize = vec2( sdw::Float{ float( atmosphereBackground->getTargetSize().width ) }
			, float( atmosphereBackground->getTargetSize().height ) );
		auto luminance = writer.declLocale< sdw::Vec4 >( "luminance" );
		auto transmittance = writer.declLocale< sdw::Vec4 >( "transmittance" );
		atmosphereBackground->getPixelTransLum( lightSurface.clipPosition().xy()
			, targetSize
			, lightSurface.clipPosition().z()
			, transmittance
			, luminance );
		output = luminance.xyz() / luminance.a();
	}

	//*********************************************************************************************

	AtmospherePhongLightingModel::AtmospherePhongLightingModel( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdfHelpers
		, c3d::DiffuseBRDFUPtr diffuse
		, c3d::SpecularBRDFUPtr specular
		, c3d::SheenBRDFUPtr sheen
		, c3d::ClearcoatBRDFUPtr clearcoat
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
		: c3d::PhongLightingModel{ lightingModelId
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
			, enableVolumetric }
	{
		m_prefix = cuT( "atm_phong_" );
	}

	c3d::LightingModelUPtr AtmospherePhongLightingModel::create( castor3d::LightingModelID lightingModelId
		, c3d::DiffuseBrdfDesc const & diffuseBrdf
		, c3d::SpecularBrdfDesc const & specularBrdf
		, c3d::SheenBrdfDesc const & sheenBrdf
		, c3d::ClearcoatBrdfDesc const & clearcoatBrdf
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdfHelpers
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, AtmospherePhongLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, ( diffuseBrdf.create
				? diffuseBrdf.create( writer, brdfHelpers )
				: castor3d::PhongPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
			, ( specularBrdf.create
				? specularBrdf.create( writer, brdfHelpers )
				: castor3d::PhongPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
			, ( sheenBrdf.create
				? sheenBrdf.create( writer, brdfHelpers )
				: castor3d::PhongPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
			, ( clearcoatBrdf.create
				? clearcoatBrdf.create( writer, brdfHelpers )
				: castor3d::PhongPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void AtmospherePhongLightingModel::doInitialiseBackground( c3d::BackgroundModel & background )
	{
		initBackground( background, m_shadowModel );
	}

	sdw::Vec3 AtmospherePhongLightingModel::doComputeRadiance( c3d::Light const & light
		, sdw::Vec3 const & lightDirection )const
	{
		return compRadiance( lightDirection );
	}

	void AtmospherePhongLightingModel::doComputeScatteringTerm( c3d::ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec3 const & radiance
		, sdw::Vec2 const & lightIntensity
		, c3d::BlendComponents const & components
		, c3d::LightSurface const & lightSurface
		, sdw::Vec3 output )
	{
		compScatteringTerm( lightSurface, output );
		doApplyVolumetric( shadows
			, shadowMapIndex
			, lightIntensity
			, lightSurface
			, output
			, true /*multiply*/ );
	}

	//*********************************************************************************************

	AtmospherePbrLightingModel::AtmospherePbrLightingModel( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdfHelpers
		, c3d::DiffuseBRDFUPtr diffuseBrdf
		, c3d::SpecularBRDFUPtr specularBrdf
		, c3d::SheenBRDFUPtr sheenBrdf
		, c3d::ClearcoatBRDFUPtr clearcoatBrdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
		: c3d::PbrLightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, std::move( diffuseBrdf )
			, std::move( specularBrdf )
			, std::move( sheenBrdf )
			, std::move( clearcoatBrdf )
			, shadowModel
			, lights
			, enableVolumetric }
	{
		m_prefix = cuT( "atm_pbr_" );
	}

	c3d::LightingModelUPtr AtmospherePbrLightingModel::create( castor3d::LightingModelID lightingModelId
		, c3d::DiffuseBrdfDesc const & diffuseBrdf
		, c3d::SpecularBrdfDesc const & specularBrdf
		, c3d::SheenBrdfDesc const & sheenBrdf
		, c3d::ClearcoatBrdfDesc const & clearcoatBrdf
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdfHelpers
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, AtmospherePbrLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, ( diffuseBrdf.create
				? diffuseBrdf.create( writer, brdfHelpers )
				: castor3d::PbrPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
			, ( specularBrdf.create
				? specularBrdf.create( writer, brdfHelpers )
				: castor3d::PbrPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
			, ( sheenBrdf.create
				? sheenBrdf.create( writer, brdfHelpers )
				: castor3d::PbrPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
			, ( clearcoatBrdf.create
				? clearcoatBrdf.create( writer, brdfHelpers )
				: castor3d::PbrPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void AtmospherePbrLightingModel::doInitialiseBackground( c3d::BackgroundModel & background )
	{
		initBackground( background, m_shadowModel );
	}

	sdw::Vec3 AtmospherePbrLightingModel::doComputeRadiance( c3d::Light const & light
		, sdw::Vec3 const & lightDirection )const
	{
		return compRadiance( lightDirection );
	}

	void AtmospherePbrLightingModel::doComputeScatteringTerm( c3d::ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec3 const & radiance
		, sdw::Vec2 const & lightIntensity
		, c3d::BlendComponents const & components
		, c3d::LightSurface const & lightSurface
		, sdw::Vec3 output )
	{
		compScatteringTerm( lightSurface, output );
		doApplyVolumetric( shadows
			, shadowMapIndex
			, lightIntensity
			, lightSurface
			, output
			, true /*multiply*/ );
	}

	//*********************************************************************************************
}
