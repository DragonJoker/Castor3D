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
			, BlendComponents & components
			, sdw::CombinedImage2DRg32 const & brdf ) = 0;
		C3D_API virtual sdw::RetVec3 computeRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & refractionRatio
			, BlendComponents & components ) = 0;

		C3D_API sdw::RetVec3 virtual computeSpecularReflections( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness
			, BlendComponents & components
			, sdw::CombinedImage2DRg32 const & brdf )
		{
			return computeReflections( wsIncident, wsNormal, components, brdf );
		}

		C3D_API sdw::RetVec3 virtual computeSpecularRefractions( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness
			, sdw::Float const & refractionRatio
			, BlendComponents & components
			, sdw::CombinedImage2DRg32 const & brdfMap )
		{
			return computeRefractions( wsIncident, wsNormal, refractionRatio, components );
		}

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
