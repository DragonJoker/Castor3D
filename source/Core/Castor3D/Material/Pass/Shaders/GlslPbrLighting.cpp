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
		, DiffuseBRDFUPtr diffuse
		, SpecularBRDFUPtr specular
		, SheenBRDFUPtr sheen
		, ClearcoatBRDFUPtr clearcoat
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
		: LightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdfHelpers
			, shadowModel
			, lights
			, true
			, true
			, true
			, enableVolumetric
			, cuT( "c3d_pbr_" ) }
		, m_diffuse{ std::move( diffuse ) }
		, m_specular{ std::move( specular ) }
		, m_sheen{ std::move( sheen ) }
		, m_clearcoat{ std::move( clearcoat ) }
	{
	}

	castor::StringView PbrLightingModel::getName()
	{
		return cuT( "c3d.pbr" );
	}

	LightingModelUPtr PbrLightingModel::create( LightingModelID lightingModelId
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

	void PbrLightingModel::adjustDirectLighting( BlendComponents const & components
		, DirectLighting & lighting )const
	{
	}

	void PbrLightingModel::doFinish( PassShaders const & passShaders
		, BlendComponents & components )
	{
		auto ior = m_writer.declLocale( "ior"
			, m_writer.ternary( components.refractionRatio == 0.0_f
				, 1.5_f
				, sdw::Float{ components.refractionRatio } ) );
		components.f0 = vec3( Utils::computeF0( ior ) );
		components.f0 = mix( components.f0, components.colour.rgb(), vec3( components.metalness ) );

		if ( components.hasMember( "specular" ) )
		{
			auto dielectricSpecularF0 = m_writer.declLocale( "dielectricSpecularF0"
				, min( components.f0 * components.specular.rgb(), vec3( 1.0_f ) ) );
			components.f0 = mix( dielectricSpecularF0, components.colour.rgb(), vec3( components.metalness ) );
			components.specular = components.f0;
		}
	}

	sdw::Vec3 PbrLightingModel::doComputeDiffuseTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float & isLit
		, sdw::Vec3 output )
	{
		auto rawDiffuse = m_writer.declLocale( "rawDiffuse"
			, m_diffuse->compute( components
				, lightSurface
				, radiance
				, intensity
				, doGetNdotL( lightSurface, components ).value() ) );
		output = doGetNdotL( lightSurface, components ).value() * rawDiffuse;
		return rawDiffuse;
	}

	void PbrLightingModel::doComputeSpecularTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec3 output )
	{
		output = m_specular->compute( components
			, lightSurface
			, radiance
			, intensity
			, doGetNdotL( lightSurface, components ).value()
			, doGetNdotH( lightSurface, components ).value() );
		output *= doGetNdotL( lightSurface, components ).value();
	}

	void PbrLightingModel::doComputeCoatingTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec3 output )
	{
		IF( m_writer, components.clearcoatFactor != 0.0_f )
		{
			lightSurface.updateN( derivVec3( components.clearcoatNormal ) );
			output = m_clearcoat->compute( components
				, lightSurface
				, radiance
				, intensity
				, doGetNdotL( lightSurface, components ).value()
				, doGetNdotH( lightSurface, components ).value() );
			output *= doGetNdotL( lightSurface, components ).value();
		}
		FI;
	}

	void PbrLightingModel::doComputeSheenTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec4 output )
	{
		IF( m_writer, !all( components.sheenColour == vec3( 0.0_f ) ) )
		{
			output = m_sheen->compute( m_utils
				, components
				, lightSurface
				, doGetNdotL( lightSurface, components ).value()
				, doGetNdotH( lightSurface, components ).value() );
		}
		FI;
	}

	sdw::Vec3 PbrLightingModel::doGetDiffuseResult( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedDiffuse )
	{
		return ( components.colour * ( lighting.diffuse() + ( indirect.diffuseColour() * ambientOcclusion ) )
			+ ( reflectedDiffuse * ambientOcclusion * lighting.ambient() ) );
	}

	sdw::Vec3 PbrLightingModel::doGetSpecularResult( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedSpecular )
	{
		return ( lighting.specular()
			+ ( reflectedSpecular * ambientOcclusion * lighting.ambient() )
			+ ( indirect.specular() * ambientOcclusion ) );
	}

	//***********************************************************************************************
}
