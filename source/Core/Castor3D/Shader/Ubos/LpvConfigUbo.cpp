#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

namespace castor3d
{
	std::string const LpvConfigUbo::LpvConfig = "LpvConfig";
	std::string const LpvConfigUbo::LightView = "c3d_lightView";
	std::string const LpvConfigUbo::MinVolumeCornerSize = "c3d_minVolumeCornerSize";
	std::string const LpvConfigUbo::GridSizes = "c3d_gridSizes";
	std::string const LpvConfigUbo::Config = "c3d_lpvConfig";

	LpvConfigUbo::LpvConfigUbo( RenderDevice const & device
		, uint32_t index )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}
	
	LpvConfigUbo::~LpvConfigUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void LpvConfigUbo::cpuUpdate( Light const & light
		, castor::BoundingBox const & aabb
		, castor::Point3f const & cameraPos
		, castor::Point3f const & cameraDir
		, uint32_t gridDim )
	{
		CU_Require( m_ubo );
		auto & lpvConfig = light.getLpvConfig();
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
		configuration.gridSizes = castor::Point4f{ gridSize->x, gridSize->y, gridSize->z, light.getBufferIndex() };
		configuration.indirectAttenuation = lpvConfig.indirectAttenuation;
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		auto ltType = light.getLightType();

		switch ( ltType )
		{
		case LightType::eDirectional:
			configuration.lightView = light.getDirectionalLight()->getViewMatrix( 3u );
			break;

		case LightType::ePoint:
			break;

		case LightType::eSpot:
			{
				auto & spotLight = *light.getSpotLight();
				configuration.lightView = spotLight.getViewMatrix();
				auto lightFov = spotLight.getCutOff();
				configuration.tanFovXHalf = ( lightFov * 0.5 ).tan();
				configuration.tanFovYHalf = ( lightFov * 0.5 ).tan();
			}
			break;
		}
	}

	castor::Grid LpvConfigUbo::cpuUpdate( DirectionalLight const & light
		, uint32_t cascadeIndex
		, castor::Grid const & grid
		, castor::BoundingBox const & aabb
		, castor::Point3f const & cameraPos
		, castor::Point3f const & cameraDir )
	{
		auto & lpvConfig = light.getLight().getLpvConfig();
		auto & configuration = m_ubo.getData();
		castor::Grid levelGrid{ grid, light.getSplitScale( cascadeIndex ), cascadeIndex };
		levelGrid.transform( cameraPos, cameraDir );

		auto minVolumeCorner = levelGrid.getMin();
		auto gridSize = levelGrid.getDimensions();
		auto cellSize = levelGrid.getCellSize();

		configuration.indirectAttenuation = lpvConfig.indirectAttenuation;
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		configuration.minVolumeCorner = castor::Point4f{ minVolumeCorner->x, minVolumeCorner->y, minVolumeCorner->z, cellSize };
		configuration.gridSizes = castor::Point4f{ gridSize->x, gridSize->y, gridSize->z, light.getLight().getBufferIndex() };
		configuration.lightView = light.getViewMatrix( cascadeIndex );

		return levelGrid;
	}
}
