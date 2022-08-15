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
			, bool enableVolumetric
			, bool isBlinnPhong );
		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );

		C3D_API std::unique_ptr< LightMaterial > declMaterial( std::string const & name
			, bool enabled )override;
		C3D_API void modifyMaterial( PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, TextureConfigData const & config
			, LightMaterial & lightMat )const override;
		C3D_API void updateMaterial( PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, LightMaterial & lightMat
			, sdw::Vec3 & emissive )const override;

		C3D_API sdw::Vec3 combine( sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & directScattering
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & ambient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 const & reflected
			, sdw::Vec3 const & refracted
			, sdw::Vec3 const & materialAlbedo )override;
		C3D_API ReflectionModelPtr getReflectionModel( uint32_t & envMapBinding
			, uint32_t envMapSet )const override;
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		C3D_API void compute( DirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, BackgroundModel & background
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output )override;
		C3D_API void compute( PointLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output )override;
		C3D_API void compute( SpotLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output )override;
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		C3D_API sdw::Vec3 computeDiffuse( DirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		C3D_API sdw::Vec3 computeDiffuse( PointLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		C3D_API sdw::Vec3 computeDiffuse( SpotLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		//\}

		bool isBlinnPhong()const
		{
			return m_isBlinnPhong;
		}

	protected:
		C3D_API sdw::RetVec3 doComputeLight( Light const & light
			, PhongLightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection
			, OutputComponents & output );
		C3D_API sdw::Vec3 doComputeLightDiffuse( Light const & light
			, PhongLightMaterial const & material
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection );

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
			, sdw::InUInt
			, OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, InPointLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InUInt
			, OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, InSpotLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InUInt
			, OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, InLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InVec3 > m_computeLightDiffuse;
		sdw::Function< sdw::Vec3
			, InOutDirectionalLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, InOutPointLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, InOutSpotLight
			, InPhongLightMaterial
			, InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computeSpotDiffuse;
	};

	class BlinnPhongLightingModel
		: public PhongLightingModel
	{
	public:
		C3D_API BlinnPhongLightingModel( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );

		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );
		C3D_API static castor::String getName();
	};
}

#endif
