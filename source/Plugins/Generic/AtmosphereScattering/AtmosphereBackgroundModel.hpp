/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackgroundModel_H___
#define ___C3DAS_AtmosphereBackgroundModel_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Shader/Shaders/GlslBackground.hpp>

namespace atmosphere_scattering
{
	class AtmosphereBackgroundModel
		: public castor3d::shader::BackgroundModel
	{
	public:
		AtmosphereBackgroundModel( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, uint32_t & binding
			, uint32_t set );

		static castor3d::shader::BackgroundModelPtr create( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, uint32_t & binding
			, uint32_t set );

		sdw::Vec3 computeReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, castor3d::shader::LightMaterial const & material
			, sdw::CombinedImage2DRg32 const & brdf )override;
		sdw::Vec3 computeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, castor3d::shader::LightMaterial const & material )override;
		sdw::Void mergeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, castor3d::shader::LightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )override;

	public:
		static castor::String const Name;
	};
}

#endif
