#include "SmaaPostEffect/LumaEdgeDetection.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"
#include "SmaaPostEffect/SMAA.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/makeVkType.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Texture/Sampler.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace smaa
{
	namespace
	{
		std::unique_ptr< sdw::Shader > doGetEdgeDetectionFPPredication( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( config.data.threshold ) );
			auto c3d_localContrastAdaptationFactor = writer.declConstant( constants::LocalContrastAdaptationFactor
				, Float( config.data.localContrastAdaptationFactor ) );
			auto c3d_predicationThreshold = writer.declConstant( constants::PredicationThreshold
				, Float( config.data.predicationThreshold ) );
			auto c3d_predicationScale = writer.declConstant( constants::PredicationScale
				, Float( config.data.predicationScale ) );
			auto c3d_predicationStrength = writer.declConstant( constants::PredicationStrength
				, Float( config.data.predicationStrength ) );
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( "vtx_offset", 1u, 3u );
			auto c3d_colourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_colourTex", 0u, 0u );
			auto c3d_predicationTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_predicationTex", 1u, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< Vec3 >( "SMAAGatherNeighbours"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & tex )
				{
					writer.returnStmt( textureGather( tex, texcoord + c3d_rtMetrics.xy() * vec2( -0.5_f, -0.5_f ) ).grb() );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "tex" } );

			/**
			 * Adjusts the threshold by means of predication.
			 */
			auto SMAACalculatePredicatedThreshold = writer.implementFunction< Vec2 >( "SMAACalculatePredicatedThreshold"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & predicationTex )
				{
					auto neighbours = writer.declLocale( "neighbours"
						, SMAAGatherNeighbours( texcoord, offset, predicationTex ) );
					auto delta = writer.declLocale( "delta"
						, abs( neighbours.xx() - neighbours.yz() ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( c3d_predicationThreshold ), delta ) );
					writer.returnStmt( c3d_predicationScale * c3d_threshold * writer.paren( 1.0_f - c3d_predicationStrength * edges ) );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "predicationTex" } );

			/**
			 * Luma Edge Detection
			 *
			 * IMPORTANT NOTICE: luma edge detection requires gamma-corrected colors, and
			 * thus 'colorTex' should be a non-sRGB texture.
			 */
			auto SMAALumaEdgeDetectionPS = writer.implementFunction< Vec2 >( "SMAALumaEdgeDetectionPS"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset )
				{
					// Calculate the threshold:
					auto threshold = writer.declLocale< Vec2 >( "threshold"
						, SMAACalculatePredicatedThreshold( texcoord, offset, c3d_predicationTex ) );

					// Calculate lumas:
					auto weights = writer.declLocale( "weights"
						, vec3( 0.2126_f, 0.7152, 0.0722 ) );
					auto L = writer.declLocale( "L"
						, dot( texture( c3d_colourTex, texcoord ).rgb(), weights ) );

					auto Lleft = writer.declLocale( "Lleft"
						, dot( texture( c3d_colourTex, offset[0].xy() ).rgb(), weights ) );
					auto Ltop = writer.declLocale( "Ltop"
						, dot( texture( c3d_colourTex, offset[0].zw() ).rgb(), weights ) );

					// We do the usual threshold:
					auto delta = writer.declLocale< Vec4 >( "delta" );
					delta.xy() = abs( L - vec2( Lleft, Ltop ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( threshold ), delta.xy() ) );

					// Then discard if there is no edge:
					IF( writer, dot( edges, vec2( 1.0_f, 1.0 ) ) == 0.0_f )
					{
						writer.discard();
					}
					FI;

					// Calculate right and bottom deltas:
					auto Lright = writer.declLocale( "Lright"
						, dot( texture( c3d_colourTex, offset[1].xy() ).rgb(), weights ) );
					auto Lbottom = writer.declLocale( "Lbottom"
						, dot( texture( c3d_colourTex, offset[1].zw() ).rgb(), weights ) );
					delta.zw() = abs( L - vec2( Lright, Lbottom ) );

					// Calculate the maximum delta in the direct neighborhood:
					auto maxDelta = writer.declLocale( "maxDelta"
						, max( delta.xy(), delta.zw() ) );

					// Calculate left-left and top-top deltas:
					auto Lleftleft = writer.declLocale( "Lleftleft"
						, dot( texture( c3d_colourTex, offset[2].xy() ).rgb(), weights ) );
					auto Ltoptop = writer.declLocale( "Ltoptop"
						, dot( texture( c3d_colourTex, offset[2].zw() ).rgb(), weights ) );
					delta.zw() = abs( vec2( Lleft, Ltop ) - vec2( Lleftleft, Ltoptop ) );

					// Calculate the final maximum delta:
					maxDelta = max( maxDelta.xy(), delta.zw() );
					auto finalDelta = writer.declLocale( "finalDelta"
						, max( maxDelta.x(), maxDelta.y() ) );

					// Local contrast adaptation:
					edges.xy() *= step( vec2( finalDelta ), c3d_localContrastAdaptationFactor * delta.xy() );

					writer.returnStmt( edges );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = vec4( 0.0_f );
					pxl_fragColour.xy() = SMAALumaEdgeDetectionPS( vtx_texture, vtx_offset );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > doGetEdgeDetectionFPNoPredication( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( config.data.threshold ) );
			auto c3d_localContrastAdaptationFactor = writer.declConstant( constants::LocalContrastAdaptationFactor
				, Float( config.data.localContrastAdaptationFactor ) );
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( "vtx_offset", 1u, 3u );
			auto c3d_colourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_colourTex", 0u, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< Vec3 >( "SMAAGatherNeighbours"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & tex )
				{
					writer.returnStmt( textureGather( tex, texcoord + c3d_rtMetrics.xy() * vec2( -0.5_f, -0.5_f ) ).grb() );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "tex" } );

			/**
			 * Luma Edge Detection
			 *
			 * IMPORTANT NOTICE: luma edge detection requires gamma-corrected colors, and
			 * thus 'colorTex' should be a non-sRGB texture.
			 */
			auto SMAALumaEdgeDetectionPS = writer.implementFunction< Vec2 >( "SMAALumaEdgeDetectionPS"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset )
				{
					// Calculate the threshold:
					auto threshold = writer.declLocale< Vec2 >( "threshold"
						, vec2( c3d_threshold, c3d_threshold ) );

					// Calculate lumas:
					auto weights = writer.declLocale( "weights"
						, vec3( 0.2126_f, 0.7152, 0.0722 ) );
					auto L = writer.declLocale( "L"
						, dot( texture( c3d_colourTex, texcoord ).rgb(), weights ) );

					auto Lleft = writer.declLocale( "Lleft"
						, dot( texture( c3d_colourTex, offset[0].xy() ).rgb(), weights ) );
					auto Ltop = writer.declLocale( "Ltop"
						, dot( texture( c3d_colourTex, offset[0].zw() ).rgb(), weights ) );

					// We do the usual threshold:
					auto delta = writer.declLocale< Vec4 >( "delta" );
					delta.xy() = abs( L - vec2( Lleft, Ltop ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( threshold ), delta.xy() ) );

					// Then discard if there is no edge:
					IF( writer, dot( edges, vec2( 1.0_f, 1.0 ) ) == 0.0_f )
					{
						writer.discard();
					}
					FI;

					// Calculate right and bottom deltas:
					auto Lright = writer.declLocale( "Lright"
						, dot( texture( c3d_colourTex, offset[1].xy() ).rgb(), weights ) );
					auto Lbottom = writer.declLocale( "Lbottom"
						, dot( texture( c3d_colourTex, offset[1].zw() ).rgb(), weights ) );
					delta.zw() = abs( L - vec2( Lright, Lbottom ) );

					// Calculate the maximum delta in the direct neighborhood:
					auto maxDelta = writer.declLocale( "maxDelta"
						, max( delta.xy(), delta.zw() ) );

					// Calculate left-left and top-top deltas:
					auto Lleftleft = writer.declLocale( "Lleftleft"
						, dot( texture( c3d_colourTex, offset[2].xy() ).rgb(), weights ) );
					auto Ltoptop = writer.declLocale( "Ltoptop"
						, dot( texture( c3d_colourTex, offset[2].zw() ).rgb(), weights ) );
					delta.zw() = abs( vec2( Lleft, Ltop ) - vec2( Lleftleft, Ltoptop ) );

					// Calculate the final maximum delta:
					maxDelta = max( maxDelta.xy(), delta.zw() );
					auto finalDelta = writer.declLocale( "finalDelta"
						, max( maxDelta.x(), maxDelta.y() ) );

					// Local contrast adaptation:
					edges.xy() *= step( vec2( finalDelta ), c3d_localContrastAdaptationFactor * delta.xy() );

					writer.returnStmt( edges );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = vec4( 0.0_f );
					pxl_fragColour.xy() = SMAALumaEdgeDetectionPS( vtx_texture, vtx_offset );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::ImageView doCreatePredicationView( ashes::Image const & texture )
		{
			ashes::ImageViewCreateInfo view
			{
				0u,
				texture,
				VK_IMAGE_VIEW_TYPE_2D,
				texture.getFormat(),
				VkComponentMapping{},
				{ VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u },
			};
			return texture.createView( view );
		}
	}

	//*********************************************************************************************

	LumaEdgeDetection::LumaEdgeDetection( castor3d::RenderTarget & renderTarget
		, ashes::ImageView const & colourView
		, ashes::Image const * predication
		, SmaaConfig const & config )
		: EdgeDetection{ renderTarget, config }
		, m_colourView{ colourView }
		, m_predicationView{ predication ? std::make_unique< ashes::ImageView >( doCreatePredicationView( *predication ) ) : nullptr }
	{
		VkExtent2D size{ renderTarget.getSize().getWidth()
			, renderTarget.getSize().getHeight() };
		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_pixelShader.shader = predication
			? doGetEdgeDetectionFPPredication( *renderTarget.getEngine()->getRenderSystem()
				, pixelSize
				, config )
			: doGetEdgeDetectionFPNoPredication( *renderTarget.getEngine()->getRenderSystem()
				, pixelSize
				, config );
		doInitialisePipeline();
	}

	castor3d::CommandsSemaphore LumaEdgeDetection::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		castor3d::CommandsSemaphore edgeDetectionCommands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & edgeDetectionCmd = *edgeDetectionCommands.commandBuffer;

		edgeDetectionCmd.begin();
		timer.beginPass( edgeDetectionCmd, passIndex );
		// Put source image in shader input layout.
		edgeDetectionCmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_colourView.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );

		if ( m_predicationView )
		{
			auto & view = *m_predicationView;
			auto subresource = view->subresourceRange;
			subresource.aspectMask = ashes::getAspectMask( view->format );
			auto barrier = castor3d::makeVkType< VkImageMemoryBarrier >( VkAccessFlags( 0u )
				, VkAccessFlags( VK_ACCESS_SHADER_READ_BIT )
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, *view.image
				, subresource );
			edgeDetectionCmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, barrier );
		}

		edgeDetectionCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, {
				castor3d::makeClearValue( 0.0, 0.0, 0.0, 0.0 ),
				castor3d::defaultClearDepthStencilValue,
			}
			, VK_SUBPASS_CONTENTS_INLINE );
		registerFrame( edgeDetectionCmd );
		edgeDetectionCmd.endRenderPass();
		timer.endPass( edgeDetectionCmd, passIndex );
		edgeDetectionCmd.end();

		return std::move( edgeDetectionCommands );
	}

	void LumaEdgeDetection::doInitialisePipeline()
	{
		VkExtent2D size{ m_colourView.image->getDimensions().width
			, m_colourView.image->getDimensions().height };
		auto & device = getCurrentRenderDevice( m_renderSystem );
		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_vertexShader ) );
		stages.push_back( makeShaderState( device, m_pixelShader ) );

		ashes::PipelineDepthStencilStateCreateInfo dsstate
		{
			0u,
			VK_FALSE,
			VK_FALSE,
			VK_COMPARE_OP_LESS,
			VK_FALSE,
			VK_TRUE,
			{
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_REPLACE,
				VK_STENCIL_OP_KEEP,
				VK_COMPARE_OP_ALWAYS,
				0xFFFFFFFFu,
				0xFFFFFFFFu,
				1u,
			},
			{
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_REPLACE,
				VK_STENCIL_OP_KEEP,
				VK_COMPARE_OP_ALWAYS,
				0xFFFFFFFFu,
				0xFFFFFFFFu,
				1u,
			},
		};
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings;
		auto * view = &m_colourView;

		if ( m_predicationView )
		{
			setLayoutBindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			view = m_predicationView.get();
		}

		createPipeline( size
			, castor::Position{}
			, stages
			, *view
			, *m_renderPass
			, std::move( setLayoutBindings )
			, {}
			, std::move( dsstate ) );
	}

	void LumaEdgeDetection::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		if ( m_predicationView )
		{
			descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
				, m_colourView
				, m_sampler->getSampler() );
		}
	}
}
