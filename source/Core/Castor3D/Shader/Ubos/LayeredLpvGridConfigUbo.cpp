#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"

CU_ImplementSmartPtr( c3d, LayeredLpvGridConfigUbo )

namespace c3d
{
	LayeredLpvGridConfigUbo::LayeredLpvGridConfigUbo( RenderDevice const & device )
		: UboT{ device }
	{
	}

	void LayeredLpvGridConfigUbo::cpuUpdate( Array< Grid const *, LpvMaxCascadesCount > const & grids
		, float indirectAttenuation )
	{
		auto & data = getNCData();

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
