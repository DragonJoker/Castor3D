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

		ast::type::BaseStructPtr MorphingData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_MorphingData" );

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

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec3 & uvw
			, sdw::Vec3 const & uvw2 )const
		{
			if ( isEnabled() )
			{
				CU_Require( !pos.getExpr()->isDummy() );
				CU_Require( !pos2.getExpr()->isDummy() );
				CU_Require( !uvw.getExpr()->isDummy() );
				CU_Require( !uvw2.getExpr()->isDummy() );
				pos = vec4( sdw::mix( pos.xyz(), pos2.xyz(), vec3( m_time ) ), 1.0f );
				uvw = sdw::mix( uvw, uvw2, vec3( m_time ) );
			}
		}

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec4 & nml
			, sdw::Vec3 const & nml2
			, sdw::Vec3 & uvw
			, sdw::Vec3 const & uvw2 )const
		{
			morph( pos, pos2, uvw, uvw2 );

			if ( isEnabled() )
			{
				CU_Require( !nml.getExpr()->isDummy() );
				CU_Require( !nml2.getExpr()->isDummy() );
				nml = vec4( sdw::mix( nml.xyz(), nml2, vec3( m_time ) ), 0.0f );
			}
		}

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec4 & nml
			, sdw::Vec3 const & nml2
			, sdw::Vec4 & tan
			, sdw::Vec3 const & tan2
			, sdw::Vec3 & uvw
			, sdw::Vec3 const & uvw2 )const
		{
			morph( pos, pos2, nml, nml2, uvw, uvw2 );

			if ( isEnabled() )
			{
				CU_Require( !tan.getExpr()->isDummy() );
				CU_Require( !tan2.getExpr()->isDummy() );
				tan = vec4( sdw::mix( tan.xyz(), tan2, vec3( m_time ) ), 0.0f );
			}
		}

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec4 & uvuv
			, sdw::Vec4 const & uvuv2 )const
		{
			if ( isEnabled() )
			{
				CU_Require( !pos.getExpr()->isDummy() );
				CU_Require( !pos2.getExpr()->isDummy() );
				CU_Require( !uvuv.getExpr()->isDummy() );
				CU_Require( !uvuv2.getExpr()->isDummy() );
				pos = vec4( sdw::mix( pos.xyz(), pos2.xyz(), vec3( m_time ) ), 1.0f );
				uvuv = sdw::mix( uvuv, uvuv2, vec4( m_time ) );
			}
		}

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec4 & nml
			, sdw::Vec3 const & nml2
			, sdw::Vec4 & uvuv
			, sdw::Vec4 const & uvuv2 )const
		{
			morph( pos, pos2, uvuv, uvuv2 );

			if ( isEnabled() )
			{
				CU_Require( !nml.getExpr()->isDummy() );
				CU_Require( !nml2.getExpr()->isDummy() );
				nml = vec4( sdw::mix( nml.xyz(), nml2, vec3( m_time ) ), 0.0f );
			}
		}

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec4 & nml
			, sdw::Vec3 const & nml2
			, sdw::Vec4 & tan
			, sdw::Vec3 const & tan2
			, sdw::Vec4 & uvuv
			, sdw::Vec4 const & uvuv2 )const
		{
			morph( pos, pos2, nml, nml2, uvuv, uvuv2 );

			if ( isEnabled() )
			{
				CU_Require( !tan.getExpr()->isDummy() );
				CU_Require( !tan2.getExpr()->isDummy() );
				tan = vec4( sdw::mix( tan.xyz(), tan2, vec3( m_time ) ), 0.0f );
			}
		}

		sdw::Float MorphingData::morph( sdw::Float & lhs, sdw::Float const & rhs )const
		{
			if ( isEnabled() )
			{
				CU_Require( !lhs.getExpr()->isDummy() );
				CU_Require( !rhs.getExpr()->isDummy() );
				lhs = sdw::mix( lhs, rhs, m_time );
			}

			return lhs;
		}

		sdw::Vec2 MorphingData::morph( sdw::Vec2 & lhs, sdw::Vec2 const & rhs )const
		{
			if ( isEnabled() )
			{
				CU_Require( !lhs.getExpr()->isDummy() );
				CU_Require( !rhs.getExpr()->isDummy() );
				lhs = sdw::mix( lhs, rhs, vec2( m_time ) );
			}

			return lhs;
		}

		sdw::Vec3 MorphingData::morph( sdw::Vec3 & lhs, sdw::Vec3 const & rhs )const
		{
			if ( isEnabled() )
			{
				CU_Require( !lhs.getExpr()->isDummy() );
				CU_Require( !rhs.getExpr()->isDummy() );
				lhs = sdw::mix( lhs, rhs, vec3( m_time ) );
			}

			return lhs;
		}

		sdw::Vec4 MorphingData::morph( sdw::Vec4 & lhs, sdw::Vec4 const & rhs )const
		{
			if ( isEnabled() )
			{
				CU_Require( !lhs.getExpr()->isDummy() );
				CU_Require( !rhs.getExpr()->isDummy() );
				lhs = vec4( sdw::mix( lhs.xyz(), rhs.xyz(), vec3( m_time ) ), 1.0_f );
			}

			return lhs;
		}

		sdw::Vec4 MorphingData::morph( sdw::Vec4 & lhs, sdw::Vec3 const & rhs )const
		{
			if ( isEnabled() )
			{
				CU_Require( !lhs.getExpr()->isDummy() );
				CU_Require( !rhs.getExpr()->isDummy() );
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
