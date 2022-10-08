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
		C3D_API ObjectsIds( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, ObjectsIds );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

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

	struct PipelineObjectsIds
		: public sdw::StructInstance
	{
		C3D_API PipelineObjectsIds( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, PipelineObjectsIds );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		C3D_API sdw::UInt getNodeId( sdw::UInt const & pipelineID
			, sdw::UInt const & instanceID )const;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Array< sdw::UVec4 > m_data;
	};

	template< ast::var::Flag FlagT >
	static sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, shader::VertexSurfaceT< FlagT > const & surface
		, sdw::UInt pipelineID
		, sdw::UInt drawID
		, PipelineFlags const & flags )
	{
		if ( flags.enableInstantiation() )
		{
			return surface.objectIds.x();
		}

		return data[pipelineID].getNodeId( drawID );
	}

	C3D_API sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, sdw::UInt pipelineID
		, sdw::UInt drawID );
	C3D_API sdw::UInt getNodeId( sdw::Array< shader::PipelineObjectsIds > const & data
		, sdw::UInt pipelineID
		, sdw::UInt drawID );

	inline sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, sdw::Array< sdw::UVec4 > const & instances
		, sdw::UInt pipelineID
		, sdw::UInt drawID
		, PipelineFlags const & flags )
	{
		if ( flags.enableInstantiation() )
		{
			return instances[drawID].x();
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
	auto c3d_objectIdsData = objectIdsDataBuffer.declMemberArray< castor3d::shader::ObjectsIds >( "d"\
		, !flags.enableInstantiation() );\
	objectIdsDataBuffer.end()

#define C3D_ObjectsIdsData( writer, flags, binding, set )\
	sdw::StorageBuffer objectsIdsDataBuffer{ writer\
		, "C3D_PipelineObjectsIds"\
		, "c3d_pipelineObjectsIds"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, !flags.enableInstantiation() };\
	auto c3d_objectsIdsData = objectsIdsDataBuffer.declMemberArray< castor3d::shader::PipelineObjectsIds >( "d"\
		, !flags.enableInstantiation() );\
	objectsIdsDataBuffer.end()

#endif
