#include "AtmosphereScattering/AtmosphereLightingModel.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
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
	{
		m_prefix = cuT( "atm_phong_" );
	}

	c3d::LightingModelUPtr AtmospherePhongLightingModel::create( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, AtmospherePhongLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdf
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
	{
		m_prefix = cuT( "atm_pbr_" );
	}

	c3d::LightingModelUPtr AtmospherePbrLightingModel::create( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, AtmospherePbrLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdf
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
