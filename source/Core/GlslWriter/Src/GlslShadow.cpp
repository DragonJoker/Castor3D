#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	Castor::String const Shadow::MapShadowDirectional = cuT( "c3d_mapShadowDirectional" );
	Castor::String const Shadow::MapShadowSpot = cuT( "c3d_mapShadowSpot" );
	Castor::String const Shadow::MapShadowPoint = cuT( "c3d_mapShadowPoint" );

	Shadow::Shadow( GlslWriter & writer )
		: m_writer{ writer }
	{
	}

	void Shadow::Declare( ShadowType type )
	{
		auto c3d_mapShadowDirectional = m_writer.DeclUniform< Sampler2DShadow >( MapShadowDirectional );
		auto c3d_mapShadowSpot = m_writer.DeclUniform< Sampler2DArrayShadow >( MapShadowSpot );
		auto c3d_mapShadowPoint = m_writer.DeclUniform< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );

		if ( type == ShadowType::ePoisson || type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.DeclUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( -0.94201624_f, -0.39906216 ),
				vec2( 0.94558609_f, -0.76890725 ),
				vec2( -0.094184101_f, -0.92938870 ),
				vec2( 0.34495938_f, 0.29387760 )
			} );
		}

		// offsets for rectangular PCF sampling
		auto c3d_pcfNumSamplingPositions = m_writer.DeclUniform< Int >( cuT( "c3d_pcfNumSamplingPositions" ), 4_i );
		auto c3d_pcfKernel = m_writer.DeclUniform< Vec2 >( cuT( "c3d_pcfKernel" ), 4u,
		{
			vec2( 1.0_f, 1.0_f ),
			vec2( -1.0_f, 1.0_f ),
			vec2( -1.0_f, -1.0_f ),
			vec2( 1.0_f, -1.0_f )
		} );

		DoDeclare_GetRandom();
		DoDeclare_GetShadowOffset();
		DoDeclare_GetLightSpacePosition();
		DoDeclare_PcfSample();
		DoDeclare_ComputeDirectionalShadow();
		DoDeclare_ComputeSpotShadow();
		DoDeclare_ComputePointShadow();
	}

	void Shadow::DeclareDirectional( ShadowType type )
	{
		auto c3d_mapShadowDirectional = m_writer.DeclUniform< Sampler2DShadow >( MapShadowDirectional );

		if ( type == ShadowType::ePoisson || type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.DeclUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( -0.94201624_f, -0.39906216 ),
				vec2( 0.94558609_f, -0.76890725 ),
				vec2( -0.094184101_f, -0.92938870 ),
				vec2( 0.34495938_f, 0.29387760 )
			} );
		}

		// offsets for rectangular PCF sampling
		auto c3d_pcfNumSamplingPositions = m_writer.DeclUniform< Int >( cuT( "c3d_pcfNumSamplingPositions" ), 4_i );
		auto c3d_pcfKernel = m_writer.DeclUniform< Vec2 >( cuT( "c3d_pcfKernel" ), 4u,
		{
			vec2( 1.0_f, 1.0_f ),
			vec2( -1.0_f, 1.0_f ),
			vec2( -1.0_f, -1.0_f ),
			vec2( 1.0_f, -1.0_f )
		} );

		DoDeclare_GetRandom();
		DoDeclare_GetShadowOffset();
		DoDeclare_GetLightSpacePosition();
		DoDeclare_PcfSampleDirectional();
		DoDeclare_ComputeDirectionalShadow();
	}

	void Shadow::DeclarePoint( ShadowType type )
	{
		auto c3d_mapShadowPoint = m_writer.DeclUniform< SamplerCubeShadow >( MapShadowPoint );

		if ( type == ShadowType::ePoisson || type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.DeclUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( -0.94201624_f, -0.39906216 ),
				vec2( 0.94558609_f, -0.76890725 ),
				vec2( -0.094184101_f, -0.92938870 ),
				vec2( 0.34495938_f, 0.29387760 )
			} );
		}

		// offsets for rectangular PCF sampling
		auto c3d_pcfNumSamplingPositions = m_writer.DeclUniform< Int >( cuT( "c3d_pcfNumSamplingPositions" ), 4_i );
		auto c3d_pcfKernel = m_writer.DeclUniform< Vec2 >( cuT( "c3d_pcfKernel" ), 4u,
		{
			vec2( 1.0_f, 1.0_f ),
			vec2( -1.0_f, 1.0_f ),
			vec2( -1.0_f, -1.0_f ),
			vec2( 1.0_f, -1.0_f )
		} );

		DoDeclare_GetRandom();
		DoDeclare_GetShadowOffset();
		DoDeclare_PcfSamplePoint();
		DoDeclare_ComputeOnePointShadow();
	}

	void Shadow::DeclareSpot( ShadowType type )
	{
		auto c3d_mapShadowSpot = m_writer.DeclUniform< Sampler2DShadow >( MapShadowSpot );

		if ( type == ShadowType::ePoisson || type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.DeclUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( -0.94201624_f, -0.39906216 ),
				vec2( 0.94558609_f, -0.76890725 ),
				vec2( -0.094184101_f, -0.92938870 ),
				vec2( 0.34495938_f, 0.29387760 )
			} );
		}

		// offsets for rectangular PCF sampling
		auto c3d_pcfNumSamplingPositions = m_writer.DeclUniform< Int >( cuT( "c3d_pcfNumSamplingPositions" ), 4_i );
		auto c3d_pcfKernel = m_writer.DeclUniform< Vec2 >( cuT( "c3d_pcfKernel" ), 4u,
			{
				vec2( 1.0_f, 1.0_f ),
				vec2( -1.0_f, 1.0_f ),
				vec2( -1.0_f, -1.0_f ),
				vec2( 1.0_f, -1.0_f )
			} );

		DoDeclare_GetRandom();
		DoDeclare_GetShadowOffset();
		DoDeclare_GetLightSpacePosition();
		DoDeclare_PcfSampleSpot();
		DoDeclare_ComputeOneSpotShadow();
	}

	Float Shadow::ComputeDirectionalShadow( Mat4 const & lightMatrix
		, Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeDirectional( lightMatrix
			, worldSpacePosition
			, lightDirection
			, normal );
	}

	Float Shadow::ComputeSpotShadow( Mat4 const & lightMatrix
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

	Float Shadow::ComputePointShadow( Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal
		, Int const & index )
	{
		return m_computePoint( worldSpacePosition
			, lightDirection
			, normal
			, index );
	}

	Float Shadow::ComputeSpotShadow( Mat4 const & lightMatrix
		, Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeOneSpot( lightMatrix
			, worldSpacePosition
			, lightDirection
			, normal );
	}

	Float Shadow::ComputePointShadow( Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeOnePoint( worldSpacePosition
			, lightDirection
			, normal );
	}

	void Shadow::DoDeclare_GetRandom()
	{
		m_getRandom = m_writer.ImplementFunction< Float >( cuT( "GetRandom" )
			, [this]( Vec4 const & seed )
			{
				auto p = m_writer.DeclLocale( cuT( "p" )
					, dot( seed, vec4( 12.9898_f, 78.233, 45.164, 94.673 ) ) );
				m_writer.Return( fract( sin( p ) * 43758.5453 ) );
			}
			, InVec4( &m_writer, cuT( "seed" ) ) );
	}

	void Shadow::DoDeclare_GetShadowOffset()
	{
		m_getShadowOffset = m_writer.ImplementFunction< Float >( cuT( "GetShadowOffset" )
			, [this]( Vec3 const & normal
				, Vec3 const & lightDirection
				, Float const & minOffset
				, Float const & maxSlopeOffset )
			{
				auto cosAlpha = m_writer.DeclLocale( cuT( "cosAlpha" )
					, clamp( dot( normal, normalize( lightDirection ) ), 0.0_f, 1.0_f ) );
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, 1.0_f - cosAlpha );
				m_writer.Return( minOffset + maxSlopeOffset * offset );
			}
			, InVec3( &m_writer, cuT( "normal" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InFloat( &m_writer, cuT( "minOffset" ) )
			, InFloat( &m_writer, cuT( "maxSlopeOffset" ) ) );
	}

	void Shadow::DoDeclare_GetLightSpacePosition()
	{
		m_getLightSpacePosition = m_writer.ImplementFunction< Vec3 >( cuT( "GetLightSpacePosition" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				// Offset the vertex position along its normal.
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" )
					, lightMatrix * vec4( worldSpacePosition, 1.0_f ) );
				// Perspective divide (result in range [-1,1]).
				lightSpacePosition.xyz() = lightSpacePosition.xyz() / lightSpacePosition.w();
				// Now put the position in range [0,1].
				lightSpacePosition.xyz() = m_writer.Paren( lightSpacePosition.xyz() * Float( 0.5 ) ) + Float( 0.5 );

				m_writer.Return( lightSpacePosition.xyz() );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeDirectionalShadow()
	{
		m_computeDirectional = m_writer.ImplementFunction< Float >( cuT( "ComputeDirectionalShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.005f;
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, lightDirection, Float( minOffset ), Float( maxSlopeOffset ) ) );
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				m_writer.Return( m_sampleDirectional( vec3( lightSpacePosition.xy(), lightSpacePosition.z() - offset ) ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeSpotShadow()
	{
		m_computeSpot = m_writer.ImplementFunction< Float >( cuT( "ComputeSpotShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal
				, Int const & index )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.001f;
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, lightDirection, Float( minOffset ), Float( maxSlopeOffset ) ) );
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				m_writer.Return( m_sampleSpot( vec3( lightSpacePosition.xy(), lightSpacePosition.z() - offset ), index ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) )
			, InInt( &m_writer, cuT( "index" ) ) );
	}

	void Shadow::DoDeclare_ComputePointShadow()
	{
		m_computePoint = m_writer.ImplementFunction< Float >( cuT( "ComputePointShadow" )
			, [this]( Vec3 const & worldSpacePosition
			, Vec3 const & lightPosition
			, Vec3 const & normal
			, Int const & index )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.002f;
				auto vertexToLight = m_writer.DeclLocale( cuT( "vertexToLight" )
					, worldSpacePosition - lightPosition );
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, vertexToLight, Float( minOffset ), Float( maxSlopeOffset ) ) );
				m_writer.Return( m_samplePoint( vertexToLight, length( vertexToLight ) / 4000.0_f - offset, index ) );
			}
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightPosition" ) )
			, InVec3( &m_writer, cuT( "normal" ) )
			, InInt( &m_writer, cuT( "index" ) ) );
	}

	void Shadow::DoDeclare_ComputeOneSpotShadow()
	{
		m_computeOneSpot = m_writer.ImplementFunction< Float >( cuT( "ComputeSpotShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				auto constexpr minOffset = 0.00005f;
				auto constexpr maxSlopeOffset = 0.0005f;
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, lightDirection, Float( minOffset ), Float( maxSlopeOffset ) ) );
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				m_writer.Return( m_sampleOneSpot( vec3( lightSpacePosition.xy(), lightSpacePosition.z() - offset ) ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeOnePointShadow()
	{
		m_computeOnePoint = m_writer.ImplementFunction< Float >( cuT( "ComputePointShadow" )
			, [this]( Vec3 const & worldSpacePosition
			, Vec3 const & lightPosition
			, Vec3 const & normal )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.002f;
				auto vertexToLight = m_writer.DeclLocale( cuT( "vertexToLight" )
					, worldSpacePosition - lightPosition );
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, vertexToLight, Float( minOffset ), Float( maxSlopeOffset ) ) );
				m_writer.Return( m_sampleOnePoint( vertexToLight, length( vertexToLight ) / 4000.0_f - offset ) );
			}
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightPosition" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::DoDeclare_PcfSample()
	{
		m_sampleDirectional = m_writer.ImplementFunction< Float >( cuT( "SampleDirectional" )
			, [&]( Vec3 const & coords )
			{
				auto c3d_pcfKernel = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_pcfKernel" ), 4u );
				auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 1.0_f - texture( c3d_mapShadowSpot, coords ) );
				auto numSamplesUsed = m_writer.DeclLocale( cuT( "numSamplesUsed" )
					, 1.0_f );
				auto pcfRadius = m_writer.DeclLocale( cuT( "pcfRadius" )
					, 1.0_f );
				auto shadowStep = m_writer.DeclLocale( cuT( "shadowStep" )
					, 0.001_f );

				FOR( m_writer, Int, i, 0, "i < c3d_pcfNumSamplingPositions", "i++" )
				{
					shadowFactor += 1.0_f - texture( c3d_mapShadowSpot, vec3( coords.xy() + c3d_pcfKernel[i] * shadowStep * pcfRadius, coords.z() ) );
					numSamplesUsed += 1.0_f;
				}
				ROF;

				m_writer.Return( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "coords" ) } );

		m_sampleSpot = m_writer.ImplementFunction< Float >( cuT( "SampleSpot" )
			, [&]( Vec3 const & coords
				, Int const & index )
			{
				auto c3d_pcfKernel = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_pcfKernel" ), 4u );
				auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
				auto findex = m_writer.DeclLocale( cuT( "findex" )
					, m_writer.Cast< Float >( index ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 1.0_f - texture( c3d_mapShadowSpot, vec4( coords.xy(), findex, coords.z() ) ) );
				auto numSamplesUsed = m_writer.DeclLocale( cuT( "numSamplesUsed" )
					, 1.0_f );
				auto pcfRadius = m_writer.DeclLocale( cuT( "pcfRadius" )
					, 1.0_f );
				auto shadowStep = m_writer.DeclLocale( cuT( "shadowStep" )
					, 0.001_f );

				FOR( m_writer, Int, i, 0, "i < c3d_pcfNumSamplingPositions", "i++" )
				{
					shadowFactor += 1.0_f - texture( c3d_mapShadowSpot, vec4( coords.xy() + c3d_pcfKernel[i] * shadowStep * pcfRadius, findex, coords.z() ) );
					numSamplesUsed += 1.0_f;
				}
				ROF;

				m_writer.Return( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "coords" ) }
			, InInt{ &m_writer, cuT( "index" ) } );

		m_samplePoint = m_writer.ImplementFunction< Float >( cuT( "SamplePoint" )
			, [&]( Vec3 const & direction
				, Float const & depth
				, Int const & index )
			{
				auto c3d_pcfKernel = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_pcfKernel" ), 4u );
				auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 0.0_f );
				auto samples = m_writer.DeclLocale( cuT( "samples" )
					, 4.0_f );
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, 20.0_f * depth );
				auto numSamplesUsed = m_writer.DeclLocale( cuT( "numSamplesUsed" )
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

				m_writer.Return( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "direction" ) }
			, InFloat{ &m_writer, cuT( "depth" ) }
			, InInt{ &m_writer, cuT( "index" ) } );
	}

	void Shadow::DoDeclare_PcfSampleDirectional()
	{
		m_sampleDirectional = m_writer.ImplementFunction< Float >( cuT( "SampleDirectional" )
			, [&]( Vec3 const & coords )
			{
				auto c3d_pcfKernel = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_pcfKernel" ), 4u );
				auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 1.0_f - texture( c3d_mapShadowSpot, coords ) );
				auto numSamplesUsed = m_writer.DeclLocale( cuT( "numSamplesUsed" )
					, 1.0_f );
				auto pcfRadius = m_writer.DeclLocale( cuT( "pcfRadius" )
					, 1.0_f );
				auto shadowStep = m_writer.DeclLocale( cuT( "shadowStep" )
					, 0.0002_f );

				FOR( m_writer, Int, i, 0, "i < c3d_pcfNumSamplingPositions", "i++" )
				{
					shadowFactor += 1.0_f - texture( c3d_mapShadowSpot, vec3( coords.xy() + c3d_pcfKernel[i] * shadowStep * pcfRadius, coords.z() ) );
					numSamplesUsed += 1.0_f;
				}
				ROF;

				m_writer.Return( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "coords" ) } );
	}

	void Shadow::DoDeclare_PcfSampleSpot()
	{
		m_sampleOneSpot = m_writer.ImplementFunction< Float >( cuT( "SampleSpot" )
			, [&]( Vec3 const & coords )
			{
				auto c3d_pcfKernel = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_pcfKernel" ), 4u );
				auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowSpot );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 1.0_f - texture( c3d_mapShadowSpot, coords ) );
				auto numSamplesUsed = m_writer.DeclLocale( cuT( "numSamplesUsed" )
					, 1.0_f );
				auto pcfRadius = m_writer.DeclLocale( cuT( "pcfRadius" )
					, 1.0_f );
				auto shadowStep = m_writer.DeclLocale( cuT( "shadowStep" )
					, 0.002_f );

				FOR( m_writer, Int, i, 0, "i < c3d_pcfNumSamplingPositions", "i++" )
				{
					shadowFactor += 1.0_f - texture( c3d_mapShadowSpot, vec3( coords.xy() + c3d_pcfKernel[i] * shadowStep * pcfRadius, coords.z() ) );
					numSamplesUsed += 1.0_f;
				}
				ROF;

				m_writer.Return( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "coords" ) } );
	}

	void Shadow::DoDeclare_PcfSamplePoint()
	{
		m_sampleOnePoint = m_writer.ImplementFunction< Float >( cuT( "SamplePoint" )
			, [&]( Vec3 const & direction
				, Float const & depth )
			{
				auto c3d_pcfKernel = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_pcfKernel" ), 4u );
				auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( Shadow::MapShadowPoint );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 0.0_f );
				auto samples = m_writer.DeclLocale( cuT( "samples" )
					, 4.0_f );
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, 20.0_f * depth );
				auto numSamplesUsed = m_writer.DeclLocale( cuT( "numSamplesUsed" )
					, 0.0_f );

				FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
				{
					FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
					{
						FOR( m_writer, Float, z, -offset, "z < offset", "z += offset / (samples * 0.5)" )
						{
							shadowFactor += 1.0_f - texture( c3d_mapShadowPoint, vec4( direction + vec3( x, y, z ), depth ) );
							numSamplesUsed += 1.0_f;
						}
						ROF;
					}
					ROF;
				}
				ROF;

				m_writer.Return( shadowFactor / numSamplesUsed );
			}
			, InVec3{ &m_writer, cuT( "direction" ) }
			, InFloat{ &m_writer, cuT( "depth" ) } );
	}
}
