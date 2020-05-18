#include "Castor3D/Render/Ssgi/SsgiRawGIPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"
#include "Castor3D/Render/Ssgi/SsgiConfig.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static constexpr uint32_t HdrMapIdx = 0u;
		static constexpr uint32_t DepthMapIdx = 1u;
#if Ssgi_CulledBuffer
		static constexpr uint32_t HdrCulledMapIdx = 3u;
		static constexpr uint32_t DepthCulledMapIdx = 4u;
#endif

		std::unique_ptr< ast::Shader > getVertexProgram( RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram( RenderSystem const & renderSystem
			, uint32_t width
			, uint32_t height )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_hdrMap = writer.declSampledImage< FImg2DRgba32 >( "c3d_hdrMap", HdrMapIdx, 0u );
			auto c3d_depthMap = writer.declSampledImage< FImg2DR32 >( "c3d_depthMap", DepthMapIdx, 0u );
#if Ssgi_CulledBuffer
			auto c3d_hdrCulledMap = writer.declSampledImage< FImg2DRgba32 >( "c3d_hdrCulledMap", HdrCulledMapIdx, 0u );
			auto c3d_depthCulledMap = writer.declSampledImage< FImg2DR32 >( "c3d_depthCulledMap", DepthCulledMapIdx, 0u );
#endif
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto c3d_size = writer.declConstant( "c3d_size"
				, vec2( Float{ float( width ) }, Float{ float( height ) } ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			auto decodeDepth = writer.implementFunction< Float >( "decodeDepth"
				, [&]( Float value )
				{
					writer.returnStmt( value );
				}
				, InFloat{ writer, "value" } );

			auto normalFromDepth = writer.implementFunction< Vec3 >( "normalFromDepth"
				, [&]( Float refDepth
					, SampledImage2DR32 tex
					, Vec2 texcoords )
				{
					// Delta coordinate of 1 pixel: 0.03125 = 1 (pixel) / 32 (pixels)
					auto offset1 = writer.declConstant( "offset1"
						, vec2( 0.0_f, Float( 1.0f / height ) ) );
					auto offset2 = writer.declConstant( "offset2"
						, vec2( Float( 1.0f / width ), 0.0 ) );

					// Fetch depth from depth buffer
					auto depth1 = writer.declLocale( "depth1"
						, decodeDepth( texture( tex, texcoords + offset1 ) ) );
					auto depth2 = writer.declLocale( "depth2"
						, decodeDepth( texture( tex, texcoords + offset2 ) ) );

					auto p1 = writer.declLocale( "p1"
						, vec3( offset1, depth1 - refDepth ) );
					auto p2 = writer.declLocale( "p2"
						, vec3( offset2, depth2 - refDepth ) );

					// Calculate normal
					auto normal = writer.declLocale< Vec3 >( "normal"
						, cross( p1, p2 ) );
					normal.z() = -normal.z();

					writer.returnStmt( normalize( normal ) );
				}
				, InFloat{ writer, "refDepth" }
				, InSampledImage2DR32{ writer, "tex" }
				, InVec2{ writer, "texcoords" } );

			auto normalFromPixels = writer.implementFunction< Vec3 >( "normalFromPixels"
				, [&]( SampledImage2DR32 tex
					, Vec2 texcoords
					, Float refDepth
					, Float dist )
				{
					// Fetch depth from depth buffer
					auto depth1 = writer.declLocale( "depth1"
						, decodeDepth( texture( tex, texcoords ) ) );
					// Calculate normal
					auto normal = writer.declLocale< Vec3 >( "normal"
						, vec3( vtx_texture - texcoords, refDepth - depth1 ) );
					normal.z() = -normal.z();

					// Calculate distance between texcoords
					dist = length( normal );

					writer.returnStmt( normalize( normal ) );
				}
				, InSampledImage2DR32{ writer, "tex" }
				, InVec2{ writer, "texcoords1" }
				, InFloat{ writer, "refDepth" }
				, OutFloat{ writer, "dist" } );

			auto calculateGI = writer.implementFunction< Vec3 >( "calculateGI"
				, [&]( Vec3 pixelNormal
					, Vec2 coord
					, Float refDepth )
				{
					auto dist = writer.declLocale< Float >( "dist" );

					// Calculate normal from the pixel to current pixel
					auto lightToPixelNormal = writer.declLocale( "lightToPixelNormal"
						, normalFromPixels( c3d_depthMap, coord, refDepth, dist ) );
					// Calculate normal from current pixel to the pixel
					auto pixelToLightNormal = writer.declLocale( "pixelToLightNormal"
						, -lightToPixelNormal );

					// Get the pixel color
					auto lightColor = writer.declLocale( "lightColor"
						, texture( c3d_hdrMap, coord ).rgb() );
					// Calculate normal for the pixel
					auto lightNormal = writer.declLocale( "lightNormal"
						, normalFromDepth( refDepth, c3d_depthMap, coord ).xyz() );
					// Calculate GI
					auto gi = writer.declLocale( "gi"
						, ( lightColor
							* max( 0.0_f, dot( lightNormal, lightToPixelNormal ) )
							* max( 0.0_f, dot( pixelNormal, pixelToLightNormal ) ) / dist ) );

#if Ssgi_CulledBuffer
					// Calculate normal from the cull pixel to current pixel
					lightToPixelNormal = normalFromPixels( c3d_depthCulledMap, coord, refDepth, dist );
					// Calculate normal from current pixel to the cull pixel
					pixelToLightNormal = -lightToPixelNormal;

					// Get the cull pixel color, base color need to be lighten to simulate direct light effect.
					lightColor = texture2D( c3d_hdrCulledMap, coord ).rgb() * 5.0_f;
					// Calculate normal for the cull pixel
					lightNormal = normalFromDepth( c3d_depthCulledMap, coord );
					// Flip the normal
					lightNormal = -lightNormal;

					// Calculate GI
					gi += lightColor
						* max( 0.0_f, dot( lightNormal, lightToPixelNormal ) )
						* max( 0.0_f, dot( pixel_normal, pixelToLightNormal ) ) / dist;
#endif
					writer.returnStmt( gi );
				}
				, InVec3{ writer, "pixelNormal" }
				, InVec2{ writer, "coord" }
				, InFloat{ writer, "refDepth" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					const int GRID_COUNT = 16;

					auto refDepth = writer.declLocale( "refDepth"
						, decodeDepth( texture( c3d_depthMap, vtx_texture ) ) );
					// Calculate normal for current pixel
					auto pixelNormal = writer.declLocale( "pixelNormal"
						, normalFromDepth( refDepth, c3d_depthMap, vtx_texture ).xyz() );
					// Prepare to accumulate GI
					auto gi = writer.declLocale( "gi"
						, vec3( 0.0_f ) );
					auto offset = writer.declLocale( "offset"
						, vec2( vtx_texture - ( vec2( Float{ GRID_COUNT / 2.0f } ) / c3d_size ) ) );

					// Accumulate GI from some uniform samples
					for ( int y = 0; y <= GRID_COUNT; ++y )
					{
						for ( int x = 0; x < GRID_COUNT; ++x )
						{
							gi += calculateGI( pixelNormal
								, offset + vec2( float( x ) + 0.5_f, float( y ) + 0.5_f ) / c3d_size
								, refDepth );
						}
					}

					// Make GI not too strong
					gi /= writer.cast< Float >( GRID_COUNT * GRID_COUNT / 3 );

					pxl_fragColor = vec4( gi, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				}
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
					{},
					ashes::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( "SsgiRawGI"
				, std::move( createInfo ) );
			return result;
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, VkSamplerAddressMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_LINEAR );
				sampler->setMagFilter( VK_FILTER_LINEAR );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size
			, bool transfer )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, name, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );

			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size.width, size.height, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT 
					| ( transfer ? VK_IMAGE_USAGE_TRANSFER_DST_BIT : 0u ) ),
			};
			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( texture.getTexture()->getDefaultView().getView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( "SsgiRawGI"
				, VkExtent2D{ size.width, size.height }
			, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	SsgiRawGIPass::SsgiRawGIPass( Engine & engine
		, VkExtent2D const & size
		, SsgiConfig const & config
		, TextureLayout const & hdrResult
		, TextureLayout const & linearisedDepth )
		: RenderQuad{ *engine.getRenderSystem(), "SsgiRawGI", VK_FILTER_LINEAR, { ashes::nullopt, RenderQuadConfig::Texcoord{} } }
		, m_hdrResult{ hdrResult }
		, m_linearisedDepth{ linearisedDepth }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram( m_renderSystem ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram( m_renderSystem, size.width, size.height ) }
		, m_renderPass{ doCreateRenderPass( getCurrentRenderDevice( m_renderSystem ), ResultFormat ) }
		, m_result{ doCreateTexture( engine, getName() + "Result", ResultFormat, size, false ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_result ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "SSGI" ), cuT( "Raw GI" ) ) }
		, m_finished{ getCurrentRenderDevice( engine )->createSemaphore( getName() ) }
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		ashes::PipelineShaderStageCreateInfoArray shaderStages;
		shaderStages.push_back( makeShaderState( device, m_vertexShader ) );
		shaderStages.push_back( makeShaderState( device, m_pixelShader ) );

		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( HdrMapIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		this->createPipeline( size
			, {}
			, shaderStages
			, linearisedDepth.getDefaultView().getView()
			, *m_renderPass
			, std::move( bindings )
			, {} );
		auto commands = getCommands( *m_timer, 0u );
		m_commandBuffer = std::move( commands.commandBuffer );
		m_finished = std::move( commands.semaphore );
	}

	ashes::Semaphore const & SsgiRawGIPass::compute( ashes::Semaphore const & toWait )const
	{
		auto & renderSystem = m_renderSystem;
		auto & device = getCurrentRenderDevice( renderSystem );
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto * result = &toWait;
		auto fence = device->createFence( getName() );

		device.graphicsQueue->submit( *m_commandBuffer
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, fence.get() );
		fence->wait( ashes::MaxTimeout );
		result = m_finished.get();

		return *result;
	}

	CommandsSemaphore SsgiRawGIPass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		castor3d::CommandsSemaphore commands
		{
			device.graphicsCommandPool->createCommandBuffer( getName() ),
			device->createSemaphore( getName() )
		};
		auto & cmd = *commands.commandBuffer;

		cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
		timer.beginPass( cmd, index );
		cmd.beginDebugBlock(
			{
				"SSGI - Raw GI",
				castor3d::makeFloatArray( m_renderSystem.getEngine()->getNextRainbowColour() ),
			} );

		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { castor3d::transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerFrame( cmd );
		cmd.endRenderPass();
		cmd.endDebugBlock();
		timer.endPass( cmd, index );
		cmd.end();

		return commands;
	}

	void SsgiRawGIPass::accept( SsgiConfig & config
		, PipelineVisitorBase & visitor )
	{
		visitor.visit( "SSGI Raw GI"
			, getResult().getTexture()->getDefaultView().getView() );

		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		config.accept( m_pixelShader.name, visitor );
	}

	void SsgiRawGIPass::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( HdrMapIdx )
			, m_hdrResult.getDefaultView().getView()
			, m_sampler->getSampler() );
	}

	void SsgiRawGIPass::doRegisterFrame( ashes::CommandBuffer & commandBuffer )const
	{
	}
}
