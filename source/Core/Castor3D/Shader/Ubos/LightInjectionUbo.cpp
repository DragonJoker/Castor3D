#include "Castor3D/Shader/Ubos/LightInjectionUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

namespace castor3d
{
	std::string const LightInjectionUbo::LightInjection = "LightInjection";
	std::string const LightInjectionUbo::MinVolumeCorner = "c3d_minVolumeCorner";
	std::string const LightInjectionUbo::GridSizes = "c3d_gridSizes";
	std::string const LightInjectionUbo::Config = "c3d_config";

	LightInjectionUbo::LightInjectionUbo( Engine & engine )
		: m_engine{ engine }
		, m_ubo{ makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "LightInjectionUbo" ) }
	{
	}

	void LightInjectionUbo::update( castor::Point3f const & minVolumeCorner
		, castor::Point3ui const & gridSize
		, uint32_t lightIndex
		, float cellSize
		, float indirectAttenuation )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo->getData();
		data.minVolumeCorner = castor::Point4f{ minVolumeCorner->x, minVolumeCorner->y, minVolumeCorner->z, cellSize };
		data.gridSizes = castor::Point4ui{ gridSize->x, gridSize->y, gridSize->z, lightIndex };
		data.config = castor::Point4f{ indirectAttenuation, 0.0f, 0.0f, 0.0f };
		m_ubo->upload();
	}
}
