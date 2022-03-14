#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		castor::String const Shadow::MapVarianceDirectional = "c3d_shdMapVarianceDirectional";
		castor::String const Shadow::MapVarianceSpot = "c3d_shdMapVarianceSpot";
		castor::String const Shadow::MapVariancePoint = "c3d_shdMapVariancePoint";
		castor::String const Shadow::MapNormalDepthDirectional = "c3d_shdMapNormalDepthDirectional";
		castor::String const Shadow::MapNormalDepthSpot = "c3d_shdMapNormalDepthSpot";
		castor::String const Shadow::MapNormalDepthPoint = "c3d_shdMapNormalDepthPoint";
		castor::String const Shadow::MapPositionDirectional = "c3d_shdMapPositionDirectional";
		castor::String const Shadow::MapPositionSpot = "c3d_shdMapPositionSpot";
		castor::String const Shadow::MapPositionPoint = "c3d_shdMapPositionPoint";
		castor::String const Shadow::MapFluxDirectional = "c3d_shdMapFluxDirectional";
		castor::String const Shadow::MapFluxSpot = "c3d_shdMapFluxSpot";
		castor::String const Shadow::MapFluxPoint = "c3d_shdMapFluxPoint";

		Shadow::Shadow( ShadowOptions shadowOptions
			, ShaderWriter & writer )
			: m_writer{ writer }
			, m_shadowOptions{ std::move( shadowOptions ) }
		{
		}

		void Shadow::declare( uint32_t & index
			, uint32_t set )
		{
			auto directionalEnabled = checkFlag( m_shadowOptions.type, SceneFlag::eShadowDirectional );
			auto pointEnabled = checkFlag( m_shadowOptions.type, SceneFlag::eShadowPoint );
			auto spotEnabled = checkFlag( m_shadowOptions.type, SceneFlag::eShadowSpot );

			if ( m_shadowOptions.type )
			{
				m_writer.declConstant( "c3d_maxCascadeCount"
					, UInt( DirectionalMaxCascadesCount ) );
				m_writer.declConstantArray( "c3d_volumetricDither"
					, std::vector< Vec4 >{ vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f )
						, vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f )
						, vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f )
						, vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) } );

				if constexpr ( DirectionalMaxCascadesCount > 1u )
				{
					m_writer.declCombinedImg< FImg2DArrayRgba32 >( MapNormalDepthDirectional
						, ( directionalEnabled ? index++ : index )
						, set
						, directionalEnabled );
					m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceDirectional
						, ( directionalEnabled ? index++ : index )
						, set
						, directionalEnabled );
				}
				else
				{
					m_writer.declCombinedImg< FImg2DRgba32 >( MapNormalDepthDirectional
						, ( directionalEnabled ? index++ : index )
						, set
						, directionalEnabled );
					m_writer.declCombinedImg< FImg2DRg32 >( MapVarianceDirectional
						, ( directionalEnabled ? index++ : index )
						, set
						, directionalEnabled );
				}

				m_writer.declCombinedImg< FImgCubeArrayRgba32 >( MapNormalDepthPoint
					, ( pointEnabled ? index++ : index )
					, set
					, pointEnabled );
				m_writer.declCombinedImg< FImgCubeArrayRg32 >( MapVariancePoint
					, ( pointEnabled ? index++ : index )
					, set
					, pointEnabled );
				m_writer.declCombinedImg< FImg2DArrayRgba32 >( MapNormalDepthSpot
					, ( spotEnabled ? index++ : index )
					, set
					, spotEnabled );
				m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceSpot
					, ( spotEnabled ? index++ : index )
					, set
					, spotEnabled );
			}
		}

		void Shadow::declareDirectional( uint32_t & index
			, uint32_t set )
		{
			if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowDirectional ) )
			{
				m_writer.declConstant( "c3d_maxCascadeCount"
					, UInt( DirectionalMaxCascadesCount ) );
				m_writer.declConstantArray( "c3d_volumetricDither"
					, std::vector< Vec4 >{ vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f )
						, vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f )
						, vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f )
						, vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) } );

				if constexpr ( DirectionalMaxCascadesCount > 1u )
				{
					m_writer.declCombinedImg< FImg2DArrayRgba32 >( MapNormalDepthDirectional
						, index++
						, set );
					m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceDirectional
						, index++
						, set );
				}
				else
				{
					m_writer.declCombinedImg< FImg2DRgba32 >( MapNormalDepthDirectional
						, index++
						, set );
					m_writer.declCombinedImg< FImg2DRg32 >( MapVarianceDirectional
						, index++
						, set );
				}
			}
		}

		void Shadow::declarePoint( uint32_t & index
			, uint32_t set )
		{
			if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowPoint ) )
			{
				m_writer.declCombinedImg< FImgCubeArrayRgba32 >( MapNormalDepthPoint
					, index++
					, set );
				m_writer.declCombinedImg< FImgCubeArrayRg32 >( MapVariancePoint
					, index++
					, set );
			}
		}

		void Shadow::declareSpot( uint32_t & index
			, uint32_t set )
		{
			if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowSpot ) )
			{
				m_writer.declCombinedImg< FImg2DArrayRgba32 >( MapNormalDepthSpot
					, index++
					, set );
				m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceSpot
					, index++
					, set );
			}
		}

		Float Shadow::computeDirectional( shader::Light const & light
			, Surface const & surface
			, Mat4 const & lightMatrix
			, Vec3 const & lightDirection
			, UInt const & cascadeIndex
			, UInt const & maxCascade )
		{
			doDeclareComputeDirectionalShadow();
			return m_computeDirectional( light
				, surface
				, lightMatrix
				, lightDirection
				, cascadeIndex
				, maxCascade );
		}

		Float Shadow::computeSpot( shader::Light const & light
			, Surface const & surface
			, Mat4 const & lightMatrix
			, Vec3 const & lightDirection )
		{
			doDeclareComputeSpotShadow();
			return m_computeSpot( light
				, surface
				, lightMatrix
				, lightDirection );
		}

		Float Shadow::computePoint( shader::Light const & light
			, Surface const & surface
			, Vec3 const & lightDirection )
		{
			doDeclareComputePointShadow();
			return m_computePoint( light
				, surface
				, lightDirection );
		}

		void Shadow::computeVolumetric( shader::Light const & light
			, Surface const & surface
			, Vec3 const & eyePosition
			, Mat4 const & lightMatrix
			, Vec3 const & lightDirection
			, UInt const & cascadeIndex
			, UInt const & maxCascade
			, OutputComponents & parentOutput )
		{
			doDeclareComputeVolumetric();
			m_computeVolumetric( light
				, surface
				, eyePosition
				, lightMatrix
				, lightDirection
				, cascadeIndex
				, maxCascade
				, parentOutput );
		}

		Vec4 Shadow::getLightSpacePosition( Mat4 const & lightMatrix
			, Vec3 const & worldSpacePosition )
		{
			doDeclareGetLightSpacePosition();
			return m_getLightSpacePosition( lightMatrix
				, worldSpacePosition );
		}

		void Shadow::doDeclareGetRandom()
		{
			if ( m_getRandom )
			{
				return;
			}

			m_getRandom = m_writer.implementFunction< Float >( "c3d_shdGetRandom"
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
			if ( m_getShadowOffset )
			{
				return;
			}

			m_getShadowOffset = m_writer.implementFunction< Float >( "c3d_shdGetShadowOffset"
				, [this]( Vec3 const & normal
					, Vec3 const & lightDirection
					, Float const & minOffset
					, Float const & maxSlopeOffset )
				{
					auto cosAlpha = m_writer.declLocale( "cosAlpha"
						, clamp( dot( normal, lightDirection ), 0.0_f, 1.0_f ) );
					auto slope = m_writer.declLocale( "slope"
						, sqrt( 1.0_f - cosAlpha ) );
					m_writer.returnStmt( minOffset + maxSlopeOffset * slope );
				}
				, InVec3( m_writer, "normal" )
				, InVec3( m_writer, "lightDirection" )
				, InFloat( m_writer, "minOffset" )
				, InFloat( m_writer, "maxSlopeOffset" ) );
		}

		void Shadow::doDeclareChebyshevUpperBound()
		{
			if ( m_chebyshevUpperBound )
			{
				return;
			}

			m_chebyshevUpperBound = m_writer.implementFunction< Float >( "c3d_shdChebyshevUpperBound"
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
			if ( m_textureProj )
			{
				return;
			}

			m_textureProj = m_writer.implementFunction< Float >( "c3d_shdTextureProj"
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, CombinedImage2DArrayRgba32 const & shadowMap
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
				, InCombinedImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InInt{ m_writer, "index" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareFilterPCF()
		{
			if ( m_filterPCF )
			{
				return;
			}

			doDeclareTextureProj();
			m_filterPCF = m_writer.implementFunction< Float >( "c3d_shdFilterPCF"
				, [this]( Vec4 const & lightSpacePosition
					, CombinedImage2DArrayRgba32 const & shadowMap
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
				, InCombinedImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InInt{ m_writer, "index" }
				, InVec2{ m_writer, "invTexDim" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareTextureProjNoCascade()
		{
			if ( m_textureProjNoCascade )
			{
				return;
			}

			m_textureProjNoCascade = m_writer.implementFunction< Float >( "c3d_shdTextureProjNoCascade"
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, CombinedImage2DRgba32 const & shadowMap
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
							, shadowMap.sample( uv ) );

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
				, InCombinedImage2DRgba32{ m_writer, "shadowMap" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareFilterPCFNoCascade()
		{
			if ( m_filterPCFNoCascade )
			{
				return;
			}

			doDeclareTextureProjNoCascade();
			m_filterPCFNoCascade = m_writer.implementFunction< Float >( "c3d_shdFilterPCFNoCascade"
				, [this]( Vec4 const & lightSpacePosition
					, CombinedImage2DRgba32 const & shadowMap
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
							shadowFactor += m_textureProjNoCascade( lightSpacePosition
								, vec2( dx * float( x ), dy * float( y ) )
								, shadowMap
								, bias );
							++count;
						}
					}

					m_writer.returnStmt( shadowFactor / float( count ) );
				}
				, InVec4{ m_writer, "lightSpacePosition" }
				, InCombinedImage2DRgba32{ m_writer, "shadowMap" }
				, InVec2{ m_writer, "invTexDim" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareTextureProjCascade()
		{
			if ( m_textureProjCascade )
			{
				return;
			}

			m_textureProjCascade = m_writer.implementFunction< Float >( "c3d_shdTextureProjCascade"
				, [this]( Vec4 const & lightSpacePosition
					, Vec2 const & offset
					, CombinedImage2DArrayRgba32 const & shadowMap
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
				, InCombinedImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareFilterPCFCascade()
		{
			if ( m_filterPCFCascade )
			{
				return;
			}

			doDeclareTextureProjCascade();
			m_filterPCFCascade = m_writer.implementFunction< Float >( "c3d_shdFilterPCFCascade"
				, [this]( Vec4 const & lightSpacePosition
					, CombinedImage2DArrayRgba32 const & shadowMap
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
				, InCombinedImage2DArrayRgba32{ m_writer, "shadowMap" }
				, InVec2{ m_writer, "invTexDim" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InFloat{ m_writer, "bias" } );
		}

		void Shadow::doDeclareGetLightSpacePosition()
		{
			m_getLightSpacePosition = m_writer.implementFunction< Vec4 >( "c3d_shdGetLightSpacePosition"
				, [this]( Mat4 const & lightMatrix
					, Vec3 const & worldSpacePosition )
				{
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, lightMatrix * vec4( worldSpacePosition, 1.0_f ) );
					lightSpacePosition.xyz() /= lightSpacePosition.w();
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );
					m_writer.returnStmt( lightSpacePosition );
				}
				, InMat4( m_writer, "lightMatrix" )
				, InVec3( m_writer, "worldSpacePosition" ) );
		}

		void Shadow::doDeclareComputeDirectionalShadow()
		{
			doDeclareTextureProjNoCascade();
			doDeclareFilterPCFNoCascade();
			doDeclareTextureProjCascade();
			doDeclareFilterPCFCascade();
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			m_computeDirectional = m_writer.implementFunction< Float >( "c3d_shdComputeDirectional"
				, [this]( shader::Light const & light
					, Surface const & surface
					, Mat4 const & lightMatrix
					, Vec3 const & lightToVertex
					, UInt cascadeIndex
					, UInt const & maxCascade )
				{
					if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowDirectional ) )
					{
						auto result = m_writer.declLocale( "result"
							, 1.0_f );

						if constexpr ( DirectionalMaxCascadesCount > 1u )
						{
							auto c3d_mapNormalDepthDirectional = m_writer.getVariable< CombinedImage2DArrayRgba32 >( Shadow::MapNormalDepthDirectional );
							auto c3d_mapVarianceDirectional = m_writer.getVariable< CombinedImage2DArrayRg32 >( Shadow::MapVarianceDirectional );
							auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
								, getLightSpacePosition( lightMatrix, surface.worldPosition ) );

							IF( m_writer, light.shadowType == Int( int( ShadowType::eVariance ) ) )
							{
								auto moments = m_writer.declLocale( "moments"
									, c3d_mapVarianceDirectional
										.lod( vec3( lightSpacePosition.xy()
											, m_writer.cast< Float >( cascadeIndex ) )
										, 0.0_f ) );
								result = m_chebyshevUpperBound( moments
									, lightSpacePosition.z()
									, light.vsmShadowVariance.x()
									, light.vsmShadowVariance.y() );
							}
							ELSE
							{
								IF( m_writer, light.shadowType == Int( int( ShadowType::ePCF ) ) )
								{
									auto bias = m_writer.declLocale( "bias"
										, m_getShadowOffset( surface.worldNormal
											, normalize( lightToVertex )
											, light.pcfShadowOffsets.x()
											, light.pcfShadowOffsets.y() ) );
									result = m_filterPCFCascade( lightSpacePosition
										, c3d_mapNormalDepthDirectional
										, vec2( Float( 1.0f / float( ShadowMapPassDirectional::TileSize ) ) )
										, cascadeIndex
										, bias );
								}
								ELSE
								{
									auto bias = m_writer.declLocale( "bias"
										, m_getShadowOffset( surface.worldNormal
											, normalize( lightToVertex )
											, light.rawShadowOffsets.x()
											, light.rawShadowOffsets.y() ) );
									result = m_textureProjCascade( lightSpacePosition
										, vec2( 0.0_f )
										, c3d_mapNormalDepthDirectional
										, cascadeIndex
										, bias );
								}
								FI;
							}
							FI;
						}
						else
						{
							auto c3d_mapNormalDepthDirectional = m_writer.getVariable< CombinedImage2DRgba32 >( Shadow::MapNormalDepthDirectional );
							auto c3d_mapVarianceDirectional = m_writer.getVariable< CombinedImage2DRg32 >( Shadow::MapVarianceDirectional );
							auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
								, getLightSpacePosition( lightMatrix, surface.worldPosition ) );

							IF( m_writer, light.shadowType == Int( int( ShadowType::eVariance ) ) )
							{
								auto moments = m_writer.declLocale( "moments"
									, c3d_mapVarianceDirectional
										.lod( lightSpacePosition.xy(), 0.0_f ) );
								result = m_chebyshevUpperBound( moments
									, lightSpacePosition.z()
									, light.vsmShadowVariance.x()
									, light.vsmShadowVariance.y() );
							}
							ELSE
							{
								IF( m_writer, light.shadowType == Int( int( ShadowType::ePCF ) ) )
								{
									auto bias = m_writer.declLocale( "bias"
										, m_getShadowOffset( surface.worldNormal
											, normalize( lightToVertex )
											, light.pcfShadowOffsets.x()
											, light.pcfShadowOffsets.y() ) );
									result = m_filterPCFNoCascade( lightSpacePosition
										, c3d_mapNormalDepthDirectional
										, vec2( Float( 1.0f / float( ShadowMapPassDirectional::TileSize ) ) )
										, bias );
								}
								ELSE
								{
									auto bias = m_writer.declLocale( "bias"
										, m_getShadowOffset( surface.worldNormal
											, normalize( lightToVertex )
											, light.rawShadowOffsets.x()
											, light.rawShadowOffsets.y() ) );
									result = m_textureProjNoCascade( lightSpacePosition
										, vec2( 0.0_f )
										, c3d_mapNormalDepthDirectional
										, bias );
								}
								FI;
							}
							FI;
						}

						m_writer.returnStmt( result );
					}
					else
					{
						m_writer.returnStmt( 1.0_f );
					}
				}
				, InLight{ m_writer, "light" }
				, InSurface{ m_writer, "surface" }
				, InMat4{ m_writer, "lightMatrix" }
				, InVec3{ m_writer, "lightToVertex" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InUInt{ m_writer, "maxCascade" } );
		}

		void Shadow::doDeclareComputeSpotShadow()
		{
			doDeclareTextureProj();
			doDeclareFilterPCF();
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			m_computeSpot = m_writer.implementFunction< Float >( "c3d_shdComputeSpot"
				, [this]( shader::Light const & light
					, Surface const & surface
					, Mat4 const & lightMatrix
					, Vec3 const & lightToVertex )
				{
					if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowSpot ) )
					{
						auto c3d_mapNormalDepthSpot = m_writer.getVariable< CombinedImage2DArrayRgba32 >( Shadow::MapNormalDepthSpot );
						auto c3d_mapVarianceSpot = m_writer.getVariable< CombinedImage2DArrayRg32 >( Shadow::MapVarianceSpot );
						auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
							, getLightSpacePosition( lightMatrix, surface.worldPosition ) );
						auto result = m_writer.declLocale( "result"
							, 0.0_f );
						auto depth = m_writer.declLocale( "depth"
							, length( lightToVertex ) / light.farPlane );

						IF( m_writer, light.shadowType == Int( int( ShadowType::eVariance ) ) )
						{
							auto moments = m_writer.declLocale( "moments"
								, c3d_mapVarianceSpot
									.lod( vec3( lightSpacePosition.xy()
										, m_writer.cast< Float >( light.index ) )
									, 0.0_f ) );
							result = m_chebyshevUpperBound( moments
								, depth
								, light.vsmShadowVariance.x()
								, light.vsmShadowVariance.y() );
						}
						ELSE
						{
							IF( m_writer, light.shadowType == Int( int( ShadowType::ePCF ) ) )
							{
								auto bias = m_writer.declLocale( "bias"
									, m_getShadowOffset( surface.worldNormal
										, normalize( lightToVertex )
										, light.pcfShadowOffsets.x()
										, light.pcfShadowOffsets.y() ) );
								auto invTexDim = m_writer.declLocale( "invTexDim"
									, Float( 1.0f / float( ShadowMapPassSpot::TextureSize ) ) );
								auto scale = m_writer.declLocale( "scale"
									, 1.0_f );
								auto dx = m_writer.declLocale( "dx"
									, scale * invTexDim );
								auto dy = m_writer.declLocale( "dy"
									, scale * invTexDim );
								auto shadowMapDepth = m_writer.declLocale( "shadowFactor"
									, 0.0_f );
								int count = 0;
								int const range = 1;

								for ( int x = -range; x <= range; ++x )
								{
									for ( int y = -range; y <= range; ++y )
									{
										shadowMapDepth = c3d_mapNormalDepthSpot.sample( vec3( lightSpacePosition.xy() + vec2( dx * float( x ), dy * float( y ) )
											, m_writer.cast< Float >( light.index ) ) ).w();
										result += step( depth - bias, shadowMapDepth );
										++count;
									}
								}

								result /= float( count );
							}
							ELSE
							{
								auto bias = m_writer.declLocale( "bias"
									, m_getShadowOffset( surface.worldNormal
										, normalize( lightToVertex )
										, light.rawShadowOffsets.x()
										, light.rawShadowOffsets.y() ) );
								auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
									, c3d_mapNormalDepthSpot.sample( vec3( lightSpacePosition.xy()
										, m_writer.cast< Float >( light.index ) ) ).w() );
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
				, InLight{ m_writer, "light" }
				, InSurface{ m_writer, "surface" }
				, InMat4{ m_writer, "lightMatrix" }
				, InVec3{ m_writer, "lightToVertex" } );
		}

		void Shadow::doDeclareComputePointShadow()
		{
			doDeclareGetShadowOffset();
			doDeclareChebyshevUpperBound();
			m_computePoint = m_writer.implementFunction< Float >( "c3d_shdComputePoint"
				, [this]( shader::Light const & light
					, Surface const & surface
					, Vec3 const & lightPosition )
				{
					if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowPoint ) )
					{
						auto c3d_mapNormalDepthPoint = m_writer.getVariable< CombinedImageCubeArrayRgba32 >( Shadow::MapNormalDepthPoint );
						auto c3d_mapVariancePoint = m_writer.getVariable< CombinedImageCubeArrayRg32 >( Shadow::MapVariancePoint );
						auto lightToVertex = m_writer.declLocale( "lightToVertex"
							, surface.worldPosition - lightPosition );
						auto depth = m_writer.declLocale( "depth"
							, length( lightToVertex ) / light.farPlane );
						auto result = m_writer.declLocale( "result"
							, 0.0_f );

						IF( m_writer, light.shadowType == Int( int( ShadowType::eVariance ) ) )
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
											.lod( vec4( lightToVertex + vec3( x, y, z )
												, m_writer.cast< Float >( light.index ) )
											, 0.0_f );
										shadowFactor += m_chebyshevUpperBound( moments
											, depth
											, light.vsmShadowVariance.x()
											, light.vsmShadowVariance.y() );
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
							IF( m_writer, light.shadowType == Int( int( ShadowType::ePCF ) ) )
							{
								auto bias = m_writer.declLocale( "bias"
									, m_getShadowOffset( surface.worldNormal
										, normalize( lightToVertex )
										, light.pcfShadowOffsets.x()
										, light.pcfShadowOffsets.y() ) );
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
											shadowMapDepth = c3d_mapNormalDepthPoint.sample( vec4( lightToVertex + vec3( x, y, z )
													, m_writer.cast< Float >( light.index ) ) ).w();
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
								auto bias = m_writer.declLocale( "bias"
									, m_getShadowOffset( surface.worldNormal
										, normalize( lightToVertex )
										, light.rawShadowOffsets.x()
										, light.rawShadowOffsets.y() ) );
								auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
									, c3d_mapNormalDepthPoint.sample( vec4( lightToVertex
										, m_writer.cast< Float >( light.index ) ) ).w() );
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
				, InLight{ m_writer, "light" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "lightPosition" ) );
		}

		void Shadow::doDeclareComputeVolumetric()
		{
			if ( m_computeVolumetric )
			{
				return;
			}

			OutputComponents output{ m_writer };
			m_computeVolumetric = m_writer.implementFunction< sdw::Void >( "c3d_shdComputeVolumetric"
				, [this]( shader::Light const & light
					, Surface surface
					, Vec3 const & eyePosition
					, Mat4 const & lightMatrix
					, Vec3 const & lightDirection
					, UInt const & cascadeIndex
					, UInt const & maxCascade
					, OutputComponents & parentOutput )
				{
					if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowDirectional ) )
					{
						auto c3d_volumetricDither = m_writer.getVariableArray< Vec4 >( "c3d_volumetricDither" );

						auto rayVector = m_writer.declLocale( "rayVector"
							, surface.worldPosition - eyePosition );
						auto rayLength = m_writer.declLocale( "rayLength"
							, length( rayVector ) );
						auto rayDirection = m_writer.declLocale( "rayDirection"
							, rayVector / rayLength );
						auto stepLength = m_writer.declLocale( "stepLength"
							, rayLength / m_writer.cast< Float >( light.volumetricSteps ) );
						auto step = m_writer.declLocale( "step"
							, rayDirection * stepLength );
						auto screenUV = m_writer.declLocale( "screenUV"
							, uvec2( m_writer.cast< UInt >( surface.clipPosition.x() )
								, m_writer.cast< UInt >( surface.clipPosition.y() ) ) );
						auto ditherValue = m_writer.declLocale( "ditherValue"
							, c3d_volumetricDither[screenUV.x() % 4_u][screenUV.y() % 4_u] );

						surface.worldPosition = eyePosition + step * ditherValue;
						auto volumetric = m_writer.declLocale( "volumetric"
							, 0.0_f );

						auto RdotL = m_writer.declLocale( "RdotL"
							, dot( rayDirection, lightDirection ) );
						auto sqVolumetricScattering = m_writer.declLocale( "sqVolumetricScattering"
							, light.volumetricScattering * light.volumetricScattering );
						auto dblVolumetricScattering = m_writer.declLocale( "dblVolumetricScattering"
							, 2.0_f * light.volumetricScattering );
						auto oneMinusVolumeScattering = m_writer.declLocale( "oneMinusVolumeScattering"
							, 1.0_f - sqVolumetricScattering );
						auto scattering = m_writer.declLocale( "scattering"
							, oneMinusVolumeScattering / ( 4.0_f
								* Float{ Pi< float > }
								* pow( max( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * -RdotL, 0.0_f ), 1.5_f ) ) );
						auto maxCount = m_writer.declLocale( "maxCount"
							, m_writer.cast< Int >( light.volumetricSteps ) );

						FOR( m_writer, Int, i, 0, i < maxCount, ++i )
						{
							IF( m_writer
								, computeDirectional( light
									, surface
									, lightMatrix
									, lightDirection
									, cascadeIndex
									, maxCascade ) > 0.5_f )
							{
								volumetric += scattering;
							}
							FI;

							surface.worldPosition += step;
						}
						ROF;

						volumetric /= m_writer.cast< Float >( light.volumetricSteps );
						parentOutput.m_diffuse += volumetric * light.intensity.x() * light.colour;
						parentOutput.m_specular += volumetric * light.intensity.y() * light.colour;
					}
				}
				, InLight{ m_writer, "light" }
				, InSurface{ m_writer, "surface" }
				, InVec3{ m_writer, "eyePosition" }
				, InMat4{ m_writer, "lightMatrix" }
				, InVec3{ m_writer, "lightDirection" }
				, InUInt{ m_writer, "cascadeIndex" }
				, InUInt{ m_writer, "maxCascade" }
				, output );
		}
	}
}
