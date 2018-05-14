#include "GlslShadow.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		auto constexpr minDirectionalOffset = 0.00001f;
		auto constexpr maxDirectionalSlopeOffset = 0.0001f;
		auto constexpr minSpotOffset = 0.00001f;
		auto constexpr maxSpotSlopeOffset = 0.0001f;
		auto constexpr minPointOffset = 0.00001f;
		auto constexpr maxPointSlopeOffset = 0.0001f;

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

		void Shadow::declare( uint32_t & index )
		{
			auto c3d_mapShadowDirectional = m_writer.declSampler< Sampler2D >( MapShadowDirectional, index++, 1u );
			auto c3d_mapShadowSpot = m_writer.declSamplerArray< Sampler2D >( MapShadowSpot, index, 1u, SpotShadowMapCount );
			index += SpotShadowMapCount;
			auto c3d_mapShadowPoint = m_writer.declSamplerArray< SamplerCube >( MapShadowPoint, index, 1u, PointShadowMapCount );
			index += PointShadowMapCount;
			doDeclareGetRandom();
			doDeclareTextureProj();
			doDeclareFilterPCF();
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
			auto c3d_mapShadowDirectional = m_writer.declSampler< Sampler2D >( MapShadowDirectional, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureProj();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureProj();
				doDeclareFilterPCF();
				break;

			case ShadowType::eVariance:
				doDeclareChebyshevUpperBound();
				break;
			}

			doDeclareGetLightSpacePosition();
			doDeclareComputeOneDirectionalShadow( type );
		}

		void Shadow::declarePoint( ShadowType type
			, uint32_t & index )
		{
			auto c3d_mapShadowPoint = m_writer.declSampler< SamplerCube >( MapShadowPoint, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureProj();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureProj();
				doDeclareFilterPCF();
				break;

			case ShadowType::eVariance:
				doDeclareGetShadowOffset();
				doDeclareChebyshevUpperBound();
				break;
			}

			doDeclareComputeOnePointShadow( type );
		}

		void Shadow::declareSpot( ShadowType type
			, uint32_t & index )
		{
			auto c3d_mapShadowSpot = m_writer.declSampler< Sampler2D >( MapShadowSpot, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureProj();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureProj();
				doDeclareFilterPCF();
				break;

			case ShadowType::eVariance:
				doDeclareChebyshevUpperBound();
				break;
			}

			doDeclareGetLightSpacePosition();
			doDeclareComputeOneSpotShadow( type );
		}

		Float Shadow::computeDirectionalShadow( Int const & shadowType
			, Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal )
		{
			return m_computeDirectional( shadowType
				, lightMatrix
				, worldSpacePosition
				, lightDirection
				, normal );
		}

		Float Shadow::computeSpotShadow( Int const & shadowType
			, Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal
			, Int const & index )
		{
			return m_computeSpot( shadowType
				, lightMatrix
				, worldSpacePosition
				, lightDirection
				, normal
				, index );
		}

		Float Shadow::computePointShadow( Int const & shadowType
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal
			, glsl::Float const & farPlane
			, Int const & index )
		{
			return m_computePoint( shadowType
				, worldSpacePosition
				, lightDirection
				, normal
				, farPlane
				, index );
		}

		Float Shadow::computeDirectionalShadow( Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal )
		{
			return m_computeOneDirectional( lightMatrix
				, worldSpacePosition
				, lightDirection
				, normal );
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
					, Vec3 const & lightDirection
					, Float const & minOffset
					, Float const & maxSlopeOffset )
				{
					auto cosAlpha = m_writer.declLocale( cuT( "cosAlpha" )
						, clamp( dot( normal, normalize( lightDirection ) ), 0.0_f, 1.0_f ) );
					auto offset = m_writer.declLocale( cuT( "offset" )
						, sqrt( 1.0_f - cosAlpha ) );
					m_writer.returnStmt( minOffset + maxSlopeOffset * offset );
				}
				, InVec3( &m_writer, cuT( "normal" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InFloat( &m_writer, cuT( "minOffset" ) )
				, InFloat( &m_writer, cuT( "maxSlopeOffset" ) ) );
		}

		void Shadow::doDeclareChebyshevUpperBound()
		{
			m_chebyshevUpperBound = m_writer.implementFunction< Float >( cuT( "chebyshevUpperBound" )
				, [this]( Vec2 const & moments
					, Float const & distance
					, Float const & varianceFactor
					, Float const & lightBleedingAmount )
				{
					auto p = m_writer.declLocale( cuT( "p" )
						, step( moments.x() + lightBleedingAmount, distance ) );
					auto variance = m_writer.declLocale( cuT( "variance" )
						, glsl::max( moments.y() - m_writer.paren( moments.x() * moments.x() ), varianceFactor ) );
					auto d = m_writer.declLocale( cuT( "d" )
						, distance - moments.x() );
					variance /= variance + d * d;
					m_writer.returnStmt( glsl::max( p, clamp( variance, 0.0_f, 1.0_f ) ) );
				}
				, InVec2{ &m_writer, cuT( "moments" ) }
				, InFloat{ &m_writer, cuT( "distance" ) }
				, InFloat{ &m_writer, cuT( "varianceFactor" ) }
				, InFloat{ &m_writer, cuT( "lightBleedingAmount" ) } );
		}

		void Shadow::doDeclareTextureProj()
		{
			m_textureProj = m_writer.implementFunction< Float >( cuT( "textureProj" )
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, Sampler2D const & shadowMap
					, Float const & bias )
				{
					auto shadow = m_writer.declLocale( cuT( "shadow" )
						, 1.0_f );
					auto shadowCoord = m_writer.declLocale( cuT( "shadowCoord" )
						, lightSpacePosition );

					IF( m_writer, shadowCoord.z() > -1.0_f && shadowCoord.z() < 1.0_f )
					{
						auto uv = m_writer.declLocale( cuT( "uv" )
							, shadowCoord.st() + offset );
						auto dist = m_writer.declLocale( cuT( "dist" )
							, texture( shadowMap, uv ).r() );

						IF( m_writer, shadowCoord.w() > 0 )
						{
							shadow = step( shadowCoord.z() - bias, dist );
						}
						FI;
					}
					FI;

					m_writer.returnStmt( shadow );
				}
				, InVec4{ &m_writer, cuT( "lightSpacePosition" ) }
				, InVec2{ &m_writer, cuT( "offset" ) }
				, InSampler2D{ &m_writer, cuT( "shadowMap" ) }
				, InFloat{ &m_writer, cuT( "bias" ) } );
		}

		void Shadow::doDeclareFilterPCF()
		{
			m_filterPCF = m_writer.implementFunction< Float >( cuT( "filterPCF" )
				, [this]( Vec4 const & lightSpacePosition
					, Sampler2D const & shadowMap
					, Float const & bias )
				{
					auto texDim = m_writer.declLocale( cuT( "texDim" )
						, textureSize( shadowMap, 0 ) );
					auto scale = m_writer.declLocale( cuT( "scale" )
						, 1.0_f );
					auto dx = m_writer.declLocale( cuT( "dx" )
						, scale * 1.0_f / m_writer.cast< Float >( texDim.x() ) );
					auto dy = m_writer.declLocale( cuT( "dy" )
						, scale * 1.0_f / m_writer.cast< Float >( texDim.y() ) );

					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 0.0_f );
					auto count = m_writer.declLocale( cuT( "count" )
						, 0_i );
					auto range = m_writer.declLocale( cuT( "range" )
						, 1_i );

					FOR( m_writer, Int, x, -range, "x <= range", "x++" )
					{
						FOR( m_writer, Int, y, -range, "y <= range", "y++" )
						{
							shadowFactor += m_textureProj( lightSpacePosition
								, vec2( dx * x, dy * y )
								, shadowMap
								, bias );
							count = count + 1_i;
						}
						ROF;
					}
					ROF;

					m_writer.returnStmt( shadowFactor / count );
				}
				, InVec4{ &m_writer, cuT( "lightSpacePosition" ) }
				, InSampler2D{ &m_writer, cuT( "shadowMap" ) }
				, InFloat{ &m_writer, cuT( "bias" ) } );
		}

		void Shadow::doDeclareGetLightSpacePosition()
		{
			m_getLightSpacePosition = m_writer.implementFunction< Vec4 >( cuT( "getLightSpacePosition" )
				, [this]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition )
				{
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, lightMatrix * vec4( worldSpacePosition, 1.0_f ) );
					auto projCoords = m_writer.declLocale( cuT( "projCoords" )
						, lightSpacePosition / lightSpacePosition.w() );
					projCoords.x() = glsl::fma( projCoords.x(), 0.5_f, 0.5_f );
					projCoords.y() = glsl::fma( projCoords.y(), 0.5_f, 0.5_f );

					if ( !m_writer.isZeroToOneDepth() )
					{
						projCoords.z() = glsl::fma( projCoords.z(), 0.5_f, 0.5_f );
					}

					m_writer.returnStmt( projCoords );
				}
				, InMat4( &m_writer, cuT( "lightMatrix" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) ) );
		}

		void Shadow::doDeclareComputeDirectionalShadow()
		{
			m_computeDirectional = m_writer.implementFunction< Float >( cuT( "computeDirectionalShadow" )
				, [this]( Int const & shadowType
					, Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, Vec3 const & normal )
				{
					auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					IF( m_writer, shadowType == Int( int( ShadowType::eRaw ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, m_getShadowOffset( normal
								, lightDirection
								, Float( minDirectionalOffset )
								, Float( maxDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( m_textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowDirectional
							, bias ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, m_getShadowOffset( normal
								, lightDirection
								, Float( minDirectionalOffset )
								, Float( maxDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( m_filterPCF( lightSpacePosition
							, c3d_mapShadowDirectional
							, bias ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowDirectional, lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( m_chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, 0.0000004_f
							, 0.01_f ) );
					}
					FI;
				}
				, InInt( &m_writer, cuT( "shadowType" ) )
				, InMat4( &m_writer, cuT( "lightMatrix" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InVec3( &m_writer, cuT( "normal" ) ) );
		}

		void Shadow::doDeclareComputeSpotShadow()
		{
			m_computeSpot = m_writer.implementFunction< Float >( cuT( "computeSpotShadow" )
				, [this]( Int const & shadowType
					, Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, Vec3 const & normal
					, Int const & index )
				{
					auto c3d_mapShadowSpot = m_writer.getBuiltinArray< Sampler2D >( Shadow::MapShadowSpot, SpotShadowMapCount );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					IF( m_writer, shadowType == Int( int( ShadowType::eRaw ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, m_getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( m_textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowSpot[index]
							, bias ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, m_getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( m_filterPCF( lightSpacePosition
							, c3d_mapShadowSpot[index]
							, bias ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowSpot[index], lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( m_chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, 0.0000004_f
							, 0.01_f ) );
					}
					FI;
				}
				, InInt( &m_writer, cuT( "shadowType" ) )
				, InMat4( &m_writer, cuT( "lightMatrix" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InVec3( &m_writer, cuT( "normal" ) )
				, InInt( &m_writer, cuT( "index" ) ) );
		}

		void Shadow::doDeclareComputePointShadow()
		{
			m_computePoint = m_writer.implementFunction< Float >( cuT( "computePointShadow" )
				, [this]( Int const & shadowType
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightPosition
					, Vec3 const & normal
					, Float const & farPlane
					, Int const & index )
				{
					auto c3d_mapShadowPoint = m_writer.getBuiltinArray< SamplerCube >( MapShadowPoint, PointShadowMapCount );
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
					auto bias = m_writer.declLocale( cuT( "bias" )
						, m_getShadowOffset( normal
							, vertexToLight
							, Float( minPointOffset )
							, Float( maxPointSlopeOffset ) ) );

					IF( m_writer, shadowType == Int( int( ShadowType::eRaw ) ) )
					{
						auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
							, texture( c3d_mapShadowPoint[index], vertexToLight ).x() );
						shadowFactor += step( depth - bias, shadowMapDepth );
					}
					ELSE
					{
						FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
						{
							FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
							{
								FOR( m_writer, Float, z, -offset, "z < offset", "z += offset / (samples * 0.5)" )
								{
									IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
									{
										auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
											, texture( c3d_mapShadowPoint[index], vertexToLight + vec3( x, y, z ) ).x() );
										shadowMapDepth += bias;
										shadowFactor += step( depth, shadowMapDepth );
									}
									ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
									{
										auto moments = m_writer.declLocale( cuT( "moments" )
											, texture( c3d_mapShadowPoint[index]
												, vertexToLight + vec3( x, y, z ) ).xy() );
										shadowFactor += 1.0_f - m_chebyshevUpperBound( moments
											, depth
											, 0.00000002_f
											, 0.01_f );
									}
									FI;

									numSamplesUsed += 1.0_f;
								}
								ROF;
							}
							ROF;
						}
						ROF;
					}
					FI;

					m_writer.returnStmt( shadowFactor / numSamplesUsed );
				}
				, InInt( &m_writer, cuT( "shadowType" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightPosition" ) )
				, InVec3( &m_writer, cuT( "normal" ) )
				, InFloat( &m_writer, cuT( "farPlane" ) )
				, InInt( &m_writer, cuT( "index" ) ) );
		}

		void Shadow::doDeclareComputeOneDirectionalShadow( ShadowType type )
		{
			m_computeOneDirectional = m_writer.implementFunction< Float >( cuT( "computeDirectionalShadow" )
				, [this, type]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, Vec3 const & normal )
				{
					auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					if ( type == ShadowType::eRaw )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, m_getShadowOffset( normal
								, lightDirection
								, Float( minDirectionalOffset )
								, Float( maxDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( m_textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowDirectional
							, bias ) );
					}
					else if ( type == ShadowType::ePCF )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, m_getShadowOffset( normal
								, lightDirection
								, Float( minDirectionalOffset )
								, Float( maxDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( m_filterPCF( lightSpacePosition
							, c3d_mapShadowDirectional
							, bias ) );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowDirectional, lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( m_chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, 0.0000004_f
							, 0.01_f ) );
					}
				}
				, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InVec3( &m_writer, cuT( "normal" ) ) );
		}

		void Shadow::doDeclareComputeOneSpotShadow( ShadowType type )
		{
			m_computeOneSpot = m_writer.implementFunction< Float >( cuT( "computeSpotShadow" )
				, [this, type]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, Vec3 const & normal )
				{
					auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowSpot );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					if ( type == ShadowType::eRaw )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, m_getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( m_textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowSpot
							, bias ) );
					}
					else if ( type == ShadowType::ePCF )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, m_getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( m_filterPCF( lightSpacePosition
							, c3d_mapShadowSpot
							, bias ) );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowSpot, lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( m_chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, 0.0000004_f
							, 0.01_f ) );
					}
				}
				, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InVec3( &m_writer, cuT( "normal" ) ) );
		}

		void Shadow::doDeclareComputeOnePointShadow( ShadowType type )
		{
			m_computeOnePoint = m_writer.implementFunction< Float >( cuT( "computePointShadow" )
				, [this, type]( Vec3 const & worldSpacePosition
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
					auto bias = m_writer.declLocale( cuT( "bias" )
						, m_getShadowOffset( normal
							, vertexToLight
							, Float( minPointOffset )
							, Float( maxPointSlopeOffset ) ) );

					if ( type == ShadowType::eRaw )
					{
						auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
							, texture( c3d_mapShadowPoint, vec3( vertexToLight ) ).r() );
						m_writer.returnStmt( step( depth - bias, shadowMapDepth ) );
					}
					else
					{
						FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
						{
							FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
							{
								FOR( m_writer, Float, z, -offset, "z < offset", "z += offset / (samples * 0.5)" )
								{
									if ( type == ShadowType::ePCF )
									{
										auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
											, texture( c3d_mapShadowPoint, vertexToLight + vec3( x, y, z ) ).x() );
										shadowMapDepth += bias;
										shadowFactor += step( depth, shadowMapDepth );
									}
									else if ( type == ShadowType::eVariance )
									{
										auto moments = m_writer.declLocale( cuT( "moments" )
											, texture( c3d_mapShadowPoint
												, vertexToLight + vec3( x, y, z ) ).xy() );
										shadowFactor += 1.0_f - m_chebyshevUpperBound( moments
											, depth
											, 0.00000002_f
											, 0.01_f );
									}
									numSamplesUsed += 1.0_f;
								}
								ROF;
							}
							ROF;
						}
						ROF;

						m_writer.returnStmt( shadowFactor / numSamplesUsed );
					}
				}
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightPosition" ) )
				, InVec3( &m_writer, cuT( "normal" ) )
				, InFloat( &m_writer, cuT( "farPlane" ) ) );
		}
	}
}
