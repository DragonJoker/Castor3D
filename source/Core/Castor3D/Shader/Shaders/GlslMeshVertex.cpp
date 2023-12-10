#include "Castor3D/Shader/Shaders/GlslMeshVertex.hpp"

#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslMeshlet.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	MeshVertexBase::MeshVertexBase( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		// Base
		, position{ this->getMember< sdw::Vec4 >( "position", true ) }
		, normal{ this->getMember< sdw::Vec3 >( "normal", true ) }
		, tangent{ this->getMember< sdw::Vec4 >( "tangent", true ) }
		, bitangent{ this->getMember< sdw::Vec3 >( "bitangent", true ) }
		, texture0{ this->getMember< sdw::Vec3 >( "texture0", true ) }
		, texture1{ this->getMember< sdw::Vec3 >( "texture1", true ) }
		, texture2{ this->getMember< sdw::Vec3 >( "texture2", true ) }
		, texture3{ this->getMember< sdw::Vec3 >( "texture3", true ) }
		, colour{ this->getMember< sdw::Vec3 >( "colour", true ) }
		, passMasks{ this->getMember< sdw::UVec4 >( "passMasks", true ) }
		// Velocity
		, velocity{ this->getMember< sdw::Vec3 >( "velocity", true ) }
		// Instantiation
		, objectIds{ this->getMember< sdw::UVec4 >( "objectIds", true ) }
	{
	}

	ast::type::IOStructPtr MeshVertexBase::makeIOType( ast::type::TypesCache & cache
		, sdw::EntryPoint entryPoint
		, SubmeshShaders const & submeshShaders
		, sdw::var::Flag flag )
	{
		auto result = cache.getIOStruct( "C3D_MeshVertex"
			, entryPoint
			, flag );

		if ( result->empty() )
		{
			submeshShaders.fillVertexSurface( *result );
		}

		return result;
	}

	ast::type::BaseStructPtr MeshVertexBase::makeType( ast::type::TypesCache & cache
		, SubmeshShaders const & submeshShaders )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eC, "C3D_MeshVertex" );

		if ( result->empty() )
		{
			submeshShaders.fillVertexSurface( *result );
		}

		return result;
	}

	//*********************************************************************************************

#define DeclareSsbo( Name, Type, Enable, Stride )\
	[&writer, &flags, &firstBinding, set, Stride]()\
	{\
		sdw::StorageBuffer Name##Buffer{ writer\
			, #Name + std::string{ "Buffer" }\
			, firstBinding++\
			, set\
			, ast::type::MemoryLayout::eStd430\
			, ( Enable ) && stride == 0u };\
		auto result = Name##Buffer.declMemberArray< Type >( #Name, Enable );\
		Name##Buffer.end();\
		return result;\
	}()

	MeshBuffersBase::MeshBuffersBase( sdw::ShaderWriter & writer
		, PipelineFlags const & flags
		, uint32_t & firstBinding
		, uint32_t set
		, uint32_t stride )
		: positions{ [&writer, &flags, &firstBinding, set, stride]()
			{
				sdw::StorageBuffer c3d_inPositionBuffer{ writer
					, std::string{ "c3d_inPositionsBuffer" }
					, firstBinding++
					, set
					, ast::type::MemoryLayout::eStd430
					, flags.enablePosition() };
				auto result = c3d_inPositionBuffer.declMemberArray< MeshPosition >( "c3d_inPositions"
					, flags.enablePosition()
					, stride );
				c3d_inPositionBuffer.end();
				return result;
			}() }
		, normals{ DeclareSsbo( c3d_inNormals
			, sdw::Vec4
			, flags.enableNormal()
			, stride ) }
		, tangents{ DeclareSsbo( c3d_inTangents
			, sdw::Vec4
			, flags.enableTangentSpace()
			, stride ) }
		, bitangents{ DeclareSsbo( c3d_inBitangents
			, sdw::Vec4
			, flags.enableBitangent()
			, stride ) }
		, textures0{ DeclareSsbo( c3d_inTexcoords0
			, sdw::Vec4
			, flags.enableTexcoord0()
			, stride ) }
		, textures1{ DeclareSsbo( c3d_inTexcoords1
			, sdw::Vec4
			, flags.enableTexcoord1()
			, stride ) }
		, textures2{ DeclareSsbo( c3d_inTexcoords2
			, sdw::Vec4
			, flags.enableTexcoord2()
			, stride ) }
		, textures3{ DeclareSsbo( c3d_inTexcoords3
			, sdw::Vec4
			, flags.enableTexcoord3()
			, stride ) }
		, colours{ DeclareSsbo( c3d_inColours
			, sdw::Vec4
			, flags.enableColours()
			, stride ) }
		, passMasks{ DeclareSsbo( c3d_inPassMasks
			, sdw::UVec4
			, flags.enablePassMasks()
			, stride ) }
		, velocities{ DeclareSsbo( c3d_inVelocities
			, sdw::Vec4
			, flags.enableVelocity()
			, stride ) }
	{
	}

	//*********************************************************************************************

	MeshBuffers::MeshBuffers( sdw::ShaderWriter & writer
		, PipelineFlags const & flags
		, uint32_t baseBinding
		, uint32_t firstBinding
		, uint32_t set
		, uint32_t stride
		, bool meshlets )
		: MeshBuffersBase{ writer
			, flags
			, baseBinding
			, set
			, stride }
		, meshlets{ [&writer, &firstBinding, set, stride, meshlets]()
			{
				sdw::StorageBuffer c3d_inMeshletsBuffer{ writer
					, "c3d_inMeshletsBuffer"
					, firstBinding++
					, set
					, ast::type::MemoryLayout::eStd430
					, ( meshlets && stride == 0u ) };
				auto result = c3d_inMeshletsBuffer.declMemberArray< Meshlet >( "c3d_inMeshletsBuffer"
					, ( meshlets && stride == 0u ) );
				c3d_inMeshletsBuffer.end();
				return result;
			}() }
		, indices{ DeclareSsbo( c3d_inIndices
			, sdw::UInt
			, flags.enableIndices()
			, stride ) }
	{
	}

	MeshBuffers::MeshBuffers( sdw::ShaderWriter & writer
		, PipelineFlags const & flags
		, uint32_t firstBinding
		, uint32_t set
		, uint32_t stride
		, bool meshlets )
		: MeshBuffers{ writer
			, flags
			, firstBinding + 2u
			, firstBinding
			, set
			, stride
			, meshlets }
	{
	}

	//*********************************************************************************************

	MeshletBuffers::MeshletBuffers( sdw::ShaderWriter & writer
		, PipelineFlags const & flags
		, uint32_t baseBinding
		, uint32_t firstBinding
		, uint32_t set
		, uint32_t stride )
		: MeshBuffersBase{ writer
			, flags
			, firstBinding
			, set
			, stride }
		, meshlets{ [&writer, baseBinding, set, &flags]()
			{
				sdw::StorageBuffer c3d_inMeshletsBuffer{ writer
					, "c3d_inMeshletsBuffer"
					, baseBinding
					, set
					, ast::type::MemoryLayout::eStd430
					, flags.enableMeshletID() };
				auto result = c3d_inMeshletsBuffer.declMemberArray< Meshlet >( "c3d_inMeshletsBuffer"
					, flags.enableMeshletID() );
				c3d_inMeshletsBuffer.end();
				return result;
			}() }
		, instances{ DeclareSsbo( c3d_inInstances
			, ObjectIds
			, flags.enableInstantiation()
			, stride ) }
	{
	}

	MeshletBuffers::MeshletBuffers( sdw::ShaderWriter & writer
		, PipelineFlags const & flags
		, uint32_t firstBinding
		, uint32_t set )
		: MeshletBuffers{ writer
			, flags
			, firstBinding
			, firstBinding + 1u
			, set
			, 0u }
	{
	}

	//*********************************************************************************************
}
