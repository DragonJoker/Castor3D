#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( c3d, LpvGridConfigUbo )

namespace c3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::IVec3 LpvGridData::worldToGrid( sdw::Vec3 const & pos )const
		{
			return ivec3( ( pos - minVolumeCorner ) / vec3( cellSize() ) - vec3( 0.5_f ) );
		}

		sdw::IVec3 LpvGridData::worldToGrid( sdw::Vec3 const & pos
			, sdw::Vec3 const & nml )const
		{
			return ivec3( ( pos - minVolumeCorner ) / vec3( cellSize() ) + 0.5_f * nml );
		}

		sdw::Vec3 LpvGridData::worldToTex( sdw::Vec3 const & pos )const
		{
			return ( pos - minVolumeCorner ) / cellSize() / gridSize;
		}

		sdw::Vec2 LpvGridData::gridToScreen( sdw::IVec2 const & pos )const
		{
			return ( vec2( pos.xy() ) + 0.5_f ) / vec2( gridSize.xy() ) * 2.0_f - 1.0_f;
		}

		sdw::Vec3 LpvGridData::nextGrid( sdw::IVec3 const & pos
			, sdw::Vec3 const & dir )const
		{
			return ( vec3( pos ) + 0.5_f * dir ) / gridSize;
		}
	}

	//*********************************************************************************************

	LpvGridConfigUbo::LpvGridConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( MemoryPropertyFlags::eNone ) }
	{
	}
	
	LpvGridConfigUbo::~LpvGridConfigUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	Grid const & LpvGridConfigUbo::cpuUpdate( BoundingBox const & aabb
		, Point3f const & cameraPos
		, uint32_t gridDim
		, float indirectAttenuation )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		auto cellSize = std::max( std::max( aabb.getDimensions()->x
			, aabb.getDimensions()->y )
			, aabb.getDimensions()->z ) / float( gridDim );
		m_grid = { gridDim, cellSize, aabb.getMax(), aabb.getMin(), 1.0f };

		auto minVolumeCorner = m_grid.getMin();
		auto gridSize = m_grid.getDimensions();
		cellSize = m_grid.getCellSize();

		configuration.minVolumeCorner = Point4f{ minVolumeCorner->x, minVolumeCorner->y, minVolumeCorner->z, cellSize };
		configuration.gridSizeAtt = Point4f{ gridSize->x, gridSize->y, gridSize->z, indirectAttenuation };
		configuration.cameraPos = Point4f{ cameraPos->x, cameraPos->y, cameraPos->z, 0.0f };

		return m_grid;
	}

	Grid const & LpvGridConfigUbo::cpuUpdate( float gridLevelScale
		, Grid const & grid
		, Point3f const & cameraPos
		, Point3f const & cameraDir
		, float indirectAttenuation )
	{
		auto & configuration = m_ubo.getData();
		m_grid = { grid, gridLevelScale };
		m_grid.transform( cameraPos, cameraDir );

		auto minVolumeCorner = m_grid.getMin();
		auto gridSize = m_grid.getDimensions();
		auto cellSize = m_grid.getCellSize();

		configuration.minVolumeCorner = Point4f{ minVolumeCorner->x, minVolumeCorner->y, minVolumeCorner->z, cellSize };
		configuration.gridSizeAtt = Point4f{ gridSize->x, gridSize->y, gridSize->z, indirectAttenuation };
		configuration.cameraPos = Point4f{ cameraPos->x, cameraPos->y, cameraPos->z, 0.0f };

		return m_grid;
	}
}
