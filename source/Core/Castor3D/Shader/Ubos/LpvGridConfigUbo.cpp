#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

CU_ImplementCUSmartPtr( castor3d, LpvGridConfigUbo )

namespace castor3d
{
	uint32_t const LpvGridConfigUbo::BindingPoint = 12;
	std::string const LpvGridConfigUbo::LpvGridConfig = "LpvGridConfig";
	std::string const LpvGridConfigUbo::MinVolumeCornerSize = "c3d_minVolumeCornerSize";
	std::string const LpvGridConfigUbo::GridSizes = "c3d_gridSizes";

	LpvGridConfigUbo::LpvGridConfigUbo()
	{
	}

	LpvGridConfigUbo::LpvGridConfigUbo( RenderDevice const & device )
	{
		initialise( device );
	}
	
	LpvGridConfigUbo::~LpvGridConfigUbo()
	{
		cleanup();
	}

	void LpvGridConfigUbo::initialise( RenderDevice const & device )
	{
		m_device = &device;
		m_ubo = m_device->uboPools->getBuffer< Configuration >( 0u );
	}

	void LpvGridConfigUbo::cleanup()
	{
		if ( m_ubo )
		{
			m_device->uboPools->putBuffer( m_ubo );
			m_device = nullptr;
			m_ubo = {};
		}
	}

	void LpvGridConfigUbo::cpuUpdate( castor::BoundingBox const & aabb
		, castor::Point3f const & cameraPos
		, castor::Point3f const & cameraDir
		, uint32_t gridDim
		, float indirectAttenuation )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		auto cellSize = std::max( std::max( aabb.getDimensions()->x
			, aabb.getDimensions()->y )
			, aabb.getDimensions()->z ) / gridDim;
		castor::Grid grid{ gridDim, cellSize, aabb.getMax(), aabb.getMin(), 1.0f, 0 };
		grid.transform( cameraPos, cameraDir );

		auto minVolumeCorner = grid.getMin();
		auto gridSize = grid.getDimensions();
		cellSize = grid.getCellSize();

		configuration.minVolumeCorner = castor::Point4f{ minVolumeCorner->x, minVolumeCorner->y, minVolumeCorner->z, cellSize };
		configuration.gridSizeAtt = castor::Point4f{ gridSize->x, gridSize->y, gridSize->z, indirectAttenuation };
	}

	castor::Grid LpvGridConfigUbo::cpuUpdate( uint32_t gridLevel
		, float gridLevelScale
		, castor::Grid const & grid
		, castor::BoundingBox const & aabb
		, castor::Point3f const & cameraPos
		, castor::Point3f const & cameraDir
		, float indirectAttenuation )
	{
		auto & configuration = m_ubo.getData();
		castor::Grid levelGrid{ grid, gridLevelScale, gridLevel };
		levelGrid.transform( cameraPos, cameraDir );

		auto minVolumeCorner = levelGrid.getMin();
		auto gridSize = levelGrid.getDimensions();
		auto cellSize = levelGrid.getCellSize();

		configuration.minVolumeCorner = castor::Point4f{ minVolumeCorner->x, minVolumeCorner->y, minVolumeCorner->z, cellSize };
		configuration.gridSizeAtt = castor::Point4f{ gridSize->x, gridSize->y, gridSize->z, indirectAttenuation };

		return levelGrid;
	}
}
