/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslImgBackgroundModel_H___
#define ___C3D_GlslImgBackgroundModel_H___

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

namespace castor3d::shader
{
	class ImgBackgroundModel
		: public BackgroundModel
	{
	public:
		C3D_API ImgBackgroundModel( sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, uint32_t & binding
			, uint32_t set );

		C3D_API static BackgroundModelPtr create( sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, uint32_t & binding
			, uint32_t set );

		C3D_API sdw::Vec3 computeReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, LightMaterial const & material
			, sdw::CombinedImage2DRg32 const & brdf )override;
		C3D_API sdw::Vec3 computeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, LightMaterial const & material )override;
		C3D_API sdw::Void mergeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, LightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )override;

	public:
		static castor::String const Name;
	};
}

#endif
