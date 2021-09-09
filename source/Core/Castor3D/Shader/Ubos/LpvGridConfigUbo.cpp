#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/Source.hpp>

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
			, cameraPos4{ getMember< sdw::Vec4 >( "cameraPosition" ) }
			, minVolumeCorner{ minVolumeCornerSize.xyz() }
			, gridWidth{ gridSizeAtt.x() }
			, gridHeight{ gridSizeAtt.y() }
			, gridDepth{ gridSizeAtt.z() }
			, gridSize{ gridSizeAtt.xyz() }
			, cameraPos{ cameraPos4.xyz() }
			, m_cellSize{ minVolumeCornerSize.w() }
			, m_indirectAttenuation{ gridSizeAtt.w() }
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
				, "C3D_LpvGridData" );

			if ( result->empty() )
			{
				result->declMember( "minVolumeCornerSize", ast::type::Kind::eVec4F );
				result->declMember( "gridSizeAtt", ast::type::Kind::eVec4F );
				result->declMember( "cameraPosition", ast::type::Kind::eVec4F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > LpvGridData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

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

	std::string const LpvGridConfigUbo::LpvGridData = "c3d_lpvGridData";
	std::string const LpvGridConfigUbo::LpvGridConfig = "lpvGridConfig";

	LpvGridConfigUbo::LpvGridConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}
	
	LpvGridConfigUbo::~LpvGridConfigUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	castor::Grid const & LpvGridConfigUbo::cpuUpdate( castor::BoundingBox const & aabb
		, castor::Point3f const & cameraPos
		, castor::Point3f const & cameraDir
		, uint32_t gridDim
		, float indirectAttenuation )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		auto cellSize = std::max( std::max( aabb.getDimensions()->x
			, aabb.getDimensions()->y )
			, aabb.getDimensions()->z ) / float( gridDim );
		m_grid = { gridDim, cellSize, aabb.getMax(), aabb.getMin(), 1.0f, 0 };
		//m_grid.transform( cameraPos, cameraDir );

		auto minVolumeCorner = m_grid.getMin();
		auto gridSize = m_grid.getDimensions();
		cellSize = m_grid.getCellSize();

		configuration.minVolumeCorner = castor::Point4f{ minVolumeCorner->x, minVolumeCorner->y, minVolumeCorner->z, cellSize };
		configuration.gridSizeAtt = castor::Point4f{ gridSize->x, gridSize->y, gridSize->z, indirectAttenuation };
		configuration.cameraPos = castor::Point4f{ cameraPos->x, cameraPos->y, cameraPos->z, 0.0f };

		return m_grid;
	}

	castor::Grid const & LpvGridConfigUbo::cpuUpdate( uint32_t gridLevel
		, float gridLevelScale
		, castor::Grid const & grid
		, castor::BoundingBox const & aabb
		, castor::Point3f const & cameraPos
		, castor::Point3f const & cameraDir
		, float indirectAttenuation )
	{
		auto & configuration = m_ubo.getData();
		m_grid = { grid, gridLevelScale, gridLevel };
		m_grid.transform( cameraPos, cameraDir );

		auto minVolumeCorner = m_grid.getMin();
		auto gridSize = m_grid.getDimensions();
		auto cellSize = m_grid.getCellSize();

		configuration.minVolumeCorner = castor::Point4f{ minVolumeCorner->x, minVolumeCorner->y, minVolumeCorner->z, cellSize };
		configuration.gridSizeAtt = castor::Point4f{ gridSize->x, gridSize->y, gridSize->z, indirectAttenuation };
		configuration.cameraPos = castor::Point4f{ cameraPos->x, cameraPos->y, cameraPos->z, 0.0f };

		return m_grid;
	}
}
