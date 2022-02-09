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
	struct ModelIndex
		: public sdw::StructInstance
	{
		C3D_API ModelIndex( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, ModelIndex );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		C3D_API sdw::UVec4 getTextures0( ProgramFlags programFlags
			, sdw::UVec4 const & instanceData )const;
		C3D_API sdw::UVec4 getTextures0()const;
		C3D_API sdw::UVec4 getTextures1( ProgramFlags programFlags
			, sdw::UVec4 const & instanceData )const;
		C3D_API sdw::UVec4 getTextures1()const;
		C3D_API sdw::Int getTextures( ProgramFlags programFlags
			, sdw::Int const & instanceData )const;
		C3D_API sdw::Int getTextures()const;
		C3D_API sdw::UInt getMaterialId( ProgramFlags programFlags
			, sdw::Int const & instanceData )const;
		C3D_API sdw::UInt getMaterialId()const;
		C3D_API sdw::Int getNodeId( ProgramFlags programFlags
			, sdw::Int const & instanceData )const;
		C3D_API sdw::Int getNodeId()const;
		C3D_API static sdw::UInt getTexture( sdw::UVec4 const & textures0
			, sdw::UVec4 const & textures1
			, uint32_t index );

	public:
		C3D_API static castor::String const BufferName;
		C3D_API static castor::String const DataName;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::UVec4 m_textures0;
		sdw::UVec4 m_textures1;
		sdw::Int m_textures;
		sdw::Int m_materialId;
		sdw::Int m_nodeId;
	};
}

#define UBO_MODEL_INDEX( writer, binding, set )\
	sdw::Ubo modelBufferIndex{ writer\
		, castor3d::shader::ModelIndex::BufferName\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_modelIndex = modelBufferIndex.declMember< castor3d::shader::ModelIndex >( castor3d::shader::ModelIndex::DataName );\
	modelBufferIndex.end()

#endif
