#include "Castor3D/Shader/Shaders/GlslMeshlet.hpp"

#include "Castor3D/Limits.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		Meshlet::Meshlet( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: sdw::StructInstance{ writer, std::move( expr ), enabled }
			, vertices{ getMemberArray< sdw::UInt >( "vertices" ) }
			, indices{ getMemberArray< sdw::UInt8 >( "indices" ) }
			, vertexCount{ getMember< sdw::UInt >( "vertexCount" ) }
			, triangleCount{ getMember< sdw::UInt >( "triangleCount" ) }
		{
		}

		sdw::type::BaseStructPtr Meshlet::makeType( sdw::type::TypesCache & cache )
		{
			auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
				, "C3D_Meshlet" );

			if ( result->empty() )
			{
				result->declMember( "vertices"
					, sdw::type::Kind::eUInt
					, MaxMeshletVertexCount );
				result->declMember( "indices"
					, sdw::type::Kind::eUInt8
					, MaxMeshletTriangleCount * 3u );
				result->declMember( "vertexCount"
					, sdw::type::Kind::eUInt
					, sdw::type::NotArray );
				result->declMember( "triangleCount"
					, sdw::type::Kind::eUInt
					, sdw::type::NotArray );
				result->declMember( "pad"
					, sdw::type::Kind::eUInt
					, sdw::type::NotArray );
			}

			return result;
		}
	}
}
