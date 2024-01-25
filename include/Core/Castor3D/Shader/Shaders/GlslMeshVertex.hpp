/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMeshVertex_H___
#define ___C3D_GlslMeshVertex_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

#include "Castor3D/Shader/Shaders/GlslObjectIds.hpp"

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

	struct MeshPosition
		: public sdw::StructInstance
	{
		MeshPosition( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: sdw::StructInstance{ writer, castor::move( expr ), enabled }
			, position{ getMember< sdw::Vec4 >( "position" ) }
			, fill{ getMemberArray< sdw::Vec4 >( "fill", true ) }
		{
		}

		SDW_DeclStructInstance( , MeshPosition );

		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, uint32_t stride )
		{
			auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
				, "C3D_MeshPosition" );

			if ( result->empty() )
			{
				result->declMember( "position"
					, sdw::type::Kind::eVec4F
					, sdw::type::NotArray );
				auto fillCount = stride == 0u
					? 0u
					: uint32_t( ( stride / sizeof( castor::Point4f ) ) - 1u );
				result->declMember( "fill"
					, sdw::type::Kind::eVec4F
					, fillCount
					, fillCount > 0u );
			}

			return result;
		}

	public:
		sdw::Vec4 position;
		sdw::Array< sdw::Vec4 > fill;
	};

	struct MeshBuffersBase
	{
		C3D_API MeshBuffersBase( sdw::ShaderWriter & writer
			, PipelineFlags const & flags
			, uint32_t & firstBinding
			, uint32_t set
			, uint32_t stride );

		sdw::Array< MeshPosition > positions;
		sdw::Vec4Array normals;
		sdw::Vec4Array tangents;
		sdw::Vec4Array bitangents;
		sdw::Vec4Array textures0;
		sdw::Vec4Array textures1;
		sdw::Vec4Array textures2;
		sdw::Vec4Array textures3;
		sdw::Vec4Array colours;
		sdw::UVec4Array passMasks;
		sdw::Vec4Array velocities;
	};

	struct MeshBuffers
		: public MeshBuffersBase
	{
	private:
		MeshBuffers( sdw::ShaderWriter & writer
			, PipelineFlags const & flags
			, uint32_t baseBinding
			, uint32_t firstBinding
			, uint32_t set
			, uint32_t stride
			, bool meshlets );

	public:
		C3D_API MeshBuffers( sdw::ShaderWriter & writer
			, PipelineFlags const & flags
			, uint32_t firstBinding
			, uint32_t set
			, uint32_t stride
			, bool meshlets );

		sdw::Array< Meshlet > meshlets;
		sdw::UIntArray indices;
	};

	struct MeshletBuffers
		: public MeshBuffersBase
	{
	private:
		MeshletBuffers( sdw::ShaderWriter & writer
			, PipelineFlags const & flags
			, uint32_t baseBinding
			, uint32_t firstBinding
			, uint32_t set
			, uint32_t stride );

	public:
		C3D_API MeshletBuffers( sdw::ShaderWriter & writer
			, PipelineFlags const & flags
			, uint32_t firstBinding
			, uint32_t set );

		sdw::Array< Meshlet > meshlets;
		sdw::Array< ObjectIds > instances;
	};
}

#include "GlslMeshVertex.inl"

#endif
