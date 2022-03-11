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
#include <ShaderWriter/CompositeTypes/Ssbo.hpp>
#include <ShaderWriter/CompositeTypes/Struct.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d::shader
{
	C3D_API castor::String getLightingModelName( Engine const & engine
		, PassTypeID passType );

	struct LightMaterial
		: public sdw::StructInstance
	{
		C3D_API LightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, LightMaterial );

		C3D_API virtual void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, Material const & material ) = 0;
		C3D_API virtual void create( sdw::Vec3 const & albedo
			, sdw::Vec4 const & data3
			, sdw::Vec4 const & data2
			, sdw::Float const & ambient = 0.0_f ) = 0;
		C3D_API virtual void create( Material const & material ) = 0;
		C3D_API virtual void output( sdw::Vec4 & outData2, sdw::Vec4 & outData3 )const = 0;
		C3D_API virtual sdw::Vec3 getAmbient( sdw::Vec3 const & ambientLight )const = 0;
		C3D_API virtual void adjustDirectSpecular( sdw::Vec3 & directSpecular )const = 0;
		C3D_API virtual sdw::Vec3 getIndirectAmbient( sdw::Vec3 const & indirectAmbient )const = 0;
		C3D_API virtual sdw::Float getMetalness()const = 0;
		C3D_API virtual sdw::Float getRoughness()const = 0;

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		/**
		*\name
		*	Convertors
		*/
		//\{
		static C3D_API sdw::Vec3 computeF0( sdw::Vec3 const & albedo
			, sdw::Float const & metalness );
		static C3D_API sdw::Float computeMetalness( sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular );
		static C3D_API sdw::Float computeRoughness( sdw::Float const & glossiness );
		//\}

		sdw::Vec4 edgeFactors;
		sdw::Vec4 edgeColour;
		sdw::Vec4 specific;
		sdw::Vec3 albedo;
		sdw::Vec3 specular;
		sdw::Float edgeWidth;
		sdw::Float depthFactor;
		sdw::Float normalFactor;
		sdw::Float objectFactor;

	protected:
		sdw::Float albDiv;
		sdw::Float spcDiv;

	public:
		sdw::Float sssProfileIndex;
		sdw::Float sssTransmittance;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	class LightingModel
	{
	public:
		C3D_API LightingModel( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool isOpaqueProgram
			, bool hasSsbo
			, std::string prefix );
		C3D_API virtual ~LightingModel() = default;
		C3D_API virtual sdw::Vec3 combine( sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & ambient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 const & reflected
			, sdw::Vec3 const & refracted
			, sdw::Vec3 const & materialAlbedo ) = 0;
		C3D_API virtual std::unique_ptr< LightMaterial > declMaterial( std::string const & name ) = 0;
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
		C3D_API void computeCombined( LightMaterial const & material
			, SceneData const & sceneData
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const;
		C3D_API static LightingModelPtr createModel( Utils & utils
			, castor::String const & name
			, uint32_t lightsBufBinding
			, uint32_t lightsBufSet
			, ShadowOptions const & shadows
			, SssProfiles const * sssProfiles
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool isOpaqueProgram
			, bool hasSsbo );
		template< typename LightsBufBindingT >
		static LightingModelPtr createModelT( Utils & utils
			, castor::String const & name
			, LightsBufBindingT lightsBufBinding
			, uint32_t lightsBufSet
			, ShadowOptions const & shadows
			, SssProfiles const * sssProfiles
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool isOpaqueProgram
			, bool hasSsbo )
		{
			return createModel( utils
				, name
				, uint32_t( lightsBufBinding )
				, lightsBufSet
				, shadows
				, sssProfiles
				, shadowMapBinding
				, shadowMapSet
				, isOpaqueProgram
				, hasSsbo );
		}
		template< typename LightBindingT >
		static LightingModelPtr createModel( Utils & utils
			, castor::String const & name
			, LightType light
			, LightBindingT lightBinding
			, uint32_t lightSet
			, bool lightUbo
			, ShadowOptions const & shadows
			, SssProfiles const * sssProfiles
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool hasSsbo )
		{
			return createModel( utils
				, name
				, light
				, lightUbo
				, uint32_t( lightBinding )
				, lightSet
				, shadows
				, sssProfiles
				, shadowMapBinding
				, shadowMapSet
				, hasSsbo );
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
		C3D_API sdw::Vec3 computeCombinedDiffuse( LightMaterial const & material
			, SceneData const & sceneData
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const;
		C3D_API static LightingModelPtr createDiffuseModel( Utils & utils
			, castor::String const & name
			, uint32_t lightsBufBinding
			, uint32_t lightsBufSet
			, ShadowOptions const & shadows
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool isOpaqueProgram
			, bool hasSsbo );
		template< typename LightsBufBindingT >
		static LightingModelPtr createDiffuseModelT( Utils & utils
			, castor::String const & name
			, LightsBufBindingT lightsBufBinding
			, uint32_t lightsBufSet
			, ShadowOptions const & shadows
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool isOpaqueProgram
			, bool hasSsbo )
		{
			return createDiffuseModel( utils
				, name
				, uint32_t( lightsBufBinding )
				, lightsBufSet
				, shadows
				, shadowMapBinding
				, shadowMapSet
				, isOpaqueProgram
				, hasSsbo );
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
		C3D_API virtual void compute( TiledDirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const = 0;
		C3D_API virtual void compute( DirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const = 0;
		C3D_API virtual void compute( PointLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const = 0;
		C3D_API virtual void compute( SpotLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const = 0;
		C3D_API virtual void computeMapContributions( PassFlags const & passFlags
			, TextureFlagsArray const & textures
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UVec4 const & textures0
			, sdw::UVec4 const & textures1
			, sdw::Vec3 & texCoords
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittance
			, LightMaterial & lightMat
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition ) = 0;
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		C3D_API virtual sdw::Vec3 computeDiffuse( TiledDirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const = 0;
		C3D_API virtual sdw::Vec3 computeDiffuse( DirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const = 0;
		C3D_API virtual sdw::Vec3 computeDiffuse( PointLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const = 0;
		C3D_API virtual sdw::Vec3 computeDiffuse( SpotLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const = 0;
		C3D_API virtual void computeMapDiffuseContributions( PassFlags const & passFlags
			, TextureFlagsArray const & textures
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::UVec4 const & textures0
			, sdw::UVec4 const & textures1
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, LightMaterial & lightMat ) = 0;
		//\}
		//\}
		/**
		*\name
		*	Light accessors
		*/
		//\{
		C3D_API DirectionalLight getDirectionalLight( sdw::UInt const & index )const;
		C3D_API TiledDirectionalLight getTiledDirectionalLight( sdw::UInt const & index )const;
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

		virtual void doDeclareModel() = 0;
		virtual void doDeclareComputeDirectionalLight() = 0;
		virtual void doDeclareComputeTiledDirectionalLight() = 0;
		virtual void doDeclareComputePointLight() = 0;
		virtual void doDeclareComputeSpotLight() = 0;
		virtual void doDeclareDiffuseModel() = 0;
		virtual void doDeclareComputeDirectionalLightDiffuse() = 0;
		virtual void doDeclareComputeTiledDirectionalLightDiffuse() = 0;
		virtual void doDeclareComputePointLightDiffuse() = 0;
		virtual void doDeclareComputeSpotLightDiffuse() = 0;

	private:
		C3D_API static LightingModelPtr createModel( Utils & utils
			, castor::String const & name
			, LightType light
			, bool lightUbo
			, uint32_t lightBinding
			, uint32_t lightSet
			, ShadowOptions const & shadows
			, SssProfiles const * sssProfiles
			, uint32_t & shadowMapBinding
			, uint32_t shadowMapSet
			, bool hasSsbo );

	protected:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		std::unique_ptr< sdw::Struct > m_type;
		std::unique_ptr< sdw::Ssbo > m_ssbo;
		std::unique_ptr < sdw::RImageBufferRgba32 > m_tbo;
		bool m_isOpaqueProgram;
		bool m_hasSsbo;
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
		sdw::Function< shader::TiledDirectionalLight
			, sdw::InUInt > m_getTiledDirectionalLight;
		sdw::Function< shader::PointLight
			, sdw::InUInt > m_getPointLight;
		sdw::Function< shader::SpotLight
			, sdw::InUInt > m_getSpotLight;
	};
}

#endif
