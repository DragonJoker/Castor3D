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

		C3D_API static std::unique_ptr< BackgroundModel > createModel( Scene const & scene
			, sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, uint32_t & binding
			, uint32_t set );

		C3D_API virtual sdw::RetVec3 computeReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, LightMaterial const & material
			, sdw::CombinedImage2DRg32 const & brdf ) = 0;
		C3D_API virtual sdw::RetVec3 computeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, LightMaterial const & material ) = 0;
		C3D_API virtual sdw::RetVoid mergeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, LightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction ) = 0;

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
