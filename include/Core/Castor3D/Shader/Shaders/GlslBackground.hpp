/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBackgroundModel_H___
#define ___C3D_GlslBackgroundModel_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

namespace castor3d::shader
{
	class BackgroundModel
	{
	public:
		C3D_API BackgroundModel( sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize );
		C3D_API virtual ~BackgroundModel() = default;

		C3D_API static BackgroundModelPtr createModel( Scene const & scene
			, sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, bool needsForeground
			, uint32_t & binding
			, uint32_t set );

		C3D_API virtual void computeReflections( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & difF
			, sdw::Vec3 const & spcF
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular );
		C3D_API virtual sdw::RetVec3 computeDiffuseReflections( sdw::Vec3 const & albedo
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & fresnel
			, sdw::Float const & metalness );
		C3D_API virtual sdw::RetVec3 computeSpecularReflections( sdw::Vec3 const & fresnel
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, sdw::Float const & roughness
			, sdw::CombinedImage2DRgba32 const & brdf );
		C3D_API virtual sdw::RetVec3 computeSheenReflections( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf );

		C3D_API virtual sdw::RetVec3 computeRefractions( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & V
			, sdw::Float const & refractionRatio
			, BlendComponents & components );
		C3D_API virtual sdw::RetVec3 computeSpecularRefractions( sdw::Vec3 const & fresnel
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, sdw::Float const & roughness
			, sdw::Float const & refractionRatio
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf );

		C3D_API virtual void applyVolume( sdw::Vec2 const fragCoord
			, sdw::Float const linearDepth
			, sdw::Vec2 const targetSize
			, sdw::Vec2 const cameraPlanes
			, sdw::Vec4 & output );

		C3D_API sdw::Vec3 getBrdf( sdw::CombinedImage2DRgba32 const & brdfMap
			, sdw::Float const & NdotV
			, sdw::Float const & roughness );

		VkExtent2D const & getTargetSize()const
		{
			return m_targetSize;
		}

	protected:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		VkExtent2D m_targetSize;
	};
}

#endif
