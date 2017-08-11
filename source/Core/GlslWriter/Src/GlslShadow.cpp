#include "GlslShadow.hpp"

using namespace castor;

namespace glsl
{
	castor::String const Shadow::MapShadowDirectional = cuT( "c3d_mapShadowDirectional" );
	castor::String const Shadow::MapShadowSpot = cuT( "c3d_mapShadowSpot" );
	castor::String const Shadow::MapShadowPoint = cuT( "c3d_mapShadowPoint" );

	Shadow::Shadow( GlslWriter & writer )
		: m_writer{ writer }
	{
	}

	void Shadow::declare( ShadowType type )
	{
		auto c3d_mapShadowDirectional = m_writer.declUniform< Sampler2DShadow >( MapShadowDirectional );
		auto c3d_mapShadowSpot = m_writer.declUniform< Sampler2DShadow >( MapShadowSpot, SpotShadowMapCount );
		auto c3d_mapShadowPoint = m_writer.declUniform< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
		doDeclareGetRandom();
		doDeclareGetShadowOffset();
		doDeclareGetLightSpacePosition();
		doDeclarePcfSample();
		doDeclareComputeDirectionalShadow();
		doDeclareComputeSpotShadow();
		doDeclareComputePointShadow();
	}

	void Shadow::declareDirectional( ShadowType type )
	{
		auto c3d_mapShadowDirectional = m_writer.declUniform< Sampler2DShadow >( MapShadowDirectional );
		doDeclareGetRandom();
		doDeclareGetShadowOffset();
		doDeclareGetLightSpacePosition();
		doDeclarePcfSampleDirectional();
		doDeclareComputeDirectionalShadow();
	}

	void Shadow::declarePoint( ShadowType type )
	{
		auto c3d_mapShadowPoint = m_writer.declUniform< SamplerCubeShadow >( MapShadowPoint );
		doDeclareGetRandom();
		doDeclareGetShadowOffset();
		doDeclarePcfSamplePoint();
		doDeclareComputeOnePointShadow();
	}

	void Shadow::declareSpot( ShadowType type )
	{
		auto c3d_mapShadowSpot = m_writer.declUniform< Sampler2DShadow >( MapShadowSpot );
		doDeclareGetRandom();
		doDeclareGetShadowOffset();
		doDeclareGetLightSpacePosition();
		doDeclarePcfSampleSpot();
		doDeclareComputeOneSpotShadow();
	}

	Float Shadow::computeDirectionalShadow( Mat4 const & lightMatrix
		, Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeDirectional( lightMatrix
			, worldSpacePosition
			, lightDirection
			, normal );
	}

	Float Shadow::computeSpotShadow( Mat4 const & lightMatrix
		, Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal
		, Int const & index )
	{
		return m_computeSpot( lightMatrix
			, worldSpacePosition
			, lightDirection
			, normal
			, index );
	}

	Float Shadow::computePointShadow( Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal
		, Int const & index )
	{
		return m_computePoint( worldSpacePosition
			, lightDirection
			, normal
			, index );
	}

	Float Shadow::computeSpotShadow( Mat4 const & lightMatrix
		, Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeOneSpot( lightMatrix
			, worldSpacePosition
			, lightDirection
			, normal );
	}

	Float Shadow::computePointShadow( Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeOnePoint( worldSpacePosition
			, lightDirection
			, normal );
	}

	void Shadow::doDeclareGetRandom()
	{
		m_getRandom = m_writer.implementFunction< Float >( cuT( "getRandom" )
			, [this]( Vec4 const & seed )
			{
				auto p = m_writer.declLocale( cuT( "p" )
					, dot( seed, vec4( 12.9898_f, 78.233, 45.164, 94.673 ) ) );
				m_writer.returnStmt( fract( sin( p ) * 43758.5453 ) );
			}
			, InVec4( &m_writer, cuT( "seed" ) ) );
	}

	void Shadow::doDeclareGetShadowOffset()
	{
		m_getShadowOffset = m_writer.implementFunction< Float >( cuT( "getShadowOffset" )
			, [this]( Vec3 const & normal
				, Vec3 const & lightDirection )
			{
				auto cosAlpha = m_writer.declLocale( cuT( "cosAlpha" )
					, clamp( dot( normal, lightDirection ), 0.0_f, 1.0_f ) );
				auto offsetScale = m_writer.declLocale( cuT( "offsetScale" )
					, sqrt( 1.0_f - cosAlpha * cosAlpha ) ); // sin( acos( l_cosAlpha ) )
				m_writer.returnStmt( offsetScale );
			}
			, InVec3( &m_writer, cuT( "normal" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) ) );
	}

	void Shadow::doDeclareGetLightSpacePosition()
	{
		m_getLightSpacePosition = m_writer.implementFunction< Vec3 >( cuT( "getLightSpacePosition" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				// Offset the vertex position along its normal.
				auto offset = m_writer.declLocale( cuT( "offset" ), m_getShadowOffset( normal, lightDirection ) * 2.0_f );
				auto worldSpace = m_writer.declLocale( cuT( "worldSpace" ), worldSpacePosition + m_writer.paren( normal * offset ) );
				auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" ), lightMatrix * vec4( worldSpace, 1.0_f ) );
				// Perspective divide (result in range [-1,1]).
				lightSpacePosition.xyz() = lightSpacePosition.xyz() / lightSpacePosition.w();
				// Now put the position in range [0,1].
				lightSpacePosition.xyz() = m_writer.paren( lightSpacePosition.xyz() * Float( 0.5 ) ) + Float( 0.5 );
				m_writer.returnStmt( lightSpacePosition.xyz() );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::doDeclareComputeDirectionalShadow()
	{
		m_computeDirectional = m_writer.implementFunction< Float >( cuT( "computeDirectionalShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				m_writer.returnStmt( m_sampleDirectional( vec3( lightSpacePosition.xy(), lightSpacePosition.z() ) ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::doDeclareComputeSpotShadow()
	{
		m_computeSpot = m_writer.implementFunction< Float >( cuT( "computeSpotShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal
				, Int const & index )
			{
				auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				m_writer.returnStmt( m_sampleSpot( vec3( lightSpacePosition.xy(), lightSpacePosition.z() ), index ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) )
			, InInt( &m_writer, cuT( "index" ) ) );
	}

	void Shadow::doDeclareComputePointShadow()
	{
		m_computePoint = m_writer.implementFunction< Float >( cuT( "computePointShadow" )
			, [this]( Vec3 const & worldSpacePosition
			, Vec3 const & lightPosition
			, Vec3 const & normal
			, Int const & index )
			{
				auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
					, worldSpacePosition - lightPosition );
				auto offset = m_writer.declLocale( cuT( "offset" )
					, m_getShadowOffset( normal, vertexToLight ) );
				auto worldSpace = m_writer.declLocale( cuT( "worldSpace" )
					, worldSpacePosition + m_writer.paren( normal * offset ) );
				vertexToLight = worldSpace - lightPosition;
				m_writer.returnStmt( m_samplePoint( vertexToLight, length( vertexToLight ) / 4000.0_f, index ) );
			}
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightPosition" ) )
			, InVec3( &m_writer, cuT( "normal" ) )
			, InInt( &m_writer, cuT( "index" ) ) );
	}

	void Shadow::doDeclareComputeOneSpotShadow()
	{
		m_computeOneSpot = m_writer.implementFunction< Float >( cuT( "computeSpotShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				m_writer.returnStmt( m_sampleOneSpot( vec3( lightSpacePosition.xy(), lightSpacePosition.z() ) ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::doDeclareComputeOnePointShadow()
	{
		m_computeOnePoint = m_writer.implementFunction< Float >( cuT( "computePointShadow" )
			, [this]( Vec3 const & worldSpacePosition
			, Vec3 const & lightPosition
			, Vec3 const & normal )
			{
				auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
					, worldSpacePosition - lightPosition );
				auto offset = m_writer.declLocale( cuT( "offset" )
					, m_getShadowOffset( normal, vertexToLight ) );
				auto worldSpace = m_writer.declLocale( cuT( "worldSpace" )
					, worldSpacePosition + m_writer.paren( normal * offset ) );
				vertexToLight = worldSpace - lightPosition;
				m_writer.returnStmt( m_sampleOnePoint( vertexToLight, length( vertexToLight ) / 4000.0_f ) );
			}
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightPosition" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::doDeclarePcfSample()
	{
		m_sampleDirectional = m_writer.implementFunction< Float >( cuT( "SampleDirectional" )
			, [&]( Vec3 const & coords )
			{
				auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
				auto shadowStep = m_writer.declLocale( cuT( "shadowStep" )
					, 0.0004_f );
				auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
					, 0.0_f );
				auto samples = m_writer.declLocale( cuT( "samples" )
					, 4.0_f );
				auto offset = m_writer.declLocale( cuT( "offset" )
					, shadowStep * 1.5 );
				auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
					, 0.0_f );

				FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
				{
					FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
					{
						shadowFactor += 1.0_f - texture( c3d_mapShadowDirectional, vec3( coords.xy() + vec2( x, y ), coords.z() ) );
						numSamplesUsed += 1.0_f;
					}
					ROF;
				}
				ROF;

				m_writer.returnStmt( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "coords" ) } );

		m_sampleSpot = m_writer.implementFunction< Float >( cuT( "SampleSpot" )
			, [&]( Vec3 const & coords
				, Int const & index )
			{
				auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
				auto findex = m_writer.declLocale( cuT( "findex" )
					, m_writer.cast< Float >( index ) );
				auto shadowStep = m_writer.declLocale( cuT( "shadowStep" )
					, 0.002_f );
				auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
					, 0.0_f );
				auto samples = m_writer.declLocale( cuT( "samples" )
					, 4.0_f );
				auto offset = m_writer.declLocale( cuT( "offset" )
					, shadowStep * 1.5 );
				auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
					, 0.0_f );

				FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
				{
					FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
					{
						shadowFactor += 1.0_f - texture( c3d_mapShadowSpot, vec4( coords.xy() + vec2( x, y ), findex, coords.z() ) );
						numSamplesUsed += 1.0_f;
					}
					ROF;
				}
				ROF;

				m_writer.returnStmt( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "coords" ) }
			, InInt{ &m_writer, cuT( "index" ) } );

		m_samplePoint = m_writer.implementFunction< Float >( cuT( "SamplePoint" )
			, [&]( Vec3 const & direction
				, Float const & depth
				, Int const & index )
			{
				auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
				auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
					, 0.0_f );
				auto samples = m_writer.declLocale( cuT( "samples" )
					, 4.0_f );
				auto offset = m_writer.declLocale( cuT( "offset" )
					, 20.0_f * depth );
				auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
					, 0.0_f );

				FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
				{
					FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
					{
						FOR( m_writer, Float, z, -offset, "z < offset", "z += offset / (samples * 0.5)" )
						{
							shadowFactor += 1.0_f - texture( c3d_mapShadowPoint[index], vec4( direction + vec3( x, y, z ), depth ) );
							numSamplesUsed += 1.0_f;
						}
						ROF;
					}
					ROF;
				}
				ROF;

				m_writer.returnStmt( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "direction" ) }
			, InFloat{ &m_writer, cuT( "depth" ) }
			, InInt{ &m_writer, cuT( "index" ) } );
	}

	void Shadow::doDeclarePcfSampleDirectional()
	{
		m_sampleDirectional = m_writer.implementFunction< Float >( cuT( "SampleDirectional" )
			, [&]( Vec3 const & coords )
			{
				auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
				auto shadowStep = m_writer.declLocale( cuT( "shadowStep" )
					, 0.0004_f );
				auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
					, 0.0_f );
				auto samples = m_writer.declLocale( cuT( "samples" )
					, 4.0_f );
				auto offset = m_writer.declLocale( cuT( "offset" )
					, shadowStep * 1.5 );
				auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
					, 0.0_f );

				FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
				{
					FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
					{
						shadowFactor += 1.0_f - texture( c3d_mapShadowSpot, vec3( coords.xy() + vec2( x, y ), coords.z() ) );
						numSamplesUsed += 1.0_f;
					}
					ROF;
				}
				ROF;

				m_writer.returnStmt( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "coords" ) } );
	}

	void Shadow::doDeclarePcfSampleSpot()
	{
		m_sampleOneSpot = m_writer.implementFunction< Float >( cuT( "SampleSpot" )
			, [&]( Vec3 const & coords )
			{
				auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2DShadow >( Shadow::MapShadowSpot );
				auto shadowStep = m_writer.declLocale( cuT( "shadowStep" )
					, 0.002_f );
				auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
					, 0.0_f );
				auto samples = m_writer.declLocale( cuT( "samples" )
					, 4.0_f );
				auto offset = m_writer.declLocale( cuT( "offset" )
					, shadowStep * 1.5 );
				auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
					, 0.0_f );

				FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
				{
					FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
					{
						shadowFactor += 1.0_f - texture( c3d_mapShadowSpot, vec3( coords.xy() + vec2( x, y ), coords.z() ) );
						numSamplesUsed += 1.0_f;
					}
					ROF;
				}
				ROF;

				m_writer.returnStmt( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "coords" ) } );
	}

	void Shadow::doDeclarePcfSamplePoint()
	{
		m_sampleOnePoint = m_writer.implementFunction< Float >( cuT( "SamplePoint" )
			, [&]( Vec3 const & direction
				, Float const & depth )
			{
				auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCubeShadow >( Shadow::MapShadowPoint );
				auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
					, 0.0_f );
				auto samples = m_writer.declLocale( cuT( "samples" )
					, 4.0_f );
				auto offset = m_writer.declLocale( cuT( "offset" )
					, max( 0.00001_f, 20.0_f * depth ) );
				auto step = m_writer.declLocale( cuT( "step" )
					, offset / m_writer.paren( samples * 0.5 ) );
				auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
					, 0.0_f );

				FOR( m_writer, Float, x, -offset, "x < offset", "x += step" )
				{
					FOR( m_writer, Float, y, -offset, "y < offset", "y += step" )
					{
						FOR( m_writer, Float, z, -offset, "z < offset", "z += step" )
						{
							shadowFactor += 1.0_f - texture( c3d_mapShadowPoint, vec4( direction + vec3( x, y, z ), depth ) );
							numSamplesUsed += 1.0_f;
						}
						ROF;
					}
					ROF;
				}
				ROF;

				m_writer.returnStmt( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "direction" ) }
			, InFloat{ &m_writer, cuT( "depth" ) } );
	}
}
