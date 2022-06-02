#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"

#include "Castor3D/Limits.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	castor::String const ObjectIds::DataName = cuT( "c3d_objectIds" );

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
			, "C3D_ObjectIds" );

		if ( result->empty() )
		{
			result->declMember( "data"
				, ast::type::Kind::eVec4U
				, sdw::type::NotArray );
		}

		return result;
	}

	//*********************************************************************************************

	castor::String const ObjectsIds::BufferName = cuT( "ObjectIdsBuffer" );
	castor::String const ObjectsIds::DataName = cuT( "c3d_objectIdsData" );

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
			, "C3D_" + BufferName );

		if ( result->empty() )
		{
			result->declMember( "data"
				, ast::type::Kind::eVec4U
				, MaxNodesPerPipeline );
		}

		return result;
	}

	//*********************************************************************************************

	sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, sdw::UInt pipelineID
		, sdw::Int drawID )
	{
		auto & writer = *data.getWriter();
		return writer.declLocale( "objectIdsData"
			, data[pipelineID].getNodeId( writer.cast< sdw::UInt >( drawID ) ) );
	}

	//*********************************************************************************************
}
