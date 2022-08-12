/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackgroundModel_H___
#define ___C3DAS_AtmosphereBackgroundModel_H___

#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/ScatteringModel.hpp"

#include <Castor3D/Shader/Shaders/GlslBackground.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/CompositeTypes/UniformBuffer.hpp>

namespace atmosphere_scattering
{
	class AtmosphereBackgroundModel
		: public castor3d::shader::BackgroundModel
	{
	public:
		AtmosphereBackgroundModel( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, VkExtent2D targetSize
			, uint32_t & binding
			, uint32_t set );

		static castor3d::shader::BackgroundModelPtr create( sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, VkExtent2D targetSize
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
		sdw::Vec3 getSunRadiance( sdw::Vec3 const & sunDir );

		sdw::Void getPixelTransLum( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::Vec4 & transmittance
			, sdw::Vec4 & luminance )
		{
			return scattering.getPixelTransLum( fragPos
				, fragSize
				, fragDepth
				, transmittance
				, luminance );
		}

	public:
		static castor::String const Name;
		sdw::UniformBuffer cameraBuffer;
		CameraData cameraData;
		sdw::UniformBuffer atmosphereBuffer;
		AtmosphereData atmosphereData;
		AtmosphereModel atmosphere;
		ScatteringModel scattering;
	};
}

#endif
