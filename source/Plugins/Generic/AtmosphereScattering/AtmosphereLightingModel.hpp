/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslAtmosphereLightingModel_H___
#define ___C3D_GlslAtmosphereLightingModel_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp>
#include <Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

namespace atmosphere_scattering
{
	namespace c3d = castor3d::shader;

	class AtmosphereScatteringModel
		: public c3d::ScatteringModel
	{
	public:
		explicit AtmosphereScatteringModel( sdw::ShaderWriter & writer );
		void initialiseBackground( c3d::BackgroundModel & background
			, c3d::Shadow & shadowModel )override;
		sdw::Vec3 computeRadiance( c3d::Light const & light
			, sdw::Vec3 const & lightDirection )const override;
		void computeScattering( c3d::LightingModel & lighting
			, c3d::ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec3 const & radiance
			, sdw::Float const & lightIntensity
			, c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface
			, sdw::Vec3 output )override;

		static castor::StringView constexpr Name{ cuT( "atmosphere_scattering" ) };
		static c3d::ScatteringModelPtr create( sdw::ShaderWriter & writer );

	public:
		AtmosphereBackgroundModel * atmosphereBackground{};
	};
 }

#endif
