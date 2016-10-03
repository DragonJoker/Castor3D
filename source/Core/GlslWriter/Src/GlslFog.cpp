#include "GlslFog.hpp"

using namespace Castor;

namespace GLSL
{
	Fog::Fog( uint8_t p_flags, GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
		auto c3d_fFogDensity = m_writer.GetBuiltin< Float >( cuT( "c3d_fFogDensity" ) );
		auto c3d_v4BackgroundColour = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_v4BackgroundColour" ) );
		auto c3d_v3CameraPosition = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_v3CameraPosition" ) );

		if ( p_flags != 0 )
		{
			m_writer.ImplementFunction< Vec4 >( cuT( "ApplyFog" ), [&]( Vec4 const & p_colour, Float const & p_dist, Float const & p_y )
			{
				auto l_dist = m_writer.GetLocale< Float >( cuT( "l_z" ), p_dist / 100 );
				auto l_density = m_writer.GetLocale< Float >( cuT( "l_density" ), c3d_fFogDensity );

				if ( p_flags == 1 )
				{
					// Linear
					auto l_fogFactor = m_writer.GetLocale< Float >( cuT( "l_fogFactor" ), m_writer.Paren( Float( 80 ) - l_dist ) / m_writer.Paren( Float( 80 ) - Float( 20 ) ) );
					l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == 2 )
				{
					// Exponential
					auto l_fogFactor = m_writer.GetLocale< Float >( cuT( "l_fogFactor" ), exp( -l_density * l_dist / 100 ) );
					l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == 3 )
				{
					//Squared exponential
					auto l_fogFactor = m_writer.GetLocale< Float >( cuT( "l_fogFactor" ), exp( -l_density * l_density * l_dist * l_dist / 100 ) );
					l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == 4 )
				{
					// Coloured
					auto l_fogFactor = m_writer.GetLocale< Float >( cuT( "l_fogFactor" ), Float( 0.0f ) );
					l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == 5 )
				{
					// Ground
					//my camera y is 10.0. you can change it or pass it as a uniform
					auto l_be = m_writer.GetLocale< Float >( cuT( "l_be" ), m_writer.Paren( c3d_v3CameraPosition.y() - p_y ) * 0.004 );// 0.004 is just a factor; change it if you want
					auto l_bi = m_writer.GetLocale< Float >( cuT( "l_bi" ), m_writer.Paren( c3d_v3CameraPosition.y() - p_y ) * 0.001 );// 0.001 is just a factor; change it if you want
					auto l_extinction = m_writer.GetLocale< Float >( cuT( "l_ext" ), exp( -l_dist * l_be ) );
					auto l_inscattering = m_writer.GetLocale< Float >( cuT( "l_insc" ), exp( -l_dist * l_bi ) );
					m_writer.Return( m_writer.Paren( p_colour * l_extinction ) + m_writer.Paren( c3d_v4BackgroundColour * m_writer.Paren( Float( 1 ) - l_inscattering ) ) );
				}
			}, InParam< Vec4 >( &m_writer, cuT( "p_colour" ) )
				, InParam< Float >( &m_writer, cuT( "p_dist" ) )
				, InParam< Float >( &m_writer, cuT( "p_y" ) ) );
		}
	}

	void Fog::ApplyFog( Vec4 & p_colour, Float const & p_dist, Float const & p_y )
	{
		p_colour = WriteFunctionCall< Vec4 >( &m_writer, cuT( "ApplyFog" ), p_colour, p_dist, p_y );
	}
}
