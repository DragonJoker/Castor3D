/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMeshVertex_H___
#define ___C3D_GlslMeshVertex_H___

#include "SdwModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d::shader
{
	struct MeshVertexBase
		: public sdw::StructInstance
	{
		C3D_API MeshVertexBase( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		C3D_API static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, SubmeshShaders const & submeshShaders
			, sdw::var::Flag flag );
		C3D_API static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, SubmeshShaders const & submeshShaders );

		// Base
		sdw::Vec4 position;
		sdw::Vec3 normal;
		sdw::Vec4 tangent;
		sdw::Vec3 bitangent;
		sdw::Vec3 texture0;
		sdw::Vec3 texture1;
		sdw::Vec3 texture2;
		sdw::Vec3 texture3;
		sdw::Vec3 colour;
		sdw::UVec4 passMasks;
		// Velocity
		sdw::Vec3 velocity;
		// Instantiation
		sdw::UVec4 objectIds;
	};

	template< sdw::var::Flag FlagT >
	struct MeshVertexT
		: public MeshVertexBase
	{
		MeshVertexT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , MeshVertexT );
		
		static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, SubmeshShaders const & submeshShaders );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, SubmeshShaders const & submeshShaders );
	};
}

#include "GlslMeshVertex.inl"

#endif
