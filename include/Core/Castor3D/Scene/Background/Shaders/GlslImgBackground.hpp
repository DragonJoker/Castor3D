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

		C3D_API sdw::RetVec3 computeReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, BlendComponents & components
			, sdw::CombinedImage2DRg32 const & brdf )override;
		C3D_API sdw::RetVec3 computeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, BlendComponents & components )override;
		C3D_API sdw::RetVoid mergeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, BlendComponents & components
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )override;

	public:
		static castor::String const Name;
		sdw::Function< sdw::Vec3 > m_computeReflections;
		sdw::Function< sdw::Vec3 > m_computeRefractions;
		sdw::Function< sdw::Void > m_mergeReflRefr;
	};
}

#endif
