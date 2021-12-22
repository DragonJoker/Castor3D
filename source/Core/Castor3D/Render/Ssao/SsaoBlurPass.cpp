#include "Castor3D/Render/Ssao/SsaoBlurPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FrameGraph.hpp>

CU_ImplementCUSmartPtr( castor3d, SsaoBlurPass )

using namespace castor;

namespace castor3d
{
	namespace
	{
		enum Idx : uint32_t
		{
			SsaoCfgUboIdx = 0u,
			GpInfoUboIdx,
			BlurCfgUboIdx,
			NmlImgIdx,
			InpImgIdx,
			BntImgIdx,
		};

		ShaderPtr getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr getPixelProgram( bool useNormalsBuffer )
		{
			using namespace sdw;
			FragmentWriter writer;

			UBO_SSAO_CONFIG( writer, SsaoCfgUboIdx, 0u );
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			Ubo configuration{ writer, "BlurConfiguration", BlurCfgUboIdx, 0u };
			/** (1, 0) or (0, 1)*/
			auto c3d_axis = configuration.declMember< IVec2 >( "c3d_axis" );
			auto c3d_dummy = configuration.declMember< IVec2 >( "c3d_dummy" );
			auto c3d_gaussian = configuration.declMember< Vec4 >( "c3d_gaussian", 2u );
			configuration.end();
			auto c3d_mapNormal = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapNormal", NmlImgIdx, 0u, useNormalsBuffer );
			auto c3d_mapInput = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapInput", InpImgIdx, 0u );
			auto c3d_mapBentInput = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBentInput", BntImgIdx, 0u );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_readMultiplyFirst = writer.declConstant( "c3d_readMultiplyFirst", vec3( 2.0_f ) );
			auto c3d_readAddSecond = writer.declConstant( "c3d_readAddSecond", vec3( 1.0_f ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec3 >( "pxl_fragColor", 0u );
			auto pxl_bentNormal = writer.declOutput< Vec3 >( "pxl_bentNormal", 1u );

#define result pxl_fragColor.r()
#define keyPassThrough pxl_fragColor.g()

			/** Returns a number on (0, 1) */
			auto unpackKey = writer.implementFunction< Float >( "unpackKey"
				, [&]( Float const & p )
				{
					writer.returnStmt( p );
				}
				, InFloat{ writer, "p" } );

			// Reconstruct camera-space P.xyz from screen-space S = (x, y) in
			// pixels and camera-space z < 0.  Assumes that the upper-left pixel center
			// is at (0.5, 0.5) [but that need not be the location at which the sample tap
			// was placed!]
			// Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
			auto reconstructCSPosition = writer.implementFunction< Vec3 >( "reconstructCSPosition"
				, [&]( Vec2 const & S
					, Float const & z
					, Vec4 const & projInfo )
				{
					writer.returnStmt( vec3( sdw::fma( S.xy(), projInfo.xy(), projInfo.zw() ) * z, z ) );
				}
				, InVec2{ writer, "S" }
				, InFloat{ writer, "z" }
				, InVec4{ writer, "projInfo" } );

			auto positionFromKey = writer.implementFunction< Vec3 >( "positionFromKey"
				, [&]( Float const & key
					, IVec2 const & ssCenter
					, Vec4 const & projInfo )
				{
					auto z = writer.declLocale( "z"
						, key * c3d_ssaoConfigData.farPlaneZ );
					auto position = writer.declLocale( "position"
						, reconstructCSPosition( vec2( ssCenter ) + vec2( 0.5_f )
							, z
							, projInfo ) );
					writer.returnStmt( position );
				}
				, InFloat{ writer, "key" }
				, InIVec2{ writer, "ssCenter" }
				, InVec4{ writer, "projInfo" } );

			auto getTapInformation = writer.implementFunction< Vec3 >( "getTapInformation"
				, [&]( IVec2 const & tapLoc
					, Float tapKey
					, Float value
					, Vec3 bent )
				{
					auto temp = writer.declLocale( "temp"
						, c3d_mapInput.fetch( tapLoc, 0_i ).rgb() );
					tapKey = unpackKey( temp.g() );
					value = temp.r();

					if ( useNormalsBuffer )
					{
						temp = c3d_mapNormal.fetch( tapLoc, 0_i ).xyz();
						temp = normalize( sdw::fma( temp, c3d_readMultiplyFirst.xyz(), c3d_readAddSecond.xyz() ) );
					}
					else
					{
						temp = vec3( 0.0_f );
					}

					bent = vec3( 0.0_f );
					writer.returnStmt( temp );
				}
				, InIVec2{ writer, "tapLoc" }
				, OutFloat{ writer, "tapKey" }
				, OutFloat{ writer, "value" }
				, OutVec3{ writer, "bent" } );

			auto square = writer.implementFunction< Float >( "square"
				, [&]( Float const & x )
				{
					writer.returnStmt( x * x );
				}
				, InFloat{ writer, "x" } );

			auto calculateBilateralWeight = writer.implementFunction< Float >( "calculateBilateralWeight"
				, [&]( Float const & key
					, Float const & tapKey
					, IVec2 const & tapLoc
					, Vec3 const & normal
					, Vec3 const & tapNormal
					, Vec3 const & position )
				{
					auto scale = writer.declLocale( "scale"
						, 1.5_f * c3d_ssaoConfigData.invRadius );

					// The "bilateral" weight. As depth difference increases, decrease weight.
					// The key values are in scene-specific scale. To make them scale-invariant, factor in
					// the AO radius, which should be based on the scene scale.
					auto depthWeight = writer.declLocale( "depthWeight"
						, max( 0.0_f, 1.0_f - ( c3d_ssaoConfigData.edgeSharpness * 2000.0_f ) * abs( tapKey - key ) * scale ) );
					auto k_normal = writer.declLocale( "k_normal"
						, 1.0_f );
					auto k_plane = writer.declLocale( "k_plane"
						, 1.0_f );

					// Prevents blending over creases. 
					auto normalWeight = writer.declLocale( "normalWeight"
						, 1.0_f );
					auto planeWeight = writer.declLocale( "planeWeight"
						, 1.0_f );

					if ( useNormalsBuffer )
					{
						auto normalCloseness = writer.declLocale( "normalCloseness"
							, dot( tapNormal, normal ) );

						IF( writer, c3d_ssaoConfigData.blurHighQuality == 0_i )
						{
							normalCloseness = normalCloseness * normalCloseness;
							normalCloseness = normalCloseness * normalCloseness;
							k_normal = 4.0_f;
						}
						FI;

						auto normalError = writer.declLocale( "normalError"
							, ( 1.0_f - normalCloseness ) * k_normal );
						normalWeight = max( 1.0_f - c3d_ssaoConfigData.edgeSharpness * normalError, 0.0_f );

						IF( writer, c3d_ssaoConfigData.blurHighQuality )
						{
							auto lowDistanceThreshold2 = writer.declLocale( "lowDistanceThreshold2"
								, 0.001_f );

							auto tapPosition = writer.declLocale( "tapPosition"
								, positionFromKey( tapKey, tapLoc, c3d_ssaoConfigData.projInfo ) );

							// Change position in camera space
							auto dq = writer.declLocale( "dq"
								, position - tapPosition );

							// How far away is this point from the original sample
							// in camera space? (Max value is unbounded)
							auto distance2 = writer.declLocale( "distance2"
								, dot( dq, dq ) );

							// How far off the expected plane (on the perpendicular) is this point?  Max value is unbounded.
							auto planeError = writer.declLocale( "planeError"
								, max( abs( dot( dq, tapNormal ) ), abs( dot( dq, normal ) ) ) );

							// Minimum distance threshold must be scale-invariant, so factor in the radius
							planeWeight = writer.ternary( distance2 * square( scale ) < lowDistanceThreshold2
								, 1.0_f
								, Float{ pow( max( 0.0_f
										, 1.0_f - c3d_ssaoConfigData.edgeSharpness * 2.0 * k_plane * planeError / sqrt( distance2 ) )
									, 2.0_f ) } );
						}
						FI;
					}

					writer.returnStmt( depthWeight * normalWeight * planeWeight );
				}
				, InFloat{ writer, "key" }
				, InFloat{ writer, "tapKey" }
				, InIVec2{ writer, "tapLoc" }
				, InVec3{ writer, "normal" }
				, InVec3{ writer, "tapNormal" }
				, InVec3{ writer, "position" } );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto ssCenter = writer.declLocale( "ssCenter"
						, ivec2( in.fragCoord.xy() ) );

					auto temp = writer.declLocale( "temp"
						, c3d_mapInput.fetch( ssCenter, 0_i ) );
					auto sum = writer.declLocale( "sum"
						, temp.r() );
					auto bentNormal = writer.declLocale( "bentNormal"
						, c3d_gpInfoData.readNormal( c3d_mapBentInput.fetch( ssCenter, 0_i ).xyz() ) );

					keyPassThrough = temp.g();
					auto key = writer.declLocale( "key"
						, unpackKey( keyPassThrough ) );
					auto normal = writer.declLocale( "normal"
						, vec3( 0.0_f ) );

					if ( useNormalsBuffer )
					{
						normal = normalize( c3d_gpInfoData.readNormal( c3d_mapNormal.fetch( ssCenter, 0_i ).xyz() ) );
					}

					IF( writer, key == 1.0_f )
					{
						// Sky pixel (if you aren't using depth keying, disable this test)
						result = sum;
						pxl_bentNormal = c3d_gpInfoData.writeNormal( bentNormal );
						writer.returnStmt();
					}
					FI;

					// Base weight for depth falloff.  Increase this for more blurriness,
					// decrease it for better edge discrimination
					auto BASE = writer.declLocale( "BASE"
						, c3d_gaussian[0_u][0_u] );
					auto totalWeight = writer.declLocale( "totalWeight"
						, BASE );
					sum *= totalWeight;
					bentNormal *= totalWeight;

					auto position = writer.declLocale( "position"
						, positionFromKey( key, ssCenter, c3d_ssaoConfigData.projInfo ) );

					FOR( writer, Int, r, -c3d_ssaoConfigData.blurRadius, r <= c3d_ssaoConfigData.blurRadius, ++r )
					{
						// We already handled the zero case above.  This loop should be unrolled and the static branch optimized out,
						// so the IF statement has no runtime cost
						IF( writer, r != 0_i )
						{
							auto tapLoc = writer.declLocale( "tapLoc"
								, ssCenter + c3d_axis * ( r * c3d_ssaoConfigData.blurStepSize ) );

							// spatial domain: offset gaussian tap
							auto absR = writer.declLocale( "absR"
								, writer.cast< UInt >( abs( r ) ) );
							auto weight = writer.declLocale( "weight"
								, 0.3_f + c3d_gaussian[absR % 2_u][absR / 2_u] );

							auto tapKey = writer.declLocale< Float >( "tapKey" );
							auto value = writer.declLocale< Float >( "value" );
							auto bent = writer.declLocale( "bent"
								, vec3( 0.0_f ) );
							auto tapNormal = writer.declLocale( "tapNormal"
								, getTapInformation( tapLoc, tapKey, value, bent ) );

							auto bilateralWeight = writer.declLocale( "bilateralWeight"
								, calculateBilateralWeight( key
									, tapKey
									, tapLoc
									, normal
									, tapNormal
									, position ) );

							weight *= bilateralWeight;
							sum += value * weight;
							bentNormal += bent * weight;
							totalWeight += weight;
						}
						FI;
					}
					ROF;

					auto const epsilon = writer.declLocale( "epsilon"
						, 0.0001_f );
					result = sum / ( totalWeight + epsilon );
					bentNormal /= ( totalWeight + epsilon );
					pxl_bentNormal = c3d_gpInfoData.writeNormal( bentNormal );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );

#undef result
#undef keyPassThrough
		}

		Texture doCreateTexture( RenderDevice const & device
			, crg::ResourceHandler & handler
			, String const & name
			, VkFormat format
			, VkExtent2D const & size
			, bool transferDst )
		{
			return { device
				, handler
				, name
				, 0u
				, { size.width, size.height, 1u }
				, 1u
				, 1u
				, format
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
					| VkImageUsageFlags( transferDst ? VK_IMAGE_USAGE_TRANSFER_DST_BIT : VkImageUsageFlagBits{} ) ) };
		}

		castor::String getName( bool useNormalsBuffer )
		{
			return "SsaoBlur" + ( useNormalsBuffer ? std::string{ "Nml" } : std::string{} );
		}

		crg::rq::Config getConfig( VkExtent2D const & renderSize
			, SsaoConfig const & ssaoConfig
			, ashes::PipelineShaderStageCreateInfoArray const & stages0
			, ashes::PipelineShaderStageCreateInfoArray const & stages1 )
		{
			crg::rq::Config result;
			result.renderSize( renderSize );
			result.enabled( &ssaoConfig.enabled );
			result.programs( { crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages0 )
				, crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages1 ) } );
			return result;
		}

		crg::ru::Config makeConfig( bool isVertical
			, crg::Attachment const & attach
			, crg::Attachment const & bentAttach )
		{
			crg::ru::Config result{ 2u, false };

			if ( isVertical )
			{
				result.implicitAction( attach.view()
					, crg::RecordContext::clearAttachment( attach ) );
				result.implicitAction( bentAttach.view()
					, crg::RecordContext::clearAttachment( bentAttach ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	SsaoBlurPass::RenderQuad::RenderQuad( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, crg::rq::Config rqConfig
		, crg::ru::Config ruConfig
		, SsaoConfig const & ssaoConfig )
		: crg::RenderQuad{ pass
			, context
			, graph
			, std::move( ruConfig )
			, std::move( rqConfig ) }
		, ssaoConfig{ ssaoConfig }
	{
	}

	uint32_t SsaoBlurPass::RenderQuad::doGetPassIndex()const
	{
		return ssaoConfig.useNormalsBuffer ? 1u : 0u;
	}

	//*********************************************************************************************

	SsaoBlurPass::Program::Program( RenderDevice const & device
		, bool useNormalsBuffer
		, castor::String const & prefix )
		: vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, prefix + getName( useNormalsBuffer )
			, getVertexProgram() }
		, pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, prefix + getName( useNormalsBuffer )
			, getPixelProgram( useNormalsBuffer ) }
		, stages{ makeShaderState( device, vertexShader )
			, makeShaderState( device, pixelShader ) }
	{
	}

	//*********************************************************************************************

	SsaoBlurPass::SsaoBlurPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, crg::FramePass const & previousPass
		, String const & prefix
		, VkExtent2D const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, Point2i const & axis
		, Texture const & input
		, Texture const & bentInput
		, Texture const & normals )
		: m_device{ device }
		, m_graph{ graph }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_bentInput{ bentInput }
		, m_config{ config }
		, m_size{ size }
		, m_result{ doCreateTexture( m_device, graph.getHandler(), graph.getName() + "SsaoBlur" + prefix, SsaoBlurPass::ResultFormat, m_size, axis->y != 0 ) }
		, m_bentResult{ doCreateTexture( m_device, graph.getHandler(), graph.getName() + "SsaoBentNormals" + prefix, m_bentInput.getFormat(), m_size, axis->y != 0 ) }
		, m_configurationUbo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
		, m_programs{ Program{ device, false, graph.getName() }, Program{ device, true, graph.getName() } }
	{
		stepProgressBar( progress, "Creating " + graph.getName() + " SSAO " + prefix + " blur pass" );
		auto & configuration = m_configurationUbo.getData();
		configuration.axis = axis;
		auto & pass = graph.createPass( "SsaoBlur" + prefix
			, [this, progress, prefix, config, axis]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising SSAO " + prefix + " blur pass" );
				auto bentResIt = pass.images.rbegin();
				auto resIt = std::next( bentResIt );
				auto result = std::make_unique< RenderQuad >( pass
					, context
					, graph
					, getConfig( m_size
						, config
						, m_programs[0].stages
						, m_programs[1].stages )
					, makeConfig( axis->y != 0
						, *resIt
						, *bentResIt )
					, m_config );
				m_device.renderSystem.getEngine()->registerTimer( graph.getName() + "/SSAO"
					, result->getTimer() );
				return result;
			} );
		m_lastPass = &pass;
		pass.addDependency( previousPass );
		m_ssaoConfigUbo.createPassBinding( pass, SsaoCfgUboIdx );
		m_gpInfoUbo.createPassBinding( pass, GpInfoUboIdx );
		m_configurationUbo.createPassBinding( pass, "SsaoBlurCfg", BlurCfgUboIdx );
		pass.addSampledView( normals.sampledViewId, NmlImgIdx );
		pass.addSampledView( input.sampledViewId, InpImgIdx );
		pass.addSampledView( bentInput.sampledViewId, BntImgIdx );
		pass.addOutputColourView( m_result.targetViewId, opaqueWhiteClearColor );
		pass.addOutputColourView( m_bentResult.targetViewId, transparentBlackClearColor );
		m_result.create();
		m_bentResult.create();
	}

	SsaoBlurPass::~SsaoBlurPass()
	{
		m_bentResult.destroy();
		m_result.destroy();
	}

	void SsaoBlurPass::update( CpuUpdater & updater )
	{
		if ( m_config.blurRadius.isDirty() )
		{
			auto & configuration = m_configurationUbo.getData();

			switch ( m_config.blurRadius.value().value() )
			{
			case 1u:
				configuration.gaussian[0][0] = 0.5f;
				configuration.gaussian[0][1] = 0.25f;
				break;
			case 2u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				break;
			case 3u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				configuration.gaussian[0][3] = 0.092902f;
				break;
			case 4u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				configuration.gaussian[0][3] = 0.092902f;
				configuration.gaussian[1][0] = 0.062970f;
				break;
			case 5u:
				configuration.gaussian[0][0] = 0.111220f;
				configuration.gaussian[0][1] = 0.107798f;
				configuration.gaussian[0][2] = 0.098151f;
				configuration.gaussian[0][3] = 0.083953f;
				configuration.gaussian[1][0] = 0.067458f;
				configuration.gaussian[1][1] = 0.050920f;
				break;
			default:
				configuration.gaussian[0][0] = 0.111220f;
				configuration.gaussian[0][1] = 0.107798f;
				configuration.gaussian[0][2] = 0.098151f;
				configuration.gaussian[0][3] = 0.083953f;
				configuration.gaussian[1][0] = 0.067458f;
				configuration.gaussian[1][1] = 0.050920f;
				configuration.gaussian[1][2] = 0.036108f;
				break;
			}
		}
	}

	void SsaoBlurPass::accept( bool horizontal
		, SsaoConfig & config
		, PipelineVisitorBase & visitor )
	{
		if ( horizontal )
		{
			visitor.visit( "SSAO HBlurred AO"
				, getResult()
				, m_graph.getFinalLayoutState( getResult().sampledViewId ).layout
				, TextureFactors{}.invert( true ) );
		}
		else
		{
			visitor.visit( "SSAO Blurred AO"
				, getResult()
				, m_graph.getFinalLayoutState( getResult().sampledViewId ).layout
				, TextureFactors{}.invert( true ) );
		}

		if ( horizontal )
		{
			visitor.visit( "HBlurred Bent Normals"
				, getBentResult()
				, m_graph.getFinalLayoutState( getBentResult().sampledViewId ).layout
				, TextureFactors{}.invert( true ) );
		}
		else
		{
			visitor.visit( "Blurred Bent Normals"
				, getBentResult()
				, m_graph.getFinalLayoutState( getBentResult().sampledViewId ).layout
				, TextureFactors{}.invert( true ) );
		}

		if ( m_config.useNormalsBuffer )
		{
			visitor.visit( m_programs[1].vertexShader );
			visitor.visit( m_programs[1].pixelShader );
		}
		else
		{
			visitor.visit( m_programs[0].vertexShader );
			visitor.visit( m_programs[0].pixelShader );
		}

		config.accept( "SsaoBlur", visitor );
	}
}
