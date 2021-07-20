/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPbrLightingModel_H___
#define ___C3D_GlslPbrLightingModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class PbrLightingModel
		: public LightingModel
	{
	protected:
		C3D_API explicit PbrLightingModel( bool isSpecularGlossiness
			, sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, bool isOpaqueProgram );

	public:
		C3D_API sdw::Vec3 combine( sdw::Vec3 const & directDiffuse
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
		C3D_API ReflectionModelPtr getReflectionModel( PassFlags const & passFlags
			, uint32_t & envMapBinding
			, uint32_t envMapSet )const override;
		C3D_API ReflectionModelPtr getReflectionModel( uint32_t envMapBinding
			, uint32_t envMapSet )const override;
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		C3D_API void compute( TiledDirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const override;
		C3D_API void compute( DirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const override;
		C3D_API void compute( PointLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const override;
		C3D_API void compute( SpotLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows
			, OutputComponents & output )const override;
		C3D_API void computeMapContributions( PassFlags const & passFlags
			, FilteredTextureFlags const & textures
			, sdw::Float const & gamma
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 & texCoords
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittanc
			, LightMaterial & lightMat
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )override;
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		C3D_API sdw::Vec3 computeDiffuse( TiledDirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		C3D_API sdw::Vec3 computeDiffuse( DirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		C3D_API sdw::Vec3 computeDiffuse( PointLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		C3D_API sdw::Vec3 computeDiffuse( SpotLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Int const & receivesShadows )const override;
		C3D_API void computeMapDiffuseContributions( PassFlags const & passFlags
			, FilteredTextureFlags const & textures
			, sdw::Float const & gamma
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, LightMaterial & lightMat )override;
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
		CookTorranceBRDF m_cookTorrance;
		sdw::Function< sdw::Void
			, InTiledDirectionalLight
			, InPbrLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt
			, OutputComponents & > m_computeTiledDirectional;
		sdw::Function< sdw::Void
			, InDirectionalLight
			, InPbrLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt
			, OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, InPointLight
			, InPbrLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt
			, OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, InSpotLight
			, InPbrLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt
			, OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, InTiledDirectionalLight
			, InPbrLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeTiledDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, InDirectionalLight
			, InPbrLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, InPointLight
			, InPbrLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, InSpotLight
			, InPbrLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeSpotDiffuse;
	};

	class PbrMRLightingModel
		: public PbrLightingModel
	{
	public:
		C3D_API PbrMRLightingModel( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, bool isOpaqueProgram );

		C3D_API static const castor::String getName();
		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, bool isOpaqueProgram );

		C3D_API std::unique_ptr< LightMaterial > declMaterial( std::string const & name )override;
	};

	class PbrSGLightingModel
		: public PbrLightingModel
	{
	public:
		C3D_API PbrSGLightingModel( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, bool isOpaqueProgram );

		C3D_API static const castor::String getName();
		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, bool isOpaqueProgram );

		C3D_API std::unique_ptr< LightMaterial > declMaterial( std::string const & name )override;
	};
}

#endif