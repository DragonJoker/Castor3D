#include "Castor3D/Shader/Ubos/LayeredLpvConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

namespace castor3d
{
	std::string const LayeredLpvConfigUbo::LayeredLpvConfig = "LayeredLpvConfig";
	std::string const LayeredLpvConfigUbo::AllMinVolumeCorners = "c3d_allMinVolumeCorners";
	std::string const LayeredLpvConfigUbo::AllCellSizes = "c3d_allCellSizes";
	std::string const LayeredLpvConfigUbo::GridSize = "c3d_gridSize";
	std::string const LayeredLpvConfigUbo::Config = "c3d_config";

	LayeredLpvConfigUbo::LayeredLpvConfigUbo( Engine & engine )
		: m_engine{ engine }
		, m_ubo{ makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "LayeredLpvConfigUbo" ) }
	{
	}

	void LayeredLpvConfigUbo::update( std::array< castor::Grid, shader::DirectionalMaxCascadesCount > const & grids
		, float indirectAttenuation )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo->getData();

		for ( auto i = 0u; i < grids.size(); ++i )
		{
			auto min = grids[i].getMin();
			data.allMinVolumeCorners[i] = castor::Point4f{ min->x, min->y, min->z, 0.0f };
			data.allCellSizes[i] = grids[i].getCellSize();
		}

		auto dim = grids[0].getDimensions();
		data.gridSize = castor::Point4ui{ dim->x, dim->y, dim->z, 0u };
		data.config = castor::Point4f{ indirectAttenuation, 0.0f, 0.0f, 0.0f };
		m_ubo->upload();
	}
}
