#include "Castor3D/Shader/Shaders/GlslMeshVertex.hpp"

#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
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
			submeshShaders.fillSurface( *result );
		}

		return result;
	}

	ast::type::BaseStructPtr MeshVertexBase::makeType( ast::type::TypesCache & cache
		, SubmeshShaders const & submeshShaders )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eC, "C3D_MeshVertex" );

		if ( result->empty() )
		{
			submeshShaders.fillSurface( *result );
		}

		return result;
	}
}
