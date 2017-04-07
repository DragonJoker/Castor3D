#include "GlslUtils.hpp"

#include "GlslIntrinsics.hpp"

using namespace Castor;

namespace GLSL
{
	Utils::Utils( GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
	}

	void Utils::DeclareCalcTexCoord()
	{
		m_calcTexCoord = m_writer.ImplementFunction< Vec2 >( cuT( "CalcTexCoord" )
			, [&]()
			{
				auto gl_FragCoord = m_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );
				auto c3d_renderSize = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_renderSize" ) );
				m_writer.Return( gl_FragCoord.xy() / c3d_renderSize );
			} );
	}

	void Utils::DeclareCalcVSPosition()
	{
		m_calcVSPosition = m_writer.ImplementFunction< Vec3 >( cuT( "CalcVSPosition" )
			, [&]( Vec2 const & p_uv
				, Mat4 const & p_invProj )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto l_depth = m_writer.GetLocale( cuT( "l_texCoord" )
					, texture( c3d_mapDepth, p_uv, 0.0_f ).x() );
				auto l_csPosition = m_writer.GetLocale( cuT( "l_psPosition" )
					, vec3( p_uv * 2.0f - 1.0f, l_depth * 2.0 - 1.0 ) );
				auto l_vsPosition = m_writer.GetLocale( cuT( "l_vsPosition" )
					, p_invProj * vec4( l_csPosition, 1.0 ) );
				l_vsPosition.xyz() /= l_vsPosition.w();
				m_writer.Return( l_vsPosition.xyz() );
			}, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InMat4{ &m_writer, cuT( "p_invProj" ) } );
	}

	void Utils::DeclareCalcVSDepth()
	{
		m_calcVSDepth = m_writer.ImplementFunction< Float >( cuT( "CalcVSPosition" )
			, [&]( Vec2 const & p_uv
				, Mat4 const & p_projection )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto l_depth = m_writer.GetLocale( cuT( "l_depth" )
					, texture( c3d_mapDepth, p_uv ).r() );
				m_writer.Return( p_projection[3][2] / m_writer.Paren( l_depth * 2.0_f - 1.0_f - p_projection[2][2] ) );
			}, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InMat4{ &m_writer, cuT( "p_projection" ) } );
	}

	void Utils::DeclareCalcWSPosition()
	{
		m_calcWSPosition = m_writer.ImplementFunction< Vec3 >( cuT( "CalcWSPosition" )
			, [&]( Vec2 const & p_uv
				, Mat4 const & p_invViewProj )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto l_depth = m_writer.GetLocale( cuT( "l_texCoord" )
					, texture( c3d_mapDepth, p_uv, 0.0_f ).x() );
				auto l_csPosition = m_writer.GetLocale( cuT( "l_psPosition" )
					, vec3( p_uv * 2.0f - 1.0f, l_depth * 2.0 - 1.0 ) );
				auto l_wsPosition = m_writer.GetLocale( cuT( "l_wsPosition" )
					, p_invViewProj * vec4( l_csPosition, 1.0 ) );
				l_wsPosition.xyz() /= l_wsPosition.w();
				m_writer.Return( l_wsPosition.xyz() );
			}, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InMat4{ &m_writer, cuT( "p_invViewProj" ) } );
	}

	Vec2 Utils::CalcTexCoord()
	{
		return m_calcTexCoord();
	}

	Vec3 Utils::CalcVSPosition( Vec2 const & p_uv
		, Mat4 const & p_invProj )
	{
		return m_calcVSPosition( p_uv, p_invProj );
	}

	Float Utils::CalcVSDepth( Vec2 const & p_uv
		, Mat4 const & p_projection )
	{
		return m_calcVSDepth( p_uv, p_projection );
	}

	Vec3 Utils::CalcWSPosition( Vec2 const & p_uv
		, Mat4 const & p_invViewProj )
	{
		return m_calcWSPosition( p_uv, p_invViewProj );
	}
}
