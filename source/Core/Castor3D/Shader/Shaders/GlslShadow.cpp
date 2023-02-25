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
		castor::String const Shadow::MapDepthCmpDirectional = "c3d_shdMapDepthCmpDirectional";
		castor::String const Shadow::MapDepthCmpSpot = "c3d_shdMapDepthCmpSpot";
		castor::String const Shadow::MapDepthCmpPoint = "c3d_shdMapDepthCmpPoint";
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
				, std::vector< sdw::Vec2 >{ vec2( -0.613392_d, 0.617481_d )
					, vec2( 0.170019_d, -0.040254_d )
					, vec2( -0.299417_d, 0.791925_d )
					, vec2( 0.645680_d, 0.493210_d )
					, vec2( -0.651784_d, 0.717887_d )
					, vec2( 0.421003_d, 0.027070_d )
					, vec2( -0.817194_d, -0.271096_d )
					, vec2( -0.705374_d, -0.668203_d )
					, vec2( 0.977050_d, -0.108615_d )
					, vec2( 0.063326_d, 0.142369_d )
					, vec2( 0.203528_d, 0.214331_d )
					, vec2( -0.667531_d, 0.326090_d )
					, vec2( -0.098422_d, -0.295755_d )
					, vec2( -0.885922_d, 0.215369_d )
					, vec2( 0.566637_d, 0.605213_d )
					, vec2( 0.039766_d, -0.396100_d )
					, vec2( 0.751946_d, 0.453352_d )
					, vec2( 0.078707_d, -0.715323_d )
					, vec2( -0.075838_d, -0.529344_d )
					, vec2( 0.724479_d, -0.580798_d )
					, vec2( 0.222999_d, -0.215125_d )
					, vec2( -0.467574_d, -0.405438_d )
					, vec2( -0.248268_d, -0.814753_d )
					, vec2( 0.354411_d, -0.887570_d )
					, vec2( 0.175817_d, 0.382366_d )
					, vec2( 0.487472_d, -0.063082_d )
					, vec2( -0.084078_d, 0.898312_d )
					, vec2( 0.488876_d, -0.783441_d )
					, vec2( 0.470016_d, 0.217933_d )
					, vec2( -0.696890_d, -0.549791_d )
					, vec2( -0.149693_d, 0.605762_d )
					, vec2( 0.034211_d, 0.979980_d )
					, vec2( 0.503098_d, -0.308878_d )
					, vec2( -0.016205_d, -0.872921_d )
					, vec2( 0.385784_d, -0.393902_d )
					, vec2( -0.146886_d, -0.859249_d )
					, vec2( 0.643361_d, 0.164098_d )
					, vec2( 0.634388_d, -0.049471_d )
					, vec2( -0.688894_d, 0.007843_d )
					, vec2( 0.464034_d, -0.188818_d )
					, vec2( -0.440840_d, 0.137486_d )
					, vec2( 0.364483_d, 0.511704_d )
					, vec2( 0.034028_d, 0.325968_d )
					, vec2( 0.099094_d, -0.308023_d )
					, vec2( 0.693960_d, -0.366253_d )
					, vec2( 0.678884_d, -0.204688_d )
					, vec2( 0.001801_d, 0.780328_d )
					, vec2( 0.145177_d, -0.898984_d )
					, vec2( 0.062655_d, -0.611866_d )
					, vec2( 0.315226_d, -0.604297_d )
					, vec2( -0.780145_d, 0.486251_d )
					, vec2( -0.371868_d, 0.882138_d )
					, vec2( 0.200476_d, 0.494430_d )
					, vec2( -0.494552_d, -0.711051_d )
					, vec2( 0.612476_d, 0.705252_d )
					, vec2( -0.578845_d, -0.768792_d )
					, vec2( -0.772454_d, -0.090976_d )
					, vec2( 0.504440_d, 0.372295_d )
					, vec2( 0.155736_d, 0.065157_d )
					, vec2( 0.391522_d, 0.849605_d )
					, vec2( -0.620106_d, -0.328104_d )
					, vec2( 0.789239_d, -0.419965_d )
					, vec2( -0.545396_d, 0.538133_d )
					, vec2( -0.178564_d, -0.596057_d ) } ) }
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
					, sdw::UInt( MaxDirectionalCascadesCount ) );
				m_writer.declConstantArray( "c3d_volumetricDither"
					, std::vector< sdw::Vec4 >{ vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f )
						, vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f )
						, vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f )
						, vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) } );

				m_writer.declCombinedImg< FImg2DArrayR32 >( MapDepthDirectional
					, ( directionalEnabled ? index++ : index )
					, set
					, directionalEnabled );
				m_writer.declCombinedImg< FImg2DArrayShadowR32 >( MapDepthCmpDirectional
					, ( directionalEnabled ? index++ : index )
					, set
					, directionalEnabled );
				m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceDirectional
					, ( directionalEnabled ? index++ : index )
					, set
					, directionalEnabled );

				m_writer.declCombinedImg< FImgCubeArrayR32 >( MapDepthPoint
					, ( pointEnabled ? index++ : index )
					, set
					, pointEnabled );
				m_writer.declCombinedImg< FImgCubeArrayShadowR32 >( MapDepthCmpPoint
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
				m_writer.declCombinedImg< FImg2DArrayShadowR32 >( MapDepthCmpSpot
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
					, sdw::UInt( MaxDirectionalCascadesCount ) );
				m_writer.declConstantArray( "c3d_volumetricDither"
					, std::vector< sdw::Vec4 >{ vec4( 0.0_f, 0.5_f, 0.125_f, 0.625_f )
						, vec4( 0.75_f, 0.22_f, 0.875_f, 0.375_f )
						, vec4( 0.1875_f, 0.6875_f, 0.0625_f, 0.5625_f )
						, vec4( 0.9375_f, 0.4375_f, 0.8125_f, 0.3125_f ) } );

				m_writer.declCombinedImg< FImg2DArrayR32 >( MapDepthDirectional
					, index++
					, set );
				m_writer.declCombinedImg< FImg2DArrayShadowR32 >( MapDepthCmpDirectional
					, index++
					, set );
				m_writer.declCombinedImg< FImg2DArrayRg32 >( MapVarianceDirectional
					, index++
					, set );

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
				m_writer.declCombinedImg< FImgCubeArrayShadowR32 >( MapDepthCmpPoint
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
				m_writer.declCombinedImg< FImg2DArrayShadowR32 >( MapDepthCmpSpot
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
							auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
								, getLightSpacePosition( lightMatrix, wsPosition ) );

							IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::eVariance ) ) )
							{
								auto c3d_mapVarianceDirectional = m_writer.getVariable< sdw::CombinedImage2DArrayRg32 >( Shadow::MapVarianceDirectional );
								auto moments = m_writer.declLocale( "moments"
									, c3d_mapVarianceDirectional
										.lod( vec3( lightSpacePosition.xy()
											, m_writer.cast< sdw::Float >( cascadeIndex ) )
										, 0.0_f ) );
								result = chebyshevUpperBound( moments
									, lightSpacePosition.z()
									, shadows.vsmMinVariance()
									, shadows.vsmLightBleedingReduction() );
							}
							ELSEIF( shadows.shadowType() == sdw::UInt( int( ShadowType::ePCF ) ) )
							{
								auto c3d_mapNormalDepthCmpDirectional = m_writer.getVariable< sdw::CombinedImage2DArrayShadowR32 >( Shadow::MapDepthCmpDirectional );
								auto depthBias = m_writer.declLocale( "depthBias"
									, getShadowOffset( wsNormal
										, lightToVertex
										, -shadows.pcfShadowOffsets().x()
										, -shadows.pcfShadowOffsets().y() ) );
								result = filterPCF( lightSpacePosition.xyz()
									, c3d_mapNormalDepthCmpDirectional
									, vec2( sdw::Float( 1.0f / float( ShadowMapDirectionalTextureSize ) ) )
									, cascadeIndex
									, depthBias
									, shadows.pcfSampleCount()
									, shadows.pcfFilterSize() );
							}
							ELSE
							{
								auto c3d_mapNormalDepthDirectional = m_writer.getVariable< sdw::CombinedImage2DArrayR32 >( Shadow::MapDepthDirectional );
								auto depthBias = m_writer.declLocale( "depthBias"
									, getShadowOffset( wsNormal
										, lightToVertex
										, -shadows.rawShadowOffsets().x()
										, -shadows.rawShadowOffsets().y() ) );
								auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
									, c3d_mapNormalDepthDirectional.sample( vec3( lightSpacePosition.xy()
										, m_writer.cast< sdw::Float >( cascadeIndex ) ) ) );
								result = step( 1.0_f - ( lightSpacePosition.z() - depthBias ), 1.0_f - shadowMapDepth );
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
								auto c3d_mapVarianceSpot = m_writer.getVariable< sdw::CombinedImage2DArrayRg32 >( Shadow::MapVarianceSpot );
								auto moments = m_writer.declLocale( "moments"
									, c3d_mapVarianceSpot
										.lod( vec3( lightSpacePosition.xy()
											, shadows.shadowMapIndex() )
										, 0.0_f ) );
								result = chebyshevUpperBound( moments
									, lightSpacePosition.z()
									, shadows.vsmMinVariance()
									, shadows.vsmLightBleedingReduction() );
							}
							ELSE
							{
								IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::ePCF ) ) )
								{
									auto c3d_mapNormalDepthCmpSpot = m_writer.getVariable< sdw::CombinedImage2DArrayShadowR32 >( Shadow::MapDepthCmpSpot );
									auto depthBias = m_writer.declLocale( "depthBias"
										, getShadowOffset( wsNormal
											, lightDirection
											, -shadows.pcfShadowOffsets().x()
											, -shadows.pcfShadowOffsets().y() ) );
									result = filterPCF( lightSpacePosition.xyz()
										, c3d_mapNormalDepthCmpSpot
										, vec2( sdw::Float( 1.0f / float( ShadowMapSpotTextureSize ) ) )
										, m_writer.cast< sdw::UInt >( shadows.shadowMapIndex() )
										, depthBias
										, shadows.pcfSampleCount()
										, shadows.pcfFilterSize() / 2.0_f );
								}
								ELSE
								{
									auto c3d_mapNormalDepthSpot = m_writer.getVariable< sdw::CombinedImage2DArrayR32 >( Shadow::MapDepthSpot );
									auto depthBias = m_writer.declLocale( "depthBias"
										, getShadowOffset( wsNormal
											, lightDirection
											, -shadows.rawShadowOffsets().x()
											, -shadows.rawShadowOffsets().y() ) );
									auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
										, c3d_mapNormalDepthSpot.sample( vec3( lightSpacePosition.xy()
											, shadows.shadowMapIndex() ) ) );
									result = step( 1.0_f - ( lightSpacePosition.z() - depthBias ), 1.0_f - shadowMapDepth );
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
							auto lightToVertex = m_writer.declLocale( "lightToVertex"
								, -wsVertexToLight );
							auto lightDirection = m_writer.declLocale( "lightDirection"
								, normalize( lightToVertex ) );
							auto result = m_writer.declLocale( "result"
								, 0.0_f );

							IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::eVariance ) ) )
							{
								auto c3d_mapVariancePoint = m_writer.getVariable< sdw::CombinedImageCubeArrayRg32 >( Shadow::MapVariancePoint );
								auto moments = m_writer.declLocale( "moments"
									, c3d_mapVariancePoint
									.lod( vec4( lightToVertex
										, shadows.shadowMapIndex() )
										, 0.0_f ) );
								result = chebyshevUpperBound( moments
									, depth
									, shadows.vsmMinVariance()
									, shadows.vsmLightBleedingReduction() );
							}
							ELSE
							{
								IF( m_writer, shadows.shadowType() == sdw::UInt( int( ShadowType::ePCF ) ) )
								{
									auto c3d_mapNormalDepthCmpPoint = m_writer.getVariable< sdw::CombinedImageCubeArrayShadowR32 >( Shadow::MapDepthCmpPoint );
									auto depthBias = m_writer.declLocale( "depthBias"
										, getShadowOffset( wsNormal
											, lightDirection
											, -shadows.pcfShadowOffsets().x()
											, -shadows.pcfShadowOffsets().y() ) );
									result = filterPCF( lightToVertex
										, c3d_mapNormalDepthCmpPoint
										, vec2( sdw::Float( 1.0f / float( ShadowMapPointTextureSize ) ) )
										, m_writer.cast< sdw::UInt >( shadows.shadowMapIndex() )
										, depth
										, depthBias
										, shadows.pcfSampleCount()
										, shadows.pcfFilterSize() );
								}
								ELSE
								{
									auto c3d_mapNormalDepthPoint = m_writer.getVariable< sdw::CombinedImageCubeArrayR32 >( Shadow::MapDepthPoint );
									auto depthBias = m_writer.declLocale( "depthBias"
										, getShadowOffset( wsNormal
											, lightDirection
											, -shadows.rawShadowOffsets().x()
											, -shadows.rawShadowOffsets().y() ) );
									auto shadowMapDepth = m_writer.declLocale( "shadowMapDepth"
										, c3d_mapNormalDepthPoint.sample( vec4( lightToVertex
											, shadows.shadowMapIndex() ) ) );
									result = step( 1.0_f - ( depth - depthBias ), 1.0_f - shadowMapDepth );
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

		sdw::RetFloat Shadow::filterPCF( sdw::Vec3 const & plightSpacePosition
			, sdw::CombinedImage2DArrayShadowR32 const & pshadowMap
			, sdw::Vec2 const & pinvTexDim
			, sdw::UInt const & parrayIndex
			, sdw::Float const & pdepthBias
			, sdw::UInt const & psampleCount
			, sdw::Float const & pfilterSize )
		{
			if ( !m_filterPCFArray )
			{
				m_filterPCFArray = m_writer.implementFunction< sdw::Float >( "c3d_shdFilterPCFArray"
					, [this]( sdw::Vec3 const & lightSpacePosition
						, sdw::CombinedImage2DArrayShadowR32 const & shadowMap
						, sdw::Vec2 const & invTexDim
						, sdw::UInt const & arrayIndex
						, sdw::Float const & depthBias
						, sdw::UInt const & sampleCount
						, sdw::Float const & filterSize )
					{
						auto sampleScale = m_writer.declLocale( "sampleScale"
							, filterSize * invTexDim );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 0.0_f );

						// Get a value to randomly rotate the kernel by
						auto screenPos = m_writer.declLocale( "screenPos"
							, uvec2( lightSpacePosition.xy() * invTexDim ) );
						auto randomSamplePos = m_writer.declLocale( "randomSamplePos"
							, screenPos.x() % RandomDataCount );
						auto theta = m_writer.declLocale( "theta"
							, ( *m_randomData )[randomSamplePos][screenPos.y() % 4_u] * sdw::Float{ castor::Pi< float > } );
						auto randomRotationMatrix = m_writer.declLocale( "randomRotationMatrix"
							, mat2x2( vec2( cos( theta ), -sin( theta ) )
								, vec2( sin( theta ), cos( theta ) ) ) );

						FOR( m_writer, sdw::UInt, i, 0_u, i < sampleCount, ++i )
						{
							auto sampleOffset = m_writer.declLocale( "sampleOffset"
								, ( randomRotationMatrix * m_poissonSamples[i] ) * sampleScale );
							shadowFactor += shadowMap.sample( vec3( lightSpacePosition.xy() + sampleOffset
									, m_writer.cast< sdw::Float >( arrayIndex ) )
								, lightSpacePosition.z() - depthBias );
						}
						ROF;

						m_writer.returnStmt( shadowFactor / m_writer.cast< sdw::Float >( sampleCount ) );
					}
					, sdw::InVec3{ m_writer, "lightSpacePosition" }
					, sdw::InCombinedImage2DArrayShadowR32{ m_writer, "shadowMap" }
					, sdw::InVec2{ m_writer, "invTexDim" }
					, sdw::InUInt{ m_writer, "arrayIndex" }
					, sdw::InFloat{ m_writer, "depthBias" }
					, sdw::InUInt{ m_writer, "sampleCount" }
					, sdw::InFloat{ m_writer, "filterSize" } );
			}

			return m_filterPCFArray( plightSpacePosition
				, pshadowMap
				, pinvTexDim
				, parrayIndex
				, pdepthBias
				, psampleCount
				, pfilterSize );
		}

		sdw::RetFloat Shadow::filterPCF( sdw::Vec3 const & plightToVertex
			, sdw::CombinedImageCubeArrayShadowR32 const & pshadowMap
			, sdw::Vec2 const & pinvTexDim
			, sdw::UInt const & pcubeIndex
			, sdw::Float const & pdepth
			, sdw::Float const & pdepthBias
			, sdw::UInt const & psampleCount
			, sdw::Float const & pfilterSize )
		{
			if ( !m_filterPCFCube )
			{
				m_filterPCFCube = m_writer.implementFunction< sdw::Float >( "c3d_shdFilterPCFCube"
					, [this]( sdw::Vec3 const & lightToVertex
						, sdw::CombinedImageCubeArrayShadowR32 const & shadowMap
						, sdw::Vec2 const & invTexDim
						, sdw::UInt const & cubeIndex
						, sdw::Float const & depth
						, sdw::Float const & depthBias
						, sdw::UInt const & sampleCount
						, sdw::Float const & filterSize )
					{
						int count = 0;
						int const samples = 4;

						auto offset = m_writer.declLocale( "offset"
							, ( filterSize * depth ) / 4.0_f );
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
									shadowFactor += shadowMap.sample( vec4( lightToVertex + sampleOffset
											, m_writer.cast< sdw::Float >( cubeIndex ) )
										, depth - depthBias );
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
					, sdw::InCombinedImageCubeArrayShadowR32{ m_writer, "shadowMap" }
					, sdw::InVec2{ m_writer, "invTexDim" }
					, sdw::InUInt{ m_writer, "cubeIndex" }
					, sdw::InFloat{ m_writer, "depth" }
					, sdw::InFloat{ m_writer, "depthBias" }
					, sdw::InUInt{ m_writer, "sampleCount" }
					, sdw::InFloat{ m_writer, "filterSize" } );
			}

			return m_filterPCFCube( plightToVertex
				, pshadowMap
				, pinvTexDim
				, pcubeIndex
				, pdepth
				, pdepthBias
				, psampleCount
				, pfilterSize );
		}

		sdw::RetFloat Shadow::chebyshevUpperBound( sdw::Vec2 const & pmoments
			, sdw::Float const & pdepth
			, sdw::Float const & pminVariance
			, sdw::Float const & plightBleedingReduction )
		{
			if ( !m_chebyshevUpperBound )
			{
				m_chebyshevUpperBound = m_writer.implementFunction< sdw::Float >( "c3d_shdChebyshevUpperBound"
					, [this]( sdw::Vec2 const & moments
						, sdw::Float const & depth
						, sdw::Float const & minVariance
						, sdw::Float const & lightBleedingReduction )
					{
						auto p = m_writer.declLocale( "p"
							, step( 1.0_f - moments.x(), 1.0_f - depth ) );
						auto variance = m_writer.declLocale( "variance"
							, moments.y() - ( moments.x() * moments.x() ) );
						variance = sdw::max( variance, minVariance );
						auto d = m_writer.declLocale( "d"
							, moments.x() - depth );
						variance /= variance + d * d;
						variance = clamp( ( variance - lightBleedingReduction ) / ( 1.0_f - lightBleedingReduction ), 0.0_f, 1.0_f );
						m_writer.returnStmt( m_writer.ternary( p == 0.0_f
							, 1.0_f
							, variance ) );
					}
					, sdw::InVec2{ m_writer, "moments" }
					, sdw::InFloat{ m_writer, "depth" }
					, sdw::InFloat{ m_writer, "minVariance" }
					, sdw::InFloat{ m_writer, "lightBleedingReduction" } );
			}

			return m_chebyshevUpperBound( pmoments
				, pdepth
				, pminVariance
				, plightBleedingReduction );
		}
	}
}
