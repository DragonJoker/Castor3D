#include "Castor3D/Shader/Shaders/GlslMeshlet.hpp"

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
			, m_counts{ getMember< sdw::UVec4 >( "counts" ) }
			, vertexCount{ m_counts.x() }
			, triangleCount{ m_counts.y() }
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
					, 64u );
				result->declMember( "indices"
					, sdw::type::Kind::eUInt8
					, 128u * 3u );
				result->declMember( "counts"
					, sdw::type::Kind::eVec4U
					, sdw::type::NotArray );
			}

			return result;
		}
	}
}
