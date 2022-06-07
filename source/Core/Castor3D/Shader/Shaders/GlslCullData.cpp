#include "Castor3D/Shader/Shaders/GlslCullData.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		CullData::CullData( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: sdw::StructInstance{ writer, std::move( expr ), enabled }
			, sphere{ getMember< sdw::Vec4 >( "sphere" ) }
			, cone{ getMember< sdw::Vec4 >( "cone" ) }
		{
		}

		sdw::type::BaseStructPtr CullData::makeType( sdw::type::TypesCache & cache )
		{
			auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
				, "C3D_CullData" );

			if ( result->empty() )
			{
				result->declMember( "sphere"
					, sdw::type::Kind::eVec4F
					, sdw::type::NotArray );
				result->declMember( "cone"
					, sdw::type::Kind::eVec4F
					, sdw::type::NotArray );
			}

			return result;
		}
	}
}
