/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectIdsUbo_H___
#define ___C3D_ObjectIdsUbo_H___

#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslObjectIds.hpp"

#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d::shader
{
	struct ObjectsIds
		: public sdw::StructInstanceHelperT< "C3D_ObjectsIdsData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::U32Vec4ArrayField< "data", uint32_t( MaxNodesPerPipeline / 4u ) > >
	{
		SDW_DeclStructInstance( C3D_API, ObjectsIds );

		ObjectsIds( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, m_data{ StructInstanceHelperT::getMember< "data" >() }
		{
		}

		sdw::UInt getNodeId( sdw::UInt const & index )const
		{
			return m_data[index / 4_u][index % 4_u];
		}

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Array< sdw::UVec4 > m_data;
	};

	C3D_API sdw::UInt getNodeId( sdw::ArrayStorageBufferT< shader::ObjectsIds > const & data
		, sdw::UInt const & pipelineID
		, sdw::UInt const & drawID );
	C3D_API sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, sdw::UInt const & pipelineID
		, sdw::UInt const & drawID );

	template< ast::var::Flag FlagT, template< typename DataT > typename ArrayT >
	static sdw::UInt getNodeId( ArrayT< shader::ObjectsIds > const & data
		, shader::MeshVertexT< FlagT > const & surface
		, sdw::UInt const & pipelineID
		, sdw::UInt const & drawID
		, PipelineFlags const & flags )
	{
		if ( flags.enableInstantiation() )
		{
			return surface.objectIds.x();
		}

		return getNodeId( data, pipelineID, drawID );
	}

	template< template< typename DataT > typename ArrayT >
	static sdw::UInt getNodeId( ArrayT< shader::ObjectsIds > const & data
		, sdw::Array< sdw::UVec4 > const & instances
		, sdw::UInt const & pipelineID
		, sdw::UInt const & drawID
		, PipelineFlags const & flags )
	{
		if ( flags.enableInstantiation() )
		{
			return instances[drawID].x();
		}

		return getNodeId( data, pipelineID, drawID );
	}

	template< template< typename DataT > typename ArrayT >
	static sdw::UInt getNodeId( ArrayT< shader::ObjectsIds > const & data
		, sdw::Array< shader::ObjectIds > const & instances
		, sdw::UInt const & pipelineID
		, sdw::UInt const & drawID
		, PipelineFlags const & flags )
	{
		if ( flags.enableInstantiation() )
		{
			return instances[drawID].nodeId();
		}

		return getNodeId( data, pipelineID, drawID );
	}
}

#define C3D_ObjectIdsData( writer, flags, binding, set )\
	sdw::StorageBuffer objectIdsDataBuffer{ writer\
		, "C3D_ObjectsIds"\
		, "c3d_objectsIds"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, !flags.enableInstantiation() };\
	auto c3d_objectIdsData = objectIdsDataBuffer.declMemberArray< c3d::shader::ObjectsIds >( "d"\
		, !flags.enableInstantiation() );\
	objectIdsDataBuffer.end()

#endif
