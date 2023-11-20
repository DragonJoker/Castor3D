/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslToonLightingModel_H___
#define ___C3D_GlslToonLightingModel_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp>
#include <Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

namespace atmosphere_scattering
{
	namespace c3d = castor3d::shader;

	class AtmosphereLightingModel
	{
	protected:
		void initBackground( c3d::BackgroundModel & background
			, c3d::Shadow & shadowModel );
		sdw::Vec3 compRadiance( sdw::Vec3 const & lightDirection )const;
		void compScatteringTerm( c3d::LightSurface const & lightSurface
			, sdw::Vec3 & output );

	public:
		AtmosphereBackgroundModel * atmosphereBackground{};
	};

	class AtmospherePhongLightingModel
		: public c3d::PhongLightingModel
		, public AtmosphereLightingModel
	{
	public:
		AtmospherePhongLightingModel( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

		static c3d::LightingModelUPtr create( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

	protected:
		void doInitialiseBackground( c3d::BackgroundModel & pbackground )override;
		sdw::Vec3 doComputeRadiance( c3d::Light const & light
			, sdw::Vec3 const & lightDirection )const override;
		void doComputeScatteringTerm( c3d::ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec3 const & radiance
			, sdw::Vec2 const & lightIntensity
			, c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface
			, sdw::Vec3 output )override;
	};

	class AtmospherePbrLightingModel
		: public c3d::PbrLightingModel
		, public AtmosphereLightingModel
	{
	public:
		explicit AtmospherePbrLightingModel( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

		static c3d::LightingModelUPtr create( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

	protected:
		void doInitialiseBackground( c3d::BackgroundModel & pbackground )override;
		sdw::Vec3 doComputeRadiance( c3d::Light const & light
			, sdw::Vec3 const & lightDirection )const override;
		void doComputeScatteringTerm( c3d::ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec3 const & radiance
			, sdw::Vec2 const & lightIntensity
			, c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface
			, sdw::Vec3 output )override;
	};
 }

#endif
