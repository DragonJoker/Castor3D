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

		C3D_API void computeReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular )override;
		C3D_API sdw::RetVec3 computeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, BlendComponents & components )override;
		C3D_API sdw::RetVec3 computeSpecularReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf )override;
		C3D_API sdw::RetVec3 computeSpecularRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness
			, sdw::Float const & refractionRatio
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdfMap )override;
		C3D_API  sdw::RetVec3 computeSheenReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf )override;
		C3D_API void computeIridescenceReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf
			, sdw::Vec3 const & iridescenceFresnel
			, sdw::Vec3 const & iridescenceF0
			, sdw::Float const & iridescenceFactor
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular )override;

	public:
		static castor::String const Name;

	private:
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflections;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeRefractions;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32 > m_computeSpecularReflections;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat > m_computeSpecularRefractions;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32 > m_computeSheenReflections;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeIridescenceReflections;
	};
}

#endif
