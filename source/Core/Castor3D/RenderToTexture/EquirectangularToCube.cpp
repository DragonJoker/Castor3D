#include "Castor3D/RenderToTexture/EquirectangularToCube.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <Ashes/Buffer/StagingBuffer.hpp>
#include <Ashes/Command/Queue.hpp>
#include <Ashes/Core/Device.hpp>
#include <Ashes/Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/Pipeline/DepthStencilState.hpp>
#include <Ashes/Pipeline/InputAssemblyState.hpp>
#include <Ashes/Pipeline/MultisampleState.hpp>
#include <Ashes/Pipeline/Scissor.hpp>
#include <Ashes/Pipeline/VertexLayout.hpp>
#include <Ashes/Pipeline/Viewport.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/RenderSubpass.hpp>
#include <Ashes/RenderPass/RenderSubpassState.hpp>
#include "Castor3D/Shader/GlslToSpv.hpp"
#include <Ashes/Sync/Fence.hpp>
#include <Ashes/Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>

#include <CastorUtils/Math/TransformationMatrix.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::ShaderStageStateArray doCreateProgram( RenderSystem & renderSystem
			, ashes::Format format )
		{
			auto & device = getCurrentDevice( renderSystem );

			ShaderModule vtx{ ashes::ShaderStageFlag::eVertex, "EquirectangularToCube" };
			{
				VertexWriter writer;

				// Inputs
				auto position = writer.declInput< Vec4 >( "position", 0u );
				auto matrixUbo = sdw::Ubo{ writer, "Matrix", 0u, 0u };
				auto mtxViewProjection = matrixUbo.declMember< Mat4 >( "mtxViewProjection" );
				matrixUbo.end();

				// Outputs
				auto vtx_position = writer.declOutput< Vec3 >( "vtx_position", 0u );
				auto out = writer.getOut();

				writer.implementFunction< sdw::Void >( "main"
					, [&]()
					{
						vtx_position = position.xyz();
						out.gl_out.gl_Position = mtxViewProjection * position;
					} );
				vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
			}

			ShaderModule pxl{ ashes::ShaderStageFlag::eFragment, "EquirectangularToCube" };
			{
				FragmentWriter writer;

				// Inputs
				auto mapColour = writer.declSampledImage< FImg2DRgba32 >( "mapColour", 1u, 0u );
				auto vtx_position = writer.declInput< Vec3 >( "vtx_position", 0u );

				// Outputs
				auto pxl_colour = writer.declOutput< Vec4 >( "pxl_colour", 0u );

				shader::Utils utils{ writer, renderSystem.isTopDown() };
				utils.declareInvertVec2Y();
				
				auto sampleSphericalMap = writer.implementFunction< Vec2 >( "sampleSphericalMap"
					, [&]( Vec3 const & v )
					{
						auto uv = writer.declLocale( "uv"
							, vec2( atan2( v.z(), v.x() ), asin( v.y() ) ) );
						uv *= vec2( 0.1591_f, 0.3183_f );
						uv += 0.5_f;
						writer.returnStmt( uv );
					}
					, InVec3{ writer, "v" } );

				writer.implementFunction< sdw::Void >( "main"
					, [&]()
					{
						auto uv = writer.declLocale( "uv"
							, sampleSphericalMap( normalize( vtx_position ) ) );
						pxl_colour = vec4( texture( mapColour, utils.topDownToBottomUp( uv ) ).rgb(), 1.0 );
					} );
				pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
			}

			std::vector< ashes::ShaderStageState > shaderStages;
			shaderStages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
			shaderStages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
			shaderStages[0].module->loadShader( renderSystem.compileShader( vtx ) );
			shaderStages[1].module->loadShader( renderSystem.compileShader( pxl ) );

			return shaderStages;
		}

		ashes::RenderPassPtr doCreateRenderPass( ashes::Device const & device
			, ashes::Format format )
		{
			ashes::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

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
		auto size = ashes::Extent2D{ target.getWidth(), target.getHeight() };
		auto program = doCreateProgram( renderSystem, equiRectangular.getPixelFormat() );
		uint32_t face = 0u;

		for ( auto & facePipeline : m_frameBuffers )
		{
			ashes::FrameBufferAttachmentArray attaches;
			facePipeline.view = target.getTexture().createView( ashes::TextureViewType::e2D
				, target.getPixelFormat()
				, 0u
				, 1u
				, face
				, 1u );
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
			m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );

			if ( face == 0u )
			{
				m_commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eTransfer
					, ashes::PipelineStageFlag::eFragmentShader
					, m_view.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
			}

			m_commandBuffer->beginRenderPass( *m_renderPass
				, *frameBuffer.frameBuffer
				, { ashes::ClearColorValue{ 0, 0, 0, 0 } }
				, ashes::SubpassContents::eInline );
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
