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
	namespace c3ds = c3d::shader;

	class AtmosphereScatteringModel
		: public c3ds::ScatteringModel
	{
	public:
		explicit AtmosphereScatteringModel( sdw::ShaderWriter & writer );
		void initialiseBackground( c3ds::BackgroundModel & background
			, c3ds::Shadow & shadowModel )override;
		sdw::Vec3 computeRadiance( c3ds::Light const & light
			, sdw::Vec3 const & lightDirection )const override;
		void computeScattering( c3ds::LightingModel & lighting
			, c3ds::ShadowData const & shadows
			, sdw::Int const shadowMapIndex
			, sdw::Vec3 const & radiance
			, sdw::Float const & lightIntensity
			, c3ds::BlendComponents const & components
			, c3ds::LightSurface const & lightSurface
			, sdw::Vec3 output )override;

		static c3d::StringView constexpr Name{ cuT( "atmosphere_scattering" ) };
		static c3ds::ScatteringModelPtr create( sdw::ShaderWriter & writer );

	public:
		AtmosphereBackgroundModel * atmosphereBackground{};
	};
 }

#endif
