/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectIdsUbo_H___
#define ___C3D_ObjectIdsUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d::shader
{
	struct ObjectIds
		: public sdw::StructInstance
	{
		C3D_API ObjectIds( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, ObjectIds );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		C3D_API static castor::String const DataName;

	private:
		sdw::UVec4 m_data;

	public:
		sdw::UInt nodeId;
		sdw::UInt morphingId;
		sdw::UInt skinningId;
	};

	struct ObjectsIds
		: public sdw::StructInstance
	{
		struct Ids
		{
			sdw::UInt nodeId;
		};

		C3D_API ObjectsIds( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, ObjectsIds );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		sdw::UInt getNodeId( sdw::UInt const & index )const
		{
			return m_data[index].x();
		}

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		C3D_API static castor::String const BufferName;
		C3D_API static castor::String const DataName;

	private:
		sdw::Array< sdw::UVec4 > m_data;
	};

	template< ast::var::Flag FlagT >
	static sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, shader::VertexSurfaceT< FlagT > const & surface
		, sdw::UInt pipelineID
		, sdw::UInt drawID
		, ProgramFlags const & flags )
	{
		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			return surface.objectIds.x();
		}

		return data[pipelineID].getNodeId( drawID );
	}

	C3D_API sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, sdw::UInt pipelineID
		, sdw::UInt drawID );

	static sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, sdw::Array< sdw::UVec4 > const & instances
		, sdw::UInt pipelineID
		, sdw::UInt drawID
		, ProgramFlags const & flags )
	{
		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			return instances[drawID].x();
		}

		return getNodeId( data, pipelineID, drawID );
	}
}

#define C3D_ObjectIdsData( writer, binding, set )\
	sdw::Ssbo objectIdsDataBuffer{ writer\
		, castor3d::shader::ObjectsIds::BufferName\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, true };\
	auto c3d_objectIdsData = objectIdsDataBuffer.declMemberArray< castor3d::shader::ObjectsIds >( castor3d::shader::ObjectsIds::DataName );\
	objectIdsDataBuffer.end()

#define C3D_ObjectIdsDataOpt( writer, binding, set, enable )\
	sdw::Ssbo objectIdsDataBuffer{ writer\
		, castor3d::shader::ObjectsIds::BufferName\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, enable };\
	auto c3d_objectIdsData = objectIdsDataBuffer.declMemberArray< castor3d::shader::ObjectsIds >( castor3d::shader::ObjectsIds::DataName, enable );\
	objectIdsDataBuffer.end()

#endif
