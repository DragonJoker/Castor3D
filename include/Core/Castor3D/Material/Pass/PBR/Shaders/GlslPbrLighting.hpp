/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPbrLightingModel_H___
#define ___C3D_GlslPbrLightingModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class PbrLightingModel
		: public LightingModel
	{
	public:
		C3D_API explicit PbrLightingModel( sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );

		C3D_API static const castor::String getName();
		C3D_API static LightingModelPtr create( sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );

		C3D_API sdw::Float getFinalTransmission( BlendComponents const & components
			, sdw::Vec3 const & incident )override;
		C3D_API sdw::Vec3 adjustDirectAmbient( BlendComponents const & components
			, sdw::Vec3 const & directAmbient )const override;
		C3D_API sdw::Vec3 adjustDirectSpecular( BlendComponents const & components
			, sdw::Vec3 const & directSpecular )const override;
		C3D_API sdw::Vec3 adjustRefraction( BlendComponents const & components
			, sdw::Vec3 const & refraction )const;
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

	private:
		C3D_API sdw::Vec3 doCombine( BlendComponents const & components
			, sdw::Vec3 const & incident
			, sdw::Vec3 const & directDiffuse
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec3 const & directSpecular
			, sdw::Vec3 const & directScattering
			, sdw::Vec3 const & directCoatingSpecular
			, sdw::Vec3 const & indirectSpecular
			, sdw::Vec3 const & directAmbient
			, sdw::Vec3 const & indirectAmbient
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & emissive
			, sdw::Vec3 const & reflected
			, sdw::Vec3 const & refracted
			, sdw::Vec3 const & coatReflected )override;

	public:
		CookTorranceBRDF m_cookTorrance;
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
}

#endif
