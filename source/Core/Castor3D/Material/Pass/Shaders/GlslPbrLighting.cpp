#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"

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
	PbrLightingModel::PbrLightingModel( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
		: LightingModel{ lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, true
			, enableVolumetric
			, "c3d_pbr_" }
		, m_cookTorrance{ writer, brdf }
		, m_sheen{ writer, brdf }
	{
	}

	const castor::String PbrLightingModel::getName()
	{
		return cuT( "c3d.pbr" );
	}

	LightingModelUPtr PbrLightingModel::create( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, PbrLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, enableVolumetric );
	}

	sdw::Float PbrLightingModel::getFinalTransmission( BlendComponents const & components
		, sdw::Vec3 const & incident )
	{
		return mix( components.transmission, 0.0_f, components.metalness );
	}

	sdw::Vec3 PbrLightingModel::adjustDirectAmbient( BlendComponents const & components
		, sdw::Vec3 const & directAmbient )const
	{
		return directAmbient;
	}

	sdw::Vec3 PbrLightingModel::adjustDirectSpecular( BlendComponents const & components
		, sdw::Vec3 const & directSpecular )const
	{
		return directSpecular;
	}

	sdw::Vec3 PbrLightingModel::adjustRefraction( BlendComponents const & components
		, sdw::Vec3 const & refraction )const
	{
		if ( components.hasMember( "metalness" ) )
		{
			return refraction * ( 1.0_f - components.getMember< sdw::Float >( "metalness" ) );
		}

		return refraction;
	}

	void PbrLightingModel::doFinish( BlendComponents & components )
	{
		auto ior = m_writer.declLocale( "ior"
			, m_writer.ternary( components.refractionRatio == 0.0_f
				, 1.5_f
				, sdw::Float( components.refractionRatio ) ) );
		components.f0 = vec3( Utils::computeF0( ior ) );
		components.f0 = mix( components.f0, components.colour.rgb(), vec3( components.metalness ) );

		if ( components.hasMember( "specular" ) )
		{
			auto dielectricSpecularF0 = min( components.f0 * components.specular.rgb(), vec3( 1.0_f ) );
			components.f0 = mix( dielectricSpecularF0, components.colour.rgb(), vec3( components.metalness ) );
			components.specular = components.f0;
		}
	}

	sdw::Vec3 PbrLightingModel::doComputeDiffuseTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float & isLit
		, sdw::Vec3 & output )
	{
		auto rawDiffuse = m_writer.declLocale( "rawDiffuse"
			, m_cookTorrance.computeDiffuse( radiance
				, intensity
				, lightSurface.difF()
				, components.metalness ) );
		output = doGetNdotL( lightSurface, components ) * rawDiffuse;
		return rawDiffuse;
	}

	void PbrLightingModel::doComputeSpecularTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec3 & output )
	{
		output = m_cookTorrance.computeSpecular( radiance
			, intensity
			, doGetNdotL( lightSurface, components )
			, doGetNdotH( lightSurface, components )
			, lightSurface.NdotV()
			, lightSurface.spcF()
			, components.roughness * components.roughness );
		output *= doGetNdotL( lightSurface, components );
	}

	void PbrLightingModel::doComputeCoatingTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec3 & output )
	{
		IF( m_writer, components.clearcoatFactor != 0.0_f )
		{
			lightSurface.updateN( components.clearcoatNormal );
			output = m_cookTorrance.computeSpecular( radiance
				, intensity
				, doGetNdotL( lightSurface, components )
				, doGetNdotH( lightSurface, components )
				, lightSurface.NdotV()
				, lightSurface.F()
				, components.clearcoatRoughness );
			output *= doGetNdotL( lightSurface, components );
		}
		FI;
	}

	void PbrLightingModel::doComputeSheenTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec2 & output )
	{
		IF( m_writer, !all( components.sheenFactor == vec3( 0.0_f ) ) )
		{
			output = m_sheen.compute( lightSurface
				, doGetNdotL( lightSurface, components )
				, doGetNdotH( lightSurface, components )
				, components.sheenRoughness );
		}
		FI;
	}

	sdw::Vec3 PbrLightingModel::doGetDiffuseBrdf( BlendComponents const & components
		, sdw::Vec3 const & directDiffuse
		, sdw::Vec3 const & indirectDiffuse
		, sdw::Vec3 const & directAmbient
		, sdw::Vec3 const & indirectAmbient
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedDiffuse )
	{
		return ( components.colour * ( directDiffuse + ( indirectDiffuse * ambientOcclusion ) )
			+ ( reflectedDiffuse * ambientOcclusion * directAmbient ) );
	}

	sdw::Vec3 PbrLightingModel::doGetSpecularBrdf( BlendComponents const & components
		, sdw::Vec3 const & directSpecular
		, sdw::Vec3 const & indirectSpecular
		, sdw::Vec3 const & directAmbient
		, sdw::Vec3 const & indirectAmbient
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedSpecular )
	{
		return ( adjustDirectSpecular( components, directSpecular )
			+ ( reflectedSpecular * ambientOcclusion * directAmbient )
			+ ( indirectSpecular * ambientOcclusion ) );
	}

	//***********************************************************************************************
}
