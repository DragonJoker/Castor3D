#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		TextureData::TextureData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, config{ getMemberArray< sdw::UVec4 >( "config" ) }
		{
		}

		TextureData & TextureData::operator=( TextureData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr TextureData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "TextureData" );

			if ( result->empty() )
			{
				result->declMember( "config", ast::type::Kind::eVec4U, 3u );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > TextureData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}

	//*********************************************************************************************

	castor::String const TexturesUbo::BufferTextures = cuT( "Textures" );
	castor::String const TexturesUbo::TextureData = cuT( "c3d_textureData" );

	//*********************************************************************************************
}
