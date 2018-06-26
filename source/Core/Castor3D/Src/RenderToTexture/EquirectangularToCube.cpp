#include "EquirectangularToCube.hpp"

#include "Render/RenderSystem.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Command/Queue.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Image/Texture.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Sync/Fence.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>

#include <Math/TransformationMatrix.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		renderer::ShaderStageStateArray doCreateProgram( RenderSystem & renderSystem
			, renderer::Format format )
		{
			auto & device = getCurrentDevice( renderSystem );

			glsl::Shader vtx;
			{
				GlslWriter writer{ renderSystem.createGlslWriter() };

				// Inputs
				auto position = writer.declAttribute< Vec4 >( cuT( "position" ), 0u );
				auto matrixUbo = glsl::Ubo{ writer, cuT( "Matrix" ), 0u, 0u };
				auto mtxViewProjection = matrixUbo.declMember< Mat4 >( cuT( "mtxViewProjection" ) );
				matrixUbo.end();

				// Outputs
				auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ), 0u );
				auto out = gl_PerVertex{ writer };

				std::function< void() > main = [&]()
				{
					vtx_position = position.xyz();
					out.gl_Position() = mtxViewProjection * writer.rendererScalePosition( position );
				};

				writer.implementFunction< void >( cuT( "main" ), main );
				vtx = writer.finalise();
			}

			glsl::Shader pxl;
			{
				GlslWriter writer{ renderSystem.createGlslWriter() };

				// Inputs
				auto mapColour = writer.declSampler< Sampler2D >( cuT( "mapColour" ), 1u, 0u );
				auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ), 0u );

				// Outputs
				auto pxl_colour = writer.declFragData< Vec4 >( cuT( "pxl_colour" ), 0u );
				
				auto sampleSphericalMap = writer.implementFunction< Vec2 >( cuT( "sampleSphericalMap" )
					, [&]( Vec3 const & v )
					{
						auto uv = writer.declLocale( cuT( "uv" ), vec2( atan( v.z(), v.x() ), asin( v.y() ) ) );
						uv *= vec2( 0.1591_f, 0.3183_f );
						uv += 0.5_f;
						writer.returnStmt( uv );
					}
					, InVec3{ &writer, cuT( "v" ) } );

				std::function< void() > main = [&]()
				{
					auto uv = writer.declLocale( cuT( "uv" ), sampleSphericalMap( normalize( vtx_position ) ) );
					pxl_colour = vec4( texture( mapColour, writer.adjustTexCoords( uv ) ).rgb(), 1.0 );
				};

				writer.implementFunction< void >( cuT( "main" ), main );
				pxl = writer.finalise();
			}

			std::vector< renderer::ShaderStageState > shaderStages;
			shaderStages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
			shaderStages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
			shaderStages[0].module->loadShader( vtx.getSource() );
			shaderStages[1].module->loadShader( pxl.getSource() );

			return shaderStages;
		}

		renderer::RenderPassPtr doCreateRenderPass( renderer::Device const & device
			, renderer::Format format )
		{
			renderer::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}
	}

	//*********************************************************************************************

	EquirectangularToCube::EquirectangularToCube( TextureLayout const & equiRectangular
		, RenderSystem & renderSystem
		, TextureLayout const & target )
		: RenderCube{ renderSystem, false }
		, m_device{ getCurrentDevice( renderSystem ) }
		, m_commandBuffer{ m_device.getGraphicsCommandPool().createCommandBuffer() }
		, m_view{ equiRectangular.getDefaultView() }
		, m_renderPass{ doCreateRenderPass( m_device, target.getPixelFormat() ) }
	{
		auto size = renderer::Extent2D{ target.getWidth(), target.getHeight() };
		auto program = doCreateProgram( renderSystem, equiRectangular.getPixelFormat() );
		uint32_t face = 0u;

		for ( auto & facePipeline : m_frameBuffers )
		{
			renderer::FrameBufferAttachmentArray attaches;
			facePipeline.view = target.getTexture().createView( renderer::TextureViewType::e2D
				, target.getPixelFormat()
				, 0u
				, 1u
				, face
				, 1u ); &target.getImage( face ).getView();
			attaches.emplace_back( *m_renderPass->getAttachments().begin(), *facePipeline.view );
			facePipeline.frameBuffer = m_renderPass->createFrameBuffer( size, std::move( attaches ) );
			++face;
		}

		createPipelines( size
			, Position{}
			, program
			, m_view
			, *m_renderPass
			, {} );
	}

	void EquirectangularToCube::render()
	{
		uint32_t face = 0u;

		for ( auto & frameBuffer : m_frameBuffers )
		{
			m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit );

			if ( face == 0u )
			{
				m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTransfer
					, renderer::PipelineStageFlag::eFragmentShader
					, m_view.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			}

			m_commandBuffer->beginRenderPass( *m_renderPass
				, *frameBuffer.frameBuffer
				, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
				, renderer::SubpassContents::eInline );
			registerFrame( *m_commandBuffer, face );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();

			m_device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
			m_device.getGraphicsQueue().waitIdle();

			++face;
		}
	}

	//*********************************************************************************************
}
