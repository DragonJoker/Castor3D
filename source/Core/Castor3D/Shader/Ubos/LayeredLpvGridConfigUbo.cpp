#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ShaderWriter/Writer.hpp>

CU_ImplementSmartPtr( c3d, LayeredLpvGridConfigUbo )

namespace c3d
{
	//*********************************************************************************************

	LayeredLpvGridConfigUbo::LayeredLpvGridConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( MemoryPropertyFlags::eNone ) }
	{
	}

	LayeredLpvGridConfigUbo::~LayeredLpvGridConfigUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void LayeredLpvGridConfigUbo::cpuUpdate( Array< Grid const *, LpvMaxCascadesCount > const & grids
		, float indirectAttenuation )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();

		for ( auto i = 0u; i < grids.size(); ++i )
		{
			auto min = grids[i]->getMin();
			data.allMinVolumeCorners[i] = Point4f{ min->x, min->y, min->z, 0.0f };
			data.allCellSizes[i] = grids[i]->getCellSize();
		}

		auto dim = grids[0]->getDimensions();
		data.gridSizeAtt = Point4f{ dim->x, dim->y, dim->z, indirectAttenuation };
	}
}
