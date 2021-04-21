#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ShaderWriter/Writer.hpp>

CU_ImplementCUSmartPtr( castor3d, LayeredLpvGridConfigUbo )

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		LayeredLpvGridData::LayeredLpvGridData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, allMinVolumeCorners{ getMemberArray< sdw::Vec4 >( "allMinVolumeCorners" ) }
			, allCellSizes{ getMember< sdw::Vec4 >( "allCellSizes" ) }
			, gridSizesAtt{ getMember< sdw::Vec4 >( "gridSizesAtt" ) }
			, gridSizes{ gridSizesAtt.xyz() }
			, indirectAttenuation{ gridSizesAtt.w() }
		{
		}

		LayeredLpvGridData & LayeredLpvGridData::operator=( LayeredLpvGridData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr LayeredLpvGridData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "LayeredLpvGridData" );

			if ( result->empty() )
			{
				result->declMember( "allMinVolumeCorners", ast::type::Kind::eVec4F, LpvMaxCascadesCount );
				result->declMember( "allCellSizes", ast::type::Kind::eVec4F );
				result->declMember( "gridSizesAtt", ast::type::Kind::eVec4F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > LayeredLpvGridData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}

	//*********************************************************************************************

	std::string const LayeredLpvGridConfigUbo::LayeredLpvConfig = "LayeredLpvConfig";
	std::string const LayeredLpvGridConfigUbo::LayeredLpvGridData = "c3d_llpvGridData";

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

	void LayeredLpvGridConfigUbo::cpuUpdate( std::array< castor::Grid, shader::LpvMaxCascadesCount > const & grids
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
		data.gridSizeAtt = castor::Point4f{ dim->x, dim->y, dim->z, indirectAttenuation };
	}
}
