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
	class ScatteringModel
	{
	public:
		C3D_API ScatteringModel( sdw::ShaderWriter & writer );
		C3D_API virtual ~ScatteringModel() = default;

		C3D_API virtual void initialiseBackground( BackgroundModel & background
			, Shadow & shadowModel );
		C3D_API virtual sdw::Vec3 computeRadiance( Light const & light
			, sdw::Vec3 const & lightDirection )const;
		C3D_API virtual void computeScattering( LightingModel & lighting
			, ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec3 const & radiance
			, sdw::Vec2 const & lightIntensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 output );

		static castor::StringView constexpr Name{ cuT( "default" ) };
		static ScatteringModelPtr create( sdw::ShaderWriter & writer );

	protected:
		sdw::ShaderWriter & m_writer;
	};

	class LightingModel
	{
	public:
		C3D_API LightingModel( LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdfHelpers
			, LightingModelSpec spec
			, Shadow & shadowModel
			, Lights & lights
			, bool hasIblSupport
			, bool hasBackgroundReflectionsSupport
			, bool hasBackgroundRefractionSupport
			, bool enableVolumetric
			, castor::String prefix );
		C3D_API virtual ~LightingModel() = default;

		LightingModelID getLightingModelId()const
		{
			return m_lightingModelId;
		}

		bool hasIblSupport()const
		{
			return m_hasIblSupport;
		}

		bool hasBackgroundReflectionsSupport()const
		{
			return m_hasBackgroundReflectionsSupport;
		}

		bool hasBackgroundRefractionSupport()const
		{
			return m_hasBackgroundRefractionSupport;
		}

		C3D_API virtual sdw::Vec3 combine( DebugOutput & debugOutput
			, ReflectionModel & reflections
			, sdw::CombinedImage2DRgba32 const & brdf
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & incident
			, DirectLighting const & directLighting
			, IndirectLighting const & indirectLighting
			, sdw::Float const & ambientOcclusion
			, ReflectionRefraction const & reflRefr );
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
		C3D_API void compute( DebugOutputCategory const & debugOutput
			, DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		C3D_API void compute( DebugOutputCategory const & debugOutput
			, PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		C3D_API void compute( DebugOutputCategory const & debugOutput
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
		C3D_API sdw::Vec3 computeDiffuse( DebugOutputCategory const & debugOutput
			, DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDiffuse( DebugOutputCategory const & debugOutput
			, PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows );
		C3D_API sdw::Vec3 computeDiffuse( DebugOutputCategory const & debugOutput
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
		C3D_API void computeAllButDiffuse( DebugOutputCategory const & debugOutput
			, DirectionalLight const & light
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		C3D_API void computeAllButDiffuse( DebugOutputCategory const & debugOutput
			, PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		C3D_API void computeAllButDiffuse( DebugOutputCategory const & debugOutput
			, SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, DirectLighting & output );
		//\}
		//\}
		C3D_API void applyVolumetric( ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec2 const & lightIntensity
			, LightSurface const & lightSurface
			, sdw::Vec3 output
			, bool multiply );

	protected:
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

		C3D_API virtual void doInitLightSpecifics( LightSurface const & lightSurface
			, BlendComponents const & components );
		C3D_API virtual DerivFloat doGetNdotL( LightSurface const & lightSurface
			, BlendComponents const & components );
		C3D_API virtual DerivFloat doGetNdotH( LightSurface const & lightSurface
			, BlendComponents const & components );

		C3D_API virtual sdw::Vec3 doGetDiffuseResult( BlendComponents const & components
			, DirectLighting const & lighting
			, IndirectLighting const & indirect
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedDiffuse ) = 0;
		C3D_API virtual sdw::Vec3 doGetSpecularResult( BlendComponents const & components
			, DirectLighting const & lighting
			, IndirectLighting const & indirect
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedSpecular ) = 0;

	private:
		C3D_API virtual void doComputeLight( Light const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & attenuation
			, sdw::Vec3 & radiance
			, DirectLighting & output );
		C3D_API sdw::Vec3 doComputeLightDiffuse( Light const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & attenuation
			, sdw::Vec3 & radiance );
		C3D_API virtual void doInternalComputeLightDiffuse( Light const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & attenuation
			, sdw::Vec3 const & radiance
			, sdw::Vec3 & result );
		C3D_API void doComputeLightAllButDiffuse( Light const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & attenuation
			, sdw::Vec3 & radiance
			, DirectLighting & output );
		C3D_API virtual sdw::Vec3 doInternalComputeLightSpecular( Light const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & attenuation
			, sdw::Vec3 const & radiance
			, DirectLighting & output );
		C3D_API virtual void doInternalComputeLayers( BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & lightIntensity
			, DirectLighting & output );

	protected:
		LightingModelID m_lightingModelId;
		sdw::ShaderWriter & m_writer;
		Materials const & m_materials;
		Utils & m_utils;
		BRDFHelpers & m_brdfHelpers;
		Shadow & m_shadowModel;
		Lights & m_lights;
		DiffuseBRDFPtr m_diffuse;
		SpecularBRDFPtr m_specular;
		SheenBRDFPtr m_sheen;
		ClearcoatBRDFPtr m_clearcoat;
		ScatteringModelPtr m_scattering;
		bool m_hasBackgroundReflectionsSupport;
		bool m_hasBackgroundRefractionSupport;
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
