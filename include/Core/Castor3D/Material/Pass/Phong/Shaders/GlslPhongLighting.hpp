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
			, Materials const & materials
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric
			, bool isBlinnPhong );
		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );

		C3D_API sdw::Vec3 combine( BlendComponents const & components
			, sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & directScattering
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 const & reflected
			, sdw::Vec3 const & refracted )override;
		C3D_API sdw::Vec3 adjustDirectAmbient( BlendComponents const & components
			, sdw::Vec3 const & directAmbient )const override;
		C3D_API sdw::Vec3 adjustDirectSpecular( BlendComponents const & components
			, sdw::Vec3 const & directSpecular )const override;
		C3D_API ReflectionModelPtr getReflectionModel( uint32_t & envMapBinding
			, uint32_t envMapSet )const override;
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		C3D_API void compute( DirectionalLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, BackgroundModel & background
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output )override;
		C3D_API void compute( PointLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, OutputComponents & output )override;
		C3D_API void compute( SpotLight const & light
			, BlendComponents const & components
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
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		C3D_API sdw::Vec3 computeDiffuse( PointLight const & light
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		C3D_API sdw::Vec3 computeDiffuse( SpotLight const & light
			, BlendComponents const & components
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
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection
			, OutputComponents & output );
		C3D_API sdw::Vec3 doComputeLightDiffuse( Light const & light
			, BlendComponents const & components
			, Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection );

	public:
		C3D_API static castor::String getName();
		bool m_isBlinnPhong{};
		std::string m_prefix;
		sdw::Function< sdw::Vec3
			, InLight
			, InBlendComponents
			, InSurface
			, sdw::InVec3
			, sdw::InVec3
			, OutputComponents & > m_computeLight;
		sdw::Function< sdw::Void
			, InDirectionalLight
			, InBlendComponents
			, InSurface
			, sdw::InVec3
			, sdw::InUInt
			, OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, InPointLight
			, InBlendComponents
			, InSurface
			, sdw::InVec3
			, sdw::InUInt
			, OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, InSpotLight
			, InBlendComponents
			, InSurface
			, sdw::InVec3
			, sdw::InUInt
			, OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, InLight
			, InBlendComponents
			, InSurface
			, sdw::InVec3
			, sdw::InVec3 > m_computeLightDiffuse;
		sdw::Function< sdw::Vec3
			, InOutDirectionalLight
			, InBlendComponents
			, InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, InOutPointLight
			, InBlendComponents
			, InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, InOutSpotLight
			, InBlendComponents
			, InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computeSpotDiffuse;
	};

	class BlinnPhongLightingModel
		: public PhongLightingModel
	{
	public:
		C3D_API BlinnPhongLightingModel( sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );

		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );
		C3D_API static castor::String getName();
	};
}

#endif
