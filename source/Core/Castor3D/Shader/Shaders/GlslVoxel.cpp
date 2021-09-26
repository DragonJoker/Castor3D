#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		Voxel::Voxel( ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, colorMask{ getMember< UInt >( "colorMask" ) }
			, normalMask{ getMember< UInt >( "normalMask" ) }
		{
		}

		ast::type::StructPtr Voxel::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
				, "C3D_Voxel" );

			if ( result->empty() )
			{
				result->declMember( "colorMask", ast::type::Kind::eUInt );
				result->declMember( "normalMask", ast::type::Kind::eUInt );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > Voxel::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}
}
