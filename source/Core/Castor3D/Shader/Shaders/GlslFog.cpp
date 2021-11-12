#include "Castor3D/Shader/Shaders/GlslFog.hpp"

#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		Fog::Fog( ShaderWriter & writer )
		{
			m_fog = writer.implementFunction< Vec4 >( "c3d_applyFog"
				, [&writer]( Vec4 const & bgColour
					, Vec4 const & colour
					, Float const & dist
					, Float const & y
					, SceneData const & sceneData )
				{
					auto z = writer.declLocale( "z", dist / 100.0_f );
					auto density = writer.declLocale( "density", sceneData.m_fogInfo.y() );
					auto result = writer.declLocale( "result", colour );
					auto fogFactor = writer.declLocale( "fogFactor", 0.0_f );

					IF( writer, sceneData.fogType == UInt( uint32_t( FogType::eLinear ) ) )
					{
						// Linear
						fogFactor = writer.declLocale( "fogFactor"
							, ( 80.0_f - z ) / ( density * ( 80.0_f - 20.0_f ) ) );
						fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
						result = vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() );
					}
					ELSEIF( sceneData.fogType == UInt( uint32_t( FogType::eExponential ) ) )
					{
						// Exponential
						fogFactor = writer.declLocale( "fogFactor"
							, exp( -density * z / 100.0_f ) );
						fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
						result = vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() );
					}
					ELSEIF( sceneData.fogType == UInt( uint32_t( FogType::eSquaredExponential ) ) )
					{
						//Squared exponential
						fogFactor = writer.declLocale( "fogFactor"
							, exp( -density * density * z * z / 100.0_f ) );
						fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
						result = vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() );
					}
					ELSEIF( sceneData.fogType == UInt( 4u ) )
					{
						// Coloured
						fogFactor = writer.declLocale( "fogFactor"
							, 0.0_f );
						fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
						result = vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() );
					}
					ELSEIF( sceneData.fogType == UInt( 5u ) )
					{
						// Ground
						auto be = writer.declLocale( "be"
							, ( sceneData.m_cameraPosition.y() - y ) * 0.004_f );
						auto bi = writer.declLocale( "bi"
							, ( sceneData.m_cameraPosition.y() - y ) * 0.001_f );
						auto extinction = writer.declLocale( "ext"
							, exp( -z * be ) );
						auto inscattering = writer.declLocale( "insc"
							, exp( -z * bi ) );
						result = sdw::fma( bgColour
							, vec4( 1.0_f - inscattering )
							, colour * extinction );
					}
					FI;

					writer.returnStmt( result );
				}
				, InVec4( writer, "bgColour" )
				, InVec4( writer, "colour" )
				, InFloat( writer, "dist" )
				, InFloat( writer, "y" )
				, InSceneData( writer, "sceneData" ) );
		}

		Vec4 Fog::apply( Vec4 const & bgColour
			, Vec4 const & colour
			, Float const & dist
			, Float const & y
			, SceneData const & sceneData )
		{
			return m_fog( bgColour
				, colour
				, dist
				, y
				, sceneData );
		}
	}
}
