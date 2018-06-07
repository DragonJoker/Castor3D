#include "GlslShadow.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		//***********************************************************************************************

		String paramToString( String & p_sep, OutputComponents const & p_value )
		{
			StringStream result;
			result << paramToString( p_sep, p_value.m_diffuse );
			result << paramToString( p_sep, p_value.m_specular );
			return result.str();
		}

		String toString( OutputComponents const & p_value )
		{
			StringStream result;
			result << toString( p_value.m_diffuse ) << ", ";
			result << toString( p_value.m_specular );
			return result.str();
		}

		//***********************************************************************************************

		OutputComponents::OutputComponents( GlslWriter & p_writer )
			: m_diffuse{ &p_writer, cuT( "outDiffuse" ) }
			, m_specular{ &p_writer, cuT( "outSpecular" ) }
		{
		}

		OutputComponents::OutputComponents( InOutVec3 const & p_v3Diffuse
			, InOutVec3 const & p_v3Specular )
			: m_diffuse{ p_v3Diffuse }
			, m_specular{ p_v3Specular }
		{
		}

		//***********************************************************************************************

		auto constexpr minRawDirectionalOffset = 0.001f;
		auto constexpr maxRawDirectionalSlopeOffset = 0.001f;
		auto constexpr minPcfDirectionalOffset = 0.001f;
		auto constexpr maxPcfDirectionalSlopeOffset = 0.001f;
		auto constexpr minSpotOffset = 0.00001f;
		auto constexpr maxSpotSlopeOffset = 0.0001f;
		auto constexpr minPointOffset = 0.00001f;
		auto constexpr maxPointSlopeOffset = 0.0001f;
		auto constexpr varianceDirectionalBias = 0.15f;
		auto constexpr varianceDirectionalMin = 0.000001f;
		auto constexpr varianceSpotBias = 0.15f;
		auto constexpr varianceSpotMin = 0.000001f;
		auto constexpr variancePointBias = 0.15f;
		auto constexpr variancePointMin = 0.000001f;

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
			doDeclareVolumetric();
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
			doDeclareOneVolumetric( type );
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
			, Float const & farPlane
			, Int const & index )
		{
			return m_computePoint( shadowType
				, worldSpacePosition
				, lightDirection
				, normal
				, farPlane
				, index );
		}

		void Shadow::computeVolumetric( Int const & shadowType
			, Vec3 const & worldSpacePosition
			, Vec3 const & eyePosition
			, Mat4 const & lightMatrix
			, Vec3 const & lightDirection
			, Vec3 const & lightColour
			, Vec2 const & lightIntensity
			, UInt const & lightVolumetricSteps
			, Float const & lightVolumetricScattering
			, OutputComponents & parentOutput )
		{
			m_writer << m_computeVolumetric( shadowType
				, worldSpacePosition
				, eyePosition
				, lightMatrix
				, lightDirection
				, lightColour
				, lightIntensity
				, lightVolumetricSteps
				, lightVolumetricScattering
				, parentOutput );
			m_writer << glsl::endi;
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

		void Shadow::computeVolumetric( Vec3 const & worldSpacePosition
			, Vec3 const & eyePosition
			, Mat4 const & lightMatrix
			, Vec3 const & lightDirection
			, Vec3 const & lightColour
			, Vec2 const & lightIntensity
			, UInt const & lightVolumetricSteps
			, Float const & lightVolumetricScattering
			, OutputComponents & parentOutput )
		{
			m_writer << m_computeOneVolumetric( worldSpacePosition
				, eyePosition
				, lightMatrix
				, lightDirection
				, lightColour
				, lightIntensity
				, lightVolumetricSteps
				, lightVolumetricScattering
				, parentOutput );
			m_writer << glsl::endi;
		}

		Float Shadow::chebyshevUpperBound( Vec2 const & moments
			, Float const & distance
			, Float const & maxVariance
			, Float const & varianceBias )
		{
			return m_chebyshevUpperBound( moments
				, distance
				, maxVariance
				, varianceBias );
		}

		Float Shadow::getShadowOffset( Vec3 const & normal
			, Vec3 const & lightDirection
			, Float const & minOffset
			, Float const & maxSlopeOffset )
		{
			return m_getShadowOffset( normal
				, lightDirection
				, minOffset
				, maxSlopeOffset );
		}

		Float Shadow::textureProj( Vec4 const & lightSpacePosition
			, Vec2 const & offset
			, Sampler2D const & shadowMap
			, Float const & bias )
		{
			return m_textureProj( lightSpacePosition
				, offset
				, shadowMap
				, bias );
		}

		Float Shadow::filterPCF( Vec4 const & lightSpacePosition
			, Sampler2D const & shadowMap
			, Float const & bias )
		{
			return m_filterPCF( lightSpacePosition
				, shadowMap
				, bias );
		}

		Vec4 Shadow::getLightSpacePosition( Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition )
		{
			return m_getLightSpacePosition( lightMatrix
				, worldSpacePosition );
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
					, Float const & minVariance
					, Float const & varianceBias )
				{
					auto p = m_writer.declLocale( cuT( "p" )
						, step( moments.x() + varianceBias, distance ) );
					auto variance = m_writer.declLocale( cuT( "variance" )
						, moments.y() - m_writer.paren( moments.x() * moments.x() ) );
					variance = glsl::max( variance, minVariance );
					auto d = m_writer.declLocale( cuT( "d" )
						, distance - moments.x() );
					variance /= variance + d * d;
					m_writer.returnStmt( glsl::max( p, variance ) );
				}
				, InVec2{ &m_writer, cuT( "moments" ) }
				, InFloat{ &m_writer, cuT( "distance" ) }
				, InFloat{ &m_writer, cuT( "minVariance" ) }
				, InFloat{ &m_writer, cuT( "varianceBias" ) } );
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
							shadowFactor += textureProj( lightSpacePosition
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
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					IF( m_writer, shadowType == Int( int( ShadowType::eRaw ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minRawDirectionalOffset )
								, Float( maxRawDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowDirectional
							, bias ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minPcfDirectionalOffset )
								, Float( maxPcfDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( filterPCF( lightSpacePosition
							, c3d_mapShadowDirectional
							, bias ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowDirectional, lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceDirectionalMin )
							, Float( varianceDirectionalBias ) ) );
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
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					IF( m_writer, shadowType == Int( int( ShadowType::eRaw ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowSpot[index]
							, bias ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( filterPCF( lightSpacePosition
							, c3d_mapShadowSpot[index]
							, bias ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowSpot[index], lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceSpotMin )
							, Float( varianceSpotBias ) ) );
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

					IF( m_writer, shadowType == Int( int( ShadowType::eRaw ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
							, texture( c3d_mapShadowPoint[index], vertexToLight ).x() );
						m_writer.returnStmt( step( depth - bias, shadowMapDepth ) );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
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
									auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
										, texture( c3d_mapShadowPoint[index], vertexToLight + vec3( x, y, z ) ).x() );
									shadowFactor += step( depth - bias, shadowMapDepth );
									numSamplesUsed += 1.0_f;
								}
								ROF;
							}
							ROF;
						}
						ROF;

						m_writer.returnStmt( shadowFactor / numSamplesUsed );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
					{
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
									shadowFactor += chebyshevUpperBound( moments
										, depth
										, Float( variancePointMin )
										, Float( variancePointBias ) );
									numSamplesUsed += 1.0_f;
								}
								ROF;
							}
							ROF;
						}
						ROF;

						m_writer.returnStmt( shadowFactor / numSamplesUsed );
					}
					FI;
				}
				, InInt( &m_writer, cuT( "shadowType" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightPosition" ) )
				, InVec3( &m_writer, cuT( "normal" ) )
				, InFloat( &m_writer, cuT( "farPlane" ) )
				, InInt( &m_writer, cuT( "index" ) ) );
		}

		void Shadow::doDeclareVolumetric()
		{
			OutputComponents output{ m_writer };
			m_computeVolumetric = m_writer.implementFunction< Void >( cuT( "computeVolumetric" )
				, [this]( Int const & shadowType
					, Vec3 const & position
					, Vec3 const & eyePosition
					, Mat4 const & lightMatrix
					, Vec3 const & lightDirection
					, Vec3 const & lightColour
					, Vec2 const & lightIntensity
					, UInt const & lightVolumetricSteps
					, Float const & lightVolumetricScattering
					, OutputComponents & parentOutput )
				{
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto volumetricDither = m_writer.declConstant< Mat4 >( cuT( "volumetricDither" )
						, mat4(
							0.0_f, 0.5_f, 0.125_f, 0.625_f,
							0.75_f, 0.22_f, 0.875_f, 0.375_f,
							0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f,
							0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) );
					auto gl_FragCoord = m_writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

					auto rayVector = m_writer.declLocale( cuT( "rayVector" )
						, position - eyePosition );
					auto rayLength = m_writer.declLocale( cuT( "rayLength" )
						, length( rayVector ) );
					auto rayDirection = m_writer.declLocale( cuT( "rayDirection" )
						, rayVector / rayLength );
					auto stepLength = m_writer.declLocale( cuT( "stepLength" )
						, rayLength / lightVolumetricSteps );
					auto step = m_writer.declLocale( cuT( "step" )
						, rayDirection * stepLength );
					auto screenUV = m_writer.declLocale( cuT( "screenUV" )
						, ivec2( gl_FragCoord.xy() ) );
					auto ditherValue = m_writer.declLocale( cuT( "ditherValue" )
						, volumetricDither[screenUV.x() % 4][screenUV.y() % 4] );

					auto currentPosition = m_writer.declLocale( cuT( "currentPosition" )
						, eyePosition + step * ditherValue );
					auto volumetric = m_writer.declLocale( cuT( "volumetric" )
						, 0.0_f );

					auto RdotL = m_writer.declLocale( cuT( "RdotL" )
						, dot( rayDirection, lightDirection ) );
					auto sqVolumetricScattering = m_writer.declLocale( cuT( "sqVolumetricScattering" )
						, lightVolumetricScattering * lightVolumetricScattering );
					auto dblVolumetricScattering = m_writer.declLocale( cuT( "dblVolumetricScattering" )
						, 2.0_f * lightVolumetricScattering );
					auto oneMinusVolumeScattering = m_writer.declLocale( cuT( "oneMinusVolumeScattering" )
						, 1.0_f - sqVolumetricScattering );
					auto scattering = m_writer.declLocale( cuT( "scattering" )
						, oneMinusVolumeScattering / m_writer.paren( 4.0_f
							* Float( PI )
							* pow( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * RdotL, 1.5_f ) ) );

					FOR( m_writer, Int, i, 0, "i < int( lightVolumetricSteps )", "++i" )
					{
						IF ( m_writer, m_computeDirectional( shadowType, lightMatrix, currentPosition, lightDirection, vec3( 0.0_f ) ) < 0.5_f )
						{
							volumetric += scattering;
						}
						FI;

						currentPosition += step;
					}
					ROF;

					volumetric /= lightVolumetricSteps;
					parentOutput.m_diffuse += volumetric * lightIntensity.x() * 1.0_f * lightColour;
					parentOutput.m_specular += volumetric * lightIntensity.y() * 1.0_f * lightColour;
				}
				, InInt{ &m_writer, cuT( "shadowType" ) }
				, InVec3{ &m_writer, cuT( "position" ) }
				, InVec3{ &m_writer, cuT( "eyePosition" ) }
				, InMat4{ &m_writer, cuT( "lightMatrix" ) }
				, InVec3{ &m_writer, cuT( "lightDirection" ) }
				, InVec3{ &m_writer, cuT( "lightColour" ) }
				, InVec2{ &m_writer, cuT( "lightIntensity" ) }
				, InUInt{ &m_writer, cuT( "lightVolumetricSteps" ) }
				, InFloat{ &m_writer, cuT( "lightVolumetricScattering" ) }
				, output );
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
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					if ( type == ShadowType::eRaw )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minRawDirectionalOffset )
								, Float( maxRawDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowDirectional
							, bias ) );
					}
					else if ( type == ShadowType::ePCF )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minPcfDirectionalOffset )
								, Float( maxPcfDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( filterPCF( lightSpacePosition
							, c3d_mapShadowDirectional
							, bias ) );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowDirectional, lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceDirectionalMin )
							, Float( varianceDirectionalBias ) ) );
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
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					if ( type == ShadowType::eRaw )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowSpot
							, bias ) );
					}
					else if ( type == ShadowType::ePCF )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( filterPCF( lightSpacePosition
							, c3d_mapShadowSpot
							, bias ) );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowSpot, lightSpacePosition.xy() ).xy() );
						m_writer.returnStmt( chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceSpotMin )
							, Float( varianceSpotBias ) ) );
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

					if ( type == ShadowType::eRaw )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
							, texture( c3d_mapShadowPoint, vec3( vertexToLight ) ).r() );
						m_writer.returnStmt( step( depth - bias, shadowMapDepth ) );
					}
					else if( type == ShadowType::ePCF )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
							, 0.0_f );
						auto samples = m_writer.declLocale( cuT( "samples" )
							, 4.0_f );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
							, 0.0_f );
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );

						FOR( m_writer, Float, x, -offset, "x < offset", "x += offset / (samples * 0.5)" )
						{
							FOR( m_writer, Float, y, -offset, "y < offset", "y += offset / (samples * 0.5)" )
							{
								FOR( m_writer, Float, z, -offset, "z < offset", "z += offset / (samples * 0.5)" )
								{
									auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
										, texture( c3d_mapShadowPoint, vertexToLight + vec3( x, y, z ) ).x() );
									shadowFactor += step( depth - bias, shadowMapDepth );
									numSamplesUsed += 1.0_f;
								}
								ROF;
							}
							ROF;
						}
						ROF;

						m_writer.returnStmt( shadowFactor / numSamplesUsed );
					}
					else if ( type == ShadowType::eVariance )
					{
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
									shadowFactor += chebyshevUpperBound( moments
										, depth
										, Float( variancePointMin )
										, Float( variancePointBias ) );
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

		void Shadow::doDeclareOneVolumetric( ShadowType type )
		{
			OutputComponents output{ m_writer };
			m_computeOneVolumetric = m_writer.implementFunction< Void >( cuT( "computeVolumetric" )
				, [this]( Vec3 const & position
					, Vec3 const & eyePosition
					, Mat4 const & lightMatrix
					, Vec3 const & lightDirection
					, Vec3 const & lightColour
					, Vec2 const & lightIntensity
					, UInt const & lightVolumetricSteps
					, Float const & lightVolumetricScattering
					, OutputComponents & parentOutput )
				{
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto volumetricDither = m_writer.declConstant< Mat4 >( cuT( "volumetricDither" )
						, mat4(
							0.0_f, 0.5_f, 0.125_f, 0.625_f,
							0.75_f, 0.22_f, 0.875_f, 0.375_f,
							0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f,
							0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) );
					auto gl_FragCoord = m_writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

					auto rayVector = m_writer.declLocale( cuT( "rayVector" )
						, position - eyePosition );
					auto rayLength = m_writer.declLocale( cuT( "rayLength" )
						, length( rayVector ) );
					auto rayDirection = m_writer.declLocale( cuT( "rayDirection" )
						, rayVector / rayLength );
					auto stepLength = m_writer.declLocale( cuT( "stepLength" )
						, rayLength / lightVolumetricSteps );
					auto step = m_writer.declLocale( cuT( "step" )
						, rayDirection * stepLength );
					auto screenUV = m_writer.declLocale( cuT( "screenUV" )
						, ivec2( gl_FragCoord.xy() ) );
					auto ditherValue = m_writer.declLocale( cuT( "ditherValue" )
						, volumetricDither[screenUV.x() % 4][screenUV.y() % 4] );

					auto currentPosition = m_writer.declLocale( cuT( "currentPosition" )
						, eyePosition + step * ditherValue );
					auto volumetric = m_writer.declLocale( cuT( "volumetric" )
						, 0.0_f );

					auto RdotL = m_writer.declLocale( cuT( "RdotL" )
						, dot( rayDirection, lightDirection ) );
					auto sqVolumetricScattering = m_writer.declLocale( cuT( "sqVolumetricScattering" )
						, lightVolumetricScattering * lightVolumetricScattering );
					auto dblVolumetricScattering = m_writer.declLocale( cuT( "dblVolumetricScattering" )
						, 2.0_f * lightVolumetricScattering );
					auto oneMinusVolumeScattering = m_writer.declLocale( cuT( "oneMinusVolumeScattering" )
						, 1.0_f - sqVolumetricScattering );
					auto scattering = m_writer.declLocale( cuT( "scattering" )
						, oneMinusVolumeScattering / m_writer.paren( 4.0_f
							* Float( PI )
							* pow( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * RdotL, 1.5_f ) ) );

					FOR( m_writer, Int, i, 0, "i < int( lightVolumetricSteps )", "++i" )
					{
						IF ( m_writer, m_computeOneDirectional( lightMatrix, currentPosition, lightDirection, vec3( 0.0_f ) ) > 0.5_f )
						{
							volumetric += scattering;
						}
						FI;

						currentPosition += step;
					}
					ROF;

					volumetric /= lightVolumetricSteps;
					parentOutput.m_diffuse += volumetric * lightIntensity.x() * 1.0_f * lightColour;
					parentOutput.m_specular += volumetric * lightIntensity.y() * 1.0_f * lightColour;
				}
				, InVec3{ &m_writer, cuT( "position" ) }
				, InVec3{ &m_writer, cuT( "eyePosition" ) }
				, InMat4{ &m_writer, cuT( "lightMatrix" ) }
				, InVec3{ &m_writer, cuT( "lightDirection" ) }
				, InVec3{ &m_writer, cuT( "lightColour" ) }
				, InVec2{ &m_writer, cuT( "lightIntensity" ) }
				, InUInt{ &m_writer, cuT( "lightVolumetricSteps" ) }
				, InFloat{ &m_writer, cuT( "lightVolumetricScattering" ) }
				, output );
		}
	}
}
