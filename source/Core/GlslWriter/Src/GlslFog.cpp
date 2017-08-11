#include "GlslFog.hpp"

using namespace castor;

namespace glsl
{
	Fog::Fog( FogType p_flags, GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
		if ( p_flags != FogType::eDisabled )
		{
			auto apply = [this, p_flags]( Vec4 const & p_colour, Float const & p_dist, Float const & p_y )
			{
				auto c3d_fFogDensity = m_writer.getBuiltin< Float >( cuT( "c3d_fFogDensity" ) );
				auto c3d_v4BackgroundColour = m_writer.getBuiltin< Vec4 >( cuT( "c3d_v4BackgroundColour" ) );
				auto c3d_v3CameraPosition = m_writer.getBuiltin< Vec4 >( cuT( "c3d_v3CameraPosition" ) );

				auto dist = m_writer.declLocale( cuT( "z" ), p_dist / 100 );
				auto density = m_writer.declLocale( cuT( "density" ), c3d_fFogDensity );

				if ( p_flags == FogType::eLinear )
				{
					// Linear
					auto fogFactor = m_writer.declLocale( cuT( "fogFactor" ), m_writer.paren( 80.0_f - dist ) / m_writer.paren( 80.0_f - 20.0_f ) );
					fogFactor = clamp( fogFactor, 0.0f, 1.0f );
					m_writer.returnStmt( vec4( mix( c3d_v4BackgroundColour, p_colour, fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType::eExponential )
				{
					// Exponential
					auto fogFactor = m_writer.declLocale( cuT( "fogFactor" ), exp( -density * dist / 100 ) );
					fogFactor = clamp( fogFactor, 0.0f, 1.0f );
					m_writer.returnStmt( vec4( mix( c3d_v4BackgroundColour, p_colour, fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType::eSquaredExponential )
				{
					//Squared exponential
					auto fogFactor = m_writer.declLocale( cuT( "fogFactor" ), exp( -density * density * dist * dist / 100 ) );
					fogFactor = clamp( fogFactor, 0.0f, 1.0f );
					m_writer.returnStmt( vec4( mix( c3d_v4BackgroundColour, p_colour, fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType( 4 ) )
				{
					// Coloured
					auto fogFactor = m_writer.declLocale( cuT( "fogFactor" ), 0.0_f );
					fogFactor = clamp( fogFactor, 0.0f, 1.0f );
					m_writer.returnStmt( vec4( mix( c3d_v4BackgroundColour, p_colour, fogFactor ).rgb(), p_colour.a() ) );
				}
				else if ( p_flags == FogType( 5 ) )
				{
					// Ground
					//my camera y is 10.0. you can change it or pass it as a uniform
					auto be = m_writer.declLocale( cuT( "be" ), m_writer.paren( c3d_v3CameraPosition.y() - p_y ) * 0.004 );// 0.004 is just a factor; change it if you want
					auto bi = m_writer.declLocale( cuT( "bi" ), m_writer.paren( c3d_v3CameraPosition.y() - p_y ) * 0.001 );// 0.001 is just a factor; change it if you want
					auto extinction = m_writer.declLocale( cuT( "ext" ), exp( -dist * be ) );
					auto inscattering = m_writer.declLocale( cuT( "insc" ), exp( -dist * bi ) );
					m_writer.returnStmt( m_writer.paren( p_colour * extinction ) + m_writer.paren( c3d_v4BackgroundColour * m_writer.paren( 1.0_f - inscattering ) ) );
				}
			};
			m_writer.implementFunction< Vec4 >( cuT( "applyFog" ), apply
												, InVec4( &m_writer, cuT( "p_colour" ) )
												, InFloat( &m_writer, cuT( "p_dist" ) )
												, InFloat( &m_writer, cuT( "p_y" ) ) );
		}
	}

	void Fog::applyFog( Vec4 & p_colour, Float const & p_dist, Float const & p_y )
	{
		p_colour = writeFunctionCall< Vec4 >( &m_writer, cuT( "applyFog" ), p_colour, p_dist, p_y );
	}
}
