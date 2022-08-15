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
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric
			, bool isBlinnPhong );
		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric );

		std::unique_ptr< c3d::LightMaterial > declMaterial( std::string const & name
			, bool enabled )override;

		c3d::ReflectionModelPtr getReflectionModel( uint32_t & envMapBinding
			, uint32_t envMapSet )const override;
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		void compute( c3d::DirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, c3d::BackgroundModel & background
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, c3d::OutputComponents & output )override;
		void compute( c3d::PointLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, c3d::OutputComponents & output )override;
		void compute( c3d::SpotLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, c3d::OutputComponents & output )override;
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		sdw::Vec3 computeDiffuse( c3d::DirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		sdw::Vec3 computeDiffuse( c3d::PointLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		sdw::Vec3 computeDiffuse( c3d::SpotLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		//\}

	private:
		void doComputeLight( c3d::Light const & light
			, ToonPhongLightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection
			, c3d::OutputComponents & output );
		sdw::Vec3 doComputeLightDiffuse( c3d::Light const & light
			, ToonPhongLightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & lightDirection );

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
			, sdw::InUInt
			, c3d::OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, c3d::InPointLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt
			, c3d::OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, c3d::InSpotLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt
			, c3d::OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, c3d::InLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InVec3 > m_computeLightDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutDirectionalLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutPointLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutSpotLight
			, InToonPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computeSpotDiffuse;
	};

	class ToonBlinnPhongLightingModel
		: public ToonPhongLightingModel
	{
	public:
		ToonBlinnPhongLightingModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric );

		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric );
		static castor::String getName();
	};

	class ToonPbrLightingModel
		: public c3d::PbrLightingModel
	{
	public:
		explicit ToonPbrLightingModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric );

		static const castor::String getName();
		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric );

		std::unique_ptr< c3d::LightMaterial > declMaterial( std::string const & name
			, bool enabled )override;

		c3d::ReflectionModelPtr getReflectionModel( uint32_t & envMapBinding
			, uint32_t envMapSet )const override;
		/**
		*\name
		*	Diffuse + Specular
		*/
		//\{
		void compute( c3d::DirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, c3d::BackgroundModel & background
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, c3d::OutputComponents & output )override;
		void compute( c3d::PointLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, c3d::OutputComponents & output )override;
		void compute( c3d::SpotLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows
			, c3d::OutputComponents & output )override;
		//\}
		/**
		*\name
		*	Diffuse only
		*/
		//\{
		sdw::Vec3 computeDiffuse( c3d::DirectionalLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		sdw::Vec3 computeDiffuse( c3d::PointLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		sdw::Vec3 computeDiffuse( c3d::SpotLight const & light
			, c3d::LightMaterial const & material
			, c3d::Surface const & surface
			, sdw::Vec3 const & worldEye
			, sdw::UInt const & receivesShadows )override;
		//\}

	public:
		sdw::Function< sdw::Void
			, c3d::InDirectionalLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt
			, c3d::OutputComponents & > m_computeDirectional;
		sdw::Function< sdw::Void
			, c3d::InPointLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt
			, c3d::OutputComponents & > m_computePoint;
		sdw::Function< sdw::Void
			, c3d::InSpotLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt
			, c3d::OutputComponents & > m_computeSpot;
		sdw::Function< sdw::Vec3
			, c3d::InOutDirectionalLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computeDirectionalDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutPointLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computePointDiffuse;
		sdw::Function< sdw::Vec3
			, c3d::InOutSpotLight
			, InToonPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InUInt > m_computeSpotDiffuse;
	};
 }

#endif
