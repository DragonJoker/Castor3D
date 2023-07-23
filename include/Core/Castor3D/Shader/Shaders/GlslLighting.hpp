/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLighting_H___
#define ___C3D_GlslLighting_H___

#include "SdwModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/CompositeTypes/StorageBuffer.hpp>
#include <ShaderWriter/CompositeTypes/Struct.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

#include <unordered_set>

namespace castor3d::shader
{
	class LightingModel
	{
	public:
		C3D_API LightingModel( LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, Shadow & shadowModel
			, Lights & lights
			, bool hasIblSupport
			, bool enableVolumetric
			, std::string prefix );
		C3D_API virtual ~LightingModel() = default;

		inline LightingModelID getLightingModelId()const
		{
			return m_lightingModelId;
		}

		inline bool hasIblSupport()const
		{
			return m_hasIblSupport;
		}

		C3D_API void finish( PassShaders const & passShaders
			, SurfaceBase const & surface
			, Utils & utils
			, sdw::Vec3 const worldEye
			, BlendComponents & components );
		C3D_API sdw::Vec3 combine( DebugOutput & debugOutput
			, BlendComponents const & components
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
			, sdw::Vec3 reflectedDiffuse
			, sdw::Vec3 reflectedSpecular
			, sdw::Vec3 refracted
			, sdw::Vec3 coatReflected
			, sdw::Vec3 sheenReflected );
		C3D_API sdw::Vec3 combine( DebugOutput & debugOutput
			, BlendComponents const & components
			, sdw::Vec3 const & incident
			, sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 reflectedDiffuse
			, sdw::Vec3 reflectedSpecular
			, sdw::Vec3 refracted );

		C3D_API virtual sdw::Float getFinalTransmission( BlendComponents const & components
			, sdw::Vec3 const & incident ) = 0;
		C3D_API virtual sdw::Vec3 adjustDirectAmbient( BlendComponents const & components
			, sdw::Vec3 const & directAmbient )const = 0;
		C3D_API virtual sdw::Vec3 adjustDirectSpecular( BlendComponents const & components
			, sdw::Vec3 const & directSpecular )const = 0;
		C3D_API virtual sdw::Vec3 adjustRefraction( BlendComponents const & components
			, sdw::Vec3 const & refraction )const = 0;
		/**
		*\name
		*	Deferred renderring
		*/
		//\{
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		C3D_API void compute( DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void compute( PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void compute( SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		C3D_API sdw::Vec3 computeDiffuse( DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDiffuse( PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDiffuse( SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		//\}
		//\}

	protected:
		C3D_API void doComputeAttenuation( sdw::Float const attenuation
			, OutputComponents & output );
		C3D_API void doApplyShadows( DirectionalShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Vec2 const & lightIntensity
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void doApplyShadows( PointShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Float const lightRange
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void doApplyShadows( SpotShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Float const lightRange
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API void doApplyShadowsDiffuse( DirectionalShadowData const & light
			, sdw::Int const shadowMapIndex
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 & output );
		C3D_API void doApplyShadowsDiffuse( PointShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Float const lightRange
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 & output );
		C3D_API void doApplyShadowsDiffuse( SpotShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Float const lightRange
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 & output );
		C3D_API void doApplyVolumetric( ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec2 const & lightIntensity
			, LightSurface const & lightSurface
			, sdw::Vec3 & output
			, bool multiply );

		C3D_API virtual void doInitialiseBackground( BackgroundModel & background );
		C3D_API virtual sdw::Vec3 doComputeRadiance( Light const & light
			, sdw::Vec3 const & lightDirection )const;
		C3D_API virtual void doComputeSheenTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec2 & output );
		C3D_API virtual void doComputeScatteringTerm( ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec3 const & radiance
			, sdw::Vec2 const & lightIntensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & output );
		C3D_API virtual void doInitLightSpecifics( LightSurface const & lightSurface
			, BlendComponents const & components );
		C3D_API virtual sdw::Float doGetNdotL( LightSurface const & lightSurface
			, BlendComponents const & components );
		C3D_API virtual sdw::Float doGetNdotH( LightSurface const & lightSurface
			, BlendComponents const & components );

		C3D_API virtual void doFinish( BlendComponents & components ) = 0;
		C3D_API virtual sdw::Vec3 doComputeDiffuseTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float & isLit
			, sdw::Vec3 & output ) = 0;
		C3D_API virtual void doComputeSpecularTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec3 & output ) = 0;
		C3D_API virtual void doComputeCoatingTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec3 & output ) = 0;
		C3D_API virtual sdw::Vec3 doGetDiffuseBrdf( BlendComponents const & components
			, sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedDiffuse ) = 0;
		C3D_API virtual sdw::Vec3 doGetSpecularBrdf( BlendComponents const & components
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedSpecular ) = 0;

	private:
		C3D_API virtual sdw::Vec3 doComputeLight( Light light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & radiance
			, OutputComponents & output );
		C3D_API virtual sdw::Vec3 doComputeLightDiffuse( Light light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & radiance );

	protected:
		LightingModelID m_lightingModelId;
		sdw::ShaderWriter & m_writer;
		Materials const & m_materials;
		Utils & m_utils;
		Shadow & m_shadowModel;
		Lights & m_lights;
		bool m_hasIblSupport;
		bool m_enableVolumetric;
		std::string m_prefix;
		std::unique_ptr< sdw::Struct > m_type;
		std::unique_ptr< sdw::UInt > m_directionalCascadeIndex;
		std::unique_ptr< sdw::UInt > m_directionalCascadeCount;
		std::unique_ptr< sdw::Mat4 > m_directionalTransform;
		sdw::Function< sdw::Void
			, PDirectionalLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, PPointLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, PSpotLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, InOutDirectionalLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, InOutPointLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, InOutSpotLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt > m_computeSpotDiffuse;
	};
}

#endif
