#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		castor::String const MorphingData::BufferName = cuT( "Morphing" );
		castor::String const MorphingData::DataName = cuT( "c3d_morphingData" );

		MorphingData::MorphingData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_data{ getMember< sdw::Vec4 >( "data" ) }
			, m_time{ m_data.x() }
		{
		}

		ast::type::BaseStructPtr MorphingData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_MorphingData" );

			if ( result->empty() )
			{
				result->declMember( "data", ast::type::Kind::eVec4F );
			}

			return result;
		}

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec3 & uvw0
			, sdw::Vec3 const & uvw02
			, sdw::Vec3 & uvw1
			, sdw::Vec3 const & uvw12
			, sdw::Vec3 & uvw2
			, sdw::Vec3 const & uvw22
			, sdw::Vec3 & uvw3
			, sdw::Vec3 const & uvw32 )const
		{
			if ( isEnabled() )
			{
				CU_Require( !pos.getExpr()->isDummy() );
				CU_Require( !pos2.getExpr()->isDummy() );
				pos = vec4( sdw::mix( pos.xyz(), pos2.xyz(), vec3( m_time ) ), 1.0f );

				if ( !uvw0.getExpr()->isDummy()
					&& !uvw02.getExpr()->isDummy() )
				{
					uvw0 = sdw::mix( uvw0.xyz(), uvw02, vec3( m_time ) );
				}

				if ( !uvw1.getExpr()->isDummy()
					&& !uvw12.getExpr()->isDummy() )
				{
					uvw1 = sdw::mix( uvw1.xyz(), uvw12, vec3( m_time ) );
				}

				if ( !uvw2.getExpr()->isDummy()
					&& !uvw22.getExpr()->isDummy() )
				{
					uvw2 = sdw::mix( uvw2.xyz(), uvw22, vec3( m_time ) );
				}

				if ( !uvw3.getExpr()->isDummy()
					&& !uvw32.getExpr()->isDummy() )
				{
					uvw3 = sdw::mix( uvw3.xyz(), uvw32, vec3( m_time ) );
				}
			}
		}

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec4 & nml
			, sdw::Vec3 const & nml2
			, sdw::Vec3 & uvw0
			, sdw::Vec3 const & uvw02
			, sdw::Vec3 & uvw1
			, sdw::Vec3 const & uvw12
			, sdw::Vec3 & uvw2
			, sdw::Vec3 const & uvw22
			, sdw::Vec3 & uvw3
			, sdw::Vec3 const & uvw32 )const
		{
			morph( pos, pos2
				, uvw0, uvw02
				, uvw1, uvw12
				, uvw2, uvw22
				, uvw3, uvw32 );

			if ( isEnabled()
				&& !nml.getExpr()->isDummy()
				&& !nml2.getExpr()->isDummy() )
			{
				nml = vec4( sdw::mix( nml.xyz(), nml2, vec3( m_time ) ), 0.0f );
			}
		}

		void MorphingData::morph( sdw::Vec4 & pos
			, sdw::Vec4 const & pos2
			, sdw::Vec4 & nml
			, sdw::Vec3 const & nml2
			, sdw::Vec4 & tan
			, sdw::Vec3 const & tan2
			, sdw::Vec3 & uvw0
			, sdw::Vec3 const & uvw02
			, sdw::Vec3 & uvw1
			, sdw::Vec3 const & uvw12
			, sdw::Vec3 & uvw2
			, sdw::Vec3 const & uvw22
			, sdw::Vec3 & uvw3
			, sdw::Vec3 const & uvw32 )const
		{
			morph( pos, pos2
				, nml, nml2
				, uvw0, uvw02
				, uvw1, uvw12
				, uvw2, uvw22
				, uvw3, uvw32 );

			if ( isEnabled()
				&& !tan.getExpr()->isDummy()
				&& !tan2.getExpr()->isDummy() )
			{
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
				pos = vec4( sdw::mix( pos.xyz(), pos2.xyz(), vec3( m_time ) ), 1.0f );

				if ( !uvuv.getExpr()->isDummy()
					&& !uvuv2.getExpr()->isDummy() )
				{
					uvuv = sdw::mix( uvuv, uvuv2, vec4( m_time ) );
				}
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

			if ( isEnabled()
				&& !tan.getExpr()->isDummy()
				&& !tan2.getExpr()->isDummy() )
			{
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
}
