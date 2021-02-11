#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d
{
	namespace shader
	{
		Surface::Surface( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, clipPosition{ getMember< sdw::Vec2 >( "clipPosition" ) }
			, viewPosition{ getMember< sdw::Vec3 >( "viewPosition" ) }
			, worldPosition{ getMember< sdw::Vec3 >( "worldPosition" ) }
			, worldNormal{ getMember< sdw::Vec3 >( "worldNormal" ) }
		{
		}

		Surface & Surface::operator=( Surface const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		void Surface::create( sdw::Vec2 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal )
		{
			clipPosition = clip;
			viewPosition = view;
			worldPosition = world;
			worldNormal = normal;
		}

		void Surface::create( sdw::Vec3 world
			, sdw::Vec3 normal )
		{
			create( vec2( 0.0_f ), vec3( 0.0_f ), world, normal );
		}

		ast::type::StructPtr Surface::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
				, "Surface" );

			if ( result->empty() )
			{
				result->declMember( "clipPosition", ast::type::Kind::eVec2F );
				result->declMember( "viewPosition", ast::type::Kind::eVec3F );
				result->declMember( "worldPosition", ast::type::Kind::eVec3F );
				result->declMember( "worldNormal", ast::type::Kind::eVec3F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > Surface::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}
}
