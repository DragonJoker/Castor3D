#include "GlslFog.hpp"

using namespace Castor;

namespace GLSL
{
	Fog::Fog( GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
		auto c3d_fFogDensity = m_writer.GetBuiltin< Float >( cuT( "c3d_fFogDensity" ) );
		auto c3d_v4BackgroundColour = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_v4BackgroundColour" ) );
		auto c3d_v3CameraPosition = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_v3CameraPosition" ) );
		auto vtx_view = m_writer.GetBuiltin< Vec4 >( cuT( "vtx_view" ) );

		m_writer.ImplementFunction< Vec4 >( cuT( "ApplyFog" ), [&]( Vec4 const & p_colour, Float const & p_z )
		{
			auto l_dist = m_writer.GetLocale< Float >( cuT( "l_z" ), p_z / 100 );
			auto l_density = m_writer.GetLocale< Float >( cuT( "l_density" ), c3d_fFogDensity );

			IF( m_writer, "c3d_iFogType == 1" )
			{
				// Linear
				auto l_fogFactor = m_writer.GetLocale< Float >( cuT( "l_fogFactor" ), m_writer.Paren( Float( 80 ) - l_dist ) / m_writer.Paren( Float( 80 ) - Float( 20 ) ) );
				l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
				m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
			}
			ELSEIF( m_writer, "c3d_iFogType == 2" )
			{
				// Exponential
				auto l_fogFactor = m_writer.GetLocale< Float >( cuT( "l_fogFactor" ), exp( -l_density * l_dist / 100 ) );
				l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
				m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
			}
			ELSEIF( m_writer, "c3d_iFogType == 3" )
			{
				//Squared exponential
				auto l_fogFactor = m_writer.GetLocale< Float >( cuT( "l_fogFactor" ), exp( -l_density * l_density * l_dist * l_dist / 100 ) );
				l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
				m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
			}
			ELSEIF( m_writer, "c3d_iFogType == 4" )
			{
				// Coloured
				auto l_fogFactor = m_writer.GetLocale< Float >( cuT( "l_fogFactor" ), Float( 0.0f ) );
				l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
				m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
			}
			ELSEIF( m_writer, "c3d_iFogType == 5" )
			{
				// Ground
				//my camera y is 10.0. you can change it or pass it as a uniform
				auto l_be = m_writer.GetLocale< Float >( cuT( "l_be" ), m_writer.Paren( c3d_v3CameraPosition.y() - vtx_view.y() ) * 0.004 );// 0.004 is just a factor; change it if you want
				auto l_bi = m_writer.GetLocale< Float >( cuT( "l_bi" ), m_writer.Paren( c3d_v3CameraPosition.y() - vtx_view.y() ) * 0.001 );// 0.001 is just a factor; change it if you want
				auto l_extinction = m_writer.GetLocale< Float >( cuT( "l_ext" ), exp( -l_dist * l_be ) );
				auto l_inscattering = m_writer.GetLocale< Float >( cuT( "l_insc" ), exp( -l_dist * l_bi ) );
				m_writer.Return( m_writer.Paren( p_colour * l_extinction ) + m_writer.Paren( c3d_v4BackgroundColour * m_writer.Paren( Float( 1 ) - l_inscattering ) ) );
			}
			FI
		}, InParam< Vec4 >( &m_writer, cuT( "p_colour" ) )
			, InParam< Float >( &m_writer, cuT( "p_z" ) ) );
	}

	void Fog::ApplyFog( Vec4 & p_colour, Float const & p_z )
	{
		IF( m_writer, "c3d_iFogType != 0" )
		{
			p_colour = WriteFunctionCall< Vec4 >( &m_writer, cuT( "ApplyFog" ), p_colour, p_z );
		}
		FI
	}
}
