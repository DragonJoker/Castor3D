#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		MorphingData::MorphingData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_time{ getMember< sdw::Float >( "time" ) }
		{
		}

		MorphingData & MorphingData::operator=( MorphingData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr MorphingData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "MorphingData" );

			if ( result->empty() )
			{
				result->declMember( "time", ast::type::Kind::eFloat );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > MorphingData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Float MorphingData::morph( sdw::Float & lhs, sdw::Float const & rhs )const
		{
			if ( isEnabled() )
			{
				lhs = sdw::mix( lhs, rhs, m_time );
			}

			return lhs;
		}

		sdw::Vec2 MorphingData::morph( sdw::Vec2 & lhs, sdw::Vec2 const & rhs )const
		{
			if ( isEnabled() )
			{
				lhs = sdw::mix( lhs, rhs, vec2( m_time ) );
			}

			return lhs;
		}

		sdw::Vec3 MorphingData::morph( sdw::Vec3 & lhs, sdw::Vec3 const & rhs )const
		{
			if ( isEnabled() )
			{
				lhs = sdw::mix( lhs, rhs, vec3( m_time ) );
			}

			return lhs;
		}

		sdw::Vec4 MorphingData::morph( sdw::Vec4 & lhs, sdw::Vec4 const & rhs )const
		{
			if ( isEnabled() )
			{
				lhs = vec4( sdw::mix( lhs.xyz(), rhs.xyz(), vec3( m_time ) ), 1.0_f );
			}

			return lhs;
		}

		sdw::Vec4 MorphingData::morph( sdw::Vec4 & lhs, sdw::Vec3 const & rhs )const
		{
			if ( isEnabled() )
			{
				lhs = vec4( sdw::mix( lhs.xyz(), rhs, vec3( m_time ) ), 1.0_f );
			}

			return lhs;
		}
	}

	//*********************************************************************************************

	castor::String const MorphingUbo::BufferMorphing = cuT( "Morphing" );
	castor::String const MorphingUbo::MorphingData = cuT( "c3d_morphingData" );

	//*********************************************************************************************
}
