#include "GlslFog.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		Fog::Fog( FogType flags, GlslWriter & writer )
			: m_writer{ writer }
		{
			if ( flags != FogType::eDisabled )
			{
				m_writer.implementFunction< Vec4 >( cuT( "applyFog" )
					, [this, flags]( Vec4 const & colour
						, Float const & dist
						, Float const & y )
					{
						auto c3d_fogDensity = m_writer.getBuiltin< Float >( cuT( "c3d_fogDensity" ) );
						auto c3d_backgroundColour = m_writer.getBuiltin< Vec4 >( cuT( "c3d_backgroundColour" ) );
						auto c3d_cameraPosition = m_writer.getBuiltin< Vec4 >( cuT( "c3d_cameraPosition" ) );

						auto z = m_writer.declLocale( cuT( "z" ), dist / 100 );
						auto density = m_writer.declLocale( cuT( "density" ), c3d_fogDensity );

						if ( flags == FogType::eLinear )
						{
							// Linear
							auto fogFactor = m_writer.declLocale( cuT( "fogFactor" )
								, m_writer.paren( 80.0_f - z ) / m_writer.paren( 80.0_f - 20.0_f ) );
							fogFactor = clamp( fogFactor, 0.0f, 1.0f );
							m_writer.returnStmt( vec4( mix( c3d_backgroundColour, colour, fogFactor ).rgb(), colour.a() ) );
						}
						else if ( flags == FogType::eExponential )
						{
							// Exponential
							auto fogFactor = m_writer.declLocale( cuT( "fogFactor" )
								, exp( -density * z / 100 ) );
							fogFactor = clamp( fogFactor, 0.0f, 1.0f );
							m_writer.returnStmt( vec4( mix( c3d_backgroundColour, colour, fogFactor ).rgb(), colour.a() ) );
						}
						else if ( flags == FogType::eSquaredExponential )
						{
							//Squared exponential
							auto fogFactor = m_writer.declLocale( cuT( "fogFactor" )
								, exp( -density * density * z * z / 100 ) );
							fogFactor = clamp( fogFactor, 0.0f, 1.0f );
							m_writer.returnStmt( vec4( mix( c3d_backgroundColour, colour, fogFactor ).rgb(), colour.a() ) );
						}
						else if ( flags == FogType( 4 ) )
						{
							// Coloured
							auto fogFactor = m_writer.declLocale( cuT( "fogFactor" )
								, 0.0_f );
							fogFactor = clamp( fogFactor, 0.0f, 1.0f );
							m_writer.returnStmt( vec4( mix( c3d_backgroundColour, colour, fogFactor ).rgb(), colour.a() ) );
						}
						else if ( flags == FogType( 5 ) )
						{
							// Ground
							//my camera y is 10.0. you can change it or pass it as a uniform
							auto be = m_writer.declLocale( cuT( "be" )
								, m_writer.paren( c3d_cameraPosition.y() - y ) * 0.004 );// 0.004 is just a factor; change it if you want
							auto bi = m_writer.declLocale( cuT( "bi" )
								, m_writer.paren( c3d_cameraPosition.y() - y ) * 0.001 );// 0.001 is just a factor; change it if you want
							auto extinction = m_writer.declLocale( cuT( "ext" )
								, exp( -z * be ) );
							auto inscattering = m_writer.declLocale( cuT( "insc" )
								, exp( -z * bi ) );
							m_writer.returnStmt( glsl::fma( c3d_backgroundColour
								, vec4( 1.0_f - inscattering )
								, colour * extinction ) );
						}
					}
					, InVec4( &m_writer, cuT( "colour" ) )
					, InFloat( &m_writer, cuT( "dist" ) )
					, InFloat( &m_writer, cuT( "y" ) ) );
			}
		}

		void Fog::applyFog( Vec4 & colour, Float const & dist, Float const & y )
		{
			colour = writeFunctionCall< Vec4 >( &m_writer, cuT( "applyFog" ), colour, dist, y );
		}
}
}
