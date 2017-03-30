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

	void Utils::DeclareCalcVSToWS()
	{
		m_calcVSToWS = m_writer.ImplementFunction< Vec3 >( cuT( "CalcVSToWS" )
			, [&]( Vec3 const & p_vsPosition )
			{
				auto c3d_mtxInvView = m_writer.GetBuiltin< Mat4 >( cuT( "c3d_mtxInvView" ) );
				// Transform by the inverse view matrix
				m_writer.Return( m_writer.GetLocale( cuT( "l_vsPosition" )
					, c3d_mtxInvView * vec4( p_vsPosition, 1.0_f ) ).xyz() );
			}, InVec3{ &m_writer, cuT( "p_vsPosition" ) } );
	}

	void Utils::DeclareCalcVSPosition()
	{
		m_calcVSPosition = m_writer.ImplementFunction< Vec3 >( cuT( "CalcWSPosition" )
			, [&]( Vec2 const & p_coords )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto c3d_mtxInvViewProj = m_writer.GetBuiltin< Mat4 >( cuT( "c3d_mtxInvViewProj" ) );
				// Get the depth value for this pixel
				auto l_z = m_writer.GetLocale( cuT( "l_z" )
					, texture( c3d_mapDepth, p_coords ).x() );
				// Get x/w and y/w from the viewport position
				auto l_xy = m_writer.GetLocale( cuT( "l_xy" )
					, p_coords * 2 - 1 );
				m_writer.Return( vec3( l_xy, l_z ) );
			}, InVec2{ &m_writer, cuT( "p_coords" ) } );
	}

	Vec2 Utils::CalcTexCoord()
	{
		return m_calcTexCoord();
	}

	Vec3 Utils::CalcVSToWS( Vec3 const & p_vsPosition )
	{
		return m_calcVSToWS( p_vsPosition );
	}

	Vec3 Utils::CalcVSPosition( Vec2 const & p_coords )
	{
		return m_calcVSPosition( p_coords );
	}
}
