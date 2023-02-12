#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslRay.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		castor::String const Shadow::MapDepthDirectional = "c3d_shdMapDepthDirectional";
		castor::String const Shadow::MapDepthSpot = "c3d_shdMapDepthSpot";
		castor::String const Shadow::MapDepthPoint = "c3d_shdMapDepthPoint";
		castor::String const Shadow::MapVarianceDirectional = "c3d_shdMapVarianceDirectional";
		castor::String const Shadow::MapVarianceSpot = "c3d_shdMapVarianceSpot";
		castor::String const Shadow::MapVariancePoint = "c3d_shdMapVariancePoint";
		castor::String const Shadow::MapNormalDirectional = "c3d_shdMapNormalDirectional";
		castor::String const Shadow::MapNormalSpot = "c3d_shdMapNormalSpot";
		castor::String const Shadow::MapNormalPoint = "c3d_shdMapNormalPoint";
		castor::String const Shadow::MapPositionDirectional = "c3d_shdMapPositionDirectional";
		castor::String const Shadow::MapPositionSpot = "c3d_shdMapPositionSpot";
		castor::String const Shadow::MapPositionPoint = "c3d_shdMapPositionPoint";
		castor::String const Shadow::MapFluxDirectional = "c3d_shdMapFluxDirectional";
		castor::String const Shadow::MapFluxSpot = "c3d_shdMapFluxSpot";
		castor::String const Shadow::MapFluxPoint = "c3d_shdMapFluxPoint";
		castor::String const Shadow::RandomBuffer = "c3d_shdRandomBuffer";

		Shadow::Shadow( ShadowOptions shadowOptions
			, sdw::ShaderWriter & writer )
			: m_writer{ writer }
			, m_shadowOptions{ std::move( shadowOptions ) }
			, m_poissonSamples{ m_writer.declConstantArray( "c3d_poissonSamples"
				, std::vector< sdw::Vec2 >{ vec2( -0.5119625_f, -0.4827938_f )
				, vec2( -0.2171264_f, -0.4768726_f )
				, vec2( -0.7552931_f, -0.2426507_f )
				, vec2( -0.7136765_f, -0.4496614_f )
				, vec2( -0.5938849_f, -0.6895654_f )
				, vec2( -0.3148003_f, -0.7047654_f )
				, vec2( -0.42215_f, -0.2024607_f )
				, vec2( -0.9466816_f, -0.2014508_f )
				, vec2( -0.8409063_f, -0.03465778_f )
				, vec2( -0.6517572_f, -0.07476326_f )
				, vec2( -0.1041822_f, -0.02521214_f )
				, vec2( -0.3042712_f, -0.02195431_f )
				, vec2( -0.5082307_f, 0.1079806_f )
				, vec2( -0.08429877_f, -0.2316298_f )
				, vec2( -0.9879128_f, 0.1113683_f )
				, vec2( -0.3859636_f, 0.3363545_f )
				, vec2( -0.1925334_f, 0.1787288_f )
				, vec2( 0.003256182_f, 0.138135_f )
				, vec2( -0.8706837_f, 0.3010679_f )
				, vec2( -0.6982038_f, 0.1904326_f )
				, vec2( 0.1975043_f, 0.2221317_f )
				, vec2( 0.1507788_f, 0.4204168_f )
				, vec2( 0.3514056_f, 0.09865579_f )
				, vec2( 0.1558783_f, -0.08460935_f )
				, vec2( -0.0684978_f, 0.4461993_f )
				, vec2( 0.3780522_f, 0.3478679_f )
				, vec2( 0.3956799_f, -0.1469177_f )
				, vec2( 0.5838975_f, 0.1054943_f )
				, vec2( 0.6155105_f, 0.3245716_f )
				, vec2( 0.3928624_f, -0.4417621_f )
				, vec2( 0.1749884_f, -0.4202175_f )
				, vec2( 0.6813727_f, -0.2424808_f )
				, vec2( -0.6707711_f, 0.4912741_f )
				, vec2( 0.0005130528_f, -0.8058334_f )
				, vec2( 0.02703013_f, -0.6010728_f )
				, vec2( -0.1658188_f, -0.9695674_f )
				, vec2( 0.4060591_f, -0.7100726_f )
				, vec2( 0.7713396_f, -0.4713659_f )
				, vec2( 0.573212_f, -0.51544_f )
				, vec2( -0.3448896_f, -0.9046497_f )
				, vec2( 0.1268544_f, -0.9874692_f )
				, vec2( 0.7418533_f, -0.6667366_f )
				, vec2( 0.3492522_f, 0.5924662_f )
				, vec2( 0.5679897_f, 0.5343465_f )
				, vec2( 0.5663417_f, 0.7708698_f )
				, vec2( 0.7375497_f, 0.6691415_f )
				, vec2( 0.2271994_f, -0.6163502_f )
				, vec2( 0.2312844_f, 0.8725659_f )
				, vec2( 0.4216993_f, 0.9002838_f )
				, vec2( 0.4262091_f, -0.9013284_f )
				, vec2( 0.2001408_f, -0.808381_f )
				, vec2( 0.149394_f, 0.6650763_f )
				, vec2( -0.09640376_f, 0.9843736_f )
				, vec2( 0.7682328_f, -0.07273844_f )
				, vec2( 0.04146584_f, 0.8313184_f )
				, vec2( 0.9705266_f, -0.1143304_f )
				, vec2( 0.9670017_f, 0.1293385_f )
				, vec2( 0.9015037_f, -0.3306949_f )
				, vec2( -0.5085648_f, 0.7534177_f )
				, vec2( 0.9055501_f, 0.3758393_f )
				, vec2( 0.7599946_f, 0.1809109_f )
				, vec2( -0.2483695_f, 0.7942952_f )
				, vec2( -0.4241052_f, 0.5581087_f )
				, vec2( -0.1020106_f, 0.6724468_f ) } ) }
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
					, sdw::UInt( DirectionalMaxCascadesCount ) );
				m_writer.declConstantArray( "c3d_volumetricDither"
					, std::vector< sdw::Vec4 >{ vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f )
						, vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f )
						, vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f )
						, vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) } );

				if constexpr ( DirectionalMaxCascadesCount > 1u )
				{
					m_writer.declCombinedImg< FImg2DArrayR32 >( MapDepthDirectional
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
					m_writer.declCombinedImg< FImg2DR32 >( MapDepthDirectional
						, ( directionalEnabled ? index++ : index )
						, set
						, directionalEnabled );
					m_writer.declCombinedImg< FImg2DRg32 >( MapVarianceDirectional
						, ( directionalEnabled ? index++ : index )
						, set
						, directionalEnabled );
				}

				m_writer.declCombinedImg< FImgCubeArrayR32 >( MapDepthPoint
					, ( pointEnabled ? index++ : index )
					, set
					, pointEnabled );
				m_writer.declCombinedImg< FImgCubeArrayRg32 >( MapVariancePoint
					, ( pointEnabled ? index++ : index )
					, set
					, pointEnabled );
				m_writer.declCombinedImg< FImg2DArrayR32 >( MapDepthSpot
					, ( spotEnabled ? index++ : index )
					, set
					, spotEnabled );
				m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceSpot
					, ( spotEnabled ? index++ : index )
					, set
					, spotEnabled );
				auto randomSsbo = m_writer.declStorageBuffer( RandomBuffer
					, index++
					, set );
				m_randomData = std::make_unique< sdw::Vec4Array >( randomSsbo.declMember< sdw::Vec4 >( "c3d_randomData", RandomDataCount ) );
				randomSsbo.end();
			}
		}

		void Shadow::declareDirectional( uint32_t & index
			, uint32_t set )
		{
			if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowDirectional ) )
			{
				m_writer.declConstant( "c3d_maxCascadeCount"
					, sdw::UInt( DirectionalMaxCascadesCount ) );
				m_writer.declConstantArray( "c3d_volumetricDither"
					, std::vector< sdw::Vec4 >{ vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f )
						, vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f )
						, vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f )
						, vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) } );

				if constexpr ( DirectionalMaxCascadesCount > 1u )
				{
					m_writer.declCombinedImg< FImg2DArrayR32 >( MapDepthDirectional
						, index++
						, set );
					m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceDirectional
						, index++
						, set );
				}
				else
				{
					m_writer.declCombinedImg< FImg2DR32 >( MapDepthDirectional
						, index++
						, set );
					m_writer.declCombinedImg< FImg2DRg32 >( MapVarianceDirectional
						, index++
						, set );
				}

				auto randomSsbo = m_writer.declStorageBuffer( RandomBuffer
					, index++
					, set );
				m_randomData = std::make_unique< sdw::Vec4Array >( randomSsbo.declMember< sdw::Vec4 >( "c3d_randomData", RandomDataCount ) );
				randomSsbo.end();
			}
		}

		void Shadow::declarePoint( uint32_t & index
			, uint32_t set )
		{
			if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowPoint ) )
			{
				m_writer.declCombinedImg< FImgCubeArrayR32 >( MapDepthPoint
					, index++
					, set );
				m_writer.declCombinedImg< FImgCubeArrayRg32 >( MapVariancePoint
					, index++
					, set );
				auto randomSsbo = m_writer.declStorageBuffer( RandomBuffer
					, index++
					, set );
				m_randomData = std::make_unique< sdw::Vec4Array >( randomSsbo.declMember< sdw::Vec4 >( "c3d_randomData", RandomDataCount ) );
				randomSsbo.end();
			}
		}

		void Shadow::declareSpot( uint32_t & index
			, uint32_t set )
		{
			if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowSpot ) )
			{
				m_writer.declCombinedImg< FImg2DArrayR32 >( MapDepthSpot
					, index++
					, set );
				m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceSpot
					, index++
					, set );
				auto randomSsbo = m_writer.declStorageBuffer( RandomBuffer
					, index++
					, set );
				m_randomData = std::make_unique< sdw::Vec4Array >( randomSsbo.declMember< sdw::Vec4 >( "c3d_randomData", RandomDataCount ) );
				randomSsbo.end();
			}
		}

		sdw::Float Shadow::computeDirectional( shader::ShadowData const & pshadows
			, sdw::Vec3 const & pwsVertexToLight
			, sdw::Vec3 const & pwsNormal
			, sdw::Vec3 const & pwsPosition
			, sdw::Mat4 const & plightMatrix
			, sdw::UInt const & pcascadeIndex
			, sdw::UInt const & pmaxCascade )
		{
			if ( !m_computeDirectional )
			{
				m_computeDirectional = m_writer.implementFunction< sdw::Float >( "c3d_shdComputeDirectional"
					, [this]( shader::ShadowData const & shadows
						, sdw::Vec3 const & wsVertexToLight
						, sdw::Vec3 const & wsNormal
						, sdw::Vec3 const & wsPosition
						, sdw::Mat4 const & lightMatrix
						, sdw::UInt cascadeIndex
						, sdw::UInt const & maxCascade )
					{
						if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowDirectional ) )
						{
							auto result = m_writer.declLocale( "result"
								, 1.0_f );
							auto lightToVertex = m_writer.declLocale( "lightToVertex"
								, -normalize( wsVertexToLight ) );

							if constexpr ( DirectionalMaxCascadesCount > 1u )
							{
								auto c3d_mapNormalDepthDirectional = m_writer.getVariable< sdw::CombinedImage2DArrayR32 >( Shadow::MapDepthDirectional );
								auto c3d_mapVarianceDirectional = m_writer.getVariable< sdw::CombinedImage2DArrayRg32 >( Shadow::MapVarianceDirectional );
								auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
									, getLightSpacePosition( lightMatrix, wsPosition ) );

								IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::eVariance ) ) )
								{
									auto moments = m_writer.declLocale( "moments"
										, c3d_mapVarianceDirectional
											.lod( vec3( lightSpacePosition.xy()
												, m_writer.cast< sdw::Float >( cascadeIndex ) )
											, 0.0_f ) );
									result = chebyshevUpperBound( moments
										, lightSpacePosition.z()
										, shadows.vsmShadowVariance().x()
										, shadows.vsmShadowVariance().y() );
								}
								ELSE
								{
									IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::ePCF ) ) )
									{
										auto bias = m_writer.declLocale( "bias"
											, getShadowOffset( wsNormal
												, lightToVertex
												, shadows.pcfShadowOffsets().x()
												, shadows.pcfShadowOffsets().y() ) );
										result = filterPCF( lightSpacePosition
											, c3d_mapNormalDepthDirectional
											, vec2( sdw::Float( 1.0f / float( ShadowMapDirectionalTextureSize ) ) )
											, cascadeIndex
											, bias
											, shadows.pcfSampleCount()
											, shadows.pcfFilterSize() );
									}
									ELSE
									{
										auto bias = m_writer.declLocale( "bias"
											, getShadowOffset( wsNormal
												, lightToVertex
												, shadows.rawShadowOffsets().x()
												, shadows.rawShadowOffsets().y() ) );
										auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
											, c3d_mapNormalDepthDirectional.sample( vec3( lightSpacePosition.xy()
												, m_writer.cast< sdw::Float >( cascadeIndex ) ) ) );
										result = step( lightSpacePosition.z() - bias, shadowMapDepth );
									}
									FI;
								}
								FI;
							}
							else
							{
								auto c3d_mapNormalDepthDirectional = m_writer.getVariable< sdw::CombinedImage2DR32 >( Shadow::MapDepthDirectional );
								auto c3d_mapVarianceDirectional = m_writer.getVariable< sdw::CombinedImage2DRg32 >( Shadow::MapVarianceDirectional );
								auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
									, getLightSpacePosition( lightMatrix, wsPosition ) );

								IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::eVariance ) ) )
								{
									auto moments = m_writer.declLocale( "moments"
										, c3d_mapVarianceDirectional
											.lod( lightSpacePosition.xy(), 0.0_f ) );
									result = chebyshevUpperBound( moments
										, lightSpacePosition.z()
										, shadows.vsmShadowVariance().x()
										, shadows.vsmShadowVariance().y() );
								}
								ELSE
								{
									IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::ePCF ) ) )
									{
										auto bias = m_writer.declLocale( "bias"
											, getShadowOffset( wsNormal
												, lightToVertex
												, shadows.pcfShadowOffsets().x()
												, shadows.pcfShadowOffsets().y() ) );
										result = filterPCF( lightSpacePosition
											, c3d_mapNormalDepthDirectional
											, vec2( sdw::Float( 1.0f / float( ShadowMapDirectionalTextureSize ) ) )
											, bias
											, shadows.pcfSampleCount()
											, shadows.pcfFilterSize() );
									}
									ELSE
									{
										auto bias = m_writer.declLocale( "bias"
											, getShadowOffset( wsNormal
												, lightToVertex
												, shadows.rawShadowOffsets().x()
												, shadows.rawShadowOffsets().y() ) );
										auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
											, c3d_mapNormalDepthDirectional.sample( lightSpacePosition.xy() ) );
										result += step( lightSpacePosition.z() - bias, shadowMapDepth );
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
					, InShadowData{ m_writer, "shadows" }
					, sdw::InVec3{ m_writer, "wsVertexToLight" }
					, sdw::InVec3{ m_writer, "wsNormal" }
					, sdw::InVec3{ m_writer, "wsPosition" }
					, sdw::InMat4{ m_writer, "lightMatrix" }
					, sdw::InUInt{ m_writer, "cascadeIndex" }
					, sdw::InUInt{ m_writer, "maxCascade" } );
			}

			return m_computeDirectional( pshadows
				, pwsVertexToLight
				, pwsNormal
				, pwsPosition
				, plightMatrix
				, pcascadeIndex
				, pmaxCascade );
		}

		sdw::Float Shadow::computeDirectional( shader::ShadowData const & pshadows
			, LightSurface const & plightSurface
			, sdw::Mat4 const & plightMatrix
			, sdw::UInt const & pcascadeIndex
			, sdw::UInt const & pmaxCascade )
		{
			return computeDirectional( pshadows
				, plightSurface.vertexToLight()
				, plightSurface.N()
				, plightSurface.worldPosition()
				, plightMatrix
				, pcascadeIndex
				, pmaxCascade );
		}

		sdw::Float Shadow::computeSpot( shader::ShadowData const & pshadows
			, LightSurface const & plightSurface
			, sdw::Mat4 const & plightMatrix
			, sdw::Float const & pdepth )
		{
			if ( !m_computeSpot )
			{
				m_computeSpot = m_writer.implementFunction< sdw::Float >( "c3d_shdComputeSpot"
					, [this]( shader::ShadowData const & shadows
						, sdw::Float const & depth
						, sdw::Vec3 const & wsVertexToLight
						, sdw::Vec3 const & wsNormal
						, sdw::Vec3 const & wsPosition
						, sdw::Mat4 const & lightMatrix )
					{
						if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowSpot ) )
						{
							auto c3d_mapNormalDepthSpot = m_writer.getVariable< sdw::CombinedImage2DArrayR32 >( Shadow::MapDepthSpot );
							auto c3d_mapVarianceSpot = m_writer.getVariable< sdw::CombinedImage2DArrayRg32 >( Shadow::MapVarianceSpot );
							auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
								, getLightSpacePosition( lightMatrix, wsPosition ) );
							auto lightToVertex = m_writer.declLocale( "lightToVertex"
								, -wsVertexToLight );
							auto lightDirection = m_writer.declLocale( "lightDirection"
								, normalize( lightToVertex ) );
							auto result = m_writer.declLocale( "result"
								, 0.0_f );

							IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::eVariance ) ) )
							{
								auto moments = m_writer.declLocale( "moments"
									, c3d_mapVarianceSpot
										.lod( vec3( lightSpacePosition.xy()
											, shadows.getMember< "shadowMapIndex" >() )
										, 0.0_f ) );
								result = chebyshevUpperBound( moments
									, depth
									, shadows.vsmShadowVariance().x()
									, shadows.vsmShadowVariance().y() );
							}
							ELSE
							{
								IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::ePCF ) ) )
								{
									auto bias = m_writer.declLocale( "bias"
										, getShadowOffset( wsNormal
											, lightDirection
											, shadows.pcfShadowOffsets().x()
											, shadows.pcfShadowOffsets().y() ) );
									result = filterPCF( lightSpacePosition
										, c3d_mapNormalDepthSpot
										, vec2( sdw::Float( 1.0f / float( ShadowMapSpotTextureSize ) ) )
										, shadows.shadowMapIndex()
										, depth
										, bias
										, shadows.pcfSampleCount()
										, shadows.pcfFilterSize() );
								}
								ELSE
								{
									auto bias = m_writer.declLocale( "bias"
										, getShadowOffset( wsNormal
											, lightDirection
											, shadows.rawShadowOffsets().x()
											, shadows.rawShadowOffsets().y() ) );
									auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
										, c3d_mapNormalDepthSpot.sample( vec3( lightSpacePosition.xy()
											, shadows.getMember< "shadowMapIndex" >() ) ) );
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
					, InShadowData{ m_writer, "shadows" }
					, sdw::InFloat{ m_writer, "depth" }
					, sdw::InVec3{ m_writer, "wsVertexToLight" }
					, sdw::InVec3{ m_writer, "wsNormal" }
					, sdw::InVec3{ m_writer, "wsPosition" }
					, sdw::InMat4{ m_writer, "lightMatrix" } );
			}

			return m_computeSpot( pshadows
				, pdepth
				, plightSurface.vertexToLight()
				, plightSurface.N()
				, plightSurface.worldPosition()
				, plightMatrix );
		}

		sdw::Float Shadow::computePoint( shader::ShadowData const & pshadows
			, LightSurface const & plightSurface
			, sdw::Float const & pdepth )
		{
			if ( !m_computePoint )
			{
				m_computePoint = m_writer.implementFunction< sdw::Float >( "c3d_shdComputePoint"
					, [this]( shader::ShadowData const & shadows
						, sdw::Float const & depth
						, sdw::Vec3 const & wsVertexToLight
						, sdw::Vec3 const & wsNormal
						, sdw::Vec3 const & wsPosition )
					{
						if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowPoint ) )
						{
							auto c3d_mapNormalDepthPoint = m_writer.getVariable< sdw::CombinedImageCubeArrayR32 >( Shadow::MapDepthPoint );
							auto c3d_mapVariancePoint = m_writer.getVariable< sdw::CombinedImageCubeArrayRg32 >( Shadow::MapVariancePoint );
							auto lightToVertex = m_writer.declLocale( "lightToVertex"
								, -wsVertexToLight );
							auto lightDirection = m_writer.declLocale( "lightDirection"
								, normalize( lightToVertex ) );
							auto result = m_writer.declLocale( "result"
								, 0.0_f );

							IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::eVariance ) ) )
							{
								result = filterVSM( lightToVertex
									, c3d_mapVariancePoint
									, shadows.shadowMapIndex()
									, depth
									, shadows.vsmShadowVariance().x()
									, shadows.vsmShadowVariance().y() );
							}
							ELSE
							{
								IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::ePCF ) ) )
								{
									auto bias = m_writer.declLocale( "bias"
										, getShadowOffset( wsNormal
											, lightDirection
											, shadows.pcfShadowOffsets().x()
											, shadows.pcfShadowOffsets().y() ) );
									result = filterPCF( lightToVertex
										, c3d_mapNormalDepthPoint
										, vec2( sdw::Float( 1.0f / float( ShadowMapPointTextureSize ) ) )
										, shadows.shadowMapIndex()
										, depth
										, bias
										, shadows.pcfSampleCount()
										, shadows.pcfFilterSize() );
								}
								ELSE
								{
									auto bias = m_writer.declLocale( "bias"
										, getShadowOffset( wsNormal
											, lightDirection
											, shadows.rawShadowOffsets().x()
											, shadows.rawShadowOffsets().y() ) );
									auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
										, c3d_mapNormalDepthPoint.sample( vec4( lightToVertex
											, shadows.getMember< "shadowMapIndex" >() ) ) );
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
					, InShadowData{ m_writer, "shadows" }
					, sdw::InFloat{ m_writer, "depth" }
					, sdw::InVec3{ m_writer, "wsVertexToLight" }
					, sdw::InVec3{ m_writer, "wsNormal" }
					, sdw::InVec3{ m_writer, "wsPosition" } );
			}

			return m_computePoint( pshadows
				, pdepth
				, plightSurface.vertexToLight()
				, plightSurface.N()
				, plightSurface.worldPosition() );
		}

		sdw::Float Shadow::computeVolumetric( shader::ShadowData const & shadows
			, LightSurface const & lightSurface
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade )
		{
			auto c3d_volumetricDither = m_writer.getVariableArray< sdw::Vec4 >( "c3d_volumetricDither" );

			// Prepare ray
			auto rayVector = m_writer.declLocale( "rayVector"
				, lightSurface.worldPosition() - lightSurface.eyePosition() );
			auto rayLength = m_writer.declLocale( "rayLength"
				, length( rayVector ) );
			auto rayDirection = m_writer.declLocale( "rayDirection"
				, rayVector / rayLength );
			auto stepLength = m_writer.declLocale( "stepLength"
				, rayLength / m_writer.cast< sdw::Float >( shadows.volumetricSteps() ) );
			auto screenUV = m_writer.declLocale( "screenUV"
				, uvec2( m_writer.cast< sdw::UInt >( lightSurface.clipPosition().x() )
					, m_writer.cast< sdw::UInt >( lightSurface.clipPosition().y() ) ) );
			auto ditherValue = m_writer.declLocale( "ditherValue"
				, c3d_volumetricDither[screenUV.x() % 4_u][screenUV.y() % 4_u] );
			auto ray = m_writer.declLocale( "ray"
				, Ray{ m_writer
					, sdw::fma( rayDirection, vec3( stepLength * ditherValue ), lightSurface.eyePosition() )
					, rayDirection } );

			// Compute scattering value
			auto RdotL = m_writer.declLocale( "RdotL"
				, dot( rayDirection, -lightSurface.L() ) );
			auto sqVolumetricScattering = m_writer.declLocale( "sqVolumetricScattering"
				, shadows.volumetricScattering() * shadows.volumetricScattering() );
			auto dblVolumetricScattering = m_writer.declLocale( "dblVolumetricScattering"
				, 2.0_f * shadows.volumetricScattering() );
			auto oneMinusVolumeScattering = m_writer.declLocale( "oneMinusVolumeScattering"
				, 1.0_f - sqVolumetricScattering );

			return computeVolumetric( shadows
				, lightSurface
				, ray
				, stepLength
				, lightMatrix
				, cascadeIndex
				, maxCascade
				, oneMinusVolumeScattering / ( 4.0_f
					* sdw::Float{ castor::Pi< float > }
					* pow( max( 1.0_f + sqVolumetricScattering - dblVolumetricScattering * -RdotL, 0.0_f ), 1.5_f ) ) );
		}

		sdw::Float Shadow::computeVolumetric( shader::ShadowData const & shadows
			, LightSurface const & lightSurface
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade
			, sdw::Float const & scattering )
		{
			auto c3d_volumetricDither = m_writer.getVariableArray< sdw::Vec4 >( "c3d_volumetricDither" );

			// Prepare ray
			auto rayLength = m_writer.declLocale( "rayLength"
				, lightSurface.lengthV() );
			auto rayDirection = m_writer.declLocale( "rayDirection"
				, -lightSurface.V() );
			auto stepLength = m_writer.declLocale( "stepLength"
				, rayLength / m_writer.cast< sdw::Float >( shadows.volumetricSteps() ) );
			auto screenUV = m_writer.declLocale( "screenUV"
				, uvec2( m_writer.cast< sdw::UInt >( lightSurface.clipPosition().x() )
					, m_writer.cast< sdw::UInt >( lightSurface.clipPosition().y() ) ) );
			auto ditherValue = m_writer.declLocale( "ditherValue"
				, c3d_volumetricDither[screenUV.x() % 4_u][screenUV.y() % 4_u] );
			auto ray = m_writer.declLocale( "ray"
				, Ray{ m_writer
					, sdw::fma( rayDirection, vec3( stepLength * ditherValue ), lightSurface.eyePosition() )
					, rayDirection } );

			return computeVolumetric( shadows
				, lightSurface
				, ray
				, stepLength
				, lightMatrix
				, cascadeIndex
				, maxCascade
				, scattering );
		}

		sdw::Float Shadow::computeVolumetric( shader::ShadowData const & pshadows
			, LightSurface const & plightSurface
			, Ray const & pray
			, sdw::Float const & pstepLength
			, sdw::Mat4 const & plightMatrix
			, sdw::UInt const & pcascadeIndex
			, sdw::UInt const & pmaxCascade
			, sdw::Float const & pscattering )
		{
			if ( !m_computeVolumetric )
			{
				m_computeVolumetric = m_writer.implementFunction< sdw::Float >( "c3d_shdComputeVolumetric"
					, [this]( shader::ShadowData const & shadows
						, sdw::Vec3 const & L
						, sdw::Vec3 const & N
						, Ray const & ray
						, sdw::Float const & stepLength
						, sdw::Mat4 const & lightMatrix
						, sdw::UInt const & cascadeIndex
						, sdw::UInt const & maxCascade
						, sdw::Float const & scattering )
					{
						if ( checkFlag( m_shadowOptions.type, SceneFlag::eShadowDirectional ) )
						{
							auto volumetric = m_writer.declLocale( "volumetric"
								, 0.0_f );
							auto maxCount = m_writer.declLocale( "maxCount"
								, m_writer.cast< sdw::Int >( shadows.volumetricSteps() ) );
							auto t = m_writer.declLocale( "t", 0.0_f );

							FOR( m_writer, sdw::Int, i, 0, i < maxCount, ++i )
							{
								auto w = m_writer.declLocale( "w"
									, ray.step( t ) );

								IF( m_writer
									, computeDirectional( shadows
										, L
										, N
										, w
										, lightMatrix
										, cascadeIndex
										, maxCascade ) > 0.5_f )
								{
									volumetric += scattering;
								}
								FI;

								t += stepLength;
							}
							ROF;

							volumetric /= m_writer.cast< sdw::Float >( shadows.volumetricSteps() );
							m_writer.returnStmt( volumetric );
						}
						else
						{
							m_writer.returnStmt( 0.0_f );
						}
					}
					, InShadowData{ m_writer, "shadows" }
					, sdw::InVec3{ m_writer, "L" }
					, sdw::InVec3{ m_writer, "N" }
					, InRay{ m_writer, "ray" }
					, sdw::InFloat{ m_writer, "stepLength" }
					, sdw::InMat4{ m_writer, "lightMatrix" }
					, sdw::InUInt{ m_writer, "cascadeIndex" }
					, sdw::InUInt{ m_writer, "maxCascade" }
					, sdw::InFloat{ m_writer, "scattering" } );
			}

			return m_computeVolumetric( pshadows
				, plightSurface.L()
				, plightSurface.N()
				, pray
				, pstepLength
				, plightMatrix
				, pcascadeIndex
				, pmaxCascade
				, pscattering );
		}

		sdw::Vec4 Shadow::getLightSpacePosition( sdw::Mat4 const & plightMatrix
			, sdw::Vec3 const & pworldSpacePosition )
		{
			if ( !m_getLightSpacePosition )
			{
				m_getLightSpacePosition = m_writer.implementFunction< sdw::Vec4 >( "c3d_shdGetLightSpacePosition"
					, [this]( sdw::Mat4 const & lightMatrix
						, sdw::Vec3 const & worldSpacePosition )
					{
						auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
							, lightMatrix * vec4( worldSpacePosition, 1.0_f ) );
						lightSpacePosition.xyz() /= lightSpacePosition.w();
						lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
							, vec2( 0.5_f )
							, vec2( 0.5_f ) );
						m_writer.returnStmt( lightSpacePosition );
					}
					, sdw::InMat4( m_writer, "lightMatrix" )
					, sdw::InVec3( m_writer, "worldSpacePosition" ) );
			}

			return m_getLightSpacePosition( plightMatrix
				, pworldSpacePosition );
		}

		sdw::RetFloat Shadow::getShadowOffset( sdw::Vec3 const & pnormal
			, sdw::Vec3 const & plightDirection
			, sdw::Float const & pminOffset
			, sdw::Float const & pmaxSlopeOffset )
		{
			if ( !m_getShadowOffset )
			{
				m_getShadowOffset = m_writer.implementFunction< sdw::Float >( "c3d_shdGetShadowOffset"
					, [this]( sdw::Vec3 const & normal
						, sdw::Vec3 const & lightDirection
						, sdw::Float const & minOffset
						, sdw::Float const & maxSlopeOffset )
					{
						auto cosAlpha = m_writer.declLocale( "cosAlpha"
							, clamp( dot( normal, lightDirection ), 0.0_f, 1.0_f ) );
						auto slope = m_writer.declLocale( "slope"
							, sqrt( 1.0_f - cosAlpha ) );
						m_writer.returnStmt( minOffset + maxSlopeOffset * slope );
					}
					, sdw::InVec3( m_writer, "normal" )
					, sdw::InVec3( m_writer, "lightDirection" )
					, sdw::InFloat( m_writer, "minOffset" )
					, sdw::InFloat( m_writer, "maxSlopeOffset" ) );
			}

			return m_getShadowOffset( pnormal
				, plightDirection
				, pminOffset
				, pmaxSlopeOffset );
		}

		sdw::RetFloat Shadow::filterPCF( sdw::Vec4 const & plightSpacePosition
			, sdw::CombinedImage2DArrayR32 const & pshadowMap
			, sdw::Vec2 const & pinvTexDim
			, sdw::Int const & pindex
			, sdw::Float const & pdepth
			, sdw::Float const & pbias
			, sdw::UInt const & psampleCount
			, sdw::UInt const & pfilterSize )
		{
			if ( !m_filterPCF )
			{
				m_filterPCF = m_writer.implementFunction< sdw::Float >( "c3d_shdFilterPCF"
					, [this]( sdw::Vec4 const & lightSpacePosition
						, sdw::CombinedImage2DArrayR32 const & shadowMap
						, sdw::Vec2 const & invTexDim
						, sdw::Int const & index
						, sdw::Float const & depth
						, sdw::Float const & bias
						, sdw::UInt const & sampleCount
						, sdw::UInt const & filterSize )
					{
						auto scale = m_writer.declLocale( "scale"
							, m_writer.cast< sdw::Float >( filterSize ) );
						auto dx = m_writer.declLocale( "dx"
							, scale * invTexDim.x() );
						auto dy = m_writer.declLocale( "dy"
							, scale * invTexDim.y() );
						auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
							, 0.0_f );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto sampleOffset = m_writer.declLocale( "sampleOffset"
							, vec2( 0.0_f ) );

						// Get a value to randomly rotate the kernel by
						auto screenPos = m_writer.declLocale( "screenPos"
							, uvec2( lightSpacePosition.xy() * invTexDim ) );
						auto randomSamplePos = m_writer.declLocale( "randomSamplePos"
							, uvec2( ( screenPos.x() + screenPos.y() ) % RandomDataCount
								, ( screenPos.x() + screenPos.y() ) % 4u ) );
						auto theta = m_writer.declLocale( "theta"
							, ( *m_randomData )[randomSamplePos.x()][randomSamplePos.y()] * sdw::Float{ castor::Tau< float > } );
						auto randomRotationMatrix = m_writer.declLocale( "randomRotationMatrix"
							, mat2x2( vec2( cos( theta ), -sin( theta ) )
								, vec2( sin( theta ), cos( theta ) ) ) );

						FOR( m_writer, sdw::UInt, i, 0_u, i < sampleCount, ++i )
						{
							sampleOffset = ( randomRotationMatrix * m_poissonSamples[i] ) * vec2( dx, dy );
							shadowMapDepth = shadowMap.sample( vec3( lightSpacePosition.xy() + sampleOffset
								, m_writer.cast< sdw::Float >( index ) ) );
							shadowFactor += step( depth - bias, shadowMapDepth );
						}
						ROF;

						m_writer.returnStmt( shadowFactor / m_writer.cast< sdw::Float >( sampleCount ) );
					}
					, sdw::InVec4{ m_writer, "lightSpacePosition" }
					, sdw::InCombinedImage2DArrayR32{ m_writer, "shadowMap" }
					, sdw::InVec2{ m_writer, "invTexDim" }
					, sdw::InInt{ m_writer, "index" }
					, sdw::InFloat{ m_writer, "depth" }
					, sdw::InFloat{ m_writer, "bias" }
					, sdw::InUInt{ m_writer, "sampleCount" }
					, sdw::InUInt{ m_writer, "filterSize" } );
			}

			return m_filterPCF( plightSpacePosition
				, pshadowMap
				, pinvTexDim
				, pindex
				, pdepth
				, pbias
				, psampleCount
				, pfilterSize );
		}

		sdw::RetFloat Shadow::filterPCF( sdw::Vec4 const & plightSpacePosition
			, sdw::CombinedImage2DR32 const & pshadowMap
			, sdw::Vec2 const & pinvTexDim
			, sdw::Float const & pbias
			, sdw::UInt const & psampleCount
			, sdw::UInt const & pfilterSize )
		{
			if ( !m_filterPCFNoCascade )
			{
				m_filterPCFNoCascade = m_writer.implementFunction< sdw::Float >( "c3d_shdFilterPCFNoCascade"
					, [this]( sdw::Vec4 const & lightSpacePosition
						, sdw::CombinedImage2DR32 const & shadowMap
						, sdw::Vec2 const & invTexDim
						, sdw::Float const & bias
						, sdw::UInt const & sampleCount
						, sdw::UInt const & filterSize )
					{
						auto scale = m_writer.declLocale( "scale"
							, m_writer.cast< sdw::Float >( filterSize ) );
						auto dx = m_writer.declLocale( "dx"
							, scale * invTexDim.x() );
						auto dy = m_writer.declLocale( "dy"
							, scale * invTexDim.y() );
						auto shadowMapDepth = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto sampleOffset = m_writer.declLocale( "sampleOffset"
							, vec2( 0.0_f ) );

						// Get a value to randomly rotate the kernel by
						auto screenPos = m_writer.declLocale( "screenPos"
							, uvec2( lightSpacePosition.xy() * invTexDim ) );
						auto randomSamplePos = m_writer.declLocale( "randomSamplePos"
							, uvec2( ( screenPos.x() + screenPos.y() ) % RandomDataCount
								, ( screenPos.x() + screenPos.y() ) % 4u ) );
						auto theta = m_writer.declLocale( "theta"
							, ( *m_randomData )[randomSamplePos.x()][randomSamplePos.y()] * sdw::Float{ castor::Tau< float > } );
						auto randomRotationMatrix = m_writer.declLocale( "randomRotationMatrix"
							, mat2x2( vec2( cos( theta ), -sin( theta ) )
								, vec2( sin( theta ), cos( theta ) ) ) );

						FOR( m_writer, sdw::UInt, i, 0_u, i < sampleCount, ++i )
						{
							sampleOffset = ( randomRotationMatrix * m_poissonSamples[i] ) * vec2( dx, dy );
							shadowMapDepth = shadowMap.sample( lightSpacePosition.xy() + sampleOffset );
							shadowFactor += step( lightSpacePosition.z() - bias, shadowMapDepth );
						}
						ROF;

						m_writer.returnStmt( shadowFactor / m_writer.cast< sdw::Float >( sampleCount ) );
					}
					, sdw::InVec4{ m_writer, "lightSpacePosition" }
					, sdw::InCombinedImage2DR32{ m_writer, "shadowMap" }
					, sdw::InVec2{ m_writer, "invTexDim" }
					, sdw::InFloat{ m_writer, "bias" }
					, sdw::InUInt{ m_writer, "sampleCount" }
					, sdw::InUInt{ m_writer, "filterSize" } );
			}

			return m_filterPCFNoCascade( plightSpacePosition
				, pshadowMap
				, pinvTexDim
				, pbias
				, psampleCount
				, pfilterSize );
		}

		sdw::RetFloat Shadow::filterPCF( sdw::Vec4 const & plightSpacePosition
			, sdw::CombinedImage2DArrayR32 const & pshadowMap
			, sdw::Vec2 const & pinvTexDim
			, sdw::UInt const & pcascadeIndex
			, sdw::Float const & pbias
			, sdw::UInt const & psampleCount
			, sdw::UInt const & pfilterSize )
		{
			if ( !m_filterPCFCascade )
			{
				m_filterPCFCascade = m_writer.implementFunction< sdw::Float >( "c3d_shdFilterPCFCascade"
					, [this]( sdw::Vec4 const & lightSpacePosition
						, sdw::CombinedImage2DArrayR32 const & shadowMap
						, sdw::Vec2 const & invTexDim
						, sdw::UInt const & cascadeIndex
						, sdw::Float const & bias
						, sdw::UInt const & sampleCount
						, sdw::UInt const & filterSize )
					{
						auto scale = m_writer.declLocale( "scale"
							, m_writer.cast< sdw::Float >( filterSize ) );
						auto dx = m_writer.declLocale( "dx"
							, scale * invTexDim.x() );
						auto dy = m_writer.declLocale( "dy"
							, scale * invTexDim.y() );
						auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
							, 0.0_f );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto sampleOffset = m_writer.declLocale( "sampleOffset"
							, vec2( 0.0_f ) );

						// Get a value to randomly rotate the kernel by
						auto screenPos = m_writer.declLocale( "screenPos"
							, uvec2( lightSpacePosition.xy() * invTexDim ) );
						auto randomSamplePos = m_writer.declLocale( "randomSamplePos"
							, uvec2( ( screenPos.x() + screenPos.y() ) % RandomDataCount
								, ( screenPos.x() + screenPos.y() ) % 4u ) );
						auto theta = m_writer.declLocale( "theta"
							, ( *m_randomData )[randomSamplePos.x()][randomSamplePos.y()] * sdw::Float{ castor::Tau< float > } );
						auto randomRotationMatrix = m_writer.declLocale( "randomRotationMatrix"
							, mat2x2( vec2( cos( theta ), -sin( theta ) )
								, vec2( sin( theta ), cos( theta ) ) ) );

						FOR( m_writer, sdw::UInt, i, 0_u, i < sampleCount, ++i )
						{
							sampleOffset = ( randomRotationMatrix * m_poissonSamples[i] ) * vec2( dx, dy );
							shadowMapDepth = shadowMap.sample( vec3( lightSpacePosition.xy() + sampleOffset
								, m_writer.cast< sdw::Float >( cascadeIndex ) ) );
							shadowFactor += step( lightSpacePosition.z() - bias, shadowMapDepth );
						}
						ROF;

						m_writer.returnStmt( shadowFactor / m_writer.cast< sdw::Float >( sampleCount ) );
					}
					, sdw::InVec4{ m_writer, "lightSpacePosition" }
					, sdw::InCombinedImage2DArrayR32{ m_writer, "shadowMap" }
					, sdw::InVec2{ m_writer, "invTexDim" }
					, sdw::InUInt{ m_writer, "cascadeIndex" }
					, sdw::InFloat{ m_writer, "bias" }
					, sdw::InUInt{ m_writer, "sampleCount" }
					, sdw::InUInt{ m_writer, "filterSize" } );
			}

			return m_filterPCFCascade( plightSpacePosition
				, pshadowMap
				, pinvTexDim
				, pcascadeIndex
				, pbias
				, psampleCount
				, pfilterSize );
		}

		sdw::RetFloat Shadow::filterPCF( sdw::Vec3 const & plightToVertex
			, sdw::CombinedImageCubeArrayR32 const & pshadowMap
			, sdw::Vec2 const & pinvTexDim
			, sdw::Int const & pindex
			, sdw::Float const & pdepth
			, sdw::Float const & pbias
			, sdw::UInt const & psampleCount
			, sdw::UInt const & pfilterSize )
		{
			if ( !m_filterPCFCube )
			{
				m_filterPCFCube = m_writer.implementFunction< sdw::Float >( "c3d_shdFilterPCFCube"
					, [this]( sdw::Vec3 const & lightToVertex
						, sdw::CombinedImageCubeArrayR32 const & shadowMap
						, sdw::Vec2 const & invTexDim
						, sdw::Int const & index
						, sdw::Float const & depth
						, sdw::Float const & bias
						, sdw::UInt const & sampleCount
						, sdw::UInt const & filterSize )
					{
						int count = 0;
						int const samples = 4;

						auto offset = m_writer.declLocale( "offset"
							, 20.0_f * depth );
						auto dx = m_writer.declLocale( "dx"
							, -offset );
						auto dy = m_writer.declLocale( "dy"
							, -offset );
						auto dz = m_writer.declLocale( "dz"
							, -offset );
						auto inc = m_writer.declLocale( "inc"
							, offset / ( samples * 0.5f ) );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
							, 0.0_f);
						auto sampleOffset = m_writer.declLocale( "sampleOffset"
							, vec3( 0.0_f ) );

						for ( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									sampleOffset = vec3( dx, dy, dz );
									shadowMapDepth = shadowMap.sample( vec4( lightToVertex + sampleOffset
										, m_writer.cast< sdw::Float >( index ) ) );
									shadowFactor += step( depth - bias, shadowMapDepth );
									++count;
									dz += inc;
								}

								dy += inc;
								dz = -offset;
							}

							dx += inc;
							dy = -offset;
						}

						m_writer.returnStmt( shadowFactor / float( count ) );
					}
					, sdw::InVec3{ m_writer, "lightToVertex" }
					, sdw::InCombinedImageCubeArrayR32{ m_writer, "shadowMap" }
					, sdw::InVec2{ m_writer, "invTexDim" }
					, sdw::InInt{ m_writer, "index" }
					, sdw::InFloat{ m_writer, "depth" }
					, sdw::InFloat{ m_writer, "bias" }
					, sdw::InUInt{ m_writer, "sampleCount" }
					, sdw::InUInt{ m_writer, "filterSize" } );
			}

			return m_filterPCFCube( plightToVertex
				, pshadowMap
				, pinvTexDim
				, pindex
				, pdepth
				, pbias
				, psampleCount
				, pfilterSize );
		}

		sdw::RetFloat Shadow::chebyshevUpperBound( sdw::Vec2 const & pmoments
			, sdw::Float const & pdepth
			, sdw::Float const & pminVariance
			, sdw::Float const & pvarianceBias )
		{
			if ( !m_chebyshevUpperBound )
			{
				m_chebyshevUpperBound = m_writer.implementFunction< sdw::Float >( "c3d_shdChebyshevUpperBound"
					, [this]( sdw::Vec2 const & moments
						, sdw::Float const & depth
						, sdw::Float const & minVariance
						, sdw::Float const & varianceBias )
					{
						auto p = m_writer.declLocale( "p"
							, step( moments.x() /*+ varianceBias*/, depth ) );
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
					, sdw::InVec2{ m_writer, "moments" }
					, sdw::InFloat{ m_writer, "depth" }
					, sdw::InFloat{ m_writer, "minVariance" }
					, sdw::InFloat{ m_writer, "varianceBias" } );
			}

			return m_chebyshevUpperBound( pmoments
				, pdepth
				, pminVariance
				, pvarianceBias );
		}

		sdw::RetFloat Shadow::filterVSM( sdw::Vec3 const & plightToVertex
			, sdw::CombinedImageCubeArrayRg32 const & pshadowMap
			, sdw::Int const & pindex
			, sdw::Float const & pdepth
			, sdw::Float const & pminVariance
			, sdw::Float const & pvarianceBias )
		{
			if ( !m_filterVSMCube )
			{
				m_filterVSMCube = m_writer.implementFunction< sdw::Float >( "c3d_shdFilterVSMCube"
					, [this]( sdw::Vec3 const & lightToVertex
						, sdw::CombinedImageCubeArrayRg32 const & shadowMap
						, sdw::Int const & index
						, sdw::Float const & depth
						, sdw::Float const & minVariance
						, sdw::Float const & varianceBias )
					{
						int count = 0;
						int const samples = 4;

						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );
						auto offset = m_writer.declLocale( "offset"
							, 20.0_f * depth );
						auto numSamplesUsed = m_writer.declLocale( "numSamplesUsed"
							, 0.0_f );
						auto dx = m_writer.declLocale( "dx"
							, -offset );
						auto dy = m_writer.declLocale( "dy"
							, -offset );
						auto dz = m_writer.declLocale( "dz"
							, -offset );
						auto inc = m_writer.declLocale( "inc"
							, offset / ( samples * 0.5f ) );
						auto moments = m_writer.declLocale< sdw::Vec2 >( "moments" );

						for ( int i = 0; i < samples; ++i )
						{
							for ( int j = 0; j < samples; ++j )
							{
								for ( int k = 0; k < samples; ++k )
								{
									moments = shadowMap
										.lod( vec4( lightToVertex + vec3( dx, dy, dz )
											, m_writer.cast< sdw::Float >( index ) )
											, 0.0_f );
									shadowFactor += chebyshevUpperBound( moments
										, depth
										, minVariance
										, varianceBias );
									++count;
									dz += inc;
								}

								dy += inc;
								dz = -offset;
							}

							dx += inc;
							dy = -offset;
						}

						m_writer.returnStmt( shadowFactor / float( count ) );
					}
					, sdw::InVec3{ m_writer, "lightToVertex" }
					, sdw::InCombinedImageCubeArrayRg32{ m_writer, "shadowMap" }
					, sdw::InInt{ m_writer, "index" }
					, sdw::InFloat{ m_writer, "depth" }
					, sdw::InFloat{ m_writer, "minVariance" }
					, sdw::InFloat{ m_writer, "varianceBias" } );
			}

			return m_filterVSMCube( plightToVertex
				, pshadowMap
				, pindex
				, pdepth
				, pminVariance
				, pvarianceBias );
		}
	}
}
