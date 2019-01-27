#include "GlslFog.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		Fog::Fog( FogType flags, ShaderWriter & writer )
		{
			if ( flags != FogType::eDisabled )
			{
				m_fog = writer.implementFunction< Vec4 >( "applyFog"
					, [&writer, flags]( Vec4 const & bgColour
						, Vec4 const & colour
						, Float const & dist
						, Float const & y )
					{
						auto c3d_fogDensity = writer.getVariable< Float >( "c3d_fogDensity" );
						auto c3d_cameraPosition = writer.getVariable< Vec4 >( "c3d_cameraPosition" );

						auto z = writer.declLocale( "z", dist / 100 );
						auto density = writer.declLocale( "density", c3d_fogDensity );

						if ( flags == FogType::eLinear )
						{
							// Linear
							auto fogFactor = writer.declLocale( "fogFactor"
								, writer.paren( 80.0_f - z ) / writer.paren( 80.0_f - 20.0_f ) );
							fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
							writer.returnStmt( vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() ) );
						}
						else if ( flags == FogType::eExponential )
						{
							// Exponential
							auto fogFactor = writer.declLocale( "fogFactor"
								, exp( -density * z / 100.0_f ) );
							fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
							writer.returnStmt( vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() ) );
						}
						else if ( flags == FogType::eSquaredExponential )
						{
							//Squared exponential
							auto fogFactor = writer.declLocale( "fogFactor"
								, exp( -density * density * z * z / 100.0_f ) );
							fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
							writer.returnStmt( vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() ) );
						}
						else if ( flags == FogType( 4 ) )
						{
							// Coloured
							auto fogFactor = writer.declLocale( "fogFactor"
								, 0.0_f );
							fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
							writer.returnStmt( vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() ) );
						}
						else if ( flags == FogType( 5 ) )
						{
							// Ground
							//my camera y is 10.0. you can change it or pass it as a uniform
							auto be = writer.declLocale( "be"
								, writer.paren( c3d_cameraPosition.y() - y ) * 0.004_f );// 0.004 is just a factor; change it if you want
							auto bi = writer.declLocale( "bi"
								, writer.paren( c3d_cameraPosition.y() - y ) * 0.001_f );// 0.001 is just a factor; change it if you want
							auto extinction = writer.declLocale( "ext"
								, exp( -z * be ) );
							auto inscattering = writer.declLocale( "insc"
								, exp( -z * bi ) );
							writer.returnStmt( sdw::fma( bgColour
								, vec4( 1.0_f - inscattering )
								, colour * extinction ) );
						}
					}
					, InVec4( writer, "bgColour" )
					, InVec4( writer, "colour" )
					, InFloat( writer, "dist" )
					, InFloat( writer, "y" ) );
			}
		}

		Vec4 Fog::apply( Vec4 const & bgColour
			, Vec4 const & colour
			, Float const & dist
			, Float const & y )
		{
			return m_fog( bgColour
				, colour
				, dist
				, y );
		}
}
}
