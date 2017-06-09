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
				auto gl_FragCoord = m_writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );
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
				auto l_depth = m_writer.DeclLocale( cuT( "l_texCoord" )
					, texture( c3d_mapDepth, p_uv, 0.0_f ).x() );
				auto l_csPosition = m_writer.DeclLocale( cuT( "l_psPosition" )
					, vec3( p_uv * 2.0f - 1.0f, l_depth * 2.0 - 1.0 ) );
				auto l_vsPosition = m_writer.DeclLocale( cuT( "l_vsPosition" )
					, p_invProj * vec4( l_csPosition, 1.0 ) );
				l_vsPosition.xyz() /= l_vsPosition.w();
				m_writer.Return( l_vsPosition.xyz() );
			}, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InMat4{ &m_writer, cuT( "p_invProj" ) } );
	}

	void Utils::DeclareCalcVSDepth()
	{
		m_calcVSDepth = m_writer.ImplementFunction< Float >( cuT( "CalcVSDepth" )
			, [&]( Vec2 const & p_uv
				, Mat4 const & p_projection )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto l_depth = m_writer.DeclLocale( cuT( "l_depth" )
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
				auto l_depth = m_writer.DeclLocale( cuT( "l_texCoord" )
					, texture( c3d_mapDepth, p_uv, 0.0_f ).x() );
				auto l_csPosition = m_writer.DeclLocale( cuT( "l_psPosition" )
					, vec3( p_uv * 2.0f - 1.0f, l_depth * 2.0 - 1.0 ) );
				auto l_wsPosition = m_writer.DeclLocale( cuT( "l_wsPosition" )
					, p_invViewProj * vec4( l_csPosition, 1.0 ) );
				l_wsPosition.xyz() /= l_wsPosition.w();
				m_writer.Return( l_wsPosition.xyz() );
			}, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InMat4{ &m_writer, cuT( "p_invViewProj" ) } );
	}

	void Utils::DeclareApplyGamma()
	{
		m_applyGamma = m_writer.ImplementFunction< Vec3 >( cuT( "ApplyGamma" )
			, [&]( Float const & p_gamma
				, Vec3 const & p_HDR )
			{
				m_writer.Return( pow( p_HDR, vec3( 1.0_f / p_gamma ) ) );
			}, InFloat{ &m_writer, cuT( "p_gamma" ) }
			, InVec3{ &m_writer, cuT( "p_HDR" ) } );
	}

	void Utils::DeclareRemoveGamma()
	{
		m_removeGamma = m_writer.ImplementFunction< Vec3 >( cuT( "RemoveGamma" )
			, [&]( Float const & p_gamma
				, Vec3 const & p_sRGB )
			{
				m_writer.Return( pow( p_sRGB, vec3( p_gamma ) ) );
			}, InFloat{ &m_writer, cuT( "p_gamma" ) }
			, InVec3{ &m_writer, cuT( "p_sRGB" ) } );
	}

	void Utils::DeclareLineariseDepth()
	{
		m_lineariseDepth = m_writer.ImplementFunction< Float >( cuT( "LineariseDepth" )
			, [&]( Float const & p_depth
				, Mat4 const & p_invProj )
			{
				auto c3d_fCameraNearPlane = m_writer.GetBuiltin< Float >( cuT( "c3d_fCameraNearPlane" ) );
				auto c3d_fCameraFarPlane = m_writer.GetBuiltin< Float >( cuT( "c3d_fCameraFarPlane" ) );
				auto l_z = m_writer.DeclLocale( cuT( "l_z" )
					, p_depth *2.0_f - 1.0_f );
				auto l_unprojected = m_writer.DeclLocale( cuT( "l_unprojected" )
					, p_invProj * vec4( 0.0_f, 0.0_f, l_z, 1.0_f ) );
				l_z = l_unprojected.z() / l_unprojected.w();
				m_writer.Return( m_writer.Paren( l_z - c3d_fCameraNearPlane )
					/ m_writer.Paren( c3d_fCameraFarPlane - c3d_fCameraNearPlane ) );

			}, InFloat{ &m_writer, cuT( "p_depth" ) }
			, InMat4{ &m_writer, cuT( "p_invProj" ) } );
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

	Vec3 Utils::ApplyGamma( Float const & p_gamma
		, Vec3 const & p_HDR )
	{
		return m_applyGamma( p_gamma, p_HDR );
	}

	Vec3 Utils::RemoveGamma( Float const & p_gamma
		, Vec3 const & p_sRGB )
	{
		return m_removeGamma( p_gamma, p_sRGB );
	}

	Float Utils::LineariseDepth( Float const & p_depth, Mat4 const & p_invProj )
	{
		return m_lineariseDepth( p_depth, p_invProj );
	}
}
