#include "Castor3D/Shader/Shaders/GlslFog.hpp"

#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		Fog::Fog( sdw::ShaderWriter & writer )
		{
			m_fog = writer.implementFunction< sdw::Vec4 >( "c3d_applyFog"
				, [&]( sdw::Vec4 const & bgColour
					, sdw::Vec4 const & colour
					, sdw::Vec3 const & wsPosition
					, SceneData const & sceneData )
				{
					auto result = writer.declLocale( "result", colour );
					auto density = writer.declLocale( "density", sceneData.fogDensity );
					auto wsCamera = writer.declLocale( "wsCamera", sceneData.cameraPosition );

					IF( writer, sceneData.fogType == sdw::UInt( uint32_t( FogType::eLinear ) ) )
					{
						// Linear
						auto dist = writer.declLocale( "dist", length( wsPosition - wsCamera ) );
						auto z = writer.declLocale( "z", dist / 100.0_f );
						auto fogFactor = writer.declLocale( "fogFactor"
							, ( 80.0_f - z ) / ( density * ( 80.0_f - 20.0_f ) ) );
						fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
						result = vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() );
					}
					ELSEIF( sceneData.fogType == sdw::UInt( uint32_t( FogType::eExponential ) ) )
					{
						// Exponential
						auto dist = writer.declLocale( "dist", length( wsPosition - wsCamera ) );
						auto z = writer.declLocale( "z", dist / 100.0_f );
						auto fogFactor = writer.declLocale( "fogFactor"
							, exp( -density * z / 100.0_f ) );
						fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
						result = vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() );
					}
					ELSEIF( sceneData.fogType == sdw::UInt( uint32_t( FogType::eSquaredExponential ) ) )
					{
						//Squared exponential
						auto dist = writer.declLocale( "dist", length( wsPosition - wsCamera ) );
						auto z = writer.declLocale( "z", dist / 100.0_f );
						auto fogFactor = writer.declLocale( "fogFactor"
							, exp( -density * density * z * z / 100.0_f ) );
						fogFactor = clamp( fogFactor, 0.0_f, 1.0_f );
						result = vec4( mix( bgColour, colour, vec4( fogFactor ) ).rgb(), colour.a() );
					}
					ELSEIF( sceneData.fogType == sdw::UInt( 4u ) )
					{
						// Ground
						auto dist = writer.declLocale( "dist", length( wsPosition - wsCamera ) );
						auto z = writer.declLocale( "z", dist / 100.0_f );
						auto y = writer.declLocale( "y", wsPosition.y() );
						auto be = writer.declLocale( "be"
							, ( wsCamera.y() - y ) * 0.004_f );
						auto bi = writer.declLocale( "bi"
							, ( wsCamera.y() - y ) * 0.001_f );
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
				, sdw::InVec4( writer, "bgColour" )
				, sdw::InVec4( writer, "colour" )
				, sdw::InVec3( writer, "wsPosition" )
				, InSceneData( writer, "sceneData" ) );
		}

		sdw::Vec4 Fog::apply( sdw::Vec4 const & bgColour
			, sdw::Vec4 const & colour
			, sdw::Vec3 const & wsPosition
			, SceneData const & sceneData )
		{
			return m_fog( bgColour
				, colour
				, wsPosition
				, sceneData );
		}
	}
}
