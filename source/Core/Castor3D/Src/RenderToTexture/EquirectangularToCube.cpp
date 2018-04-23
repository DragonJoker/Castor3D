#include "EquirectangularToCube.hpp"

#include "Render/RenderSystem.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Command/Queue.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
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
	namespace
	{
		renderer::SamplerPtr doCreateSampler( renderer::Device const & device )
		{
			return device.createSampler( renderer::WrapMode::eClampToEdge
				, renderer::WrapMode::eRepeat
				, renderer::WrapMode::eRepeat
				, renderer::Filter::eLinear
				, renderer::Filter::eLinear );
		}

		renderer::UniformBufferPtr< renderer::Mat4 > doCreateMatrixUbo( renderer::Device const & device
			, renderer::CommandBuffer const & commandBuffer )
		{
			static renderer::Mat4 const projection = device.perspective( float( 90.0_degrees ), 1.0f, 0.1f, 10.0f );
			static renderer::Mat4 const views[] =
			{
				convert( matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ) ),
				convert( matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ -1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ) ),
				convert( matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, +1.0f } ) ),
				convert( matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, -1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, -1.0f } ) ),
				convert( matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, +1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ) ),
				convert( matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, -1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ) )
			};

			auto result = renderer::makeUniformBuffer< renderer::Mat4 >( device
				, 6u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eDeviceLocal );

			if ( device.getClipDirection() == renderer::ClipDirection::eTopDown )
			{
				result->getData( 0u ) = projection * views[0];
				result->getData( 1u ) = projection * views[1];
				result->getData( 2u ) = projection * views[2];
				result->getData( 3u ) = projection * views[3];
				result->getData( 4u ) = projection * views[4];
				result->getData( 5u ) = projection * views[5];
			}
			else
			{
				result->getData( 0u ) = projection * views[0];
				result->getData( 1u ) = projection * views[1];
				result->getData( 2u ) = projection * views[3];
				result->getData( 3u ) = projection * views[2];
				result->getData( 4u ) = projection * views[4];
				result->getData( 5u ) = projection * views[5];
			}

			renderer::StagingBuffer stagingBuffer{ device
				, renderer::BufferTarget::eTransferDst
				, 6u * result->getAlignedSize() };
			stagingBuffer.uploadUniformData( commandBuffer
				, result->getDatas()
				, *result );
			return result;
		}

		renderer::ShaderStageStateArray doCreateProgram( RenderSystem & renderSystem
			, renderer::Format format
			, float gamma )
		{
			bool hdr = format == renderer::Format::eR32_SFLOAT
				|| format == renderer::Format::eR32G32_SFLOAT
				|| format == renderer::Format::eR32G32B32_SFLOAT
				|| format == renderer::Format::eR32G32B32A32_SFLOAT
				|| format == renderer::Format::eR16_SFLOAT
				|| format == renderer::Format::eR16G16_SFLOAT
				|| format == renderer::Format::eR16G16B16_SFLOAT
				|| format == renderer::Format::eR16G16B16A16_SFLOAT;
			auto & device = *renderSystem.getCurrentDevice();

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
				auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

				std::function< void() > main = [&]()
				{
					vtx_position = position.xyz();
					gl_Position = mtxViewProjection * position;
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

					if ( !hdr )
					{
						pxl_colour.rgb() = glsl::pow( pxl_colour.rgb(), vec3( Float( gamma ) ) );
					}
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

		renderer::VertexBufferPtr< renderer::Vec4 > doCreateVertexBuffer( renderer::Device const & device
			, renderer::CommandBuffer const & commandBuffer )
		{
			std::vector< renderer::Vec4 > vertexData
			{
				{ -1, +1, -1, +1 }, { +1, -1, -1, +1 }, { -1, -1, -1, +1 }, { +1, -1, -1, +1 }, { -1, +1, -1, +1 }, { +1, +1, -1, +1 },// Back
				{ -1, -1, +1, +1 }, { -1, +1, -1, +1 }, { -1, -1, -1, +1 }, { -1, +1, -1, +1 }, { -1, -1, +1, +1 }, { -1, +1, +1, +1 },// Left
				{ +1, -1, -1, +1 }, { +1, +1, +1, +1 }, { +1, -1, +1, +1 }, { +1, +1, +1, +1 }, { +1, -1, -1, +1 }, { +1, +1, -1, +1 },// Right
				{ -1, -1, +1, +1 }, { +1, +1, +1, +1 }, { -1, +1, +1, +1 }, { +1, +1, +1, +1 }, { -1, -1, +1, +1 }, { +1, -1, +1, +1 },// Front
				{ -1, +1, -1, +1 }, { +1, +1, +1, +1 }, { +1, +1, -1, +1 }, { +1, +1, +1, +1 }, { -1, +1, -1, +1 }, { -1, +1, +1, +1 },// Top
				{ -1, -1, -1, +1 }, { +1, -1, -1, +1 }, { -1, -1, +1, +1 }, { +1, -1, -1, +1 }, { +1, -1, +1, +1 }, { -1, -1, +1, +1 },// Bottom
			};
			auto result = renderer::makeVertexBuffer< renderer::Vec4 >( device
				, uint32_t( vertexData.size() )
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );

			if ( auto * buffer = result->lock( 0u, result->getCount(), renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, vertexData.data(), vertexData.size() * sizeof( renderer::Vec4 ) );
				result->flush( 0u, result->getCount() );
				result->unlock();
			}

			return result;
		}

		renderer::VertexLayoutPtr doCreateVertexLayout( renderer::Device const & device )
		{
			auto result = renderer::makeLayout< renderer::Vec4 >( 0u );
			result->createAttribute( 0u, renderer::Format::eR32G32B32A32_SFLOAT, 0u );
			return result;
		}

		renderer::DescriptorSetLayoutPtr doCreateDescriptorSetLayout( renderer::Device const & device )
		{
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
				{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
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
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].srcAccessMask = 0u;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}
	}

	EquirectangularToCube::EquirectangularToCube( TextureLayout const & equiRectangular
		, RenderSystem & renderSystem
		, TextureLayout const & target
		, float gamma )
		: m_device{ *renderSystem.getCurrentDevice() }
		, m_commandBuffer{ m_device.getGraphicsCommandPool().createCommandBuffer() }
		, m_view{ equiRectangular.getDefaultView() }
		, m_sampler{ doCreateSampler( m_device ) }
		, m_matrixUbo{ doCreateMatrixUbo( m_device, *m_commandBuffer ) }
		, m_vertexBuffer{ doCreateVertexBuffer( m_device, *m_commandBuffer ) }
		, m_vertexLayout{ doCreateVertexLayout( m_device ) }
		, m_descriptorLayout{ doCreateDescriptorSetLayout( m_device ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( 6u ) }
		, m_pipelineLayout{ m_device.createPipelineLayout( *m_descriptorLayout ) }
		, m_renderPass{ doCreateRenderPass( m_device, target.getPixelFormat() ) }
	{
		auto size = renderer::Extent2D{ target.getWidth(), target.getHeight() };
		uint32_t face = 0u;
		auto program = doCreateProgram( renderSystem, equiRectangular.getPixelFormat(), gamma );

		for ( auto & facePipeline : m_faces )
		{
			renderer::FrameBufferAttachmentArray attaches;
			facePipeline.view = &target.getImage( face ).getView();
			attaches.emplace_back( *m_renderPass->getAttachments().begin(), *facePipeline.view );
			facePipeline.frameBuffer = m_renderPass->createFrameBuffer( size, std::move( attaches ) );

			facePipeline.pipeline = m_pipelineLayout->createPipeline( renderer::GraphicsPipelineCreateInfo
			{
				program,
				*m_renderPass,
				renderer::VertexInputState::create( *m_vertexLayout ),
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
				renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eNone },
				renderer::MultisampleState{},
				renderer::ColourBlendState::createDefault(),
				{},
				renderer::DepthStencilState{ 0u, false, false },
				renderer::TessellationState{},
				renderer::Viewport{ size.width, size.height, 0, 0 },
				renderer::Scissor{ 0, 0, size.width, size.height }
			} );

			facePipeline.descriptorSet = m_descriptorPool->createDescriptorSet();
			facePipeline.descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
				, *m_matrixUbo
				, face
				, 1u );
			facePipeline.descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
				, m_view
				, *m_sampler );
			facePipeline.descriptorSet->update();
			++face;
		}
	}

	void EquirectangularToCube::render( renderer::CommandBuffer & commandBuffer )
	{
		for ( auto & facePipeline : m_faces )
		{
			commandBuffer.beginRenderPass( *m_renderPass
				, *facePipeline.frameBuffer
				, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
				, renderer::SubpassContents::eInline );
			commandBuffer.bindPipeline( *facePipeline.pipeline );
			commandBuffer.bindDescriptorSet( *facePipeline.descriptorSet
				, *m_pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			commandBuffer.draw( 36u );
			commandBuffer.endRenderPass();
		}
	}

	void EquirectangularToCube::render()
	{
		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			render( *m_commandBuffer );
			m_commandBuffer->end();
			auto fence = m_device.createFence();
			m_device.getGraphicsQueue().submit( *m_commandBuffer, fence.get() );
			fence->wait( renderer::FenceTimeout );
		}
	}
}
