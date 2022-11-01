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

namespace castor3d::shader
{
	C3D_API castor::String getLightingModelName( Engine const & engine
		, PassTypeID passType );

	class LightingModel
	{
	public:
		C3D_API LightingModel( sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric
			, std::string prefix );
		C3D_API virtual ~LightingModel() = default;
		C3D_API virtual sdw::Float getFinalTransmission( BlendComponents const & components
			, sdw::Vec3 const & incident ) = 0;
		C3D_API sdw::Vec3 combine( BlendComponents const & components
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
		C3D_API virtual sdw::Vec3 adjustDirectAmbient( BlendComponents const & components
			, sdw::Vec3 const & directAmbient )const = 0;
		C3D_API virtual sdw::Vec3 adjustDirectSpecular( BlendComponents const & components
			, sdw::Vec3 const & directSpecular )const = 0;
		C3D_API sdw::Vec3 combine( BlendComponents const & components
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
			, sdw::Vec3 reflectedDiffuse
			, sdw::Vec3 reflectedSpecular
			, sdw::Vec3 refracted
			, sdw::Vec3 coatReflected
			, sdw::Vec3 sheenReflected );
		C3D_API virtual ReflectionModelPtr getReflectionModel( uint32_t & envMapBinding
			, uint32_t envMapSet )const = 0;
		/**
		*\name
		*	Forward renderring
		*/
		//\{
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		C3D_API void declareModel( uint32_t lightsBufBinding
			, uint32_t lightsBufSet
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet );
		C3D_API void computeCombined( BlendComponents const & components
			, SceneData const & sceneData
			, BackgroundModel & background
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output );
		C3D_API static LightingModelPtr createModel( Engine const & engine
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, castor::String const & name
			, uint32_t lightsBufBinding
			, uint32_t lightsBufSet
			, ShadowOptions const & shadows
			, SssProfiles const * sssProfiles
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool enableVolumetric );
		template< typename LightsBufBindingT >
		static LightingModelPtr createModelT( Engine const & engine
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, castor::String const & name
			, LightsBufBindingT lightsBufBinding
			, uint32_t lightsBufSet
			, ShadowOptions const & shadows
			, SssProfiles const * sssProfiles
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool enableVolumetric )
		{
			return createModel( engine
				, materials
				, utils
				, brdf
				, name
				, uint32_t( lightsBufBinding )
				, lightsBufSet
				, shadows
				, sssProfiles
				, shadowMapBinding
				, shadowMapSet
				, enableVolumetric );
		}
		template< typename LightBindingT >
		static LightingModelPtr createModel( Engine const & engine
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, castor::String const & name
			, LightType light
			, LightBindingT lightBinding
			, uint32_t lightSet
			, bool lightUbo
			, ShadowOptions const & shadows
			, SssProfiles const * sssProfiles
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet )
		{
			return createModel( engine
				, materials
				, utils
				, brdf
				, name
				, light
				, lightUbo
				, uint32_t( lightBinding )
				, lightSet
				, shadows
				, sssProfiles
				, shadowMapBinding
				, shadowMapSet );
		}
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		C3D_API void declareDiffuseModel( uint32_t lightsBufBinding
			, uint32_t lightsBufSet
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet );
		C3D_API sdw::Vec3 computeCombinedDiffuse( BlendComponents const & components
			, SceneData const & sceneData
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows );
		C3D_API static LightingModelPtr createDiffuseModel( Engine const & engine
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, castor::String const & name
			, uint32_t lightsBufBinding
			, uint32_t lightsBufSet
			, ShadowOptions const & shadows
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool enableVolumetric );
		template< typename LightsBufBindingT >
		static LightingModelPtr createDiffuseModelT( Engine const & engine
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, castor::String const & name
			, LightsBufBindingT lightsBufBinding
			, uint32_t lightsBufSet
			, ShadowOptions const & shadows
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool enableVolumetric )
		{
			return createDiffuseModel( engine
				, materials
				, utils
				, brdf
				, name
				, uint32_t( lightsBufBinding )
				, lightsBufSet
				, shadows
				, shadowMapBinding
				, shadowMapSet
				, enableVolumetric );
		}
		//\}
		//\}
		/**
		*\name
		*	Deferred renderring
		*/
		//\{
		C3D_API void declareDirectionalModel( bool lightUbo
			, uint32_t lightBinding
			, uint32_t lightSet
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet );
		C3D_API void declarePointModel( bool lightUbo
			, uint32_t lightBinding
			, uint32_t lightSet
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet );
		C3D_API void declareSpotModel( bool lightUbo
			, uint32_t lightBinding
			, uint32_t lightSet
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet );
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		C3D_API virtual void compute( DirectionalLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, BackgroundModel & background
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output ) = 0;
		C3D_API virtual void compute( PointLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output ) = 0;
		C3D_API virtual void compute( SpotLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output ) = 0;
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		C3D_API virtual sdw::Vec3 computeDiffuse( DirectionalLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows ) = 0;
		C3D_API virtual sdw::Vec3 computeDiffuse( PointLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows ) = 0;
		C3D_API virtual sdw::Vec3 computeDiffuse( SpotLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows ) = 0;
		//\}
		//\}
		/**
		*\name
		*	Light accessors
		*/
		//\{
		C3D_API DirectionalLight getDirectionalLight( sdw::UInt const & index )const;
		C3D_API PointLight getPointLight( sdw::UInt const & index )const;
		C3D_API SpotLight getSpotLight( sdw::UInt const & index )const;
		//\}

		inline Shadow & getShadowModel()const
		{
			return *m_shadowModel;
		}

	protected:
		C3D_API Light getBaseLight( sdw::UInt & offset )const;
		C3D_API void doDeclareLightsBuffer( uint32_t binding
			, uint32_t set );
		C3D_API void doDeclareDirectionalLightUbo( uint32_t binding
			, uint32_t set );
		C3D_API void doDeclarePointLightUbo( uint32_t binding
			, uint32_t set );
		C3D_API void doDeclareSpotLightUbo( uint32_t binding
			, uint32_t set );
		C3D_API void doDeclareGetBaseLight();
		C3D_API void doDeclareGetDirectionalLight();
		C3D_API void doDeclareGetPointLight();
		C3D_API void doDeclareGetSpotLight();
		C3D_API void doDeclareGetCascadeFactors();

	private:
		virtual sdw::Vec3 doCombine( BlendComponents const & components
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
			, sdw::Vec3 const & sheenReflected ) = 0;
		C3D_API static LightingModelPtr createModel( Engine const & engine
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdf
			, castor::String const & name
			, LightType light
			, bool lightUbo
			, uint32_t lightBinding
			, uint32_t lightSet
			, ShadowOptions const & shadows
			, SssProfiles const * sssProfiles
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet );

	protected:
		sdw::ShaderWriter & m_writer;
		Materials const & m_materials;
		Utils & m_utils;
		std::unique_ptr< sdw::Struct > m_type;
		std::unique_ptr< sdw::StorageBuffer > m_ssbo;
		std::unique_ptr < sdw::RImageBufferRgba32 > m_tbo;
		bool m_enableVolumetric;
		std::string m_prefix;
		std::shared_ptr< Shadow > m_shadowModel;
		std::shared_ptr< SssTransmittance > m_sssTransmittance;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec4
			, sdw::InUInt > m_getCascadeFactors;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec4Array
			, sdw::InUInt > m_getTileFactors;
		sdw::Function< shader::Light
			, sdw::InOutUInt > m_getBaseLight;
		sdw::Function< shader::DirectionalLight
			, sdw::InUInt > m_getDirectionalLight;
		sdw::Function< shader::PointLight
			, sdw::InUInt > m_getPointLight;
		sdw::Function< shader::SpotLight
			, sdw::InUInt > m_getSpotLight;
	};
}

#endif
