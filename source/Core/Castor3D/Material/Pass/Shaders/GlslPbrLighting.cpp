#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

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
			, components.roughness );
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

	sdw::Vec3 PbrLightingModel::doCombine( BlendComponents const & components
		, sdw::Vec3 const & incident
		, sdw::Vec3 const & directDiffuse
		, sdw::Vec3 const & indirectDiffuse
		, sdw::Vec3 const & directSpecular
		, sdw::Vec3 const & directScattering
		, sdw::Vec3 const & directCoatingSpecular
		, sdw::Vec2 const & directSheen
		, sdw::Vec3 const & indirectSpecular
		, sdw::Vec3 const & directAmbient
		, sdw::Vec3 const & indirectAmbient
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & emissive
		, sdw::Vec3 const & reflectedDiffuse
		, sdw::Vec3 const & reflectedSpecular
		, sdw::Vec3 const & refracted
		, sdw::Vec3 const & coatReflected
		, sdw::Vec3 const & sheenReflected )
	{
		auto diffuseBrdf = m_writer.declLocale( "diffuseBrdf"
			, components.colour * ( directDiffuse + ( indirectDiffuse * ambientOcclusion ) )
				+ ( reflectedDiffuse * adjustDirectAmbient( components, directAmbient ) * indirectAmbient * ambientOcclusion ) );
		auto specularBrdf = m_writer.declLocale( "specularBrdf"
			, ( reflectedSpecular * adjustDirectAmbient( components, directAmbient ) * indirectAmbient * ambientOcclusion )
			+ adjustDirectSpecular( components, directSpecular ) + ( indirectSpecular * ambientOcclusion ) );
		auto metal = m_writer.declLocale( "metal"
			, specularBrdf ); // Conductor Fresnel already included there.

		IF( m_writer, components.hasTransmission )
		{
			auto specularBtdf = m_writer.declLocale( "specularBtdf"
				, adjustRefraction( components, refracted ) );
			diffuseBrdf = mix( diffuseBrdf, specularBtdf, vec3( components.transmission ) );
		}
		ELSE
		{
			diffuseBrdf += refracted;
		}
		FI;

		auto dielectric = m_writer.declLocale( "dielectric"
			, specularBrdf + diffuseBrdf );
		auto combineResult = m_writer.declLocale( "combineResult"
			, mix( dielectric, metal, vec3( components.metalness ) ) );

		IF( m_writer, !all( components.sheenFactor == vec3( 0.0_f ) ) )
		{
			combineResult = sheenReflected
				+ ( components.colour * directSheen.x() )
				+ combineResult * directSheen.y() * max( max( components.colour.r(), components.colour.g() ), components.colour.b() );
		}
		FI;

		IF( m_writer, components.clearcoatFactor != 0.0_f )
		{
			auto clearcoatNdotV = m_writer.declLocale( "clearcoatNdotV"
				, max( dot( components.clearcoatNormal, -incident ), 0.0_f ) );
			auto clearcoatFresnel = m_writer.declLocale( "clearcoatFresnel"
				, pow( 0.04_f + ( 1.0_f - 0.04_f ) * ( 1.0_f - clearcoatNdotV ), 5.0_f ) );
			combineResult = mix( combineResult
				, coatReflected + directCoatingSpecular
				, vec3( components.clearcoatFactor * clearcoatFresnel ) );
		}
		FI;

		return combineResult
			+ emissive
			+ directScattering;
	}

	//***********************************************************************************************
}
