/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelIndexUbo_H___
#define ___C3D_ModelIndexUbo_H___

#include "UbosModule.hpp"
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
		C3D_API sdw::Mat3 getNormalMtx( SubmeshFlags submeshFlags
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Mat3 getNormalMtx( ProgramFlags programsFlags
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Vec4 worldToModel( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToCurWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToPrvWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Mat4 getCurModelMtx( ProgramFlags programsFlags
			, sdw::Mat4 const & transform )const;
		C3D_API sdw::Mat4 getCurModelMtx( SkinningData const & skinning
			, sdw::UInt const & skinningId
			, sdw::UVec4 const & boneIds0
			, sdw::UVec4 const & boneIds1
			, sdw::Vec4 const & boneWeights0
			, sdw::Vec4 const & boneWeights1 )const;

		sdw::UInt getMaterialId()const
		{
			return m_materialId;
		}

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

		sdw::Vec3 const & getScale()const
		{
			return m_scale;
		}

		sdw::UInt const & getMeshletCount()const
		{
			return m_meshletCount;
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
		sdw::UInt m_materialId;
		sdw::Int m_shadowReceiver;
		sdw::Int m_envMapId;
		sdw::Int m_pad;
		sdw::Vec3 m_scale;
		sdw::UInt m_meshletCount;
	};
}

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
