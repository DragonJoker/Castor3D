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

	void PhongLightingModel::adjustDirectLighting( BlendComponents const & components
		, DirectLighting & lighting )const
	{
		lighting.ambient() *= components.colour;
		lighting.specular() *= components.specular;
	}

	void PhongLightingModel::doFinish( PassShaders const & passShaders
		, BlendComponents & components )
	{
		components.f0 = components.specular;
	}

	sdw::Vec3 PhongLightingModel::doComputeDiffuseTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float & isLit
		, sdw::Vec3 output )
	{
		isLit = 1.0_f - step( doGetNdotL( lightSurface, components ).value(), 0.0_f );
		auto rawDiffuse = m_writer.declLocale( "rawDiffuse"
			, radiance * intensity );
		output = isLit
			* rawDiffuse
			* doGetNdotL( lightSurface, components ).value();
		return rawDiffuse;
	}

	void PhongLightingModel::doComputeSpecularTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec3 output )
	{
		output = isLit
			* radiance
			* intensity
			* pow( doGetNdotH( lightSurface, components ).value()
				, clamp( components.shininess, 1.0_f, 256.0_f ) );
	}

	void PhongLightingModel::doComputeCoatingTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec3 output )
	{
		IF( m_writer, components.clearcoatFactor != 0.0_f )
		{
			lightSurface.updateN( derivVec3( components.clearcoatNormal ) );
			output = isLit
				* radiance
				* intensity
				* pow( doGetNdotH( lightSurface, components ).value()
					, clamp( components.shininess, 1.0_f, 256.0_f ) );
		}
		FI;
	}

	sdw::Vec3 PhongLightingModel::doGetDiffuseBrdf( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedDiffuse )
	{
		return ( components.colour * ( lighting.diffuse() + ( ( indirect.diffuseColour() + reflectedDiffuse ) * ambientOcclusion ) )
			+ lighting.ambient() * indirect.ambient() * ambientOcclusion );
	}

	sdw::Vec3 PhongLightingModel::doGetSpecularBrdf( BlendComponents const & components
		, DirectLighting const & lighting
		, IndirectLighting const & indirect
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & reflectedSpecular )
	{
		return ( lighting.specular()
			+ ( ( reflectedSpecular + indirect.specular() ) * ambientOcclusion ) );
	}

	//*********************************************************************************************
}
