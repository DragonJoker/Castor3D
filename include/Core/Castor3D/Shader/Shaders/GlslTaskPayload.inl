/*
See LICENSE file in root folder
*/
namespace castor3d
{
	namespace shader
	{
		template< sdw::var::Flag FlagT >
		PayloadT< FlagT >::PayloadT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: sdw::StructInstance{ writer, std::move( expr ), enabled }
			, meshletIndices{ getMemberArray< sdw::UInt >( "meshletIndices" ) }
		{
		}

		template< sdw::var::Flag FlagT >
		sdw::type::IOStructPtr PayloadT< FlagT >::makeIOType( sdw::type::TypesCache & cache )
		{
			auto result = cache.getIOStruct( sdw::type::MemoryLayout::eStd430
				, "C3D_" + ( FlagT == sdw::var::Flag::eShaderOutput
					? std::string{ "Output" }
					: std::string{ "Input" } ) + "Payload"
				, ast::var::Flag( FlagT | ast::var::Flag::ePerTask ) );

			if ( result->empty() )
			{
				result->declMember( "meshletIndices"
					, sdw::type::Kind::eUInt
					, 32u
					, ast::type::Struct::InvalidLocation );
			}

			return result;
		}

		template< sdw::var::Flag FlagT >
		sdw::type::BaseStructPtr PayloadT< FlagT >::makeType( sdw::type::TypesCache & cache )
		{
			auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
				, "C3D_Payload" );

			if ( result->empty() )
			{
				result->declMember( "meshletIndices"
					, sdw::type::Kind::eUInt
					, 32u );
			}

			return result;
		}
	}
}
