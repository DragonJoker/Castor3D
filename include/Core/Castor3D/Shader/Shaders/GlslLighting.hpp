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
			, castor::String prefix );
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
			, RasterizerSurfaceBase const & rasterSurface
			, SurfaceBase const & surface
			, Utils & utils
			, sdw::Vec3 const worldEye
			, BlendComponents & components );
		C3D_API sdw::Vec3 combine( DebugOutput & debugOutput
			, BlendComponents const & components
			, sdw::Vec3 const & incident
			, DirectLighting directLighting
			, IndirectLighting indirectLighting
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 reflectedDiffuse
			, sdw::Vec3 reflectedSpecular
			, sdw::Vec3 refracted
			, sdw::Vec3 coatReflected
			, sdw::Vec3 sheenReflected );
		C3D_API virtual sdw::Vec3 combine( DebugOutput & debugOutput
			, BlendComponents const & components
			, sdw::Vec3 const & incident
			, DirectLighting directLighting
			, IndirectLighting indirectLighting
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 reflectedDiffuse
			, sdw::Vec3 reflectedSpecular
			, sdw::Vec3 refracted );
		
		C3D_API virtual void adjustDirectLighting( BlendComponents const & components
			, DirectLighting & lighting )const = 0;
		/**
		*\name
		*	Clustered lighting
		*/
		//\{
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		C3D_API void compute( DebugOutput & debugOutput
			, DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		C3D_API void compute( DebugOutput & debugOutput
			, PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		C3D_API void compute( DebugOutput & debugOutput
			, SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		C3D_API sdw::Vec3 computeDiffuse( DebugOutput & debugOutput
			, DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDiffuse( DebugOutput & debugOutput
			, PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDiffuse( DebugOutput & debugOutput
			, SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		//\}
		/**
		*\name
		*	All but diffuse
		*/
		//\{
		C3D_API void computeAllButDiffuse( DebugOutput & debugOutput
			, DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		C3D_API void computeAllButDiffuse( DebugOutput & debugOutput
			, PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		C3D_API void computeAllButDiffuse( DebugOutput & debugOutput
			, SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		//\}
		//\}

	protected:
		C3D_API void doAttenuate( sdw::Float const attenuation
			, DirectLighting & output
			, bool withDiffuse = true );
		C3D_API void doApplyShadows( DirectionalShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Vec2 const & lightIntensity
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, DirectLighting & output
			, bool withDiffuse = true );
		C3D_API void doApplyShadows( PointShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Float const lightRange
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, DirectLighting & output
			, bool withDiffuse = true );
		C3D_API void doApplyShadows( SpotShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Float const lightRange
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::UInt const & receivesShadows
			, DirectLighting & output
			, bool withDiffuse = true );
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
			, sdw::Vec3 output
			, bool multiply );

		C3D_API virtual void doInitialiseBackground( BackgroundModel & background );
		C3D_API virtual sdw::Vec3 doComputeRadiance( Light const & light
			, sdw::Vec3 const & lightDirection )const;
		C3D_API virtual void doComputeSheenTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec2 output );
		C3D_API virtual void doComputeScatteringTerm( ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec3 const & radiance
			, sdw::Vec2 const & lightIntensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 output );
		C3D_API virtual void doInitLightSpecifics( LightSurface const & lightSurface
			, BlendComponents const & components );
		C3D_API virtual sdw::Float doGetNdotL( LightSurface const & lightSurface
			, BlendComponents const & components );
		C3D_API virtual sdw::Float doGetNdotH( LightSurface const & lightSurface
			, BlendComponents const & components );

		C3D_API virtual void doFinish( PassShaders const & passShaders
			, RasterizerSurfaceBase const & rasterSurface
			, BlendComponents & components ) = 0;
		C3D_API virtual sdw::Vec3 doComputeDiffuseTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float & isLit
			, sdw::Vec3 output ) = 0;
		C3D_API virtual void doComputeSpecularTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec3 output ) = 0;
		C3D_API virtual void doComputeCoatingTerm( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & isLit
			, sdw::Vec3 output ) = 0;
		C3D_API virtual sdw::Vec3 doGetDiffuseBrdf( BlendComponents const & components
			, DirectLighting const & lighting
			, IndirectLighting const & indirect
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedDiffuse ) = 0;
		C3D_API virtual sdw::Vec3 doGetSpecularBrdf( BlendComponents const & components
			, DirectLighting const & lighting
			, IndirectLighting const & indirect
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedSpecular ) = 0;

	private:
		C3D_API virtual sdw::Vec3 doComputeLight( Light light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & radiance
			, DirectLighting & output );
		C3D_API virtual sdw::Vec3 doComputeLightDiffuse( Light light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & radiance );
		C3D_API virtual void doComputeLightAllButDiffuse( Light light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 & radiance
			, DirectLighting & output );

	protected:
		LightingModelID m_lightingModelId;
		sdw::ShaderWriter & m_writer;
		Materials const & m_materials;
		Utils & m_utils;
		Shadow & m_shadowModel;
		Lights & m_lights;
		bool m_hasIblSupport;
		bool m_enableVolumetric;
		castor::String m_prefix;
		castor::RawUniquePtr< sdw::Struct > m_type;
		castor::RawUniquePtr< sdw::UInt > m_directionalCascadeIndex;
		castor::RawUniquePtr< sdw::UInt > m_directionalCascadeCount;
		castor::RawUniquePtr< sdw::Mat4 > m_directionalTransform;
		sdw::Function< sdw::Void
			, PDirectionalLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, InOutDirectLighting > m_computeDirectional;
		sdw::Function< sdw::Void
			, PPointLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, InOutDirectLighting > m_computePoint;
		sdw::Function< sdw::Void
			, PSpotLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, InOutDirectLighting > m_computeSpot;
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
		sdw::Function< sdw::Void
			, PDirectionalLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, InOutDirectLighting > m_computeDirectionalAllButDiffuse;
		sdw::Function< sdw::Void
			, PPointLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, InOutDirectLighting > m_computePointAllButDiffuse;
		sdw::Function< sdw::Void
			, PSpotLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, InOutDirectLighting > m_computeSpotAllButDiffuse;
	};
}

#endif
