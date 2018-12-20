#include "GlslShadow.hpp"

#include "ShadowMap/ShadowMapPassDirectional.hpp"
#include "ShadowMap/ShadowMapPassSpot.hpp"

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

		void Shadow::declare( uint32_t & index
			, uint32_t maxCascades )
		{
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// SHADOWS" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			auto c3d_maxCascadeCount = m_writer.declConstant( cuT( "c3d_maxCascadeCount" )
				, UInt( DirectionalMaxCascadesCount ) );
			auto c3d_mapShadowDirectional = m_writer.declSampler< Sampler2DArray >( MapShadowDirectional, index++, 1u );
			auto c3d_mapShadowSpot = m_writer.declSampler< Sampler2DArray >( MapShadowSpot, index++, 1u );
			auto c3d_mapShadowPoint = m_writer.declSampler< SamplerCubeArray >( MapShadowPoint, index++, 1u );
			doDeclareGetRandom();
			doDeclareTextureProj();
			doDeclareFilterPCF();
			doDeclareTextureProjCascade();
			doDeclareFilterPCFCascade();
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			doDeclareGetLightSpacePosition();
			doDeclareComputeDirectionalShadow();
			doDeclareComputeSpotShadow();
			doDeclareComputePointShadow();
			doDeclareVolumetric();
		}

		void Shadow::declareDirectional( ShadowType type
			, uint32_t & index
			, uint32_t maxCascades )
		{
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// SHADOWS" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			auto c3d_maxCascadeCount = m_writer.declConstant( cuT( "c3d_maxCascadeCount" )
				, UInt( DirectionalMaxCascadesCount ) );
			auto c3d_mapShadowDirectional = m_writer.declSampler< Sampler2DArray >( MapShadowDirectional, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureProjCascade();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureProjCascade();
				doDeclareFilterPCFCascade();
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
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// SHADOWS" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			auto c3d_mapShadowPoint = m_writer.declSampler< SamplerCube >( MapShadowPoint, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureOneProj();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureOneProj();
				doDeclareFilterOnePCF();
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
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			m_writer.inlineComment( cuT( "// SHADOWS" ) );
			m_writer.inlineComment( cuT( "//////////////////////////////////////////////////////////////////////////////" ) );
			auto c3d_mapShadowSpot = m_writer.declSampler< Sampler2D >( MapShadowSpot, index++, 1u );
			doDeclareGetRandom();

			switch ( type )
			{
			case ShadowType::eRaw:
				doDeclareGetShadowOffset();
				doDeclareTextureOneProj();
				break;

			case ShadowType::ePCF:
				doDeclareGetShadowOffset();
				doDeclareTextureOneProj();
				doDeclareFilterOnePCF();
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
			, UInt const & cascadeIndex
			, Vec3 const & normal )
		{
			return m_computeDirectional( shadowType
				, lightMatrix
				, worldSpacePosition
				, lightDirection
				, cascadeIndex
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
			, UInt const & cascadeIndex
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
				, cascadeIndex
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
			, UInt const & cascadeIndex
			, Vec3 const & normal )
		{
			return m_computeOneDirectional( lightMatrix
				, worldSpacePosition
				, lightDirection
				, cascadeIndex
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
			, UInt const & cascadeIndex
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
				, cascadeIndex
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
			, Sampler2DArray const & shadowMap
			, Int const & index
			, Float const & bias )
		{
			return m_textureProj( lightSpacePosition
				, offset
				, shadowMap
				, index
				, bias );
		}

		Float Shadow::filterPCF( Vec4 const & lightSpacePosition
			, Sampler2DArray const & shadowMap
			, Int const & index
			, glsl::Vec2 const & invTexDim
			, Float const & bias )
		{
			return m_filterPCF( lightSpacePosition
				, shadowMap
				, index
				, invTexDim
				, bias );
		}

		Float Shadow::textureProj( Vec4 const & lightSpacePosition
			, Vec2 const & offset
			, Sampler2D const & shadowMap
			, Float const & bias )
		{
			return m_textureOneProj( lightSpacePosition
				, offset
				, shadowMap
				, bias );
		}

		Float Shadow::filterPCF( Vec4 const & lightSpacePosition
			, Sampler2D const & shadowMap
			, glsl::Vec2 const & invTexDim
			, Float const & bias )
		{
			return m_filterOnePCF( lightSpacePosition
				, shadowMap
				, invTexDim
				, bias );
		}

		Float Shadow::textureProjCascade( Vec4 const & lightSpacePosition
			, Vec2 const & offset
			, Sampler2DArray const & shadowMap
			, UInt const & cascadeIndex
			, Float const & bias )
		{
			return m_textureProjCascade( lightSpacePosition
				, offset
				, shadowMap
				, cascadeIndex
				, bias );
		}

		Float Shadow::filterPCFCascade( Vec4 const & lightSpacePosition
			, Sampler2DArray const & shadowMap
			, glsl::Vec2 const & invTexDim
			, UInt const & cascadeIndex
			, Float const & bias )
		{
			return m_filterPCFCascade( lightSpacePosition
				, shadowMap
				, invTexDim
				, cascadeIndex
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
					, Sampler2DArray const & shadowMap
					, Int const & index
					, Float const & bias )
				{
					auto shadow = m_writer.declLocale( cuT( "shadow" )
						, 1.0_f );
					auto shadowCoord = m_writer.declLocale( cuT( "shadowCoord" )
						, lightSpacePosition );

					IF( m_writer, abs( shadowCoord.z() ) < 1.0_f )
					{
						auto uv = m_writer.declLocale( cuT( "uv" )
							, shadowCoord.st() + offset );
						auto dist = m_writer.declLocale( cuT( "dist" )
							, texture( shadowMap, vec3( uv, m_writer.cast< Float >( index ) ) ).r() );

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
				, InSampler2DArray{ &m_writer, cuT( "shadowMap" ) }
				, InInt{ &m_writer, cuT( "index" ) }
				, InFloat{ &m_writer, cuT( "bias" ) } );
		}

		void Shadow::doDeclareFilterPCF()
		{
			m_filterPCF = m_writer.implementFunction< Float >( cuT( "filterPCF" )
				, [this]( Vec4 const & lightSpacePosition
					, Sampler2DArray const & shadowMap
					, Int const & index
					, Vec2 const & invTexDim
					, Float const & bias )
				{
					auto scale = m_writer.declLocale( cuT( "scale" )
						, 1.0_f );
					auto dx = m_writer.declLocale( cuT( "dx" )
						, scale * invTexDim.x() );
					auto dy = m_writer.declLocale( cuT( "dy" )
						, scale * invTexDim.y() );

					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 0.0_f );
					auto count = 0;
					auto const range = 1;

					for ( int x = -range; x <= range; ++x )
					{
						for ( int y = -range; y <= range; ++y )
						{
							shadowFactor += textureProj( lightSpacePosition
								, vec2( dx * x, dy * y )
								, shadowMap
								, index
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / count );
				}
				, InVec4{ &m_writer, cuT( "lightSpacePosition" ) }
				, InSampler2DArray{ &m_writer, cuT( "shadowMap" ) }
				, InInt{ &m_writer, cuT( "index" ) }
				, InVec2{ &m_writer, cuT( "invTexDim" ) }
				, InFloat{ &m_writer, cuT( "bias" ) } );
		}

		void Shadow::doDeclareTextureOneProj()
		{
			m_textureOneProj = m_writer.implementFunction< Float >( cuT( "textureProj" )
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, Sampler2D const & shadowMap
					, Float const & bias )
				{
					auto shadow = m_writer.declLocale( cuT( "shadow" )
						, 1.0_f );
					auto shadowCoord = m_writer.declLocale( cuT( "shadowCoord" )
						, lightSpacePosition );

					IF( m_writer, abs( shadowCoord.z() ) < 1.0_f )
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

		void Shadow::doDeclareFilterOnePCF()
		{
			m_filterOnePCF = m_writer.implementFunction< Float >( cuT( "filterPCF" )
				, [this]( Vec4 const & lightSpacePosition
					, Sampler2D const & shadowMap
					, Vec2 const & invTexDim
					, Float const & bias )
				{
					auto scale = m_writer.declLocale( cuT( "scale" )
						, 1.0_f );
					auto dx = m_writer.declLocale( cuT( "dx" )
						, scale * invTexDim.x() );
					auto dy = m_writer.declLocale( cuT( "dy" )
						, scale * invTexDim.y() );

					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 0.0_f );
					auto count = 0;
					auto const range = 1;

					for ( int x = -range; x <= range; ++x )
					{
						for ( int y = -range; y <= range; ++y )
						{
							shadowFactor += textureProj( lightSpacePosition
								, vec2( dx * x, dy * y )
								, shadowMap
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / count );
				}
				, InVec4{ &m_writer, cuT( "lightSpacePosition" ) }
				, InSampler2D{ &m_writer, cuT( "shadowMap" ) }
				, InVec2{ &m_writer, cuT( "invTexDim" ) }
				, InFloat{ &m_writer, cuT( "bias" ) } );
		}

		void Shadow::doDeclareTextureProjCascade()
		{
			m_textureProjCascade = m_writer.implementFunction< Float >( cuT( "textureProjCascade" )
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, Sampler2DArray const & shadowMap
					, UInt const & cascadeIndex
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
							, texture( shadowMap, vec3( uv
								, m_writer.cast< Float >( cascadeIndex ) ) ).r() );

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
				, InSampler2DArray{ &m_writer, cuT( "shadowMap" ) }
				, InUInt{ &m_writer, cuT( "cascadeIndex" ) }
				, InFloat{ &m_writer, cuT( "bias" ) } );
		}

		void Shadow::doDeclareFilterPCFCascade()
		{
			m_filterPCFCascade = m_writer.implementFunction< Float >( cuT( "filterPCFCascade" )
				, [this]( Vec4 const & lightSpacePosition
					, Sampler2DArray const & shadowMap
					, Vec2 const & invTexDim
					, UInt const & cascadeIndex
					, Float const & bias )
				{
					auto scale = m_writer.declLocale( cuT( "scale" )
						, 1.0_f );
					auto dx = m_writer.declLocale( cuT( "dx" )
						, scale * invTexDim.x() );
					auto dy = m_writer.declLocale( cuT( "dy" )
						, scale * invTexDim.y() );

					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 0.0_f );
					auto count = 0;
					auto const range = 1;

					for ( int x = -range; x <= range; ++x )
					{
						for ( int y = -range; y <= range; ++y )
						{
							shadowFactor += textureProjCascade( lightSpacePosition
								, vec2( dx * x, dy * y )
								, shadowMap
								, cascadeIndex
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / count );
				}
				, InVec4{ &m_writer, cuT( "lightSpacePosition" ) }
				, InSampler2DArray{ &m_writer, cuT( "shadowMap" ) }
				, InVec2{ &m_writer, cuT( "invTexDim" ) }
				, InUInt{ &m_writer, cuT( "cascadeIndex" ) }
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
						, lightSpacePosition );
					//projCoords.x() = glsl::fma( projCoords.x(), 0.5_f, 0.5_f );
					//projCoords.y() = glsl::fma( projCoords.y(), 0.5_f, 0.5_f );

					if ( !m_writer.isZeroToOneDepth() )
					{
						projCoords.z() = glsl::fma( projCoords.z(), 0.5_f, 0.5_f );
					}

					m_writer.returnStmt( projCoords / lightSpacePosition.w() );
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
					, UInt const & cascadeIndex
					, Vec3 const & normal )
				{
					auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2DArray >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );
					auto result = m_writer.declLocale( cuT( "result" )
						, 0.0_f );
					
					IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minPcfDirectionalOffset )
								, Float( maxPcfDirectionalSlopeOffset ) ) );
						result = filterPCFCascade( lightSpacePosition
							, c3d_mapShadowDirectional
							, vec2( Float( 1.0f / float( ShadowMapPassDirectional::TextureSize ) ) )
							, cascadeIndex
							, bias );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowDirectional
								, vec3( lightSpacePosition.xy(), m_writer.cast< Float >( cascadeIndex ) ) ).xy() );
						result = chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceDirectionalMin )
							, Float( varianceDirectionalBias ) );
					}
					ELSE
					{
						auto bias = m_writer.declLocale( cuT( "offset" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minRawDirectionalOffset )
								, Float( maxRawDirectionalSlopeOffset ) ) );
						result = textureProjCascade( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowDirectional
							, cascadeIndex
							, bias );
					}
					FI;
					m_writer.returnStmt( result );
				}
				, InInt( &m_writer, cuT( "shadowType" ) )
				, InMat4( &m_writer, cuT( "lightMatrix" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InUInt( &m_writer, cuT( "cascadeIndex" ) )
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
					auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2DArray >( Shadow::MapShadowSpot );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );
					lightSpacePosition.xy() += vec2( 0.5_f );
					auto result = m_writer.declLocale( cuT( "result" )
						, 0.0_f );

					IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						result = filterPCF( lightSpacePosition
							, c3d_mapShadowSpot
							, index
							, vec2( Float( 1.0f / float( ShadowMapPassSpot::TextureSize ) ) )
							, bias );
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowSpot, vec3( lightSpacePosition.xy(), m_writer.cast< Float >( index ) ) ).xy() );
						result = chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceSpotMin )
							, Float( varianceSpotBias ) );
					}
					ELSE
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
						, getShadowOffset( normal
							, lightDirection
							, Float( minSpotOffset )
							, Float( maxSpotSlopeOffset ) ) );
						result = textureProj( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowSpot
							, index
							, bias );
					}
					FI;
					m_writer.returnStmt( result );
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
					auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCubeArray >( Shadow::MapShadowPoint );
					auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
						, worldSpacePosition - lightPosition );
					auto depth = m_writer.declLocale( cuT( "depth" )
						, length( vertexToLight ) / farPlane );
					auto result = m_writer.declLocale( cuT( "result" )
						, 0.0_f );

					IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
							, 0.0_f );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
							, 0.0_f );
						auto x = m_writer.declLocale( cuT( "x" )
							, -offset );
						auto y = m_writer.declLocale( cuT( "y" )
							, -offset );
						auto z = m_writer.declLocale( cuT( "z" )
							, -offset );
						auto const samples = 4;
						auto inc = m_writer.declLocale( cuT( "inc" )
							, offset / ( samples * 0.5 ) );
						auto shadowMapDepth = m_writer.declLocale< Float >( cuT( "shadowMapDepth" ) );

						for( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									shadowMapDepth = texture( c3d_mapShadowPoint, vec4( vertexToLight + vec3( x, y, z ), m_writer.cast< Float >( index ) ) ).x();
									shadowFactor += step( depth - bias, shadowMapDepth );
									numSamplesUsed += 1.0_f;
									z += inc;
								}

								y += inc;
								z = -offset;
							}

							x += inc;
							y = -offset;
						}

						result = shadowFactor / numSamplesUsed;
					}
					ELSEIF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
							, 0.0_f );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
							, 0.0_f );
						auto x = m_writer.declLocale( cuT( "x" )
							, -offset );
						auto y = m_writer.declLocale( cuT( "y" )
							, -offset );
						auto z = m_writer.declLocale( cuT( "z" )
							, -offset );
						auto const samples = 4;
						auto inc = m_writer.declLocale( cuT( "inc" )
							, offset / ( samples * 0.5 ) );
						auto moments = m_writer.declLocale< Vec2 >( cuT( "moments" ) );

						for ( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									moments = texture( c3d_mapShadowPoint
										, vec4( vertexToLight + vec3( x, y, z ), m_writer.cast< Float >( index ) ) ).xy();
									shadowFactor += chebyshevUpperBound( moments
										, depth
										, Float( variancePointMin )
										, Float( variancePointBias ) );
									numSamplesUsed += 1.0_f;
									z += inc;
								}

								y += inc;
								z = -offset;
							}

							x += inc;
							y = -offset;
						}

						result = shadowFactor / numSamplesUsed;
					}
					ELSE
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto shadowMapDepth = m_writer.declLocale( cuT( "shadowMapDepth" )
							, texture( c3d_mapShadowPoint, vec4( vertexToLight, m_writer.cast< Float >( index ) ) ).x() );
						result = step( depth - bias, shadowMapDepth );
					}
					FI;
					m_writer.returnStmt( result );
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
					, UInt const & cascadeIndex
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
						, uvec2( m_writer.cast< UInt >( gl_FragCoord.x() ), m_writer.cast< UInt >( gl_FragCoord.y() ) ) );
					auto ditherValue = m_writer.declLocale( cuT( "ditherValue" )
						, volumetricDither[screenUV.x() % 4_ui][screenUV.y() % 4_ui] );

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
							* pow( max( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * RdotL, 0.0 ), 1.5_f ) ) );
					auto maxCount = m_writer.declLocale( cuT( "maxCount" )
						, m_writer.cast< Int >( lightVolumetricSteps ) );

					FOR( m_writer, Int, i, 0, "i < maxCount", "++i" )
					{
						IF ( m_writer, m_computeDirectional( shadowType, lightMatrix, currentPosition, lightDirection, cascadeIndex, vec3( 0.0_f ) ) < 0.5_f )
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
				, InUInt{ &m_writer, cuT( "cascadeIndex" ) }
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
					, UInt const & cascadeIndex
					, Vec3 const & normal )
				{
					auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2DArray >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, getLightSpacePosition( lightMatrix, worldSpacePosition ) );

					if ( type == ShadowType::ePCF )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minPcfDirectionalOffset )
								, Float( maxPcfDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( filterPCFCascade( lightSpacePosition
							, c3d_mapShadowDirectional
							, vec2( Float( 1.0f / float( ShadowMapPassDirectional::TextureSize ) ) )
							, cascadeIndex
							, bias ) );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto moments = m_writer.declLocale( cuT( "moments" )
							, texture( c3d_mapShadowDirectional
								, vec3( lightSpacePosition.xy(), m_writer.cast< Float >( cascadeIndex ) ) ).xy() );
						m_writer.returnStmt( chebyshevUpperBound( moments
							, lightSpacePosition.z()
							, Float( varianceDirectionalMin )
							, Float( varianceDirectionalBias ) ) );
					}
					else
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minRawDirectionalOffset )
								, Float( maxRawDirectionalSlopeOffset ) ) );
						m_writer.returnStmt( textureProjCascade( lightSpacePosition
							, vec2( 0.0_f )
							, c3d_mapShadowDirectional
							, cascadeIndex
							, bias ) );
					}
				}
				, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
				, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InUInt( &m_writer, cuT( "cascadeIndex" ) )
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
					lightSpacePosition.xy() += vec2( 0.5_f );

					if ( type == ShadowType::ePCF )
					{
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, lightDirection
								, Float( minSpotOffset )
								, Float( maxSpotSlopeOffset ) ) );
						m_writer.returnStmt( filterPCF( lightSpacePosition
							, c3d_mapShadowSpot
							, vec2( Float( 1.0f / float( ShadowMapPassSpot::TextureSize ) ) )
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
					else
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

					if( type == ShadowType::ePCF )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
							, 0.0_f );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
							, 0.0_f );
						auto bias = m_writer.declLocale( cuT( "bias" )
							, getShadowOffset( normal
								, vertexToLight
								, Float( minPointOffset )
								, Float( maxPointSlopeOffset ) ) );
						auto x = m_writer.declLocale( cuT( "x" )
							, -offset );
						auto y = m_writer.declLocale( cuT( "y" )
							, -offset );
						auto z = m_writer.declLocale( cuT( "z" )
							, -offset );
						auto const samples = 4;
						auto inc = m_writer.declLocale( cuT( "inc" )
							, offset / ( samples * 0.5 ) );
						auto shadowMapDepth = m_writer.declLocale< Float >( cuT( "shadowMapDepth" ) );

						for ( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									shadowMapDepth = texture( c3d_mapShadowPoint, vertexToLight + vec3( x, y, z ) ).x();
									shadowFactor += step( depth - bias, shadowMapDepth );
									numSamplesUsed += 1.0_f;
									z += inc;
								}

								y += inc;
								z = -offset;
							}

							x += inc;
							y = -offset;
						}

						m_writer.returnStmt( shadowFactor / numSamplesUsed );
					}
					else if ( type == ShadowType::eVariance )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
							, 0.0_f );
						auto offset = m_writer.declLocale( cuT( "offset" )
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( cuT( "numSamplesUsed" )
							, 0.0_f );
						auto x = m_writer.declLocale( cuT( "x" )
							, -offset );
						auto y = m_writer.declLocale( cuT( "y" )
							, -offset );
						auto z = m_writer.declLocale( cuT( "z" )
							, -offset );
						auto const samples = 4;
						auto inc = m_writer.declLocale( cuT( "inc" )
							, offset / ( samples * 0.5 ) );
						auto moments = m_writer.declLocale< Vec2 >( cuT( "moments" ) );

						for ( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									moments = texture( c3d_mapShadowPoint
										, vertexToLight + vec3( x, y, z ) ).xy();
									shadowFactor += chebyshevUpperBound( moments
										, depth
										, Float( variancePointMin )
										, Float( variancePointBias ) );
									numSamplesUsed += 1.0_f;
									z += inc;
								}

								y += inc;
								z = -offset;
							}

							x += inc;
							y = -offset;
						}

						m_writer.returnStmt( shadowFactor / numSamplesUsed );
					}
					else
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
					, UInt const & cascadeIndex
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
						, uvec2( m_writer.cast< UInt >( gl_FragCoord.x() ), m_writer.cast< UInt >( gl_FragCoord.y() ) ) );
					auto ditherValue = m_writer.declLocale( cuT( "ditherValue" )
						, volumetricDither[screenUV.x() % 4_ui][screenUV.y() % 4_ui] );

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
							* pow( max( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * RdotL, 0.0f ), 1.5_f ) ) );
					auto maxCount = m_writer.declLocale( cuT( "maxCount" )
						, m_writer.cast< Int >( lightVolumetricSteps ) );

					FOR( m_writer, Int, i, 0, "i < maxCount", "++i" )
					{
						IF ( m_writer, m_computeOneDirectional( lightMatrix, currentPosition, lightDirection, cascadeIndex, vec3( 0.0_f ) ) > 0.5_f )
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
				, InUInt{ &m_writer, cuT( "cascadeIndex" ) }
				, InVec3{ &m_writer, cuT( "lightColour" ) }
				, InVec2{ &m_writer, cuT( "lightIntensity" ) }
				, InUInt{ &m_writer, cuT( "lightVolumetricSteps" ) }
				, InFloat{ &m_writer, cuT( "lightVolumetricScattering" ) }
				, output );
		}
	}
}
