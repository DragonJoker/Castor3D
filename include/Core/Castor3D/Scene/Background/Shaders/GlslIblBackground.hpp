/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslIblBackgroundModel_H___
#define ___C3D_GlslIblBackgroundModel_H___

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

namespace castor3d::shader
{
	class IblBackgroundModel
		: public BackgroundModel
	{
	public:
		C3D_API IblBackgroundModel( sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, uint32_t & binding
			, uint32_t set );

		C3D_API static BackgroundModelPtr create( sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, uint32_t & binding
			, uint32_t set );

		C3D_API sdw::RetVec3 computeReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, BlendComponents & components
			, sdw::CombinedImage2DRg32 const & brdf )override;
		C3D_API sdw::RetVec3 computeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, BlendComponents & components )override;

	private:
		sdw::RetVec3 doComputeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & prefiltered
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & albedo
			, sdw::Float const & roughness );

	public:
		static castor::String const Name;

	private:
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRg32 > m_computeReflections;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeRefractions;
	};
}

#endif
