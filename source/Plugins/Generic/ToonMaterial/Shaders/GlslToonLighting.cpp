#include "ToonMaterial/Shaders/GlslToonLighting.hpp"

#include <Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp>
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

namespace toon::shader
{
	//*********************************************************************************************

	void ToonLightingModel::initLightSpecifics( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		auto & writer = *lightSurface.getWriter();
		auto smoothBand = components.getMember< sdw::Float >( "smoothBand", true );
		auto ndotl = smoothStep( 0.0_f
			, fwidth( lightSurface.NdotL() ) * smoothBand
			, lightSurface.NdotL() );

		if ( !m_NdotL )
		{
			m_NdotL = castor::make_unique< sdw::Float >( writer.declLocale( "toonNdotL", castor::move( ndotl ) ) );
		}
		else
		{
			*m_NdotL = ndotl;
		}

		auto ndoth = smoothStep( 0.0_f
			, 0.01_f * smoothBand
			, lightSurface.NdotH() * getNdotL( lightSurface, components ) );

		if ( !m_NdotH )
		{
			m_NdotH = castor::make_unique< sdw::Float >( writer.declLocale( "toonNdotH", castor::move( ndoth ) ) );
		}
		else
		{
			*m_NdotH = ndoth;
		}
	}

	sdw::Float ToonLightingModel::getNdotL( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		return *m_NdotL;
	}

	sdw::Float ToonLightingModel::getNdotH( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		return *m_NdotH;
	}

	//*********************************************************************************************

	ToonPhongLightingModel::ToonPhongLightingModel( castor3d::LightingModelID lightingModelId
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
		m_prefix = cuT( "toon_phong_" );
	}

	const castor::String ToonPhongLightingModel::getName()
	{
		return cuT( "toon.phong" );
	}

	c3d::LightingModelUPtr ToonPhongLightingModel::create( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< c3d::LightingModel, ToonPhongLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void ToonPhongLightingModel::doInitLightSpecifics( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		initLightSpecifics( lightSurface, components );
	}

	sdw::Float ToonPhongLightingModel::doGetNdotL( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		return getNdotL( lightSurface, components );
	}

	sdw::Float ToonPhongLightingModel::doGetNdotH( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		return getNdotH( lightSurface, components );
	}

	//*********************************************************************************************

	ToonPbrLightingModel::ToonPbrLightingModel( castor3d::LightingModelID lightingModelId
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
		m_prefix = cuT( "toon_pbr_" );
	}

	const castor::String ToonPbrLightingModel::getName()
	{
		return cuT( "toon.pbr" );
	}

	c3d::LightingModelUPtr ToonPbrLightingModel::create( castor3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::BRDFHelpers & brdf
		, c3d::Shadow & shadowModel
		, c3d::Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< c3d::LightingModel, ToonPbrLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void ToonPbrLightingModel::doInitLightSpecifics( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		initLightSpecifics( lightSurface, components );
	}

	sdw::Float ToonPbrLightingModel::doGetNdotL( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		return getNdotL( lightSurface, components );
	}

	sdw::Float ToonPbrLightingModel::doGetNdotH( c3d::LightSurface const & lightSurface
		, c3d::BlendComponents const & components )
	{
		return getNdotH( lightSurface, components );
	}

	//*********************************************************************************************
}
