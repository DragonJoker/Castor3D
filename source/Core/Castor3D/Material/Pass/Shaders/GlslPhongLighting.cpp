#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"

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
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	PhongLightingModel::PhongLightingModel( LightingModelID lightingModelId
		, sdw::ShaderWriter & m_writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
		: LightingModel{ lightingModelId
			, m_writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, false
			, enableVolumetric
			, std::string{ "c3d_phong_" } }
	{
	}

	castor::String PhongLightingModel::getName()
	{
		return cuT( "c3d.phong" );
	}

	LightingModelUPtr PhongLightingModel::create( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric )
	{
		return castor::makeUniqueDerived< LightingModel, PhongLightingModel >( lightingModelId
			, writer
			, materials
			, utils
			, brdf
			, shadowModel
			, lights
			, enableVolumetric );
	}

	sdw::Float PhongLightingModel::getFinalTransmission( BlendComponents const & components
		, sdw::Vec3 const & incident )
	{
		return components.transmission;
	}

	sdw::Vec3 PhongLightingModel::adjustDirectAmbient( BlendComponents const & components
		, sdw::Vec3 const & directAmbient )const
	{
		return  directAmbient * components.colour;
	}

	sdw::Vec3 PhongLightingModel::adjustDirectSpecular( BlendComponents const & components
		, sdw::Vec3 const & directSpecular )const
	{
		auto specular = components.getMember( "specular", vec3( 1.0_f ) );
		return directSpecular * specular;
	}

	void PhongLightingModel::doFinish( BlendComponents & components )
	{
		components.f0 = components.specular;
	}

	sdw::Vec3 PhongLightingModel::doComputeDiffuseTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float & isLit
		, sdw::Vec3 & output )
	{
		isLit = 1.0_f - step( doGetNdotL( lightSurface, components ), 0.0_f );
		auto rawDiffuse = m_writer.declLocale( "rawDiffuse"
			, radiance * intensity );
		output = isLit
			* rawDiffuse
			* doGetNdotL( lightSurface, components );
		return rawDiffuse;
	}

	void PhongLightingModel::doComputeSpecularTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec3 & output )
	{
		output = isLit
			* radiance
			* intensity
			* pow( doGetNdotH( lightSurface, components )
				, clamp( components.shininess, 1.0_f, 256.0_f ) );
	}

	void PhongLightingModel::doComputeCoatingTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec3 & output )
	{
		IF( m_writer, components.clearcoatFactor != 0.0_f )
		{
			lightSurface.updateN( components.clearcoatNormal );
			output = isLit
				* radiance
				* intensity
				* pow( doGetNdotH( lightSurface, components )
					, clamp( components.shininess, 1.0_f, 256.0_f ) );
		}
		FI;
	}

	sdw::Vec3 PhongLightingModel::doCombine( BlendComponents const & components
		, sdw::Vec3 const & incident
		, sdw::Vec3 const & directDiffuse
		, sdw::Vec3 const & indirectDiffuse
		, sdw::Vec3 const & directSpecular
		, sdw::Vec3 const & indirectSpecular
		, sdw::Vec3 const & directAmbient
		, sdw::Vec3 const & indirectAmbient
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & emissive
		, sdw::Vec3 const & reflectedDiffuse
		, sdw::Vec3 const & reflectedSpecular
		, sdw::Vec3 const & refracted )
	{
		return m_writer.ternary( components.hasTransmission != 0_u
			, components.colour * components.transmission * ( directDiffuse + ( ( reflectedDiffuse + indirectDiffuse ) * ambientOcclusion ) )
				+ ( adjustDirectSpecular( components, directSpecular ) + ( ( reflectedSpecular + indirectSpecular ) * ambientOcclusion ) )
				+ ( adjustDirectAmbient( components, directAmbient ) * indirectAmbient * ambientOcclusion )
				+ emissive
				+ refracted
			, components.colour * ( directDiffuse + ( ( reflectedDiffuse + indirectDiffuse ) * ambientOcclusion ) )
				+ ( adjustDirectSpecular( components, directSpecular ) + ( ( reflectedSpecular + indirectSpecular ) * ambientOcclusion ) )
				+ ( adjustDirectAmbient( components, directAmbient ) * indirectAmbient * ambientOcclusion )
				+ emissive
				+ refracted );
	}

	//*********************************************************************************************
}
