#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		castor::String const Shadow::MapVarianceDirectional = "c3d_mapVarianceDirectional";
		castor::String const Shadow::MapVarianceSpot = "c3d_mapVarianceSpot";
		castor::String const Shadow::MapVariancePoint = "c3d_mapVariancePoint";
		castor::String const Shadow::MapNormalDepthDirectional = "c3d_mapNormalDepthDirectional";
		castor::String const Shadow::MapNormalDepthSpot = "c3d_mapNormalDepthSpot";
		castor::String const Shadow::MapNormalDepthPoint = "c3d_mapNormalDepthPoint";
		castor::String const Shadow::MapPositionDirectional = "c3d_mapPositionDirectional";
		castor::String const Shadow::MapPositionSpot = "c3d_mapPositionSpot";
		castor::String const Shadow::MapPositionPoint = "c3d_mapPositionPoint";
		castor::String const Shadow::MapFluxDirectional = "c3d_mapFluxDirectional";
		castor::String const Shadow::MapFluxSpot = "c3d_mapFluxSpot";
		castor::String const Shadow::MapFluxPoint = "c3d_mapFluxPoint";

		Shadow::Shadow( ShadowOptions shadowOptions
			, ShaderWriter & writer
			, Utils & utils )
			: m_writer{ writer }
			, m_utils{ utils }
			, m_shadowOptions{ std::move( shadowOptions ) }
		{
		}

		void Shadow::declare( uint32_t & index )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// SHADOWS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			auto directionalEnabled = checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowDirectional );
			auto pointEnabled = checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowPoint );
			auto spotEnabled = checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowSpot );

			if ( m_shadowOptions.enabled )
			{
				m_writer.declConstant( "c3d_maxCascadeCount"
					, UInt( DirectionalMaxCascadesCount ) );
				m_writer.declConstantArray( "c3d_volumetricDither"
					, std::vector< Vec4 >{ vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f )
						, vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f )
						, vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f )
						, vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) } );
				m_writer.declSampledImage< FImg2DArrayRgba32 >( MapNormalDepthDirectional
					, ( directionalEnabled ? index++ : index )
					, 1u
					, directionalEnabled );
				m_writer.declSampledImage< FImg2DArrayRg32 >( MapVarianceDirectional
					, ( directionalEnabled ? index++ : index )
					, 1u
					, directionalEnabled );
				m_writer.declSampledImage< FImgCubeArrayRgba32 >( MapNormalDepthPoint
					, ( pointEnabled ? index++ : index )
					, 1u
					, pointEnabled );
				m_writer.declSampledImage< FImgCubeArrayRg32 >( MapVariancePoint
					, ( pointEnabled ? index++ : index )
					, 1u
					, pointEnabled );
				m_writer.declSampledImage< FImg2DArrayRgba32 >( MapNormalDepthSpot
					, ( spotEnabled ? index++ : index )
					, 1u
					, spotEnabled );
				m_writer.declSampledImage< FImg2DArrayRg32 >( MapVarianceSpot
					, ( spotEnabled ? index++ : index )
					, 1u
					, spotEnabled );

				m_utils.declareInvertVec2Y();
				doDeclareGetRandom();
				doDeclareTextureProj();
				doDeclareFilterPCF();
				doDeclareTextureProjCascade();
				doDeclareFilterPCFCascade();
				doDeclareGetShadowOffset();
				doDeclareChebyshevUpperBound();
				doDeclareGetLightSpacePosition();
			}

			doDeclareComputeDirectionalShadow();
			doDeclareComputeSpotShadow();
			doDeclareComputePointShadow();
			doDeclareVolumetric();
		}

		void Shadow::declareDirectional( uint32_t & index )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// SHADOWS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );

			if ( checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowDirectional ) )
			{
				m_writer.declConstant( "c3d_maxCascadeCount"
					, UInt( DirectionalMaxCascadesCount ) );
				m_writer.declConstantArray( "c3d_volumetricDither"
					, std::vector< Vec4 >
				{
					vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f ),
						vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f ),
						vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f ),
						vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ),
				} );
				m_writer.declSampledImage< FImg2DArrayRgba32 >( MapNormalDepthDirectional, index++, 1u );
				m_writer.declSampledImage< FImg2DArrayRg32 >( MapVarianceDirectional, index++, 1u );

				m_utils.declareInvertVec2Y();
				doDeclareGetRandom();
				doDeclareGetShadowOffset();
				doDeclareTextureProjCascade();
				doDeclareFilterPCFCascade();
				doDeclareChebyshevUpperBound();
				doDeclareGetLightSpacePosition();
			}

			doDeclareComputeDirectionalShadow();
			doDeclareVolumetric();
		}

		void Shadow::declarePoint( uint32_t & index )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// SHADOWS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );

			if ( checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowPoint ) )
			{
				m_writer.declSampledImage< FImgCubeArrayRgba32 >( MapNormalDepthPoint, index++, 1u );
				m_writer.declSampledImage< FImgCubeArrayRg32 >( MapVariancePoint, index++, 1u );

				doDeclareGetRandom();
				doDeclareGetShadowOffset();
				doDeclareTextureProj();
				doDeclareFilterPCF();
				doDeclareChebyshevUpperBound();
			}

			doDeclareComputePointShadow();
		}

		void Shadow::declareSpot( uint32_t & index )
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// SHADOWS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );

			if ( checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowSpot ) )
			{
				m_writer.declSampledImage< FImg2DArrayRgba32 >( MapNormalDepthSpot, index++, 1u );
				m_writer.declSampledImage< FImg2DArrayRg32 >( MapVarianceSpot, index++, 1u );

				doDeclareGetRandom();
				doDeclareGetShadowOffset();
				doDeclareTextureProj();
				doDeclareFilterPCF();
				doDeclareChebyshevUpperBound();
				doDeclareGetLightSpacePosition();
			}

			doDeclareComputeSpotShadow();
		}

		Float Shadow::computeDirectional( Int const & shadowType
			, Vec2 const & shadowOffsets
			, Vec2 const & shadowVariance
			, Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, UInt const & cascadeIndex
			, UInt const & maxCascade
			, Vec3 const & normal )const
		{
			return m_computeDirectional( shadowType
				, shadowOffsets
				, shadowVariance
				, lightMatrix
				, worldSpacePosition
				, lightDirection
				, cascadeIndex
				, maxCascade
				, normal );
		}

		Float Shadow::computeSpot( Int const & shadowType
			, Vec2 const & shadowOffsets
			, Vec2 const & shadowVariance
			, Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal
			, Int const & index )const
		{
			return m_computeSpot( shadowType
				, shadowOffsets
				, shadowVariance
				, lightMatrix
				, worldSpacePosition
				, lightDirection
				, normal
				, index );
		}

		Float Shadow::computePoint( Int const & shadowType
			, Vec2 const & shadowOffsets
			, Vec2 const & shadowVariance
			, Vec3 const & worldSpacePosition
			, Vec3 const & lightDirection
			, Vec3 const & normal
			, Float const & farPlane
			, Int const & index )const
		{
			return m_computePoint( shadowType
				, shadowOffsets
				, shadowVariance
				, worldSpacePosition
				, lightDirection
				, normal
				, farPlane
				, index );
		}

		void Shadow::computeVolumetric( Int const & shadowType
			, Vec2 const & shadowOffsets
			, Vec2 const & shadowVariance
			, Vec2 const & clipSpacePosition
			, Vec3 const & worldSpacePosition
			, Vec3 const & eyePosition
			, Mat4 const & lightMatrix
			, Vec3 const & lightDirection
			, UInt const & cascadeIndex
			, UInt const & maxCascade
			, Vec3 const & lightColour
			, Vec2 const & lightIntensity
			, UInt const & lightVolumetricSteps
			, Float const & lightVolumetricScattering
			, OutputComponents & parentOutput )const
		{
			m_computeVolumetric( shadowType
				, shadowOffsets
				, shadowVariance
				, clipSpacePosition
				, worldSpacePosition
				, eyePosition
				, lightMatrix
				, lightDirection
				, cascadeIndex
				, maxCascade
				, lightColour
				, lightIntensity
				, lightVolumetricSteps
				, lightVolumetricScattering
				, parentOutput );
		}

		Vec4 Shadow::getLightSpacePosition( Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition )const
		{
			return m_getLightSpacePosition( lightMatrix
				, worldSpacePosition );
		}

		void Shadow::doDeclareGetRandom()
		{
			m_getRandom = m_writer.implementFunction< Float >( "getRandom"
				, [this]( Vec4 const & seed )
				{
					auto p = m_writer.declLocale( "p"
						, dot( seed, vec4( 12.9898_f, 78.233, 45.164, 94.673 ) ) );
					m_writer.returnStmt( fract( sin( p ) * 43758.5453_f ) );
				}
				, InVec4( m_writer, "seed" ) );
		}

		void Shadow::doDeclareGetShadowOffset()
		{
			m_getShadowOffset = m_writer.implementFunction< Float >( "getShadowOffset"
				, [this]( Vec3 const & normal
					, Vec3 const & lightDirection
					, Float const & minOffset
					, Float const & maxSlopeOffset )
				{
					auto cosAlpha = m_writer.declLocale( "cosAlpha"
						, clamp( dot( normal, normalize( lightDirection ) ), 0.0_f, 1.0_f ) );
					auto offset = m_writer.declLocale( "offset"
						, sqrt( 1.0_f - cosAlpha ) );
					m_writer.returnStmt( minOffset + maxSlopeOffset * offset );
				}
				, InVec3( m_writer, "normal" )
				, InVec3( m_writer, "lightDirection" )
				, InFloat( m_writer, "minOffset" )
				, InFloat( m_writer, "maxSlopeOffset" ) );
		}

		void Shadow::doDeclareChebyshevUpperBound()
		{
			m_chebyshevUpperBound = m_writer.implementFunction< Float >( "chebyshevUpperBound"
				, [this]( Vec2 const & moments
					, Float const & depth
					, Float const & minVariance
					, Float const & varianceBias )
				{
					//auto p = m_writer.declLocale( "p"
					//	, step( moments.x() + varianceBias, depth ) );
					auto p = m_writer.declLocale( "p"
						, step( moments.x(), depth ) );
					auto variance = m_writer.declLocale( "variance"
						, moments.y() - ( moments.x() * moments.x() ) );
					variance = sdw::max( variance, minVariance );
					auto d = m_writer.declLocale( "d"
						, depth - moments.x() );
					variance /= variance + d * d;
					m_writer.returnStmt( m_writer.ternary( p == 0.0_f
						, 1.0_f
						, variance ) );
				}
				, InVec2{ m_writer, "moments" }
				, InFloat{ m_writer, "depth" }
				, InFloat{ m_writer, "minVariance" }
				, InFloat{ m_writer, "varianceBias" } );
		}

		void Shadow::doDeclareTextureProj()
		{
			m_textureProj = m_writer.implementFunction< Float >( "textureProj"
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, SampledImage2DArrayRgba32 const & shadowMap
					, Int const & index
					, Float const & bias )
				{
					auto shadow = m_writer.declLocale( "shadow"
						, 1.0_f );
					auto shadowCoord = m_writer.declLocale( "shadowCoord"
						, lightSpacePosition );

					IF( m_writer, abs( shadowCoord.z() ) < 1.0_f )
					{
						auto uv = m_writer.declLocale( "uv"
							, shadowCoord.st() + offset );
						auto dist = m_writer.declLocale( "dist"
							, shadowMap.sample( vec3( uv, m_writer.cast< Float >( index ) ) ) );

						IF( m_writer, shadowCoord.w() > 0.0_f )
						{
							shadow = step( shadowCoord.z() - bias, dist.w() );
						}
						FI;
					}
					FI;

					m_writer.returnStmt( shadow );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InVec2{ m_writer, "offset" }
				, InSampledImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InInt{ m_writer, "index" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareFilterPCF()
		{
			m_filterPCF = m_writer.implementFunction< Float >( "filterPCF"
				, [this]( Vec4 const & lightSpacePosition
					, SampledImage2DArrayRgba32 const & shadowMap
					, Int const & index
					, Vec2 const & invTexDim
					, Float const & bias )
				{
					auto scale = m_writer.declLocale( "scale"
						, 1.0_f );
					auto dx = m_writer.declLocale( "dx"
						, scale * invTexDim.x() );
					auto dy = m_writer.declLocale( "dy"
						, scale * invTexDim.y() );

					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 0.0_f );
					auto count = 0;
					auto const range = 1;

					for ( int x = -range; x <= range; ++x )
					{
						for ( int y = -range; y <= range; ++y )
						{
							shadowFactor += m_textureProj( lightSpacePosition
								, vec2( dx * float( x ), dy * float( y ) )
								, shadowMap
								, index
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / float( count ) );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InSampledImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InInt{ m_writer, "index" }
				, InVec2{ m_writer, "invTexDim" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareTextureProjCascade()
		{
			m_textureProjCascade = m_writer.implementFunction< Float >( "textureProjCascade"
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, SampledImage2DArrayRgba32 const & shadowMap
					, UInt const & cascadeIndex
					, Float const & bias )
				{
					auto shadow = m_writer.declLocale( "shadow"
						, 1.0_f );
					auto shadowCoord = m_writer.declLocale( "shadowCoord"
						, lightSpacePosition );

					IF( m_writer, shadowCoord.z() > -1.0_f && shadowCoord.z() < 1.0_f )
					{
						auto uv = m_writer.declLocale( "uv"
							, shadowCoord.st() + offset );
						auto dist = m_writer.declLocale( "dist"
							, shadowMap.sample( vec3( uv, m_writer.cast< Float >( cascadeIndex ) ) ) );

						IF( m_writer, shadowCoord.w() > 0 )
						{
							shadow = step( shadowCoord.z() - bias, dist.w() );
						}
						FI;
					}
					FI;

					m_writer.returnStmt( shadow );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InVec2{ m_writer, "offset" }
				, InSampledImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareFilterPCFCascade()
		{
			m_filterPCFCascade = m_writer.implementFunction< Float >( "filterPCFCascade"
				, [this]( Vec4 const & lightSpacePosition
					, SampledImage2DArrayRgba32 const & shadowMap
					, Vec2 const & invTexDim
					, UInt const & cascadeIndex
					, Float const & bias )
				{
					auto scale = m_writer.declLocale( "scale"
						, 1.0_f );
					auto dx = m_writer.declLocale( "dx"
						, scale * invTexDim.x() );
					auto dy = m_writer.declLocale( "dy"
						, scale * invTexDim.y() );

					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 0.0_f );
					auto count = 0;
					auto const range = 1;

					for ( int x = -range; x <= range; ++x )
					{
						for ( int y = -range; y <= range; ++y )
						{
							shadowFactor += m_textureProjCascade( lightSpacePosition
								, vec2( dx * float( x ), dy * float( y ) )
								, shadowMap
								, cascadeIndex
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / float( count ) );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InSampledImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InVec2{ m_writer, "invTexDim" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareGetLightSpacePosition()
		{
			m_getLightSpacePosition = m_writer.implementFunction< Vec4 >( "getLightSpacePosition"
				, [this]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition )
				{
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, lightMatrix * vec4( worldSpacePosition, 1.0_f ) );
					lightSpacePosition.x() = sdw::fma( lightSpacePosition.x(), 0.5_f, 0.5_f );
					lightSpacePosition.y() = sdw::fma( lightSpacePosition.y(), 0.5_f, 0.5_f );
					m_writer.returnStmt( lightSpacePosition / lightSpacePosition.w() );
				}
				, InMat4( m_writer, "lightMatrix" )
				, InVec3( m_writer, "worldSpacePosition" ) );
		}

		void Shadow::doDeclareComputeDirectionalShadow()
		{
			m_computeDirectional = m_writer.implementFunction< Float >( "computeDirectionalShadow"
				, [this]( Int const & shadowType
					, Vec2 const & shadowOffsets
					, Vec2 const & shadowVariance
					, Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, UInt cascadeIndex
					, UInt const & maxCascade
					, Vec3 const & normal )
				{
					if ( checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowDirectional ) )
					{
						auto c3d_mapNormalDepthDirectional = m_writer.getVariable< SampledImage2DArrayRgba32 >( Shadow::MapNormalDepthDirectional );
						auto c3d_mapVarianceDirectional = m_writer.getVariable< SampledImage2DArrayRg32 >( Shadow::MapVarianceDirectional );
						auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
							, getLightSpacePosition( lightMatrix, worldSpacePosition ) );
						auto result = m_writer.declLocale( "result"
							, 1.0_f );

						IF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
						{
							auto moments = m_writer.declLocale( "moments"
								, c3d_mapVarianceDirectional
									.lod( vec3( lightSpacePosition.xy()
										, m_writer.cast< Float >( cascadeIndex ) )
									, 0.0_f ) );
							result = m_chebyshevUpperBound( moments
								, lightSpacePosition.z()
								, shadowVariance.x()
								, shadowVariance.y() );
						}
						ELSE
						{
							auto bias = m_writer.declLocale( "bias"
								, m_getShadowOffset( normal
									, lightDirection
									, shadowOffsets.x()
									, shadowOffsets.y() ) );

							IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
							{
								result = m_filterPCFCascade( lightSpacePosition
									, c3d_mapNormalDepthDirectional
									, vec2( Float( 1.0f / float( ShadowMapPassDirectional::TextureSize ) ) )
									, cascadeIndex
									, bias );
							}
							ELSE
							{
								result = m_textureProjCascade( lightSpacePosition
									, vec2( 0.0_f )
									, c3d_mapNormalDepthDirectional
									, cascadeIndex
									, bias );
							}
							FI;
						}
						FI;

						m_writer.returnStmt( result );
					}
					else
					{
						m_writer.returnStmt( 1.0_f );
					}
				}
				, InInt{ m_writer, "shadowType" }
				, InVec2{ m_writer, "shadowOffsets" }
				, InVec2{ m_writer, "shadowVariance" }
				, InMat4{ m_writer, "lightMatrix" }
				, InVec3{ m_writer, "worldSpacePosition" }
				, InVec3{ m_writer, "lightDirection" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InUInt{ m_writer, "maxCascade" }
				, InVec3{ m_writer, "normal" } );
		}

		void Shadow::doDeclareComputeSpotShadow()
		{
			m_computeSpot = m_writer.implementFunction< Float >( "computeSpotShadow"
				, [this]( Int const & shadowType
					, Vec2 const & shadowOffsets
					, Vec2 const & shadowVariance
					, Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightDirection
					, Vec3 const & normal
					, Int const & shadowMapIndex )
				{
					if ( checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowSpot ) )
					{
						auto c3d_mapNormalDepthSpot = m_writer.getVariable< SampledImage2DArrayRgba32 >( Shadow::MapNormalDepthSpot );
						auto c3d_mapVarianceSpot = m_writer.getVariable< SampledImage2DArrayRg32 >( Shadow::MapVarianceSpot );
						auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
							, getLightSpacePosition( lightMatrix, worldSpacePosition ) );
						lightSpacePosition.xy() += vec2( 0.5_f );
						auto result = m_writer.declLocale( "result"
							, 0.0_f );

						IF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
						{
							auto moments = m_writer.declLocale( "moments"
								, c3d_mapVarianceSpot
									.lod( vec3( lightSpacePosition.xy()
										, m_writer.cast< Float >( shadowMapIndex ) )
									, 0.0_f ) );
							result = m_chebyshevUpperBound( moments
								, lightSpacePosition.z()
								, shadowVariance.x()
								, shadowVariance.y() );
						}
						ELSE
						{
							auto bias = m_writer.declLocale( "bias"
								, m_getShadowOffset( normal
									, lightDirection
									, shadowOffsets.x()
									, shadowOffsets.y() ) );

							IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
							{
								result = m_filterPCF( lightSpacePosition
									, c3d_mapNormalDepthSpot
									, shadowMapIndex
									, vec2( Float( 1.0f / float( ShadowMapPassSpot::TextureSize ) ) )
									, bias );
							}
							ELSE
							{
	#if C3D_DebugSpotShadows

							result = 10.0_f * texture( c3d_mapDepthSpot
								, vec3( lightSpacePosition.xy(), shadowMapIndex ) );
	#else

							result = m_textureProj( lightSpacePosition
								, vec2( 0.0_f )
								, c3d_mapNormalDepthSpot
								, shadowMapIndex
								, bias );

	#endif
							}
							FI;
						}
						FI;

						m_writer.returnStmt( result );
					}
					else
					{
						m_writer.returnStmt( 1.0_f );
					}
				}
				, InInt{ m_writer, "shadowType" }
				, InVec2{ m_writer, "shadowOffsets" }
				, InVec2{ m_writer, "shadowVariance" }
				, InMat4{ m_writer, "lightMatrix" }
				, InVec3{ m_writer, "worldSpacePosition" }
				, InVec3{ m_writer, "lightDirection" }
				, InVec3{ m_writer, "normal" }
				, InInt{ m_writer, "shadowMapIndex" } );
		}

		void Shadow::doDeclareComputePointShadow()
		{
			m_computePoint = m_writer.implementFunction< Float >( "computePointShadow"
				, [this]( Int const & shadowType
					, Vec2 const & shadowOffsets
					, Vec2 const & shadowVariance
					, Vec3 const & worldSpacePosition
					, Vec3 const & lightPosition
					, Vec3 const & normal
					, Float const & farPlane
					, Int const & shadowMapIndex )
				{
					if ( checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowPoint ) )
					{
						auto c3d_mapNormalDepthPoint = m_writer.getVariable< SampledImageCubeArrayRgba32 >( Shadow::MapNormalDepthPoint );
						auto c3d_mapVariancePoint = m_writer.getVariable< SampledImageCubeArrayRg32 >( Shadow::MapVariancePoint );
						auto vertexToLight = m_writer.declLocale( "vertexToLight"
							, worldSpacePosition - lightPosition );
						auto depth = m_writer.declLocale( "depth"
							, length( vertexToLight ) / farPlane );
						auto result = m_writer.declLocale( "result"
							, 0.0_f );

						IF( m_writer, shadowType == Int( int( ShadowType::eVariance ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 0.0_f );
							auto offset = m_writer.declLocale( "offset"
								, 20.0_f * depth );
							auto numSamplesUsed = m_writer.declLocale( "numSamplesUsed"
								, 0.0_f );
							auto x = m_writer.declLocale( "x"
								, -offset );
							auto y = m_writer.declLocale( "y"
								, -offset );
							auto z = m_writer.declLocale( "z"
								, -offset );
							auto const samples = 4;
							auto inc = m_writer.declLocale( "inc"
								, offset / ( samples * 0.5f ) );
							auto moments = m_writer.declLocale< Vec2 >( "moments" );

							for ( int i = 0; i < samples; ++i )
							{
								for ( int j = 0; j < samples; ++j )
								{
									for ( int k = 0; k < samples; ++k )
									{
										moments = c3d_mapVariancePoint
											.lod( vec4( vertexToLight + vec3( x, y, z )
												, m_writer.cast< Float >( shadowMapIndex ) )
											, 0.0_f );
										shadowFactor += m_chebyshevUpperBound( moments
											, depth
											, shadowVariance.x()
											, shadowVariance.y() );
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
							auto bias = m_writer.declLocale( "bias"
								, m_getShadowOffset( normal
									, vertexToLight
									, shadowOffsets.x()
									, shadowOffsets.y() ) );

							IF( m_writer, shadowType == Int( int( ShadowType::ePCF ) ) )
							{
								auto shadowFactor = m_writer.declLocale( "shadowFactor"
									, 0.0_f );
								auto offset = m_writer.declLocale( "offset"
									, 20.0_f * depth );
								auto numSamplesUsed = m_writer.declLocale( "numSamplesUsed"
									, 0.0_f );
								auto x = m_writer.declLocale( "x"
									, -offset );
								auto y = m_writer.declLocale( "y"
									, -offset );
								auto z = m_writer.declLocale( "z"
									, -offset );
								auto const samples = 4;
								auto inc = m_writer.declLocale( "inc"
									, offset / ( samples * 0.5f ) );
								auto shadowMapDepth = m_writer.declLocale< Float >( "shadowMapDepth" );

								for( int i = 0; i < samples; ++i )
								{
									for ( int j = 0; j < samples; ++j )
									{
										for ( int k = 0; k < samples; ++k )
										{
											shadowMapDepth = c3d_mapNormalDepthPoint.sample( vec4( vertexToLight + vec3( x, y, z )
													, m_writer.cast< Float >( shadowMapIndex ) ) ).w();
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
							ELSE
							{
								auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
									, c3d_mapNormalDepthPoint.sample( vec4( vertexToLight
										, m_writer.cast< Float >( shadowMapIndex ) ) ).w() );
								result = step( depth - bias, shadowMapDepth );
							}
							FI;
						}
						FI;

						m_writer.returnStmt( result );
					}
					else
					{
						m_writer.returnStmt( 1.0_f );
					}
				}
				, InInt( m_writer, "shadowType" )
				, InVec2( m_writer, "shadowOffsets" )
				, InVec2( m_writer, "shadowVariance" )
				, InVec3( m_writer, "worldSpacePosition" )
				, InVec3( m_writer, "lightPosition" )
				, InVec3( m_writer, "normal" )
				, InFloat( m_writer, "farPlane" )
				, InInt( m_writer, "shadowMapIndex" ) );
		}

		void Shadow::doDeclareVolumetric()
		{
			OutputComponents output{ m_writer };
			m_computeVolumetric = m_writer.implementFunction< sdw::Void >( "computeVolumetric"
				, [this]( Int const & shadowType
					, Vec2 const & shadowOffsets
					, Vec2 const & shadowVariance
					, Vec2 const & clipSpacePosition
					, Vec3 const & worldSpacePosition
					, Vec3 const & eyePosition
					, Mat4 const & lightMatrix
					, Vec3 const & lightDirection
					, UInt const & cascadeIndex
					, UInt const & maxCascade
					, Vec3 const & lightColour
					, Vec2 const & lightIntensity
					, UInt const & lightVolumetricSteps
					, Float const & lightVolumetricScattering
					, OutputComponents & parentOutput )
				{
					if ( checkFlag( m_shadowOptions.enabled, SceneFlag::eShadowDirectional ) )
					{
						auto c3d_volumetricDither = m_writer.getVariableArray< Vec4 >( "c3d_volumetricDither" );

						auto rayVector = m_writer.declLocale( "rayVector"
							, worldSpacePosition - eyePosition );
						auto rayLength = m_writer.declLocale( "rayLength"
							, length( rayVector ) );
						auto rayDirection = m_writer.declLocale( "rayDirection"
							, rayVector / rayLength );
						auto stepLength = m_writer.declLocale( "stepLength"
							, rayLength / m_writer.cast< Float >( lightVolumetricSteps ) );
						auto step = m_writer.declLocale( "step"
							, rayDirection * stepLength );
						auto screenUV = m_writer.declLocale( "screenUV"
							, uvec2( m_writer.cast< UInt >( clipSpacePosition.x() )
								, m_writer.cast< UInt >( clipSpacePosition.y() ) ) );
						auto ditherValue = m_writer.declLocale( "ditherValue"
							, c3d_volumetricDither[screenUV.x() % 4_u][screenUV.y() % 4_u] );

						auto currentPosition = m_writer.declLocale( "currentPosition"
							, eyePosition + step * ditherValue );
						auto volumetric = m_writer.declLocale( "volumetric"
							, 0.0_f );

						auto RdotL = m_writer.declLocale( "RdotL"
							, dot( rayDirection, lightDirection ) );
						auto sqVolumetricScattering = m_writer.declLocale( "sqVolumetricScattering"
							, lightVolumetricScattering * lightVolumetricScattering );
						auto dblVolumetricScattering = m_writer.declLocale( "dblVolumetricScattering"
							, 2.0_f * lightVolumetricScattering );
						auto oneMinusVolumeScattering = m_writer.declLocale( "oneMinusVolumeScattering"
							, 1.0_f - sqVolumetricScattering );
						auto scattering = m_writer.declLocale( "scattering"
							, oneMinusVolumeScattering / ( 4.0_f
								* Float{ Pi< float > }
						*pow( max( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * RdotL, 0.0_f ), 1.5_f ) ) );
						auto maxCount = m_writer.declLocale( "maxCount"
							, m_writer.cast< Int >( lightVolumetricSteps ) );

						FOR( m_writer, Int, i, 0, i < maxCount, ++i )
						{
							IF( m_writer
								, computeDirectional( shadowType
									, shadowOffsets
									, shadowVariance
									, lightMatrix
									, currentPosition
									, lightDirection
									, cascadeIndex
									, maxCascade
									, vec3( 0.0_f ) ) > 0.5_f )
							{
								volumetric += scattering;
							}
							FI;

							currentPosition += step;
						}
						ROF;

						volumetric /= m_writer.cast< Float >( lightVolumetricSteps );
						parentOutput.m_diffuse += volumetric * lightIntensity.x() * 1.0_f * lightColour;
						parentOutput.m_specular += volumetric * lightIntensity.y() * 1.0_f * lightColour;
					}
				}
				, InInt{ m_writer, "shadowType" }
				, InVec2( m_writer, "shadowOffsets" )
				, InVec2( m_writer, "shadowVariance" )
				, InVec2{ m_writer, "clipSpacePosition" }
				, InVec3{ m_writer, "worldSpacePosition" }
				, InVec3{ m_writer, "eyePosition" }
				, InMat4{ m_writer, "lightMatrix" }
				, InVec3{ m_writer, "lightDirection" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InUInt{ m_writer, "maxCascade" }
				, InVec3{ m_writer, "lightColour" }
				, InVec2{ m_writer, "lightIntensity" }
				, InUInt{ m_writer, "lightVolumetricSteps" }
				, InFloat{ m_writer, "lightVolumetricScattering" }
				, output );
		}
	}
}
