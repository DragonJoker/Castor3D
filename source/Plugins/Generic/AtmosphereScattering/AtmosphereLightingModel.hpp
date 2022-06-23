/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslToonLightingModel_H___
#define ___C3D_GlslToonLightingModel_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Material/Pass/PBR/Shaders/GlslPbrLighting.hpp>
#include <Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

namespace atmosphere_scattering
{
	namespace c3d = castor3d::shader;

	class AtmospherePhongLightingModel
		: public c3d::PhongLightingModel
	{
	public:
		AtmospherePhongLightingModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric
			, bool isBlinnPhong );

		static const castor::String getName();
		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric );
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
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )override;
		using c3d::PhongLightingModel::compute;
		//\}

	public:
		AtmosphereBackgroundModel * m_atmosphereBackground{};
		sdw::Function< sdw::Void
			, c3d::InDirectionalLight
			, c3d::InPhongLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computeDirectional;
	};

	class AtmosphereBlinnPhongLightingModel
		: public AtmospherePhongLightingModel
	{
	public:
		AtmosphereBlinnPhongLightingModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric );

		static castor::String getName();
		static c3d::LightingModelPtr create( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, c3d::ShadowOptions shadowOptions
			, c3d::SssProfiles const * sssProfiles
			, bool enableVolumetric );
	};

	class AtmospherePbrLightingModel
		: public c3d::PbrLightingModel
	{
	public:
		explicit AtmospherePbrLightingModel( sdw::ShaderWriter & writer
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
			, sdw::Int const & receivesShadows
			, c3d::OutputComponents & output )override;
		using c3d::PbrLightingModel::compute;
		//\}

	public:
		AtmosphereBackgroundModel * m_atmosphereBackground{};
		sdw::Function< sdw::Void
			, c3d::InDirectionalLight
			, c3d::InPbrLightMaterial
			, c3d::InSurface
			, sdw::InVec3
			, sdw::InInt
			, c3d::OutputComponents & > m_computeDirectional;
	};
 }

#endif
