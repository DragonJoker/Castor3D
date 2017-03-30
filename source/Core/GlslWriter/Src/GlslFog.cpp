#include "GlslFog.hpp"

using namespace Castor;

namespace GLSL
{
	Fog::Fog( FogType p_flags, GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
		if ( p_flags != FogType::eDisabled )
		{
			auto l_apply = [this, p_flags]( Vec4 const & p_colour, Float const & p_dist, Float const & p_y )
			{
				auto c3d_fFogDensity = m_writer.GetBuiltin< Float >( cuT( "c3d_fFogDensity" ) );
				auto c3d_v4BackgroundColour = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_v4BackgroundColour" ) );
				auto c3d_v3CameraPosition = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_v3CameraPosition" ) );

				auto l_dist = m_writer.GetLocale( cuT( "l_z" ), p_dist / 100 );
				auto l_density = m_writer.GetLocale( cuT( "l_density" ), c3d_fFogDensity );

				if ( p_flags == FogType::eLinear )
				{
					// Linear
					auto l_fogFactor = m_writer.GetLocale( cuT( "l_fogFactor" ), m_writer.Paren( 80.0_f - l_dist ) / m_writer.Paren( 80.0_f - 20.0_f ) );
					l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType::eExponential )
				{
					// Exponential
					auto l_fogFactor = m_writer.GetLocale( cuT( "l_fogFactor" ), exp( -l_density * l_dist / 100 ) );
					l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType::eSquaredExponential )
				{
					//Squared exponential
					auto l_fogFactor = m_writer.GetLocale( cuT( "l_fogFactor" ), exp( -l_density * l_density * l_dist * l_dist / 100 ) );
					l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType( 4 ) )
				{
					// Coloured
					auto l_fogFactor = m_writer.GetLocale( cuT( "l_fogFactor" ), 0.0_f );
					l_fogFactor = clamp( l_fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, l_fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType( 5 ) )
				{
					// Ground
					//my camera y is 10.0. you can change it or pass it as a uniform
					auto l_be = m_writer.GetLocale( cuT( "l_be" ), m_writer.Paren( c3d_v3CameraPosition.y() - p_y ) * 0.004 );// 0.004 is just a factor; change it if you want
					auto l_bi = m_writer.GetLocale( cuT( "l_bi" ), m_writer.Paren( c3d_v3CameraPosition.y() - p_y ) * 0.001 );// 0.001 is just a factor; change it if you want
					auto l_extinction = m_writer.GetLocale( cuT( "l_ext" ), exp( -l_dist * l_be ) );
					auto l_inscattering = m_writer.GetLocale( cuT( "l_insc" ), exp( -l_dist * l_bi ) );
					m_writer.Return( m_writer.Paren( p_colour * l_extinction ) + m_writer.Paren( c3d_v4BackgroundColour * m_writer.Paren( 1.0_f - l_inscattering ) ) );
				}
			};
			m_writer.ImplementFunction< Vec4 >( cuT( "ApplyFog" ), l_apply
												, InVec4( &m_writer, cuT( "p_colour" ) )
												, InFloat( &m_writer, cuT( "p_dist" ) )
												, InFloat( &m_writer, cuT( "p_y" ) ) );
		}
	}

	void Fog::ApplyFog( Vec4 & p_colour, Float const & p_dist, Float const & p_y )
	{
		p_colour = WriteFunctionCall< Vec4 >( &m_writer, cuT( "ApplyFog" ), p_colour, p_dist, p_y );
	}
}
