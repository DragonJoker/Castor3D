/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongLightingModel_H___
#define ___C3D_GlslPhongLightingModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class PhongLightingModel
		: public LightingModel
	{
	public:
		C3D_API PhongLightingModel( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool isOpaqueProgram
			, bool isBlinnPhong );
		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool isOpaqueProgram );
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
		C3D_API std::unique_ptr< LightMaterial > declMaterial( std::string const & name )override;
		C3D_API ReflectionModelPtr getReflectionModel( uint32_t & envMapBinding
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
			, LightMaterial & lightMat )override;
		//\}

		bool isBlinnPhong()const
		{
			return m_isBlinnPhong;
		}

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
		sdw::Vec3 doComputeLight( Light const & light
			, PhongLightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection
			, OutputComponents & output );
		void doDeclareComputeLight();
		sdw::Vec3 doComputeLightDiffuse( Light const & light
			, PhongLightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection );
		void doDeclareComputeLightDiffuse();

	public:
		C3D_API static castor::String getName();
		bool m_isBlinnPhong{};
		std::string m_prefix;
		sdw::Function< sdw::Vec3
			, InLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InVec3
			, OutputComponents & > m_computeLight;
		sdw::Function< sdw::Void
			, InDirectionalLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt
			, OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, InTiledDirectionalLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt
			, OutputComponents & > m_computeTiledDirectional;
		sdw::Function< sdw::Void
			, InPointLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt
			, OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, InSpotLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt
			, OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, InLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InVec3 > m_computeLightDiffuse;
		sdw::Function< sdw::Vec3
			, InOutTiledDirectionalLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeTiledDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, InOutDirectionalLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, InOutPointLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, InOutSpotLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InInt > m_computeSpotDiffuse;
	};

	class BlinnPhongLightingModel
		: public PhongLightingModel
	{
	public:
		C3D_API BlinnPhongLightingModel( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool isOpaqueProgram );

		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool isOpaqueProgram );
		C3D_API static castor::String getName();
	};
}

#endif
