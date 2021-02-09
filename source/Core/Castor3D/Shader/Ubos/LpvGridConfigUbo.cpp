#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/Writer.hpp>

CU_ImplementCUSmartPtr( castor3d, LpvGridConfigUbo )

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		LpvGridData::LpvGridData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, minVolumeCornerSize{ getMember< sdw::Vec4 >( "minVolumeCornerSize" ) }
			, gridSizeAtt{ getMember< sdw::Vec4 >( "gridSizeAtt" ) }
			, minVolumeCorner{ minVolumeCornerSize.xyz() }
			, cellSize{ minVolumeCornerSize.w() }
			, gridSize{ gridSizeAtt.xyz() }
			, gridWidth{ gridSizeAtt.x() }
			, gridHeight{ gridSizeAtt.y() }
			, gridDepth{ gridSizeAtt.z() }
			, indirectAttenuation{ gridSizeAtt.w() }
		{
		}

		LpvGridData & LpvGridData::operator=( LpvGridData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr LpvGridData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "LpvGridData" );

			if ( result->empty() )
			{
				result->declMember( "minVolumeCornerSize", ast::type::Kind::eVec4F );
				result->declMember( "gridSizeAtt", ast::type::Kind::eVec4F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > LpvGridData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}

	//*********************************************************************************************

	uint32_t const LpvGridConfigUbo::BindingPoint = 12;
	std::string const LpvGridConfigUbo::LpvGridData = "c3d_lpvGridData";
	std::string const LpvGridConfigUbo::LpvGridConfig = "lpvGridConfig";

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
