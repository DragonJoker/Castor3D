#include "GlslShadow.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		castor::String const Shadow::MapShadowDirectional = cuT( "c3d_mapShadowDirectional" );
		castor::String const Shadow::MapShadowSpot = cuT( "c3d_mapShadowSpot" );
		castor::String const Shadow::MapShadowPoint = cuT( "c3d_mapShadowPoint" );
		castor::String const Shadow::MapDepthDirectional = cuT( "c3d_mapDepthDirectional" );
		castor::String const Shadow::MapDepthSpot = cuT( "c3d_mapDepthSpot" );
		castor::String const Shadow::MapDepthPoint = cuT( "c3d_mapDepthPoint" );

		Shadow::Shadow( GlslWriter & writer )
			: m_writer{ writer }
		{
		}

		void Shadow::declare( ShadowType type
			, uint32_t & index )
		{
			auto c3d_mapShadowDirectional = m_writer.declSampler< Sampler2D >( MapShadowDirectional, index++, 0u );
			auto c3d_mapShadowSpot = m_writer.declSamplerArray< Sampler2D >( MapShadowSpot, index, 0u, SpotShadowMapCount );
			index += SpotShadowMapCount;
			auto c3d_mapShadowPoint = m_writer.declSamplerArray< SamplerCube >( MapShadowPoint, index, 0u, PointShadowMapCount );
			index += PointShadowMapCount;
			doDeclareGetRandom();
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			doDeclareGetLightSpacePosition();
			doDeclareComputeDirectionalShadow();
			doDeclareComputeSpotShadow();
			doDeclareComputePointShadow();
		}

		void Shadow::declareDirectional( ShadowType type
			, uint32_t & index )
		{
			auto c3d_mapShadowDirectional = m_writer.declSampler< Sampler2D >( MapShadowDirectional, index++, 0u );
			auto c3d_mapDepthDirectional = m_writer.declSampler< Sampler2D >( MapDepthDirectional, index++, 0u );
			doDeclareGetRandom();
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			doDeclareGetLightSpacePosition();
			doDeclareComputeDirectionalShadow();
		}

		void Shadow::declarePoint( ShadowType type
			, uint32_t & index )
		{
			auto c3d_mapShadowPoint = m_writer.declSampler< SamplerCube >( MapShadowPoint, index++, 0u );
			auto c3d_mapDepthPoint = m_writer.declSampler< SamplerCube >( MapDepthPoint, index++, 0u );
			doDeclareGetRandom();
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			doDeclareComputeOnePointShadow();
		}

		void Shadow::declareSpot( ShadowType type
			, uint32_t & index )
		{
			auto c3d_mapShadowSpot = m_writer.declSampler< Sampler2D >( MapShadowSpot, index++, 0u );
			auto c3d_mapDepthSpot = m_writer.declSampler< Sampler2D >( MapDepthSpot, index++, 0u );
			doDeclareGetRandom();
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			doDeclareGetLightSpacePosition();
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
			, glsl::Float const & farPlane
			, Int const & index )
		{
			return m_computePoint( worldSpacePosition
				, lightDirection
				, normal
				, farPlane
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
			, Vec3 const & normal
			, glsl::Float const & farPlane )
		{
			return m_computeOnePoint( worldSpacePosition
				, lightDirection
				, normal
				, farPlane );
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
						, sqrt( 1.0_f - cosAlpha * cosAlpha ) );
					m_writer.returnStmt( offsetScale );
				}
				, InVec3( &m_writer, cuT( "normal" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) ) );
		}

		void Shadow::doDeclareChebyshevUpperBound()
		{
			m_chebyshevUpperBound = m_writer.implementFunction< Float >( cuT( "chebyshevUpperBound" )
				, [this]( Vec2 const & moments
					, Float const & distance
					, Float const & varianceFactor
					, Float const & lightBleedingAmount )
				{
					auto variance = m_writer.declLocale( cuT( "variance" )
						, glsl::max( moments.y() - m_writer.paren( moments.x() * moments.x() ), varianceFactor ) );
					auto d = m_writer.declLocale( cuT( "d" )
						, distance - moments.x() );
					variance /= variance + d * d;
					variance = clamp( m_writer.paren( variance - lightBleedingAmount ) / m_writer.paren( 1.0_f - lightBleedingAmount )
						, 0.0_f
						, 1.0_f );
					m_writer.returnStmt( variance );
				}
				, InVec2{ &m_writer, cuT( "moments" ) }
				, InFloat{ &m_writer, cuT( "distance" ) }
				, InFloat{ &m_writer, cuT( "varianceFactor" ) }
				, InFloat{ &m_writer, cuT( "lightBleedingAmount" ) } );
		}

		void Shadow::doDeclareGetLightSpacePosition()
		{
			m_getLightSpacePosition = m_writer.implementFunction< Vec3 >( cuT( "getLightSpacePosition" )
				, [this]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition )
				{
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, lightMatrix * vec4( worldSpacePosition, 1.0_f ) );
					// Perspective divide (result in range [-1,1]).
					m_writer.returnStmt( lightSpacePosition.xyz() / lightSpacePosition.w() );
				}
				, InMat4( &m_writer, cuT( "lightMatrix" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) ) );
		}

		void Shadow::doDeclareComputeDirectionalShadow()
		{
			m_computeDirectional = m_writer.implementFunction< Float >( cuT( "computeDirectionalShadow" )
				, [this]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, Vec3 const & normal )
				{
					auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getLightSpacePosition( lightMatrix, worldSpacePosition ) );
					auto moments = m_writer.declLocale( cuT( "moments" )
						, texture( c3d_mapShadowDirectional, lightSpacePosition.xy() ).xy() );

					IF( m_writer, lightSpacePosition.z() <= moments.x() )
					{
						m_writer.returnStmt( 0.0_f );
					}
					FI;

					m_writer.returnStmt( 1.0_f - m_chebyshevUpperBound( moments
						, lightSpacePosition.z()
						, 0.0000002_f
						, 0.02_f ) );
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
					auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowSpot, SpotShadowMapCount );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getLightSpacePosition( lightMatrix, worldSpacePosition ) );
					auto moments = m_writer.declLocale( cuT( "moments" )
						, texture( c3d_mapShadowSpot[index], lightSpacePosition.xy() ).xy() );

					IF( m_writer, lightSpacePosition.z() <= moments.x() )
					{
						m_writer.returnStmt( 1.0_f );
					}
					FI;

					m_writer.returnStmt( m_chebyshevUpperBound( moments
						, lightSpacePosition.z()
						, 0.0000004_f
						, 0.01_f ) );
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
				, Float const & farPlane
				, Int const & index )
				{
					auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCube >( MapShadowPoint, PointShadowMapCount );
					auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
						, worldSpacePosition - lightPosition );
					auto depth = m_writer.declLocale( cuT( "depth" )
						, length( vertexToLight ) / farPlane );
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
								auto moments = m_writer.declLocale( cuT( "moments" )
									, texture( c3d_mapShadowPoint[index]
										, vertexToLight + vec3( x, y, z ) ).xy() );
								shadowFactor += m_writer.ternary( depth <= moments.x()
									, 0.0_f
									, 1.0_f - m_chebyshevUpperBound( moments
										, depth
										, 0.00000002_f
										, 0.01_f ) );
								numSamplesUsed += 1.0_f;
							}
							ROF;
						}
						ROF;
					}
					ROF;

					m_writer.returnStmt( shadowFactor / numSamplesUsed );
				}
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightPosition" ) )
				, InVec3( &m_writer, cuT( "normal" ) )
				, InFloat( &m_writer, cuT( "farPlane" ) )
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
					auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowSpot );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getLightSpacePosition( lightMatrix, worldSpacePosition ) );
					auto moments = m_writer.declLocale( cuT( "moments" )
						, texture( c3d_mapShadowSpot, lightSpacePosition.xy() ).xy() );

					IF( m_writer, lightSpacePosition.z() <= moments.x() )
					{
						m_writer.returnStmt( 0.0_f );
					}
					FI;

					m_writer.returnStmt( 1.0_f - m_chebyshevUpperBound( moments
						, lightSpacePosition.z()
						, 0.0000004_f
						, 0.01_f ) );
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
					, Vec3 const & normal
					, Float const & farPlane )
				{
					auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCube >( MapShadowPoint );
					auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
						, worldSpacePosition - lightPosition );
					auto depth = m_writer.declLocale( cuT( "depth" )
						, length( vertexToLight ) / farPlane );
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
								auto moments = m_writer.declLocale( cuT( "moments" )
									, texture( c3d_mapShadowPoint
										, vertexToLight + vec3( x, y, z ) ).xy() );
								shadowFactor += m_writer.ternary( depth <= moments.x()
									, 0.0_f
									, 1.0_f - m_chebyshevUpperBound( moments
										, depth
										, 0.00000002_f
										, 0.01_f ) );
								numSamplesUsed += 1.0_f;
							}
							ROF;
						}
						ROF;
					}
					ROF;

					m_writer.returnStmt( shadowFactor / numSamplesUsed );
				}
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightPosition" ) )
				, InVec3( &m_writer, cuT( "normal" ) )
				, InFloat( &m_writer, cuT( "farPlane" ) ) );
		}
	}
}
