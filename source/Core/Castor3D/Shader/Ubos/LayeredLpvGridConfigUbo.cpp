#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, LayeredLpvGridConfigUbo )

namespace castor3d
{
	std::string const LayeredLpvGridConfigUbo::LayeredLpvConfig = "LayeredLpvConfig";
	std::string const LayeredLpvGridConfigUbo::AllMinVolumeCorners = "c3d_allMinVolumeCorners";
	std::string const LayeredLpvGridConfigUbo::AllCellSizes = "c3d_allCellSizes";
	std::string const LayeredLpvGridConfigUbo::GridSize = "c3d_gridSize";

	LayeredLpvGridConfigUbo::LayeredLpvGridConfigUbo()
	{
	}

	LayeredLpvGridConfigUbo::LayeredLpvGridConfigUbo( RenderDevice const & device )
	{
		initialise( device );
	}

	LayeredLpvGridConfigUbo::~LayeredLpvGridConfigUbo()
	{
		cleanup();
	}

	void LayeredLpvGridConfigUbo::initialise( RenderDevice const & device )
	{
		m_device = &device;
		m_ubo = m_device->uboPools->getBuffer< Configuration >( 0u );
	}

	void LayeredLpvGridConfigUbo::cleanup()
	{
		if ( m_ubo )
		{
			m_device->uboPools->putBuffer( m_ubo );
			m_device = nullptr;
			m_ubo = {};
		}
	}

	void LayeredLpvGridConfigUbo::cpuUpdate( std::array< castor::Grid, shader::DirectionalMaxCascadesCount > const & grids
		, float indirectAttenuation )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();

		for ( auto i = 0u; i < grids.size(); ++i )
		{
			auto min = grids[i].getMin();
			data.allMinVolumeCorners[i] = castor::Point4f{ min->x, min->y, min->z, 0.0f };
			data.allCellSizes[i] = grids[i].getCellSize();
		}

		auto dim = grids[0].getDimensions();
		data.gridSize = castor::Point4ui{ dim->x, dim->y, dim->z, 0u };
	}
}
