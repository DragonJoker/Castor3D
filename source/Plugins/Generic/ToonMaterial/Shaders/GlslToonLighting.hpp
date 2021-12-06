/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslToonLightingModel_H___
#define ___C3D_GlslToonLightingModel_H___

#include "GlslToonMaterial.hpp"

#include <Castor3D/Material/Pass/PBR/Shaders/GlslPbrLighting.hpp>
#include <Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

namespace toon::shader
{
	namespace c3d = castor3d::shader;

	class ToonPhongLightingModel
		: public c3d::PhongLightingModel
	{
	public:
		ToonPhongLightingModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo
			, bool isBlinnPhong );
		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo );
		sdw::Vec3 combine( sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & ambient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 const & reflected
			, sdw::Vec3 const & refracted
			, sdw::Vec3 const & materialAlbedo )override;
		std::unique_ptr< c3d::LightMaterial > declMaterial( std::string const & name )override;
		c3d::ReflectionModelPtr getReflectionModel( castor3d::PassFlags const & passFlags
			, uint32_t & envMapBinding
			, uint32_t envMapSet )const override;
		c3d::ReflectionModelPtr getReflectionModel( uint32_t envMapBinding
			, uint32_t envMapSet )const override;
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		void compute( c3d::TiledDirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )const override;
		void compute( c3d::DirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )const override;
		void compute( c3d::PointLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )const override;
		void compute( c3d::SpotLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )const override;
		void computeMapContributions( castor3d::PassFlags const & passFlags
			, castor3d::FilteredTextureFlags const & textures
			, c3d::TextureConfigurations const & textureConfigs
			, c3d::TextureAnimations const & textureAnims
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 & texCoords
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittance
			, c3d::LightMaterial & lightMat
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )override;
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		sdw::Vec3 computeDiffuse( c3d::TiledDirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		sdw::Vec3 computeDiffuse( c3d::DirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		sdw::Vec3 computeDiffuse( c3d::PointLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		sdw::Vec3 computeDiffuse( c3d::SpotLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		void computeMapDiffuseContributions( castor3d::PassFlags const & passFlags
			, castor3d::FilteredTextureFlags const & textures
			, c3d::TextureConfigurations const & textureConfigs
			, c3d::TextureAnimations const & textureAnims
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, c3d::LightMaterial & lightMat )override;
		//\}

	protected:
		void doDeclareModel()override;
		void doDeclareComputeDirectionalLight()override;
		void doDeclareComputeTiledDirectionalLight()override;
		void doDeclareComputePointLight()override;
		void doDeclareComputeSpotLight()override;
		void doDeclareDiffuseModel()override;
		void doDeclareComputeDirectionalLightDiffuse()override;
		void doDeclareComputeTiledDirectionalLightDiffuse()override;
		void doDeclareComputePointLightDiffuse()override;
		void doDeclareComputeSpotLightDiffuse()override;

	private:
		void doComputeLight( c3d::Light const & light
			, ToonPhongLightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection
			, c3d::OutputComponents & output );
		void doDeclareComputeLight();
		sdw::Vec3 doComputeLightDiffuse( c3d::Light const & light
			, ToonPhongLightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection );
		void doDeclareComputeLightDiffuse();

	public:
		static const castor::String getName();

		sdw::Function< sdw::Void
			, c3d::InLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InVec3
			, c3d::OutputComponents & > m_computeLight;
		sdw::Function< sdw::Void
			, c3d::InDirectionalLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, c3d::InTiledDirectionalLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computeTiledDirectional;
		sdw::Function< sdw::Void
			, c3d::InPointLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, c3d::InSpotLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, c3d::InLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InVec3 > m_computeLightDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutTiledDirectionalLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeTiledDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutDirectionalLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutPointLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutSpotLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeSpotDiffuse;
	};

	class ToonBlinnPhongLightingModel
		: public ToonPhongLightingModel
	{
	public:
		ToonBlinnPhongLightingModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo );

		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo );
		static castor::String getName();
	};

	class ToonPbrLightingModel
		: public c3d::LightingModel
	{
	protected:
		explicit ToonPbrLightingModel( bool isSpecularGlossiness
			, sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo );

	public:
		sdw::Vec3 combine( sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & ambient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 const & reflected
			, sdw::Vec3 const & refracted
			, sdw::Vec3 const & materialAlbedo )override;
		c3d::ReflectionModelPtr getReflectionModel( castor3d::PassFlags const & passFlags
			, uint32_t & envMapBinding
			, uint32_t envMapSet )const override;
		c3d::ReflectionModelPtr getReflectionModel( uint32_t envMapBinding
			, uint32_t envMapSet )const override;
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		void compute( c3d::TiledDirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )const override;
		void compute( c3d::DirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )const override;
		void compute( c3d::PointLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )const override;
		void compute( c3d::SpotLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )const override;
		void computeMapContributions( castor3d::PassFlags const & passFlags
			, castor3d::FilteredTextureFlags const & textures
			, c3d::TextureConfigurations const & textureConfigs
			, c3d::TextureAnimations const & textureAnims
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 & texCoords
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittanc
			, c3d::LightMaterial & lightMat
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )override;
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		sdw::Vec3 computeDiffuse( c3d::TiledDirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		sdw::Vec3 computeDiffuse( c3d::DirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		sdw::Vec3 computeDiffuse( c3d::PointLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		sdw::Vec3 computeDiffuse( c3d::SpotLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		void computeMapDiffuseContributions( castor3d::PassFlags const & passFlags
			, castor3d::FilteredTextureFlags const & textures
			, c3d::TextureConfigurations const & textureConfigs
			, c3d::TextureAnimations const & textureAnims
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, c3d::LightMaterial & lightMat )override;
		//\}

	protected:
		void doDeclareModel()override;
		void doDeclareComputeDirectionalLight()override;
		void doDeclareComputeTiledDirectionalLight()override;
		void doDeclareComputePointLight()override;
		void doDeclareComputeSpotLight()override;
		void doDeclareDiffuseModel()override;
		void doDeclareComputeDirectionalLightDiffuse()override;
		void doDeclareComputeTiledDirectionalLightDiffuse()override;
		void doDeclareComputePointLightDiffuse()override;
		void doDeclareComputeSpotLightDiffuse()override;

	public:
		bool m_isSpecularGlossiness;
		c3d::CookTorranceBRDF m_cookTorrance;
		sdw::Function< sdw::Void
			, c3d::InTiledDirectionalLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computeTiledDirectional;
		sdw::Function< sdw::Void
			, c3d::InDirectionalLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, c3d::InPointLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, c3d::InSpotLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, c3d::InOutTiledDirectionalLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeTiledDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutDirectionalLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutPointLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutSpotLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeSpotDiffuse;
	};

	class ToonPbrMRLightingModel
		: public ToonPbrLightingModel
	{
	public:
		ToonPbrMRLightingModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo );

		static const castor::String getName();
		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo );

		std::unique_ptr< c3d::LightMaterial > declMaterial( std::string const & name )override;
	};

	class ToonPbrSGLightingModel
		: public ToonPbrLightingModel
	{
	public:
		ToonPbrSGLightingModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo );

		static const castor::String getName();
		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, bool isOpaqueProgram
			, bool hasSsbo );

		std::unique_ptr< c3d::LightMaterial > declMaterial( std::string const & name )override;
	};
 }

#endif
