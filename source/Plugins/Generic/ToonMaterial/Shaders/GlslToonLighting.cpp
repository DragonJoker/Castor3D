#include "ToonMaterial/Shaders/GlslToonLighting.hpp"

#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp>
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

namespace toon::shader
{
	//*********************************************************************************************

	void ToonLightingModel::initLightSpecifics( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		auto & writer = *lightSurface.getWriter();
		auto smoothBand = components.getMember< sdw::Float >( "smoothBand", true );
		auto ndotl = c3ds::DerivFloat{ smoothStep( 0.0_f
				, fwidth( lightSurface.NdotL() ) * smoothBand
				, lightSurface.NdotL().value() )
			, lightSurface.NdotL().dPdx()
			, lightSurface.NdotL().dPdy() };

		if ( !m_NdotL )
		{
			m_NdotL = c3d::makeRawUnique< c3ds::DerivFloat >( writer.declLocale( "toonNdotL"
				, c3d::move( ndotl ) ) );
		}
		else
		{
			*m_NdotL = ndotl;
		}

		auto ndoth = c3ds::DerivFloat{ smoothStep( 0.0_f
				, 0.01_f * smoothBand
				, ( lightSurface.NdotH() * getNdotL( lightSurface, components ) ).value() )
			, lightSurface.NdotH().dPdx()
			, lightSurface.NdotH().dPdy() };

		if ( !m_NdotH )
		{
			m_NdotH = c3d::makeRawUnique< c3ds::DerivFloat >( writer.declLocale( "toonNdotH"
				, c3d::move( ndoth ) ) );
		}
		else
		{
			*m_NdotH = ndoth;
		}
	}

	c3ds::DerivFloat ToonLightingModel::getNdotL( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		return *m_NdotL;
	}

	c3ds::DerivFloat ToonLightingModel::getNdotH( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		return *m_NdotH;
	}

	//*********************************************************************************************

	ToonPhongLightingModel::ToonPhongLightingModel( c3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3ds::Materials const & materials
		, c3ds::Utils & utils
		, c3ds::BRDFHelpers & brdfHelpers
		, c3ds::LightingModelSpec spec
		, c3ds::Shadow & shadowModel
		, c3ds::Lights & lights
		, bool enableVolumetric )
		: c3ds::PhongLightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, std::move( spec )
			, shadowModel
			, lights
			, enableVolumetric }
	{
		m_prefix = cuT( "toon_phong_" );
	}

	const c3d::String ToonPhongLightingModel::getName()
	{
		return cuT( "toon.phong" );
	}

	c3ds::LightingModelPtr ToonPhongLightingModel::create( c3d::LightingModelID lightingModelId
		, c3ds::LightingModelDesc const & desc
		, sdw::ShaderWriter & writer
		, c3ds::Materials const & materials
		, c3ds::Utils & utils
		, c3ds::BRDFHelpers & brdfHelpers
		, c3ds::Shadow & shadowModel
		, c3ds::Lights & lights
		, bool enableVolumetric )
	{
		return c3d::makeUniqueDerived< c3ds::LightingModel, ToonPhongLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, c3ds::LightingModelSpec{ ( desc.diffuse.create
					? desc.diffuse.create( writer, brdfHelpers )
					: c3d::PhongPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
				, ( desc.specular.create
					? desc.specular.create( writer, brdfHelpers )
					: c3d::PhongPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
				, ( desc.sheen.create
					? desc.sheen.create( writer, brdfHelpers )
					: c3d::PhongPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
				, ( desc.clearcoat.create
					? desc.clearcoat.create( writer, brdfHelpers )
					: c3d::PhongPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
				, ( desc.scattering.create
					? desc.scattering.create( writer )
					: c3d::PhongPass::DefaultScatteringModel.create( writer ) ) }
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void ToonPhongLightingModel::doInitLightSpecifics( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		if ( components.hasMember( "smoothBand" ) )
		{
			initLightSpecifics( lightSurface, components );
		}
		else
		{
			c3ds::PhongLightingModel::doInitLightSpecifics( lightSurface, components );
		}
	}

	c3ds::DerivFloat ToonPhongLightingModel::doGetNdotL( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		if ( components.hasMember( "smoothBand" ) )
		{
			return getNdotL( lightSurface, components );
		}

		return c3ds::PhongLightingModel::doGetNdotL( lightSurface, components );
	}

	c3ds::DerivFloat ToonPhongLightingModel::doGetNdotH( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		if ( components.hasMember( "smoothBand" ) )
		{
			return getNdotH( lightSurface, components );
		}

		return c3ds::PhongLightingModel::doGetNdotH( lightSurface, components );
	}

	//*********************************************************************************************

	ToonPbrLightingModel::ToonPbrLightingModel( c3d::LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, c3ds::Materials const & materials
		, c3ds::Utils & utils
		, c3ds::BRDFHelpers & brdfHelpers
		, c3ds::LightingModelSpec spec
		, c3ds::Shadow & shadowModel
		, c3ds::Lights & lights
		, bool enableVolumetric )
		: c3ds::PbrLightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, std::move( spec )
			, shadowModel
			, lights
			, enableVolumetric }
	{
		m_prefix = cuT( "toon_pbr_" );
	}

	const c3d::String ToonPbrLightingModel::getName()
	{
		return cuT( "toon.pbr" );
	}

	c3ds::LightingModelPtr ToonPbrLightingModel::create( c3d::LightingModelID lightingModelId
		, c3ds::LightingModelDesc const & desc
		, sdw::ShaderWriter & writer
		, c3ds::Materials const & materials
		, c3ds::Utils & utils
		, c3ds::BRDFHelpers & brdfHelpers
		, c3ds::Shadow & shadowModel
		, c3ds::Lights & lights
		, bool enableVolumetric )
	{
		return c3d::makeUniqueDerived< c3ds::LightingModel, ToonPbrLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, c3ds::LightingModelSpec{ ( desc.diffuse.create
					? desc.diffuse.create( writer, brdfHelpers )
					: c3d::PbrPass::DefaultDiffuseBrdf.create( writer, brdfHelpers ) )
				, ( desc.specular.create
					? desc.specular.create( writer, brdfHelpers )
					: c3d::PbrPass::DefaultSpecularBrdf.create( writer, brdfHelpers ) )
				, ( desc.sheen.create
					? desc.sheen.create( writer, brdfHelpers )
					: c3d::PbrPass::DefaultSheenBrdf.create( writer, brdfHelpers ) )
				, ( desc.clearcoat.create
					? desc.clearcoat.create( writer, brdfHelpers )
					: c3d::PbrPass::DefaultClearcoatBrdf.create( writer, brdfHelpers ) )
				, ( desc.scattering.create
					? desc.scattering.create( writer )
					: c3d::PbrPass::DefaultScatteringModel.create( writer ) )}
			, shadowModel
			, lights
			, enableVolumetric );
	}

	void ToonPbrLightingModel::doInitLightSpecifics( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		if ( components.hasMember( "smoothBand" ) )
		{
			initLightSpecifics( lightSurface, components );
		}
		else
		{
			c3ds::PbrLightingModel::doInitLightSpecifics( lightSurface, components );
		}
	}

	c3ds::DerivFloat ToonPbrLightingModel::doGetNdotL( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		if ( components.hasMember( "smoothBand" ) )
		{
			return getNdotL( lightSurface, components );
		}

		return c3ds::PbrLightingModel::doGetNdotL( lightSurface, components );
	}

	c3ds::DerivFloat ToonPbrLightingModel::doGetNdotH( c3ds::LightSurface const & lightSurface
		, c3ds::BlendComponents const & components )
	{
		if ( components.hasMember( "smoothBand" ) )
		{
			return getNdotH( lightSurface, components );
		}

		return c3ds::PbrLightingModel::doGetNdotH( lightSurface, components );
	}

	//*********************************************************************************************
}
