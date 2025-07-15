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

namespace c3d::shader
{
	class ScatteringModel
	{
	public:
		C3D_API explicit ScatteringModel( sdw::ShaderWriter & writer );
		C3D_API virtual ~ScatteringModel() = default;

		C3D_API virtual void initialiseBackground( BackgroundModel & background
			, Shadow & shadowModel );
		C3D_API virtual sdw::Vec3 computeRadiance( Light const & light
			, sdw::Vec3 const & lightDirection )const;
		C3D_API virtual void computeScattering( LightingModel & lighting
			, ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec3 const & radiance
			, sdw::Float const & lightIntensity
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 output );

		static StringView constexpr Name{ cuT( "default" ) };
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
			, String prefix );
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

		C3D_API sdw::Vec3 combine( DebugOutput & debugOutput
			, ReflectionModel & reflections
			, sdw::CombinedImage2DRgba32 const & brdf
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & incident
			, sdw::Float const & ambientOcclusion
			, DirectLighting const & directLighting
			, IndirectLighting const & indirectLighting
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
			, sdw::Vec3 const & diffuse
			, DirectLighting & output );
		C3D_API void computeAllButDiffuse( DebugOutputCategory const & debugOutput
			, PointLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 const & diffuse
			, DirectLighting & output );
		C3D_API void computeAllButDiffuse( DebugOutputCategory const & debugOutput
			, SpotLight const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const & receivesShadows
			, sdw::Vec3 const & diffuse
			, DirectLighting & output );
		//\}
		//\}
		C3D_API void applyVolumetric( ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Float const & lightIntensity
			, LightSurface const & lightSurface
			, sdw::Vec3 output
			, bool multiply );

	protected:
		C3D_API void doApplyShadows( DirectionalShadowData const & light
			, sdw::Int const shadowMapIndex
			, sdw::Float const & lightIntensity
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
		C3D_API virtual void doComputeBackgroundLayers( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, ReflectionRefraction const & reflRefr
			, sdw::Vec3 const & clearcoatFresnel
			, sdw::Vec3 & backgroundResult );

	private:
		sdw::Vec3 doInternalComputeLightRawDiffuse( BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & diffuseLightIntensity
			, sdw::Vec3 const & radiance );
		void doInternalComputeLightDiffuse( Light const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & diffuseLightIntensity
			, sdw::Vec3 const & rawDiffuse
			, sdw::Vec3 & result );
		sdw::Vec3 doInternalComputeLightSpecular( Light const & light
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & lightIntensity
			, sdw::Vec3 const & radiance
			, DirectLighting & output );
		void doInternalComputeLayers( BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & lightIntensity
			, DirectLighting & output );
		virtual void doCombine( DebugOutputCategory const & debugOutput
			, ReflectionModel & reflections
			, sdw::CombinedImage2DRgba32 const & brdf
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & incident
			, sdw::Float const & ambientOcclusion
			, DirectLighting const & directLighting
			, IndirectLighting const & indirectLighting
			, ReflectionRefraction const & reflRefr
			, sdw::Vec3 & combineResult ) = 0;

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
		String m_prefix;
		RawUniquePtr< sdw::Struct > m_type;
		RawUniquePtr< sdw::UInt > m_directionalCascadeIndex;
		RawUniquePtr< sdw::UInt > m_directionalCascadeCount;
		RawUniquePtr< sdw::Mat4 > m_directionalTransform;
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
			, sdw::InVec3
			, InOutDirectLighting > m_computeDirectionalAllButDiffuse;
		sdw::Function< sdw::Void
			, PPointLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, sdw::InVec3
			, InOutDirectLighting > m_computePointAllButDiffuse;
		sdw::Function< sdw::Void
			, PSpotLight
			, InBlendComponents
			, InLightSurface
			, sdw::InUInt
			, sdw::InVec3
			, InOutDirectLighting > m_computeSpotAllButDiffuse;
	};
}

#endif
