#include "GlslFog.hpp"

using namespace Castor;

namespace GLSL
{
	Fog::Fog( FogType p_flags, GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
		if ( p_flags != FogType::eDisabled )
		{
			auto apply = [this, p_flags]( Vec4 const & p_colour, Float const & p_dist, Float const & p_y )
			{
				auto c3d_fFogDensity = m_writer.GetBuiltin< Float >( cuT( "c3d_fFogDensity" ) );
				auto c3d_v4BackgroundColour = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_v4BackgroundColour" ) );
				auto c3d_v3CameraPosition = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_v3CameraPosition" ) );

				auto dist = m_writer.DeclLocale( cuT( "z" ), p_dist / 100 );
				auto density = m_writer.DeclLocale( cuT( "density" ), c3d_fFogDensity );

				if ( p_flags == FogType::eLinear )
				{
					// Linear
					auto fogFactor = m_writer.DeclLocale( cuT( "fogFactor" ), m_writer.Paren( 80.0_f - dist ) / m_writer.Paren( 80.0_f - 20.0_f ) );
					fogFactor = clamp( fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType::eExponential )
				{
					// Exponential
					auto fogFactor = m_writer.DeclLocale( cuT( "fogFactor" ), exp( -density * dist / 100 ) );
					fogFactor = clamp( fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType::eSquaredExponential )
				{
					//Squared exponential
					auto fogFactor = m_writer.DeclLocale( cuT( "fogFactor" ), exp( -density * density * dist * dist / 100 ) );
					fogFactor = clamp( fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType( 4 ) )
				{
					// Coloured
					auto fogFactor = m_writer.DeclLocale( cuT( "fogFactor" ), 0.0_f );
					fogFactor = clamp( fogFactor, 0.0f, 1.0f );
					m_writer.Return( vec4( mix( c3d_v4BackgroundColour, p_colour, fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType( 5 ) )
				{
					// Ground
					//my camera y is 10.0. you can change it or pass it as a uniform
					auto be = m_writer.DeclLocale( cuT( "be" ), m_writer.Paren( c3d_v3CameraPosition.y() - p_y ) * 0.004 );// 0.004 is just a factor; change it if you want
					auto bi = m_writer.DeclLocale( cuT( "bi" ), m_writer.Paren( c3d_v3CameraPosition.y() - p_y ) * 0.001 );// 0.001 is just a factor; change it if you want
					auto extinction = m_writer.DeclLocale( cuT( "ext" ), exp( -dist * be ) );
					auto inscattering = m_writer.DeclLocale( cuT( "insc" ), exp( -dist * bi ) );
					m_writer.Return( m_writer.Paren( p_colour * extinction ) + m_writer.Paren( c3d_v4BackgroundColour * m_writer.Paren( 1.0_f - inscattering ) ) );
				}
			};
			m_writer.ImplementFunction< Vec4 >( cuT( "ApplyFog" ), apply
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
