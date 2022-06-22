/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslNoIblBackgroundModel_H___
#define ___C3D_GlslNoIblBackgroundModel_H___

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

namespace castor3d::shader
{
	class NoIblBackgroundModel
		: public BackgroundModel
	{
	public:
		C3D_API NoIblBackgroundModel( sdw::ShaderWriter & writer
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
		C3D_API sdw::Vec4 scatter( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth )override;

	private:
		sdw::Vec3 doComputeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & backgroundMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 const & albedo
			, sdw::Float const & roughness );

	public:
		static castor::String const Name;

	private:
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InVec3
			, sdw::InFloat > m_computeReflections;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InOutVec3
			, sdw::InFloat > m_computeRefractions;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InOutVec3
			, sdw::InFloat
			, sdw::InOutVec3
			, sdw::OutVec3 > m_mergeReflRefr;
	};
}

#endif
