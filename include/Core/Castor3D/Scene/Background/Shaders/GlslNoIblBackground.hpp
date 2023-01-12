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

		C3D_API static BackgroundModelPtr create( Engine const & engine
			, sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, bool needsForeground
			, uint32_t & binding
			, uint32_t set );

		C3D_API void computeReflections( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & difF
			, sdw::Vec3 const & spcF
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular )override;
		C3D_API sdw::RetVec3 computeRefractions( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & V
			, sdw::Float const & refractionRatio
			, BlendComponents & components )override;

	public:
		static castor::String const Name;

	private:
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InVec3
			, sdw::InFloat
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflections;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InOutVec3
			, sdw::InFloat > m_computeRefractions;
	};
}

#endif
