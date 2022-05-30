/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelIndexUbo_H___
#define ___C3D_ModelIndexUbo_H___

#include "SkinningUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d::shader
{
	struct ModelIndices
		: public sdw::StructInstance
	{
		C3D_API ModelIndices( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, ModelIndices );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		C3D_API sdw::Mat4 getPrvModelMtx( ProgramFlags programsFlags
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Mat3 getNormalMtx( ProgramFlags programsFlags
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Vec4 worldToModel( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToCurWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToPrvWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Mat4 getCurModelMtx( ProgramFlags programsFlags
			, SkinningData const & skinning
			, sdw::Mat4 const & transform
			, sdw::UInt const & skinningId
			, sdw::UVec4 const & boneIds0
			, sdw::UVec4 const & boneIds1
			, sdw::Vec4 const & boneWeights0
			, sdw::Vec4 const & boneWeights1 )const;
		C3D_API sdw::Mat4 getCurModelMtx( ProgramFlags programsFlags
			, SkinningData const & skinning
			, sdw::UInt const & skinningId
			, sdw::UVec4 const & boneIds0
			, sdw::UVec4 const & boneIds1
			, sdw::Vec4 const & boneWeights0
			, sdw::Vec4 const & boneWeights1 )const;
		C3D_API sdw::UVec4 getTextures0()const;
		C3D_API sdw::UVec4 getTextures1()const;
		C3D_API sdw::Int getTextures()const;
		C3D_API sdw::UInt getMaterialId()const;
		C3D_API static sdw::UInt getTexture( sdw::UVec4 const & textures0
			, sdw::UVec4 const & textures1
			, uint32_t index );

		sdw::Int const & isShadowReceiver()const
		{
			return m_shadowReceiver;
		}

		sdw::Int const & getEnvMapIndex()const
		{
			return m_envMapId;
		}

		sdw::Mat4 const & getModelMtx()const
		{
			return m_curMtxModel;
		}

	public:
		C3D_API static castor::String const BufferName;
		C3D_API static castor::String const DataName;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Mat4 m_prvMtxModel;
		sdw::Mat4 m_curMtxModel;
		sdw::Mat4 m_mtxNormal;
		sdw::UVec4 m_textures0;
		sdw::UVec4 m_textures1;
		sdw::IVec4 m_countsIDs;
		sdw::Int m_textures;
		sdw::Int m_materialId;
		sdw::Int m_envMapId;
		sdw::Int m_shadowReceiver;
	};

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
		sdw::UInt morphTargetsCount;
		sdw::UInt skinningId;
	};

	struct ObjectsIds
		: public sdw::StructInstance
	{
		struct Ids
		{
			sdw::UInt nodeId;
			sdw::UInt morphingId;
			sdw::UInt morphTargetsCount;
			sdw::UInt skinningId;
		};

		C3D_API ObjectsIds( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, ObjectsIds );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		sdw::UVec4 operator[]( sdw::UInt const & index )const
		{
			return m_data[index];
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
	static shader::ObjectsIds::Ids getIds( sdw::Array< shader::ObjectsIds > const & data
		, shader::VertexSurfaceT< FlagT > const & surface
		, sdw::UInt pipelineID
		, sdw::Int drawID
		, ProgramFlags const & flags )
	{
		auto & writer = *data.getWriter();

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			return { writer.declLocale( "nodeId"
					, surface.objectIds.x() )
				, writer.declLocale( "morphingId"
					, surface.objectIds.y()
					, checkFlag( flags, ProgramFlag::eMorphing ) )
				, writer.declLocale( "morphTargetsCount"
					, surface.objectIds.z()
					, checkFlag( flags, ProgramFlag::eMorphing ) )
				, writer.declLocale( "skinningId"
					, surface.objectIds.w()
					, checkFlag( flags, ProgramFlag::eSkinning ) ) };
		}

		auto objectIdsData = writer.declLocale( "objectIdsData"
			, data[pipelineID][writer.cast< sdw::UInt >( drawID )] );
		return { writer.declLocale( "nodeId"
				, objectIdsData.x() )
			, writer.declLocale( "morphingId"
				, objectIdsData.y()
				, checkFlag( flags, ProgramFlag::eMorphing ) )
			, writer.declLocale( "morphTargetsCount"
				, objectIdsData.z()
				, checkFlag( flags, ProgramFlag::eMorphing ) )
			, writer.declLocale( "skinningId"
				, objectIdsData.w()
				, checkFlag( flags, ProgramFlag::eSkinning ) ) };
	}

	template< ast::var::Flag FlagT >
	static sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, shader::VertexSurfaceT< FlagT > const & surface
		, sdw::UInt pipelineID
		, sdw::Int drawID
		, ProgramFlags const & flags )
	{
		auto & writer = *data.getWriter();

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			return surface.objectIds.x();
		}

		auto objectIdsData = writer.declLocale( "objectIdsData"
			, data[pipelineID][writer.cast< sdw::UInt >( drawID )] );
		return objectIdsData.x();
	}

	C3D_API sdw::UInt getNodeId( sdw::Array< shader::ObjectsIds > const & data
		, sdw::UInt pipelineID
		, sdw::Int drawID );
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


#define C3D_ModelsData( writer, binding, set )\
	sdw::Ssbo modelsBufferIndices{ writer\
		, castor3d::shader::ModelIndices::BufferName\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, true };\
	auto c3d_modelsData = modelsBufferIndices.declMemberArray< castor3d::shader::ModelIndices >( castor3d::shader::ModelIndices::DataName );\
	modelsBufferIndices.end()

#define C3D_ModelData( writer, binding, set )\
	sdw::Ubo modelBufferIndices{ writer\
		, castor3d::shader::ModelIndices::BufferName\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_modelData = modelBufferIndices.declMember< castor3d::shader::ModelIndices >( castor3d::shader::ModelIndices::DataName );\
	modelBufferIndices.end()

#endif
