#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"

#include "Castor3D/Limits.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************
	
	namespace objdsubo
	{
		static auto constexpr MaxUVec4PerPipeline = uint32_t( MaxNodesPerPipeline / 4u );
	}

	//*********************************************************************************************

	ObjectIds::ObjectIds( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_data{ getMember< sdw::UVec4 >( "data" ) }
		, nodeId{ m_data.x() }
		, morphingId{ m_data.y() }
		, skinningId{ m_data.z() }
	{
	}

	ast::type::BaseStructPtr ObjectIds::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_ObjectIdsData" );

		if ( result->empty() )
		{
			result->declMember( "data"
				, ast::type::Kind::eVec4U
				, sdw::type::NotArray );
		}

		return result;
	}

	//*********************************************************************************************

	ObjectsIds::ObjectsIds( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_data{ getMemberArray< sdw::UVec4 >( "data" ) }
	{
	}

	ast::type::BaseStructPtr ObjectsIds::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_ObjectsIdsData" );

		if ( result->empty() )
		{
			result->declMember( "data"
				, ast::type::Kind::eVec4U
				, objdsubo::MaxUVec4PerPipeline );
		}

		return result;
	}

	//*********************************************************************************************

	PipelineObjectsIds::PipelineObjectsIds( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_data{ getMemberArray< sdw::UVec4 >( "datas" ) }
	{
	}

	ast::type::BaseStructPtr PipelineObjectsIds::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_PipelineObjectsIdsData" );

		if ( result->empty() )
		{
			result->declMember( "datas"
				, ast::type::Kind::eVec4U
				, uint32_t( objdsubo::MaxUVec4PerPipeline * MaxPipelines ) );
		}

		return result;
	}

	sdw::UInt PipelineObjectsIds::getNodeId( sdw::UInt const & pipelineID
		, sdw::UInt const & drawId )const
	{
		return m_data[( objdsubo::MaxUVec4PerPipeline * pipelineID ) + drawId / 4_u][drawId % 4_u];
	}

	//*********************************************************************************************

	sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, sdw::UInt pipelineID
		, sdw::UInt drawID )
	{
		return data[pipelineID].getNodeId( drawID );
	}

	sdw::UInt getNodeId( sdw::Array< shader::PipelineObjectsIds > const & datas
		, sdw::UInt pipelineID
		, sdw::UInt drawID )
	{
		return datas[pipelineID].getNodeId( pipelineID, drawID );
	}

	//*********************************************************************************************
}
